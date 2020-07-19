#include <sys/types.h>
#include <sys/socket.h>
//"in" per "sockaddr_in"
#include <netinet/in.h>
//"fcntl" per la funzione "fcntl"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define BUFF_LEN 200
char buffer[BUFF_LEN];
int buffer_len = BUFF_LEN-1;     /*  allow room for terminating '\0'  */


int createSocket(int listeningPort)
{
	struct sockaddr_in bind_addr;
	int sock, code;
	
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = 0;
	bzero(bind_addr.sin_zero, 8);
	bind_addr.sin_port = listeningPort;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		printf("Impossibile creare il socket sulla porta: %d\n", listeningPort);
		printf("Errore: %i\n", errno);
		exit(1);
	}

	code = bind(sock, &bind_addr, sizeof(bind_addr));
	if (code < 0)
	{
		printf("Impossibile effettuare il binding delle interfacce di rete\n");
		printf("Errore: %i\n", errno);
		exit(1);
	}

	
	code = listen(sock, 1);
	if (code < 0)
	{
		printf("Impossibile avviare l'ascolto di nuove chiamate in ingresso\n");
		printf("Errore: %i\n", errno);
		exit(1);
	}

	return sock;
}

int acceptRequest(int sock)
{
	int sd;
	struct sockaddr bind_addr;
	int len = sizeof(bind_addr);
	sd = accept(sock, &bind_addr, &len);
	return sd;
}

void closeSocket(int sock)
{
	close(sock);
}

int main(int argc, char* argv[])
{
	printf("command line args\n");
	printf("-----------------------------\n");
	for(int k=0; k<argc; k++)
		printf("%i: %s\n", k, argv[k]);
		
	int listeningPort = strtol(argv[1], NULL, 10);
	int server_socket, client_socket, len;

	server_socket = createSocket(listeningPort);
	printf("Socket in ascolto sulla porta %d creato correttamente\n", listeningPort);

	client_socket = acceptRequest(server_socket);

	closeSocket(server_socket); // close server socket (only one client)

	for(;;)
	{
		// wait for client message
		len = read(client_socket, buffer, buffer_len);
		if (len == 0) {
			printf("Client finished the conversation\n");
			break;
		}

		// print client message
		buffer[len] = '\0';
		printf("Client message: %s\n", buffer);

		// response message
		printf("Response: \n");
		if (gets(buffer) == NULL) { // eof
			closeSocket(client_socket);
			printf("by bye\n");
			break;
		}

		write(client_socket, buffer, strlen(buffer));
	}

	exit(0);
}
