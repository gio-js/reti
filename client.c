#include <stddef.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  // per sockaddr_in 
#include <netdb.h>  // per gethostbyname
#include <errno.h>


int createSocket(char* destination, int portNumber)
{
	struct sockaddr_in bind_addr;
	struct hostent *host;
	
	int sock, errore;

	host = gethostbyname(destination);
	if (host == NULL) 
	{
		printf("Get host by name fallito su nome: %s\n", destination);
		exit(1);
	}
	
	printf("%s = ", host->h_name);
	unsigned int i=0;
	while ( host->h_addr_list[i] != NULL) {
	  printf( "%s ", inet_ntoa( *( struct in_addr*)( host->h_addr_list[i])));
	  i++;
	}
	printf("\n");
	
	bind_addr.sin_family = PF_INET;
	bind_addr.sin_port = portNumber;
    bind_addr.sin_addr = *((struct in_addr *) (host->h_addr));
    
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) 
	{
		printf("Impossibile creare il socket\n", destination, portNumber);
		printf("Errore: %i\n", errno);
		exit(1);
	}	
	
	errore = connect(sock, &bind_addr, sizeof(bind_addr));
	if (errore == -1) 
	{
		closeSocket(sock);
		printf("Impossibile contattare la destinazione: %s:%i.\n", destination, portNumber);
		printf("Errore: %i\n", errno);
		exit(1);
	}
	
	printf("Connessione a %s:%i eseguita correttamente\n", destination, portNumber);
	
	return sock;
}

void sendMessage(int sock, char* message)
{
	if (write(sock, message, strlen(message)) < 0)
	{
		printf("Unable to send message on the specified socket\n");
		closeSocket(sock);
		exit(1);
	}
	
	printf("message sent\n");
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
		
	int destination = argv[1];
	int portNumber = strtol(argv[2], NULL, 10);
	
	int socketReference = createSocket(destination, portNumber);
	
	printf("Socket reference: %i\n", socketReference);
	
	for(int i=0; i<10; i++)
	{
		char *message = "this is a message\n";
		sendMessage(socketReference, message);
	
		printf("Spedito messaggio %i: %s\n", i, message);
	}
	
	closeSocket(socketReference);
}
