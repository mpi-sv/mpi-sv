#if defined(__OSI)
  #include <osi.h>
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
#endif


#include "mpi.h"


#define NUM_RQST 4

int main (int argc, char **argv) {

  int          numprocs, myid;
  int          buf;
  int          bufrecv[NUM_RQST];
  MPI_Request  req[NUM_RQST];
  MPI_Request  req_send;
  MPI_Status   status;
  int          flag;
  int          i, k;
  int          idx;
  int          count;


  MPI_Init (NULL, NULL);
  MPI_Comm_size (MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank (MPI_COMM_WORLD, &myid);

  if ((myid > 0) && (myid < NUM_RQST)) {

    buf = myid;
    if (myid % 2) {

      usleep(1000);
      MPI_Send (&buf, 1, MPI_INT, 0, 36, MPI_COMM_WORLD);

    } else {

      MPI_Isend (&buf, 1, MPI_INT, 0, 36, MPI_COMM_WORLD, &req_send);
      MPI_Request_free(&req_send);

    }

  } else if (myid == 0) {

    req[0] = MPI_REQUEST_NULL;
    MPI_Irecv(&bufrecv[0], 1, MPI_INT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &req[1]);
    MPI_Irecv(&bufrecv[1], 1, MPI_INT, 2, MPI_ANY_TAG, MPI_COMM_WORLD, &req[2]);
    MPI_Irecv(&bufrecv[2], 1, MPI_INT, 3, MPI_ANY_TAG, MPI_COMM_WORLD, &req[3]);

    k = 0;
    while (k < NUM_RQST) {

      flag = 0;
      while (!flag) {

        MPI_Testany(NUM_RQST, &req, &idx, &flag, &status);

        if ((flag) && (idx == MPI_UNDEFINED)) {      /* All null */
          printf("All requests NULL\n");
          break;
        }
        if ((flag) && (idx != MPI_UNDEFINED)) {       /* One completed */
          printf("Request completed %d  0x%x\n", idx, req[idx]);
          k += idx;
        }
      }

      MPI_Get_count(&status, MPI_INT, &count);
      printf("(Index: %d) Received last message from %d (tag %d) with %d elements\n", idx, status.MPI_SOURCE, status.MPI_TAG, count);

    }

  }

  MPI_Finalize ();
  return 0;
}

