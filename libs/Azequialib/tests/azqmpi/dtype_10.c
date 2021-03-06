#include <stdio.h>
#include <mpi.h>

#define TAG  99
#define NELEM  10

struct Container {
	double a1;
	char a2;
};

int main( int argc, char **argv)
{
	MPI_Datatype ot[3], newType;
	struct Container sample;
	struct Container in[NELEM], out[NELEM];
	MPI_Aint os[3];
	int bc[3];
	int tag = TAG;
	int rank, numtasks;
	MPI_Status stat;
	int count;
  int i;

	/* set block counts */
	bc[0] = bc[1] = bc[2] = 1;

	/* set (relative) offsets */
	MPI_Address(&(sample.a1), &os[0]);
	MPI_Address(&(sample.a2), &os[1]);
	MPI_Address(&(sample)+1, &os[2]);

	os[1] = os[1] - os[0];
	os[2] = os[2] - os[0];
	os[0] = 0;

	/* set old types */
	ot[0] = MPI_DOUBLE;
	ot[1] = MPI_CHAR;
	ot[2] = MPI_UB;

	/* Init MPI */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

	/* Register new type */
	MPI_Type_struct(3, bc, os, ot, &newType);
	MPI_Type_commit(&newType);

	if (rank == 0) {
	  for (i = 0; i < NELEM; i++) {
		in[i].a1 = (double)i;
		in[i].a2 = 65 + i;
	  }
		MPI_Send(in, NELEM, newType, 1, tag, MPI_COMM_WORLD);
	} else {
	  for (i = 0; i < NELEM; i++) {
		out[i].a1 = (double)0;
		out[i].a2 = 0;
	  }
		MPI_Recv(out, NELEM, newType, 0, tag, MPI_COMM_WORLD, &stat);
		MPI_Get_count(&stat, newType, &count);
	  
		if (count != NELEM)
			printf("Expected %d, but got %d\n", NELEM, count);
	  for (i = 0; i < NELEM; i++) {
		fprintf(stdout, " %lf  ", out[i].a1);
		fprintf(stdout, " %c  \n", out[i].a2);
	  }
	}

    MPI_Type_free(&newType);
	MPI_Finalize();
}
