
#if defined(__OSI)
  #include <osi.h>
#else
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <time.h>
#endif

#include <azq.h>


#define  COUNT  256


int latency(int *param) {

  int      numprocs, myid;
  Rqst    *rqst[16];
  Rqst    *req[16];
  Addr     dst, src;
  int      buf;
  int      bufrecv[COUNT];
  Status   status;
  Status   st;
  int      total;
  int      i, j, k;
  int      idx;
  int      iter = 0;
  int      turn = 0;
  int      err = 0;
  struct timespec tslp;


  GRP_getSize(getGroup(), &numprocs);
  myid = getRank();

  /*if (myid == 0) {
    rqst = (Rqst *)malloc(sizeof(Rqst) * (numprocs - 1));
    for (i = 0; i < numprocs - 1; i++)
      req[i] = &rqst[i];
  }*/

  buf = 0;

  for (k = 0; k < 1000000; k++) {

    if (myid == (numprocs - 1)) {

      for (i = 0; i < numprocs - 1; i++) {
        dst.Group = getGroup();
        dst.Rank  = i;

        //fprintf(stdout, "%d doing send ... (value: %d)\n", myid, buf);
        if (0 > asend(&dst, (char *)&buf, sizeof(int), 99, &rqst[i])) {err = -3; goto error;}
      }

      /*tslp.tv_sec = 0;
      tslp.tv_nsec = (rand () % 1000000);
      nanosleep(&tslp,NULL);*/

      for (i = 0; i < numprocs - 1; i++) {
        if (0 > waitany(rqst, numprocs - 1, &idx, &status)) {err = -1; goto error;}
        //fprintf(stdout, "%d Enviado %d bytes to %d\n", myid, sizeof(int), idx);
      }

      buf++;

      if (!(k % 10000)) {fprintf(stdout, "Iter: %d\n", k);}

    } else {

      /*tslp.tv_sec = 0;
      tslp.tv_nsec = 500000;
      nanosleep(&tslp,NULL);*/

      src.Group = getGroup();
      src.Rank  = numprocs - 1;
      //src.Rank  = ADDR_RNK_ANY;

      tmd_recv(&src, (char *)bufrecv, sizeof(int), 99, &st, COM_FOREVER);
      //fprintf(stdout, "%d Recibido %d bytes from %d, value: %d  (0x%x)\n", myid, st.Count, st.Src.Rank, bufrecv[0], THR_self());

      if (k != bufrecv[0])
        fprintf(stdout, "\nERROR. Recibido: %d\n", bufrecv[0]);
    }
  }

  sleep(1);
  /*if (myid != 0)
    free(rqst);*/
  fprintf(stdout, "Thread  %d  of  %d  terminating ... (iter %d)\n", myid, numprocs, iter);

  return 0;

error:
  fprintf(stdout, "Thread  %d  terminating with error: %d\n", myid, err);
}

