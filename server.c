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
// #include </Users/encima/Dropbox/PhD/Projects/Opencv/Opencv/ImageSetProcessor.h>

using namespace std;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

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
    int bufferSize = 4096;
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[bufferSize];
    struct sockaddr_in serv_addr, cli_addr;
    int running = true;

    int addrlen;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    // Type of socket created
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Bind socket to user specified port
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
             error("ERROR on binding");
    //Create/Open a file with the below name
    FILE *dest = fopen("received.jpg", "wb");
    int key;
    // char *buffer = (char *) malloc(sizeof(char) * bufferSize);
    while(running) {
       // Start socket listening with max of 5 pending connections
       listen(sockfd, 5);
       //accept new connection
       clilen = sizeof(cli_addr);
       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
       if (newsockfd < 0) 
             error("ERROR on accept");
       bzero(buffer,bufferSize);
       int count = 0;
       int n;
       while(n != 0) {
            n = recv(newsockfd,buffer,bufferSize, 0);
            fwrite(buffer, 1, sizeof(buffer), dest);
            bzero(buffer, bufferSize);
            if (n < 0) error("ERROR reading from socket");
            printf("%d\n", n);
            if (n < 0) error("ERROR reading from socket");
            count++;
        }
       // n = write(newsockfd,"I got your message",18);
       // if (n < 0) error("ERROR writing to socket");
       printf("%d\n", count);
       running = false;
    }
    printf("%s\n", "File received, cleaning up and closing stuff");
    fclose(dest);
    close(newsockfd);
    close(sockfd);
    return 0; 
}



