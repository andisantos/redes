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
    char *data = uncast_data;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(azColName[i], "Name")) {
            if (count != 0) {
                strcat(data, ", ");
            }
            strcat(data, argv[i]);
            count++;
        }
    }

    return 0;
}

// TODO: make the course not hardcoded
void do_option_1(char* results_str, char* course) {
    char sql[500] = { 0 };
    sprintf(sql, "select * from Profile where Academic == \"%s\"", course);

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_1, results_str, &err_msg);

    if (count == 0) {
        strcpy(results_str, ERROR_MSG);
        return;
    }

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
}

int callback_option_2(void *uncast_data, int argc, char **argv, char **azColName) {
    char *data = uncast_data;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(azColName[i], "Ability")) {
            if (count != 0) {
                strcat(data, ", ");
            }
            strcat(data, argv[i]);
            count++;
        }
    }

    return 0;
}

void do_option_2(char* results_str, char *city) {
    char sql[500] = { 0 };
    sprintf(sql, "select * from Profile p inner join Abilities a on p.Id = a.Id where Addr = \"%s\";", city);

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_2, results_str, &err_msg);

    if (count == 0) {
        strcpy(results_str, ERROR_MSG);
        return;
    }

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
}

int callback_option_4(void *uncast_data, int argc, char **argv, char **azColName) {
    char *data = uncast_data;
    for (int i = 0; i < argc; i++) {
        if (!strcmp(azColName[i], "Experience")) {
            if (count != 0) {
                strcat(data, ", ");
            }
            strcat(data, argv[i]);
            count++;
        }
    }

    return 0;
}

void do_option_3(char* results_str, char *email, char *experience) {
    {
        char sql[500] = { 0 };
        sprintf(sql, "select Experience from Experiences where Id = (select Id from Profile where Email = \"%s\");", email);

        count = 0;
        int rc = sqlite3_exec(db, sql, callback_option_4, results_str, NULL);
        results_str[0] = 0;

        if (count == 0) {
            strcpy(results_str, ERROR_MSG);
            return;
        }
    }


    char sql[500] = { 0 };
    sprintf(sql, "insert into Experiences values ((select Id from Profile where Email = \"%s\"), \"%s\");", email, experience);

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, NULL, results_str, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
}


void do_option_4(char* results_str, char *email) {
    char sql[500] = { 0 };
    sprintf(sql, "select Experience from Experiences where Id = (select Id from Profile where Email = \"%s\");", email);

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_4, results_str, &err_msg);

    if (count == 0) {
        strcpy(results_str, ERROR_MSG);
        return;
    }

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
}

int callback_option_6(void *uncast_data, int argc, char **argv, char **azColName) {
    char *data = uncast_data;

    for (int i = 0; i < argc; i++) {
        if (!strcmp(azColName[i], "Photo")) {
            continue;
        } else if (!strcmp(azColName[i], "Ability") || !strcmp(azColName[i], "Experience")) {
            if (count != 0) {
                strcat(data, ", ");
            }
            strcat(data, argv[i]);
            count++;
        } else {
            strcat(data, azColName[i]);
            strcat(data, ": ");
            strcat(data, argv[i]);
            strcat(data, "\n");
            count++;
        }
    }

    return 0;
}

void do_option_6(char* results_str, char *email) {
    char sql[500] = { 0 };
    // TODO: Photo
    sprintf(sql, "select Email, Name, SurName, Addr, Academic from Profile where Email = \"%s\";", email);

    *results_str = '\n';

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_6, results_str, &err_msg);

    // this should never happen if called via do_option_5
    if (count == 0) {
        strcpy(results_str, ERROR_MSG);
        return;
    }

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
	strcat(results_str, "Photo file: ");
	strcat(results_str, email);
	strcat(results_str, ".jpg\n");


    sprintf(sql, "select Experience from Experiences where Id = (select Id from Profile where Email = \"%s\");", email);

    count = 0;
    strcat(results_str, "Experiences: ");
    rc = sqlite3_exec(db, sql, callback_option_6, results_str, &err_msg);
    strcat(results_str, "\n");

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 


    sprintf(sql, "select Ability from Abilities where Id = (select Id from Profile where Email = \"%s\");", email);

    count = 0;
    strcat(results_str, "Abilities: ");
    rc = sqlite3_exec(db, sql, callback_option_6, results_str, &err_msg);
    strcat(results_str, "\n");

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 


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

    strcat(results_str, "\n");

	//if (bytes > 0) {
		memcpy(blob_results_ptr, &bytes, sizeof(bytes));
		blob_results_ptr += sizeof(bytes);

		char filename[40] = {0};
		strcpy(filename, email);
		memcpy(blob_results_ptr, filename, sizeof(filename));
		blob_results_ptr += sizeof(filename);

		memcpy(blob_results_ptr, sqlite3_column_blob(pStmt, 0), bytes);
		blob_results_ptr += bytes;
	//}
}

int callback_option_5(void *uncast_data, int argc, char **argv, char **azColName) {
    char *data = uncast_data;

    for (int i = 0; i < argc; i++) {
        assert(!strcmp(azColName[i], "Email"));

        do_option_6(data, argv[i]);
    }

    return 0;
}


