#include <config.h>
/* This definitions must be before including proto.h */
#if defined(F77_NAME_LOWER_USCORE)
#define mpi_allreduce__ mpi_allreduce_
#elif defined(F77_NAME_LOWER_2USCORE)
/* as is */
#else 
#error "Naming Fortran functions not supported"
#endif


#include "f_proto.h"

/* NOT YET. PCS_copGet must be replaced by cop_f2c() */
void mpi_allreduce__ (char *sendbuf, char *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *ierr) {

  int              op_idx    = *op;
  Mpi_P_Comm      *comm_ptr; 
  Mpi_P_Datatype  *dtype_ptr;
  Mpi_P_Op        *op_ptr    = (Mpi_P_Op *)       PCS_copGet   (op_idx);

  comm_f2c (comm,     &comm_ptr);
  dtype_f2c(datatype, &dtype_ptr);
  
  *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, dtype_ptr, op_ptr, comm_ptr);

}
