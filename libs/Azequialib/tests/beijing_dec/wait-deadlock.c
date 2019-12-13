/* -*- Mode: C; -*- */
/* Creator: Bronis R. de Supinski (bronis@llnl.gov) Fri Mar  17 2000 */
/* no-error.c -- do some MPI calls without any errors */

#include <stdio.h>
#include "mpi.h"

#define buf_size 128

int
main (int argc, char **argv)
{
  int nprocs = -1;
  int rank = -1;
  char processor_name[128];
  int namelen = 128;
  int buf0[buf_size];
  int buf1[buf_size];
  MPI_Status status;
  MPI_Request req;

  /* init */
  MPI_Init (&argc, &argv);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Get_processor_name (processor_name, &namelen);
  printf ("(%d) is alive on %s\n", rank, processor_name);
  fflush (stdout);
	int i;
  MPI_Barrier (MPI_COMM_WORLD);

  if (nprocs < 2)
    {
      printf ("not enough tasks\n");
    }
  else if (rank == 0)
    {
      memset (buf0, 0, buf_size);

      MPI_Irecv (buf1, buf_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &req);

      MPI_Wait (&req, &status);

      MPI_Send (buf0, buf_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
  else if (rank == 1)
    {
      memset (buf1, 1, buf_size);

      MPI_Irecv (buf0, buf_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);

      MPI_Wait (&req, &status);

      MPI_Send (buf1, buf_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

  MPI_Barrier (MPI_COMM_WORLD);

  MPI_Finalize ();
  printf ("(%d) Finished normally\n", rank);
}

/* EOF */
