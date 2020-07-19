#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

/* run this program using the console pauser or add your own getch, system("pause") or input loop */


int main(int argc, char** argv) {
	
	// decls
	int server;
	
	server = socket(AF_INET, SOCK_STREAM, 0);
	listen(server, 10);
	
	while(true)
	{
		len = sizeof(caddr);
		client = accept(server, (struct sockaddr *) &caddr, sizeof(caddr));
		op = recv(client, operazione, sizeof(operazione), 0);
		if (op == "acquista") {
			recv(client, evento, len_evento,0);
			recv(client, posto, len_posto,0);
			codice = acquista(evento,posto);
			sendto(client, codice, len_codice, 0, (struct sockaddr *) &caddr, sizeof(caddr));
		}
		elseif (op == "paga") {
			recv(client, codice, len_codice,0);
			recv(client, carta, len_carta,0);
			stato = paga(codice,carta);
			sendto(client, stato, len_stato, 0, (struct sockaddr *) &caddr, sizeof(caddr));
		}
		close(client);
	}
	close(server);

	return 0;
}
