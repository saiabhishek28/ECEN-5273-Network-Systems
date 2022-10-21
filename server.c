#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define MAXSIZE 1024000
#define KEEP_ALIVE 1
#define PIPELINE 1

int default_page = 0;

int sock, client_sock;
struct sockaddr_in server_address,client_address;
socklen_t Client = sizeof(client_address);
int count_value;
struct timeval time_val;
struct hostent *hostp;
int time_optval;

char buffer[MAXSIZE];
char buffer1[MAXSIZE];
char complete_msg[MAXSIZE];

int receivedSize = 0;
int sentSize = 0;

int GETf = 1;
int POSTf = 1;

char msg[MAXSIZE];
char p_msg[MAXSIZE];

size_t buffer_size;
int conn;
FILE *fptr;
size_t fsize;
char headerBuff[MAXSIZE];
char * formatBuff;
char * pv1;
char * pv2;
char fileFormat[20];
int buffer_1,buffer_2;
char *data_value;

char * pipeline_buffer;
char * command;
char COMMAND[MAXSIZE];
char PROTOCOL[MAXSIZE];
char * fname;
char * protocol;
char * test1;
char * post_data;

char internal_error[] =
"HTTP/1.1 500 internal server error\r\n"
"Content-Type: text/html; charset = UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<body><center><h1>ERROR 500:INTERNAL SERVER </h1><br>\r\n";


