#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <string.h>
#include <vector>             // stl vector
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
using namespace std;



#define MAX_MSG_SZ      1024
#define SOCKET_ERROR        -1
#define BUFFER_SIZE         1000
#define HOST_NAME_SIZE      255
#define TRUE                 1 
#define FALSE                0

// Determine if the character is whitespace
bool isWhitespace(char c)
{ switch (c)
    {
        case '\r':
        case '\n':
        case ' ':
        case '\0':
            return true;
        default:
            return false;
    }
}

// Strip off whitespace characters from the end of the line
void chomp(char *line)
{
    int len = strlen(line);
    while (isWhitespace(line[len]))
    {
        line[len--] = '\0';
    }
}

// Read the line one character at a time, looking for the CR
// You dont want to read too far, or you will mess up the content
char * GetLine(int fds)
{
    char tline[MAX_MSG_SZ];
    char *line;
    
    int messagesize = 0;
    int amtread = 0;
    while((amtread = read(fds, tline + messagesize, 1)) < MAX_MSG_SZ)
    {
        if (amtread >= 0)
            messagesize += amtread;
        else
        {
            perror("Socket Error is:");
            fprintf(stderr, "Read Failed on file descriptor %d messagesize = %d\n", fds, messagesize);
            exit(2);
        }
        //fprintf(stderr,"%d[%c]", messagesize,message[messagesize-1]);
        if (tline[messagesize - 1] == '\n')
            break;
    }
    tline[messagesize] = '\0';
    chomp(tline);
    line = (char *)malloc((strlen(tline) + 1) * sizeof(char));
    strcpy(line, tline);
    //fprintf(stderr, "GetLine: [%s]\n", line);
    return line;
}
    
// Change to upper case and replace with underlines for CGI scripts
void UpcaseAndReplaceDashWithUnderline(char *str)
{
    int i;
    char *s;
    
    s = str;
    for (i = 0; s[i] != ':'; i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] = 'A' + (s[i] - 'a');
        
        if (s[i] == '-')
            s[i] = '_';
    }
    
}


// When calling CGI scripts, you will have to convert header strings
// before inserting them into the environment.  This routine does most
// of the conversion
char *FormatHeader(char *str, const char *prefix)
{
    char *result = (char *)malloc(strlen(str) + strlen(prefix));
    char* value = strchr(str,':') + 1;
    UpcaseAndReplaceDashWithUnderline(str);
    *(strchr(str,':')) = '\0';
    sprintf(result, "%s%s=%s", prefix, str, value);
    return result;
}

// Get the header lines from a socket
//   envformat = true when getting a request from a web client
//   envformat = false when getting lines from a CGI program

void GetHeaderLines(vector<char *> &headerLines, int skt, bool envformat)
{
    // Read the headers, look for specific ones that may change our responseCode
    char *line;
    char *tline;
    
    tline = GetLine(skt);
    while(strlen(tline) != 0)
    {
        if (strstr(tline, "Content-Length") || 
            strstr(tline, "Content-Type"))
        {
            if (envformat)
                line = FormatHeader(tline, "");
            else
                line = strdup(tline);
        }
        else
        {
            if (envformat)
                line = FormatHeader(tline, "HTTP_");
            else
            {
                line = (char *)malloc((strlen(tline) + 10) * sizeof(char));
                sprintf(line, "HTTP_%s", tline);                
            }
        }
        //fprintf(stderr, "Header --> [%s]\n", line);
        
        headerLines.push_back(line);
        free(tline);
        tline = GetLine(skt);
    }
    free(tline);
}



//===================================MAIN ============================//


int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    
    int cflag=FALSE;
    int num_download = 1;
    int dflag = FALSE;
    int c = 0;
    int argCounter=1;
    char url[500]= "";
   
   vector<char *> headerLines;
  char buffer[MAX_MSG_SZ];
  char contentType[MAX_MSG_SZ];    	

    if(argc < 4)
      {
        printf("\nUsage: client <options> host-name host-port url\n");
        return 0;
      }
    else
      {
        while ((c = getopt (argc, argv, "c:d")) != -1)
            switch (c)
              {
              case 'c':
               // printf("c is called with value %s", optarg);
                num_download = (int) strtol(optarg, (char **)NULL, 10);
                argCounter=argCounter+2;
	        cflag=TRUE;
		break;
              case 'd':
                printf("d is called");
                dflag = TRUE;
                argCounter=argCounter+1;
		break;
              case '?':
                if (optopt == 'c')
                  fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                  fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                  fprintf (stderr,
                           "Unknown option character `\\x%x'.\n",
                           optopt);
                return 1;
              default:
                abort ();
              }
        if (dflag) printf("Num Download: %i\n", num_download);
        if(dflag == TRUE)
            printf("DFlag set\n");

	//printf ("arg 0: %s\n", argv[0]);
	 //printf ("arg 1: %s\n", argv[1]);
	 //printf ("arg 2: %s\n", argv[2]);
 	 //printf ("arg 3: %s\n", argv[3]);
// ITERATING THROUGH NUMBER OF DOWNLOADS
	int counter =1;

	
	
         strcpy(strHostName,argv[argCounter]);
        if (dflag) printf ("host name: %s\n", strHostName);
         nHostPort=atoi(argv[argCounter+1]);
        if (dflag) printf ("port: %i\n", nHostPort);
	 //url = (char*)calloc((sizeof argv[argCounter+2]) + 1, sizeof(char));
	 
	  //url[sizeof argv[argCounter+2]];
	 strcpy(url,argv[argCounter+2]);
	if (dflag) printf ("URL: %s\n", url);
//      }


for(int i=0;i< num_download ;i++)
{
	if(dflag) printf("Num Download: %i\n", num_download);


   if (dflag) printf("\nMaking a socket");
    /* make a socket */
    hSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
       if(pHostInfo==NULL) {
	printf("\ninvalid host name! try again\n");
	return 0;
	}
 //printf("\nCould notasdfasdfadsfa\n");
/* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    if (dflag) printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    /* connect to host */
    if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n Make sure the Port/File Path is Valid\n");
        return 0;
    }
# define MAXMSG 1024
    char *message = (char *)malloc(MAXMSG);
    sprintf(message, "GET %s HTTP/1.1\r\nHOST:%s:%i\r\n\r\n",url,strHostName,nHostPort);
    if (dflag) printf("\nMessage:\n%s\n",message);
    write(hSocket,message,strlen(message));
    memset(pBuffer, 0, BUFFER_SIZE);

    char *startline = GetLine(hSocket);
   if(dflag)  printf("Start line %s\n\n",startline); 


   // Read the header lines
    GetHeaderLines(headerLines, hSocket , false);

  
  // Now print them out
if(dflag){
  for (int i = 0; i < headerLines.size(); i++) {
    printf("[%d] %s\n",i,headerLines[i]);
    if(strstr(headerLines[i], "Content-Type")) {
             sscanf(headerLines[i], "Content-Type: %s", contentType);
    }
  }

  printf("\n=======================\n");
  printf("Headers are finished, now read the file\n");
  printf("Content Type is %s\n",contentType);
  printf("=======================\n\n");
 }


     if(!cflag)	{	
       int rval;
      while((rval = read(hSocket,buffer,MAX_MSG_SZ)) > 0) {
           write(1,buffer,rval);
          }
	}



    if(close(hSocket) == SOCKET_ERROR)
    {
	printf("\nCould not close socket\n");
    }
    free(message);
if(cflag) printf("\nNumber of times downloaded: %i\n", counter);
	counter++;
   }
  }//THIS IS THE END OF THE ELSE
} // END OF MAIN
//==========================UTILITY CODE ==================================
