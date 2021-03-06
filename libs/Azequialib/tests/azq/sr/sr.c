/*----------------------------------------------------------------*
 *   Declaration of public types and functions                    *
 *   used by this module                                          *
 *----------------------------------------------------------------*/
#ifdef __OSI
#include <osi.h>
#endif
#include <azq.h>
#include <azq_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>


/*----------------------------------------------------------------*
 *   Definition of private constants                              *
 *----------------------------------------------------------------*/
#define  PRINT_PERIOD  1000
#define  ITERA         10000

/*----------------------------------------------------------------*
 *   Implementation of private functions                          *
 *----------------------------------------------------------------*/
double abstime (void) {

  struct timespec t;

  clock_gettime(CLOCK_REALTIME, &t);

  return ((double) t.tv_sec + 1.0e-9 * (double) t.tv_nsec);
}


#define MAX_PEND_RQST  2

int do_sender (int bufSize) {

  int      myid;
  int      gix;
  int      itr  = 0;
  int      i;
  Addr     dst;
  int      excpn;
  double   now_1;
  double   now_0;
  int     *buf0;
  int     *buf1;
  Rqst     rqst[MAX_PEND_RQST];
  Rqst_t   rqst_ptr[MAX_PEND_RQST];
  Rqst     req;
  Rqst_t   req_ptr;
  Rqst     rqst_per;
  Rqst_t   rqst_ptent;
  int      sz;


  myid = getRank();
  gix  = getGroup();
#ifdef __DEBUG
  fprintf(stdout, "Sender(%x). [%d, %d]\n", (unsigned int)(THR_self()), getGroup(), myid);
#endif

  now_0 = abstime();

  /* Get the memory for the data to be sent */
  if(NULL == (buf0 = (int *)malloc(bufSize))) {
    fprintf(stdout, "::::::::::::::::: Bug Node: Malloc Exception %d ::::::::::::::::\n", -5);
    return(-5);
  }
  if(NULL == (buf1 = (int *)malloc(bufSize))) {
    fprintf(stdout, "::::::::::::::::: Bug Node: Malloc Exception %d ::::::::::::::::\n", -5);
    return(-5);
  }


  for(i = 0; i < bufSize / sizeof(int); i++) {
    buf0[i] = 0;
    buf1[i] = 0;
  }

  /* Configure destination address */
  dst.Group = getGroup();
  dst.Rank  = myid - 1;

  /* Persistent request */
  rqst_ptent = &rqst_per;
  psend_init(&dst, (char *)buf0, bufSize, 99, rqst_ptent);


  while (1) {

    if(itr % PRINT_PERIOD == 0)  fprintf(stdout, "\t\t= S [%d %d] ============== itr [%d]  ...\n", gix, myid, itr);
    fflush(stdout);

    if(0 > (excpn = send(&dst, (char *)&buf0[0], bufSize, 99))) {
      fprintf(stderr, "\n:: S :::::::::: Sender(%x). [%d  %d]: Exception %d :::::::::: n", (int)(THR_self()), gix, myid, excpn);
      free(buf0);
      return(excpn);
    }

    itr++;
    if (itr == ITERA)
      break;
  }

  if(buf0) free(buf0);
  if(buf1) free(buf1);

  now_1 = abstime();

#ifdef __DEBUG
  fprintf(stdout, "\nmilliseconds %lf\n", (now_1 - now_0));
  fprintf(stdout, "Sender(%x). [%d, %d].  BYE\n", (unsigned int)(THR_self()), getGroup(), myid);
#endif

  return 0;
}




int do_receiver (int bufSize) {
    
  Status     status;
  int        myid;
  int        gix;
  int        itr = 0;
  double     now_1;
  double     now_0;
  int       *buf;
  Addr       src;
  int        excpn;
  int        i;
  int        tag;
  Rqst       req;
  Rqst_t     req_ptr;
  Rqst       rqst_per;
  Rqst_t     rqst_ptent;
  int        cnt;


  myid = getRank();
  gix  = getGroup();
#ifdef __DEBUG
  fprintf(stdout, "Receiver(%x). [%d, %d]\n", (unsigned int)(THR_self()), getGroup(), myid);
#endif

  now_0 = abstime();

  if(NULL == (buf = (int *)malloc(bufSize))) {
    fprintf(stdout, "::::::::::::::::: Bug Node: Malloc Exception %d ::::::::::::::::\n", -5);
    return(-5);
  }


  /* Configure source address */
  src.Group = getGroup();
  src.Rank  = myid + 1;

  /* Persistent request */
  tag = 99;
  rqst_ptent = &rqst_per;
  precv_init(&src, (char *)buf, bufSize, tag, rqst_ptent);


  while (1) {

    if(itr % PRINT_PERIOD == 0)  fprintf(stdout, "\t\t= R [%d %d] ============== itr [%d]  ...\n", gix, myid, itr);
    fflush(stdout);

  tag = 99;
  if(0 > (excpn = timed_recv(&src, (char *)buf, bufSize, tag, &status, COM_FOREVER))) {
    fprintf(stderr, "\n:: R :::::::::: Receiver(%x). [%d  %d]: Exception %d :::::::::: n", (int)(THR_self()), gix, myid, excpn);
    return(excpn);
  }

  cnt = (status.Count - sizeof(int)) / sizeof(int);

#ifdef __CHECK_
    /* ---------- 2. Run the ALGORITHM --------- */
    for(i = 0; i < bufSize / sizeof(int); i++) {
      if(buf[i] != itr) {
        fprintf(stdout, "\n :::::::::: Radiator (%x). USR %d Fail!!. Itr = %d/Value = %d :::::::::: \n", (int)THR_self(), i, itr, buf[i]);
        exit(1);
      }
    }
#endif

    if (++itr == ITERA)
      break;
    
  }
  now_1 = abstime();

  time_t result;
  result = time(NULL);

  fprintf(stdout, "\nmilliseconds %lf\n", (now_1 - now_0));
  fprintf(stdout, "Receiver(%x). [%d, %d].  time: %s  -  BYE\n", (unsigned int)(THR_self()), getGroup(), myid, asctime(localtime(&result)));

  return(0);
}



int node_main (int argc, char *argv[]) {

  int   myid;
  int   numprocs;
  int   bufsize;

  GRP_getSize(getGroup(), &numprocs);
  myid = getRank();

  bufsize = atoi(argv[1]);

  srand(abstime());

  fprintf(stdout, "[%d , %d] \n", getGroup(), myid);

  if (myid % 2)  do_sender(bufsize);
  else           do_receiver(bufsize);

  return 0;
}

