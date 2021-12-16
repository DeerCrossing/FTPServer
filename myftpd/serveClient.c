#include "serveClient.h"



void serveClient(int sd)
{
    int nr;
    int nw;
    int i = 0;
    char buf[BUFSIZ];

    while (++i)
    {

        /* read data from client */
        if ((nr = read(sd, buf, sizeof(buf))) <= 0)
        {
            exit(0);   /* connection broken down */
        }
        printf("server[%d]: %d bytes received\n", i, nr);

        /* process the data we have received */
        //reverse(buf, nr);
        printf("server[%d]: %d bytes processed\n", i, nr);

        /* send results to client */
        nw = write(sd, buf, nr);
        printf("server[%d]: %d bytes sent out\n", i, nw);
    }
}