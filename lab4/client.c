#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255
#define NCONNECTIONS 10
int  main(int argc, char* argv[])
{
    int hSocket[NCONNECTIONS];//should be an array                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;

    if(argc < 3)
      {
        printf("\nUsage: client host-name host-port\n");
        return 0;
      }
    else
      {
        strcpy(strHostName,argv[1]);
        nHostPort=atoi(argv[2]);
      }

 for ( int i=0; i< NCONNECTIONS; i++){
    printf("\nMaking a socket");
    /* make a socket */
    hSocket[i]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }
	
    printf("Socket made");
    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    /* connect to host */

	int epollfd = epoll_creat(1);
	
    if(connect(hSocket[i],(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
    strut epoll_event event;
    event.data.fd=hSocket[i];	
    epoll_ctl(epollfd, EPOLL_CTR_ADD,hSocket[i],&event);

#define MAXMSG 1024
	char *message=(char*) malloc(MAXMSG);
	sprintf(message,"GET %s HTTP/1.1\r\nHost:%s:%s\r\n\r\n",argv[3]);
	 write(hSocket[i],message,strlen(message));
}


//	memset(pBuffer, 0, BUFFER_Size);
  // 	nReadAmount=read(hSocket[i],pBuffer,BUFFER_SIZE);
//	printf("%s",pBuffer);
        for (int i =0; i< NCONNECTIONS; -++)
	{	
		struct epoll_event event;
		int rval = epoll_wait(epollfd, &event, 1, -1);
		char bufferp[10000];
		read(event.data.fg, bufferp, 10000);


	}	
 /* read from socket into buffer
    ** number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured */
    /* write what we received back to the server */
  
    printf("\nClosing socket\n");
    /* close socket */                       
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
	free(message);	

}
