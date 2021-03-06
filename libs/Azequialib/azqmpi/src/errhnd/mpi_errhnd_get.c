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

  /*----------------------------------------------------------------/
 /   Declaration of public functions implemented by this module    /
/----------------------------------------------------------------*/
#include <errhnd.h>

  /*----------------------------------------------------------------/
 /   Declaration of types and functions used by this module        /
/----------------------------------------------------------------*/
#if defined (__OSI)
  #include <osi.h>
#else
  #include <stdio.h>
  #include <string.h>
#endif

#include <com.h>

#include <mpi.h>
#include <p_errhnd.h>
#include <check.h>

/* Profiling */
#ifdef AZQMPI_PROFILING
#undef MPI_Errhandler_get
#define MPI_Errhandler_get  PMPI_Errhandler_get
#endif


/*
 *  MPI_Errhandler_get
 */
int MPI_Errhandler_get (MPI_Comm comm, MPI_Errhandler *errhandler) {

  int  mpi_errno;

#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Errhandler_get (start)\tProcess: 0x%x\n", PCS_self());
#endif

#ifdef CHECK_MODE
  if (mpi_errno = check_comm(comm))                        goto mpi_exception;
#endif

  CALL_FXN (commGetErrHnd (comm, (void *)errhandler), MPI_ERR_ARG);

#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Errhandler_get (end)  \tProcess: 0x%x\n", PCS_self());
#endif

  return MPI_SUCCESS;

mpi_exception_unnest:
mpi_exception:
  return commHandleError (MPI_COMM_WORLD, mpi_errno, "MPI_Errhanler_get");
}

