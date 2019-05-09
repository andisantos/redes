#include <stdio.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <signal.h>
#include <unistd.h>
#include <sqlite3.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include "common.h"

#define MAX_BLOB_RESULTS_SIZE 40000000

#define ERROR_MSG "\nERRO: Nenhum cadastro correspondente encontrado.\n"

#define PORT 8080 
#define SA struct sockaddr 

sqlite3* db;
int count = 0;
char blob_results_str[MAX_BLOB_RESULTS_SIZE] = { 0 };
char *blob_results_ptr = &blob_results_str[0];

int callback_option_1(void *uncast_data, int argc, char **argv, char **azColName) {
    Operation1ResponsePacket *response = uncast_data;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(azColName[i], "Name")) {
            strcpy(response->name, argv[i]);
        }
        else if (!strcmp(azColName[i], "Surname")) {
            strcpy(response->surname, argv[i]);
        }
        else {
            fprintf(stderr, "Bad column: %s\n", azColName[i]);
            assert(false);
        }
        count++;
    }

    return 0;
}

// TODO: Photo
void do_option_1(Operation1ResponsePacket* response, Operation1RequestPacket request) {
    char sql[500] = { 0 };
    sprintf(sql, "select Name, Surname from Profile where Email = \"%s\";", request.email);

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_1, (void *) response, &err_msg);

    // check if client asked for someone who doesn't exist or something like that
    if (count == 0) {
        strcpy(response->errmsg, ERROR_MSG);
        return;
    }

    // check for sql errors
    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 

    // TODO
#if 0
	sprintf(sql, "SELECT Photo FROM Profile WHERE Email = \"%s\"", email);
	sqlite3_stmt *pStmt;
	rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, 0);

	if (rc != SQLITE_OK ) {
		fprintf(stderr, "Failed to prepare statement\n");
		fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));

		sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
		exit(1);
	} 

	rc = sqlite3_step(pStmt);

	int bytes = 0;

	if (rc == SQLITE_ROW) {
		bytes = sqlite3_column_bytes(pStmt, 0);
	}

	//if (bytes > 0) {
		memcpy(blob_results_ptr, &bytes, sizeof(bytes));
		blob_results_ptr += sizeof(bytes);

		memcpy(blob_results_ptr, sqlite3_column_blob(pStmt, 0), bytes);
		blob_results_ptr += bytes;
	//}
#endif
}

int main() 
{ 
    // database related initialization

    int opendb = sqlite3_open("lab1_redes.db", &db);
    
    if(opendb != SQLITE_OK){
        printf("Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }


    // network related initialization

    int sockfd; 
    struct sockaddr_in servaddr = {0};
  
    // create TCP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (sockfd == -1) { 
        perror("Failed to create socket"); 
        exit(1); 
    }
  
    // fill IP and PORT appropriately
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 
  
    // bind socket to ip and port 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        perror("Failed to bind socket"); 
        exit(1); 
    } 
  
    puts("Server running...");

    for (;;) {
        struct sockaddr client_addr  = {0};
        int client_addr_len = sizeof(client_addr);

        for (;;) {
            Operation1RequestPacket request = {0};
            int n = recvfrom(sockfd, (char *) &request, sizeof(request), MSG_WAITALL,
                             &client_addr, &client_addr_len);
            if (n > 0) {
                printf("Received message from client: Option %d\n", request.option - OPTION_1+1);

                assert(n == sizeof(request));
                int option = request.option;
                switch (option) {
                    case OPTION_1:
                        {
                            puts("");

                            START_TIMER;

                            Operation1ResponsePacket response = {0};
                            blob_results_ptr = &blob_results_str[0];
                            do_option_1(&response, request);

                            // DEBUG
                            puts("Data to be sent:");
                            puts(response.name);
                            puts(response.surname);

                            END_TIMER;

                            sendto(sockfd, (void *) &response, sizeof(response), 0,
                                   &client_addr, client_addr_len);
                        }
                        break;
                }
            } else {
                printf("Error code: %d\n", n);
                perror("");
            }
        }
    }

    puts("Stopping server...");
    close(sockfd);

    sqlite3_close(db);

    return 0;
} 
