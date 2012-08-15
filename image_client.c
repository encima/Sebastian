#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdio.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char*argv[]) {
	FILE *fp;
	long fileSize;
	char* buffer;
	int bufferSize = 4096;

// Open supplied file in a read binary mode, check arg is a JPEG first
	fp = fopen(argv[3], "rb");
	
// get file size and rewind back to the start of the file
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);

	printf("%ld\n", fileSize);

	// Open port and get hostname from args
	int portno = atoi(argv[2]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    printf("%s\n", "Opened Socket");
    struct hostent *server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // Attempt a connection after binding the port to the socket
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
    printf("%s\n", "Sending file");
	buffer = (char *) malloc(sizeof(char) * bufferSize);
	int count = 0;
    int i;
    // As it says on the tin, read the file into the buffer and send, send, send!
    while(i < fileSize) {
    	fread(buffer, bufferSize, 1, fp);
	    send(sockfd, buffer, bufferSize, 0);
	    count++;
	    i+=bufferSize;
    }
    // Printouts to satisfy my curiousity.
    printf("%d\n", i);
    printf("%s\n", "File sent, cleaning up");
    printf("%d\n", count);
    close(sockfd);
    return 0;

}