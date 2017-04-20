//real time VoIP phone--server side with g711 integration

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/timerfd.h>
#include <time.h>

#include <stdlib.h>

#include <stdint.h> 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "g711mit.c"
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/gccmacro.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define BUFSIZE 1024

volatile sig_atomic_t keep_going = 1;

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void my_handler_for_sigint(int signumber)//handler for CTRL+C SIGINT
{
  char ans[2];
  if (signumber == SIGINT)
  {
    printf("received SIGINT\n");
    printf("Program received a CTRL-C\n");
    printf("Terminate Y/N : "); 
    scanf("%s", ans);
    if (strcmp(ans,"Y") == 0)//Y for terminate
    {
       printf("Exiting ....Press any key\n");
        keep_going = 0;//atomic variable that controls the server loop is set here;cleanup in main
	//kill(getppid(),SIGKILL);
	exit(0); 
    }
    else
    {
       printf("Continung ..\n");//No for continue
    }
  }
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc,char* argv[])
{	// char buf[MAXDATASIZE];
	int rv;
	int numbytes;
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    
     static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 8000,
        .channels = 2
    };
    pa_simple *s1 = NULL;
    int ret = 1;
    int error;

    int fd;

     if (signal(SIGINT, my_handler_for_sigint) == SIG_ERR)//register signal handler for SIGINT
      printf("\ncan't catch SIGINT\n");
	
    if (argc != 2) {
        fprintf(stderr,"usage:port number\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {//bind a name to the socket;has no address assigned to it
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {//listen for connections
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

   printf("server: waiting for connections...\n"); 

 if (!(s1 = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
unsigned char buf[BUFSIZE];
short buf2[BUFSIZE];
        ssize_t r;
struct itimerspec new_value;
    int max_exp, fd2;
    struct timespec now;
    uint64_t exp, tot_exp;
    ssize_t s3;

   if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        handle_error("clock_gettime");

   /* Create a CLOCK_REALTIME absolute timer with initial
       expiration and interval as specified in command line */

   new_value.it_value.tv_sec = now.tv_sec ;
    new_value.it_value.tv_nsec = now.tv_nsec;

  
        new_value.it_interval.tv_sec=0;
        max_exp = 100000000;
      	new_value.it_interval.tv_nsec = 10000;
 // main accept() loop
	//printf("loop");
while(1)
        {
	sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);//accept a connection

        if (new_fd == -1) {
           perror("accept");
	      
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        printf("server: got connection from %s\n", s);
	
	fd2 = timerfd_create(CLOCK_REALTIME, 0);
   
    if (fd2 == -1)
        handle_error("timerfd_create");

   if (timerfd_settime(fd2, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
        handle_error("timerfd_settime");

    for (tot_exp = 0; tot_exp < max_exp;) {
        s3 = read(fd2, &exp, sizeof(uint64_t));
        if (s3 != sizeof(uint64_t))
            handle_error("read");

        tot_exp += exp;

        close(sockfd); // child doesn't need the listener
            
   	 if ((numbytes = recv(new_fd, buf, BUFSIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    	}
	

	for(int i=0;i<BUFSIZE;i++)
	{
	buf2[i]=alaw2linear(buf[i]);//decode using a law
	//printf("decoding %d %d %d\n",i,buf2[i],buf3[i]);
	}

        /* ... and play it */
        if (pa_simple_write(s1, buf2, sizeof(buf2), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    //usleep(200);


	}

}    


close(new_fd);  // parent doesn't need this
exit(0);
if(keep_going==0)//cleanup in parent process
{close(sockfd);
printf("cleanup before exiting parent");
exit(0);
}

if (pa_simple_drain(s1, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    	}


ret = 0;

finish:

    if (s1)
        pa_simple_free(s1);

    return ret;
    return 0;
}

