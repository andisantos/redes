#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include "common.h"

int main (int argc, char **argv) {
    if (argc < 2) {
        puts("Usage: ./client <server_ip> <server_port>");
        return 1;
    }

    char *server_addr_str = argv[1];
    int server_port = atoi(argv[2]);

    long socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        fprintf(stderr, "ERROR: Failed to create TCP socket.\n");
        return 2;
    }

    struct hostent *server = gethostbyname(server_addr_str);
    if (!server) {
        fprintf(stderr, "ERROR: Failed to resolve server address.\n");
        return 2;
    }

    struct sockaddr_in server_address = {0};
    memcpy(&server_address.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);

    int err = connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address));
    if (err < 0) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        return 2;
    }
    puts("Connected to server.");

	const char *menu_str = 
	"\n\n\nEscolha uma das opcoes:\n\n"
    "(1) listar todas as pessoas formadas em um determinado curso;\n"
	"(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n"
	"(3) acrescentar um a nova experiencia em um perfil;\n"
	"(4) dado o email do perfil, retornar sua experiencia;\n"
	"(5) listar todas as informacoes de todos os perfis;\n"
	"(6) dado o email de um perfil, retornar suas informacoes.\n"
	"\n\n";

	for (;;) {
		puts(menu_str);
        char c;
        scanf(" %c", &c);

        int option = OPTION_1 + (c - '1');
        
        switch (option) {
            case OPTION_1:
                {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Digite o nome do curso: ");
                    char course[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %s", course);
                    //fgets(course, sizeof(course), stdin);
                    //printf("\nRead: %s\n", course);

                    char send_buffer[101] = { option }; 
                    strcpy(send_buffer+1, course);
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 
                    char recv_buffer[1000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);
                }
                break;
            case OPTION_2:
                {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Digite o nome da cidade: ");
                    char city[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %[^\n]", city);
                    getchar();

                    char send_buffer[101] = { option }; 
                    strcpy(send_buffer+1, city);
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 
                    char recv_buffer[1000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);
                }
                break;
            case OPTION_3:
                {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Digite o email do perfil: ");
                    char email[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %[^\n]", email);
                    getchar();

                    printf("Digite o nome da experiencia que deseja adicionar: ");
                    char experience[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %[^\n]", experience);
                    getchar();

                    char send_buffer[201] = { option }; 
                    strcpy(send_buffer+1, email);
                    strcpy(send_buffer+101, experience);
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 
                    char recv_buffer[1000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);
                }
                break;
            case OPTION_4:
                {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Digite o email do perfil: ");
                    char email[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %[^\n]", email);
                    getchar();

                    char send_buffer[101] = { option }; 
                    strcpy(send_buffer+1, email);
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 
                    char recv_buffer[1000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);
                }
                break;
            case OPTION_5:
                {
                    char send_buffer[1] = { option }; 
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 

                    char recv_buffer[45000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);

					for (int k = 0;; k++) {
						int image_size;
						int err = recv(socket_fd, &image_size, sizeof(image_size), MSG_WAITALL);
						if (err < 1) continue;
						//if (image_size == 0) continue;

						if (image_size == -1) break;

						char image_file_name[40] = { 0 };
                        strcat(image_file_name, "images/");
						recv(socket_fd, image_file_name + 7, sizeof(image_file_name), MSG_WAITALL);
						strcat(image_file_name, ".jpg");

                        if (image_size > 0) {
                            char* image_buffer = malloc(image_size);
                            recv(socket_fd, image_buffer, image_size, MSG_WAITALL);

                            FILE* f = fopen(image_file_name, "wb");
                            fwrite(image_buffer, image_size, 1, f);
                            fclose(f);

                            free(image_buffer);
                        }
					}
                }
                break;
            case OPTION_6:
                {
                    int c;
                    while ((c = getchar()) != '\n' && c != EOF);

                    printf("Digite o email do perfil: ");
                    char email[100] = { 0 }; // NOTE: buffer overflow possible
                    scanf(" %[^\n]", email);
                    getchar();

                    char send_buffer[101] = { option }; 
                    strcpy(send_buffer+1, email);
                    write(socket_fd, send_buffer, sizeof(send_buffer)); 

                    char recv_buffer[1000] = { 0 };
                    recv(socket_fd, recv_buffer, sizeof(recv_buffer), MSG_WAITALL); 
                    printf("Resposta recebida do servidor:\n%s\n", recv_buffer);

					int image_size;
					recv(socket_fd, &image_size, sizeof(image_size), MSG_WAITALL);

                    if (image_size != -1) {
                        char image_file_name[40] = { 0 };
                        strcat(image_file_name, "images/");
                        recv(socket_fd, image_file_name + 7, sizeof(image_file_name), MSG_WAITALL);
                        strcat(image_file_name, ".jpg");

                        if (image_size > 0) {
                            char* image_buffer = malloc(image_size);
                            recv(socket_fd, image_buffer, image_size, MSG_WAITALL);

                            FILE* f = fopen(image_file_name, "wb");
                            fwrite(image_buffer, image_size, 1, f);
                            fclose(f);

                            free(image_buffer);
                        }
                    }
                }
                break;
        }
	}

    return 0;
}
