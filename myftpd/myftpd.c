// myftpd.c : Defines the entry point for the application.
//

#include  <unistd.h>
#include  <sys/stat.h>
#include  <stdlib.h>
#include  <stdio.h> 
#include  <limits.h>
#include  <string.h>     // strlen(), strcmp() etc 
#include  <errno.h>      // extern int errno, EINTR, perror() 
#include  <signal.h>     // SIGCHLD, sigaction()
#include  <syslog.h>
#include  <sys/types.h>  // pid_t, u_long, u_short 
#include  <sys/socket.h> // struct sockaddr, socket(), etc 
#include  <sys/wait.h>   // waitpid(), WNOHAND
#include  <netinet/in.h> // struct sockaddr_in, htons(), htonl(),
                         // and INADDR_ANY
#include "serveClient.h"

#define port 19721

void initiateDaemon(char* cwd);
void claimChildren();


int main(int argc, char* argv[])
{
    int lsd; //listening socket descriptor
    int csd; //child socket descriptor
    int n; 
    socklen_t cli_addrlen; //length of client address
    pid_t pid;
    struct sockaddr_in ser_addr; // server socket address 
    struct sockaddr_in cli_addr; // client socket address
    char cwd[PATH_MAX + 1];

    // get the working directory
    if (argc == 1) 
    {
        getcwd(cwd, PATH_MAX);
    }
    else if (argc == 2) 
    {
        if(strlen(argv[1]) <= PATH_MAX)
        strncpy(cwd, argv[1], PATH_MAX);
    }
    else 
    {
        printf("Usage: %s [ server working directory ]\n", argv[0]);
        exit(1);
    }

    // turn the program into a daemon
    initiateDaemon(cwd);

    // set up listening socket descriptor
    if ((lsd = socket(PF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("server:socket"); 
        exit(1);
    }

    // build server Internet socket address
    bzero((char*)&ser_addr, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET; // set IpV4
    ser_addr.sin_port = htons(port); // set port
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //accept from any network interface point

    // bind server address to listening socket
    if (bind(lsd, (struct sockaddr*)&ser_addr, sizeof(ser_addr)) < 0) 
    {
        perror("server bind"); 
        exit(1);
    }

    //enable listening socket with backlog of 5
    listen(lsd, 5);

    while (1) 
    {

        // wait until a client requests a connection
        cli_addrlen = sizeof(cli_addr);
        csd = accept(lsd, (struct sockaddr*)&cli_addr, (socklen_t*)&cli_addrlen);
        if (csd < 0) 
        {
            if (errno == EINTR)   //if interrupted by SIGCHLD
            {
                continue;
            }
            perror("server:accept"); 
            exit(1);
        }

        // create a child process for this client
        if ((pid = fork()) < 0) 
        {
            perror("fork"); 
            exit(1);
        }
        else if (pid > 0) 
        {
            close(csd); //parent closes the child's socket
            continue; // parent then loops back to waiting for the next client
        }

        // now the child
        close(lsd);
        serveClient(csd);
        exit(0);
    }
}

void initiateDaemon(char* cwd)
{
    pid_t   pid;
    struct sigaction act;

    if ((pid = fork()) < 0) {
        perror("fork"); exit(1);
    }
    else if (pid > 0) {
        printf("PID: %d\n", pid);
        exit(0);                  // exit parent
    }

    // setup child conditions child
    setsid();                      // become session leader
    chdir(cwd);                    // change working directory
    umask(0);                      // clear file mode creation mask

    // catch SIGCHLD to remove zombies from system
    act.sa_handler = claimChildren;  // use modern signal claiming
    sigemptyset(&act.sa_mask);       // not to block other signals
    act.sa_flags = SA_NOCLDSTOP;     // ignore stopped children
    sigaction(SIGCHLD, (struct sigaction*)&act, (struct sigaction*)0);
}

void claimChildren()
{
    pid_t pid = 1;

    while (pid > 0) { // claim all of the zombies possible
        pid = waitpid(0, (int*)0, WNOHANG);
    }
}