char* do_option_5(char* results_str) {
    char sql[500] = { 0 };
    // TODO: Photo
    sprintf(sql, "select Email from Profile;");

    *results_str = '\n';

    char *err_msg = 0;
    count = 0;
    int rc = sqlite3_exec(db, sql, callback_option_5, results_str, &err_msg);

    if (rc != SQLITE_OK ) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        fprintf(stderr, "Errors happened! Killed process with pid %d.\n", getpid());
        exit(1);
    } 
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

    int sockfd, connfd; 
    struct sockaddr_in servaddr = {0};
  
    // create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
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
  
    // listen for incoming connections
    if ((listen(sockfd, 5)) != 0) { 
        perror("listen() failed"); 
        exit(1); 
    } 
    puts("Listening...");

    for (;;) {
        // accept new client connection
        struct sockaddr client_addr  = {0};
        int client_addr_len = sizeof(client_addr);
        connfd = accept(sockfd, &client_addr, &client_addr_len); 
        if (connfd < 0) { 
            printf("Server accept failed.\n"); 
        } else {
            int pid = fork();
            if (pid == 0) { // child process
                for (;;) {
                    char recv_buffer[1] = {0};
                    int n = recv(connfd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    if (n == 0) {
                        printf("Killing pid %d\n", getpid());
                        shutdown(connfd, SHUT_WR);
                        close(connfd);
                        exit(0);
                    } else {
                        printf("Received message from client: Option %d\n", recv_buffer[0] - OPTION_1 + 1);

                        assert(n == 1);
                        int option = recv_buffer[0];
                        switch (option) {
                            case OPTION_1:
                                {
                                    puts("");
                                    char param_buffer[100] = {0};
                                    int n = recv(connfd, param_buffer, sizeof(param_buffer), MSG_WAITALL); 

                                    START_TIMER;

                                    char results_str[1000] = { 0 };
                                    do_option_1(results_str, param_buffer);

                                    END_TIMER;

                                    write(connfd, results_str, sizeof(results_str)); 
                                }
                                break;
                            case OPTION_2:
                                {
                                    puts("");
                                    char param_buffer[100] = {0};
                                    int n = recv(connfd, param_buffer, sizeof(param_buffer), MSG_WAITALL); 

                                    START_TIMER

                                    char results_str[1000] = { 0 };
                                    do_option_2(results_str, param_buffer);

                                    END_TIMER

                                    write(connfd, results_str, sizeof(results_str)); 
                                }
                                break;
                            case OPTION_3:
                                {
                                    puts("");
                                    char param_buffer1[100] = {0};
                                    recv(connfd, param_buffer1, sizeof(param_buffer1), MSG_WAITALL); 
                                    char param_buffer2[100] = {0};
                                    recv(connfd, param_buffer2, sizeof(param_buffer2), MSG_WAITALL); 

                                    START_TIMER

                                    char results_str[1000] = { 0 };
                                    do_option_3(results_str, param_buffer1, param_buffer2);

                                    if (!strlen(results_str)) {
                                        strcpy(results_str, "Experiencia adicionada com sucesso!");
                                    }

                                    END_TIMER

                                    write(connfd, results_str, sizeof(results_str)); 
                                }
                                break;
                            case OPTION_4:
                                {
                                    puts("");
                                    char param_buffer[100] = {0};
                                    int n = recv(connfd, param_buffer, sizeof(param_buffer), MSG_WAITALL); 

                                    START_TIMER

                                    char results_str[1000] = { 0 };
                                    do_option_4(results_str, param_buffer);

                                    END_TIMER

                                    write(connfd, results_str, sizeof(results_str)); 
                                }
                                break;
                            case OPTION_5:
                                {
                                    puts("");

                                    START_TIMER

                                    char results_str[45000] = { 0 };
									blob_results_ptr = &blob_results_str[0];
                                    do_option_5(results_str);

									memset(blob_results_ptr, -1, sizeof(int));
									blob_results_ptr += sizeof(int);

                                    END_TIMER

                                    write(connfd, results_str, sizeof(results_str)); 
                                    write(connfd, blob_results_str, blob_results_ptr - &blob_results_str[0]); 
                                }
                                break;
                            case OPTION_6:
                                {
                                    puts("");
                                    char param_buffer[100] = {0};
                                    int n = recv(connfd, param_buffer, sizeof(param_buffer), MSG_WAITALL); 

                                    START_TIMER

                                    char results_str[1000] = { 0 };
									blob_results_ptr = &blob_results_str[0];
                                    do_option_6(results_str, param_buffer);

                                    if (blob_results_ptr == &blob_results_str[0]) {
                                        memset(blob_results_ptr, -1, sizeof(int));
                                        blob_results_ptr += sizeof(int);
                                    }

                                    END_TIMER

                                    write(connfd, results_str, sizeof(results_str)); 
                                    write(connfd, blob_results_str, blob_results_ptr - &blob_results_str[0]); 
                                }
                                break;
                        }
                    }
                }
            } else { // either an error happened or this is the parent process
                close(connfd);
            }
        } 
    }

    puts("Stopping server...");
    close(sockfd);

    sqlite3_close(db);

    return 0;
} 

