#include <stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <signal.h>
#include <unistd.h>
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 
  
// Function designed for chat between client and server. 
void func(int sockfd) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
    for (;;) { 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
        read(sockfd, buff, sizeof(buff)); 
        // print buffer which contains the client contents 
        printf("From client: %s\t To client : ", buff); 
        bzero(buff, MAX); 
        n = 0; 
        // copy server message in the buffer 
        while ((buff[n++] = getchar()) != '\n') 
            ; 
  
        // and send that buffer to client 
        write(sockfd, buff, sizeof(buff)); 
  
        // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            break; 
        } 
    } 
} 
  
int main() 
{ 
    int sockfd, connfd; 
    struct sockaddr_in servaddr = {0};
  
    // create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        perror("Failed to create socket"); 
        exit(0); 
    }
  
    // fill IP and PORT appropriately
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // bind socket to ip and port 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        perror("Failed to bind socket"); 
        exit(0); 
    } 
  
    // listen for incoming connections
    if ((listen(sockfd, 5)) != 0) { 
        perror("listen() failed"); 
        exit(0); 
    } 
    puts("Listening...");


    for (;;) {
        // accept new client connection
        struct sockaddr client_addr  = {0};
        int client_addr_len = sizeof(client_addr);
        connfd = accept(sockfd, &client_addr, &client_addr_len); 
        if (connfd < 0) { 
            printf("server acccept failed...\n"); 
        } else {
            int pid = fork();
            if (pid == -1)
            {
                close(connfd);
                continue;
            }
            if (pid == 0) { // child
                // TODO: get request
                // TODO: answer request
                // TODO: close socket

                // TODO: kill child process gracefully
                kill(pid, SIGKILL);
            }
        } 
    }

    puts("Stopping server...");
    close(sockfd);
} 

