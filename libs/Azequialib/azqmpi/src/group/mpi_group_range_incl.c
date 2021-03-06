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
#include <mpi.h>

#if defined(__OSI)
  #include <osi.h>
#else
  #include <stdio.h>
#endif

#include <thr.h>
#include <com.h>

#include <env.h>
#include <errhnd.h>
#include <check.h>

/* Profiling */
#ifdef AZQMPI_PROFILING
#undef MPI_Group_range_incl
#define MPI_Group_range_incl  PMPI_Group_range_incl
#endif


/**
 *  MPI_Group_range_incl
 */
int MPI_Group_range_incl (MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup) {

  int   mpi_errno;
  int  *ranks_in_newgroup;
  int   i, j, k;
  int   newsize;
  int   first, last, stride;

#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Group_incl (start)\tProcess: 0x%x\n", PCS_self());
#endif

#ifdef CHECK_MODE
  if (mpi_errno = check_group(group))                      goto mpi_exception;
  if (mpi_errno = check_valid_ranges(group, n, ranges))    goto mpi_exception;
#endif

  /* 1. Calculating the size of the new group */
  newsize = 0;
  for (i = 0; i < n; i++) {
    first  = ranges[i][0];
    last   = ranges[i][1];
    stride = ranges[i][2];
    newsize += 1 + (last - first) / stride;
  }
  if (newsize == 0) {
    groupGetRef(GROUP_EMPTY_PTR, newgroup);
    return MPI_SUCCESS;
  }
  
  /* 2. Allocate space for the newgroup */
  CALL_FXN(MALLOC(ranks_in_newgroup, newsize * sizeof(int)), MPI_ERR_INTERN);

  /* 3. New group to create */
  k = 0;
  for (i = 0; i < n; i++) {
    first  = ranges[i][0];
    last   = ranges[i][1];
    stride = ranges[i][2];

    if (stride > 0) {

      for (j = first; j <= last; j += stride) {
        ranks_in_newgroup[k] = groupGetGlobalRank(group, j);
        k++;
      }

    } else {

      for (j = first; j >= last; j += stride) {
        ranks_in_newgroup[k] = groupGetGlobalRank(group, j);
        k++;
      }

    }
  }

  /* 4. Create the group */
  CALL_FXN (PCS_groupCreate(ranks_in_newgroup, newsize, newgroup), MPI_ERR_ARG)

  /* 5. Free allocated space */
  FREE(ranks_in_newgroup);
  
#ifdef DEBUG_MODE
  fprintf(stdout, "MPI_Group_incl (end)  \tProcess: 0x%x\n", PCS_self());
#endif

  return MPI_SUCCESS;

mpi_exception_unnest:
mpi_exception:
  FREE(ranks_in_newgroup);
  return commHandleError(MPI_COMM_WORLD, mpi_errno, "MPI_Group_range_incl");
}




