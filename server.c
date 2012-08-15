/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <termios.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include </Users/encima/Dropbox/Projects/PhD/C++/ImageSetProcessor/ImageSetProcessor.h>

using namespace std;

int runServer(int port, int bufferSize);
void receiveFile(int sockfd, int bufferSize);

int sockfd, newsockfd, portno, pid;
socklen_t clilen;

struct sockaddr_in serv_addr, cli_addr;
int running = true;
int addrlen;
int fileCount = 0;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//Used to listen for interrupts from the keyboard (Currently unused)
int my_getch()
{
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
     
    termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
 
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
 
    unsigned char c;
    ssize_t nread = read(STDIN_FILENO, &c, 1);
     
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
 
    return c;
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    int bufferSize = 4096;
    int port = atoi(argv[1]);
    
    runServer(port, bufferSize);

    close(sockfd);
    return 0; 
}

int runServer(int portno, int bufferSize) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Type of socket created
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Bind socket to user specified port
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
             error("ERROR on binding");
    //Create/Open a file with the below name
    int key;
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      if (newsockfd < 0) 
        error("ERROR on accept");
      fileCount++;
      pid = fork();
      if (pid < 0)
        error("ERROR on fork");
      if (pid == 0)  {
        close(sockfd);
        printf("%s\n", "Forking receive file");
        receiveFile(newsockfd, bufferSize);
        exit(0);
      }else {
        close(newsockfd);
      }
     }

    close(newsockfd);

    return 1;
}

void receiveFile(int sockfd, int bufferSize) {
  char *fileName = (char *) malloc(sizeof(char));
  sprintf(fileName, "images/received%d.jpg", fileCount);
  FILE *dest = fopen(fileName, "wb");
  char buffer[bufferSize];
  bzero(buffer,bufferSize);
  int count = 0;
  int n;
  while(n != 0) {
    n = recv(newsockfd,buffer,bufferSize, 0);
    fwrite(buffer, 1, sizeof(buffer), dest);
    bzero(buffer, bufferSize);
    if (n < 0) error("ERROR reading from socket");
    if (n < 0) error("ERROR reading from socket");
    count++;
  }
  fclose(dest);
}



