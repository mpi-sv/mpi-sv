/* _________________________________________________________________________
   |                                                                       |
   |  Azequia (embedded) Message Passing Interface   ( AzequiaMPI )        |
   |                                                                       |
   |  Authors: DSP Systems Group                                           |
   |           http://gsd.unex.es                                          |
   |           University of Extremadura                                   |
   |           Caceres, Spain                                              |
   |           jarico@unex.es                                              |
   |                                                                       |
   |  Date:    Sept 22, 2008                                               |
   |                                                                       |
   |  Description:                                                         |
   |                                                                       |
   |                                                                       |
   |_______________________________________________________________________| */


#include <mpi.h>
#include <env.h>
#include <check.h>
#include <p_config.h>

#include <p_dtype.h>

/* Profiling */
#ifdef AZQMPI_PROFILING
#undef MPI_Type_hvector
#define MPI_Type_hvector  PMPI_Type_hvector
#endif


/*
 *  MPI_Type_hvector
 *
 */
int MPI_Type_hvector (int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype) {

  int  mpi_errno;

#ifdef CHECK_MODE
  if (mpi_errno = check_datatype(oldtype))                 goto mpi_exception;
#endif

  CALL_FXN(PCS_dtypeCreate( count,                               /* Size of arrays    */
                            &blocklength,                        /* Block lengths     */
                            &stride,                             /* Displacements     */
                            (Mpi_P_Datatype_t *)&oldtype,        /* Types             */
                            DTYPE_HVECTOR,                       /* Kind of MPI type  */
                            newtype),                            /* New type created  */
           MPI_ERR_TYPE);

  return MPI_SUCCESS;

mpi_exception_unnest:
mpi_exception:
  return commHandleError (MPI_COMM_WORLD, mpi_errno, "MPI_Type_hvector");
}
