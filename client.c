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

int main (int argc, char **argv) {
    if (argc < 2) {
        puts("Usage: ./client <server_port>");
        return 1;
    }

    // TODO: change hardcoded address
    char *server_addr_str = "0.0.0.0";
    int server_port = atoi(argv[1]);

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

	const char *menu_str = 
	"Escolha uma das opcoes:\n\n"
    "(1) listar todas as pessoas formadas em um determinado curso;\n"
	"(2) listar as habilidades dos perfis que moram em uma determinada cidade;\n"
	"(3) acrescentar um a nova experiencia em um perfil;\n"
	"(4) dado o email do perfil, retornar sua experiencia;\n"
	"(5) listar todas as informacoes de todos os perfis;\n"
	"(6) dado o email d e um perfil, retornar suas informacoes.\n"
	"\n\n";

	for (;;) {
		puts(menu_str);
		int option;
        getchar();


	}

    return 0;
}
