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
 /   Declaration of types and functions used by this module        /
/----------------------------------------------------------------*/
#include <env.h>
#include <errhnd.h>

/* Profiling */
#ifdef AZQMPI_PROFILING
#undef MPI_Keyval_free
#define MPI_Keyval_free  PMPI_Keyval_free
#endif


/**
 *  MPI_Keyval_free
 *
 */
int MPI_Keyval_free (int *keyval) {

  int   mpi_errno;

#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Keyval_free (start)\tProcess: 0x%x\n", PCS_self());
#endif

  /* 1. Check integrity of parameters */
#ifdef CHECK_MODE
  if (mpi_errno = check_keyval(*keyval))                   goto mpi_exception;
#endif

  /* 2. Free the key */
  CALL_FXN(PCS_keyFree (*keyval), MPI_ERR_OTHER);

  *keyval = MPI_KEYVAL_INVALID;

#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Keyval_free (end)  \tProcess: 0x%x\n", PCS_self());
#endif

  return MPI_SUCCESS;

mpi_exception_unnest:
mpi_exception:
  return commHandleError (MPI_COMM_WORLD, mpi_errno, "MPI_Keyval_free");
}

