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
#include <sys/time.h>
#include <poll.h>
#include <assert.h>
#include "common.h"

int main (int argc, char **argv) {
    if (argc < 2) {
        puts("Usage: ./client <server_ip> <server_port>");
        return 1;
    }

    char *server_addr_str = argv[1];
    int server_port = atoi(argv[2]);

    long socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
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

	const char *menu_str = 
	"\n\n\nEscolha uma das opcoes:\n\n"
    "(1) dado o email, retornar nome, sobrenome e foto da pessoa;\n"
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

                    Operation1RequestPacket request = {0};

                    printf("Digite o email: ");
                    scanf(" %s", request.email);

                    request.option = option;

                    START_TIMER;

                    sendto(socket_fd, (void *) &request, sizeof(request), 0,
                           (const struct sockaddr *) &server_address, sizeof(server_address));

                    Operation1ResponsePacket response = {0};
                    struct sockaddr serv_addr  = {0};
                    int serv_addr_len = sizeof(serv_addr);

                    struct pollfd sockets_to_poll[1] = { 0 };
                    sockets_to_poll[0].fd = socket_fd;
                    sockets_to_poll[0].events = POLLIN;

                    int err = poll(sockets_to_poll, sizeof(sockets_to_poll)/sizeof(sockets_to_poll[0]), 2000);
                    assert(err >= 0);
                    if (err == 0) { // timeout
                        fprintf(stderr, "Timed out.\n");
                    } else { // bom out
                        int n = recvfrom(socket_fd, (char *) &response, sizeof(response), MSG_WAITALL,
                                         (struct sockaddr *) &serv_addr, &serv_addr_len);

                        printf("Resposta recebida do servidor:\nName: %s\nSurName: %s\n",
                                response.name, response.surname);
                    }

                    END_TIMER;

                }
                break;
        }
	}

    return 0;
}