//Function that handles multiple clients
void multihreaded_connection(int conn)
{
pipeline_goto:
  bzero(buffer,MAXSIZE);
  receivedSize = read(conn, buffer,MAXSIZE);

  if(receivedSize <= 0)
  {
    printf(" Error reading data\n");
    close(conn);
    return;
  }
  else
  {
    printf(" String received from client: \n%s\n", buffer);

    //Removing null terminals
    int j = 0;
    for(int i = 0; i<receivedSize; i++)
    {
      if(buffer[i] != '\0')
      {
        p_msg[j]=buffer[i];
        j++;
      }
    }
    p_msg[j] = '\0';

    strcpy(msg, buffer);

    command = strtok(buffer," \n");
    fname = strtok(NULL," \n");
    protocol = strtok(NULL," \n");

    strcpy(COMMAND, command);
    strcpy(PROTOCOL, protocol);

    printf(" Command is %s\n",command);
    if(strncmp(command, "GET", 3) == 0)
    {
      printf(" We are in post \n");
      GETf = 1;
      POSTf = 0;
    }
    else if(strncmp(command, "POST", 4) == 0)
    {
      printf(" We are in post \n");
      GETf = 0;
      POSTf = 1;
    }

    printf(" Request is %s\n",fname);

    printf(" Protocol is %s\n",protocol);
  }

  //Default page
  if(strncmp(command,"GET",3)==0 && strcmp(fname, "/") == 0)
  {
    printf(" Default page \n");

    char index_page[MAXSIZE] = "/home/xubuntu/Desktop/ECEN-5273-Network-Systems/www/index.html";

    fptr =fopen(index_page,"r");

    if (fptr==NULL)
    {
      printf(" ERROR: Invalid File Request Received\n");
      buffer_2 = send(conn,internal_error, strlen(internal_error), 0);
      close(conn);
      return;
    }
    else
    {
      printf(" File found\n");
    }
    fseek (fptr , 0 , SEEK_END);
    fsize = ftell (fptr);
    fseek (fptr , 0 , SEEK_SET);

    sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","text/html",fsize);

    sentSize =write(conn, headerBuff, strlen(headerBuff));

    if (sentSize<=0)
    {
      printf(" Header buffer not Sent\n");
      close(conn);
      return;
    }
    else
    {
      printf(" Header buffer Sent\n");
    }
    bzero(headerBuff,sizeof(headerBuff));
    buffer_size = fread(buffer,1,MAXSIZE,fptr);
    buffer_1=write(conn, buffer, buffer_size);
    bzero(buffer,sizeof(buffer));
    fclose(fptr);
    close(conn);
  }

  //Checking commands
  if(strncmp(COMMAND,"GET",3)==0)
  {
    printf(" Command is GET \n");
  }
  else
  {
    printf(" Error: Invalid Command\n");
    buffer_2 = send(conn,internal_error, strlen(internal_error), 0);
    close(conn);
    return;
  }

  //checking protocols
  printf("Protocol is %s\n", protocol);
  if((strncmp(PROTOCOL,"HTTP/1.1",8)==0) || (strncmp(PROTOCOL,"HTTP/1.0",8)==0))
  {
    printf(" Proper HTTP protocol \n");
  }
  else
  {
    printf(" Error: Invalid Protocol \n");
    buffer_2 = send(conn,internal_error, strlen(internal_error), 0);
    close(conn);
    return;
  }
  if(strncmp(command,"GET",3)==0)
  {
    printf(" In get \n");

    char dir[MAXSIZE] = "/home/xubuntu/Desktop/ECEN-5273-Network-Systems/www";

    strcat(dir,fname);
    printf(" File-path is %s\n",dir);

    printf(" This is you should read : %s\n", fname);

    fptr =fopen(dir,"r");

    if (fptr==NULL)
    {
      printf(" ERROR: Invalid File Request Recieved\n");
      buffer_2 = send(conn,internal_error, strlen(internal_error), 0);
      close(conn);
      return;
    }
    else
    {
      printf(" File found\n");
    }
    fseek (fptr , 0 , SEEK_END);
    fsize = ftell (fptr);
    fseek (fptr , 0 , SEEK_SET);

    formatBuff = strrchr(fname,'.');
    strcpy(fileFormat,formatBuff);

    if (strcmp(fileFormat,".html")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","text/html",fsize);
    }
    else if(strcmp(fileFormat,".txt")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","text/plain",fsize);
    }
    else if(strcmp(fileFormat,".png")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","image/png",fsize);
    }
    else if(strcmp(fileFormat,".gif")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","image/gif",fsize);
    }
    else if(strcmp(fileFormat,".jpg")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","image/jpg",fsize);
    }
    else if(strcmp(fileFormat,".css")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","text/css",fsize);
    }
    else if(strcmp(fileFormat,".js")==0)
    {
      sprintf(headerBuff,"HTTP/1.1 200 DOCUMENT FOLLOWS\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n","application/javascript",fsize);
    }
    else
    {
      printf(" Error: Invalid file fileFormat\n");
      buffer_2 = send(conn,internal_error, strlen(internal_error), 0);
      close(conn);
      return;
    }
    
    printf("Header sent is %s\n",headerBuff);
    sentSize=write(conn, headerBuff, strlen(headerBuff));
    if (sentSize<=0)
    {
      printf(" Header buffer not Sent\n");
      close(conn);
      return;
    }
    
    bzero(headerBuff,sizeof(headerBuff));
    buffer_size = fread(buffer,1,MAXSIZE,fptr);
    printf("Buffer: %s \n", buffer);
    buffer_1 = send(conn, buffer, buffer_size,0);
    printf("Number of bytes sent %d \n", buffer_1);
    bzero(buffer,sizeof(buffer));
    fclose(fptr);

    // Pipelining here
    if(PIPELINE == 1)
    {
      pipeline_buffer = strstr(p_msg,"Connection: keep-alive");
      if(pipeline_buffer != NULL)
      {
        time_val.tv_sec = 20; //timeout value
        int timeout_val = setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_val, sizeof(struct timeval));
        count_value++;
        goto pipeline_goto;
      }
      else
      {
        printf("Pipeline closed \n");
        time_val.tv_sec = 0;
        int timeout_val1 = setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&time_val, sizeof(struct timeval));
        printf("Value: is %d",timeout_val1);
        count_value = 0;
      }
    }
    close(conn);
  }
}


int main (int argc, char * argv[])
{
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(argv[1]));  
  server_address.sin_addr.s_addr = INADDR_ANY;

  //Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    printf("CAnnot create Socket \n");
  }
  else
  {
    printf("Socket Created \n");
  }

  //set timer
  time_optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&time_optval , sizeof(int));

  //bind address
  if(bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
  {
    printf("Socket bind failed\n");
  }
  else
  {
    //printf("");
  }

  //listen to incoming connections
  listen (sock, 10);

  printf("Server Ready \n");
  
  while(1)
  {
    client_sock = accept (sock, (struct sockaddr *) &client_address, &Client);

    if (client_sock < 0)
    {
      printf(" Client not accepted \n");
      exit(1);
    }
    else
    {
      // Multithreading using fork
      if(!fork())
      {
        printf("Creating new thread \n");
        multihreaded_connection(client_sock);
        exit(1);
      }
      else
      {
        close(client_sock);
        printf("Thread closed\n");
      }
    }
  }
}