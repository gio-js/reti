/**
   Esempio di select in C
*/
  
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
  
#define TRUE   1
#define FALSE  0
#define PORT 8888
 
int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] , max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
      
    char buffer[1025];  //data buffer di un 1K
      
    //set dei descrittori della socket
    fd_set readfds;
      
    //Messaggio
    char *message = "ECHO Server v1.0 \r\n";
  
    //inizializzo tutti i client a 0
    for (i = 0; i < max_clients; i++) 
    {
        client_socket[i] = 0;
    }
      
    //Creo il master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
  
    //Definisco che il master socket possa utilizzare connessioni multiple (funziona anche senza)
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
  
    //tipologia di socket che creo
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    //bind sulla porta 8888 locale
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);
     
    // Definisco che il master socket accetta al massimo 3 connessioni
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
      
    //mi metto in attesa
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
     
    while(TRUE) 
    {
        //pulisco il socket set
        FD_ZERO(&readfds);
  
        //aggiungo il master socket al set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
         
        //aggiungo i socket figli al set
        for ( i = 0 ; i < max_clients ; i++) 
        {
            //descrittore del socket
            sd = client_socket[i];
             
            //Se il descrittore è valido lo aggiungo
            if(sd > 0)
                FD_SET( sd , &readfds);
             
            //guardo se il descrittore del file è il maggiore(mi serve)
            if(sd > max_sd)
                max_sd = sd;
        }
  
        //aspetto che succeda qualcosa su uno dei socket
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR)) 
        {
            printf("select error");
        }
          
        //Se succede qualcosa sul master socket allora è una connessione da accettare
        if (FD_ISSET(master_socket, &readfds)) 
        {
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //Dico all'utente il numero del socket
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            //Mando un messaggio di benvenuto
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
              
            puts("Messaggio di benvenuto spedito con successo");
              
            //aggiungo un nuovo socket all'array dei socket
            for (i = 0; i < max_clients; i++) 
            {
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Aggiungo alla lista dei socket come %d\n" , i);
                     
                    break;
                }
            }
        }
          
        //Altrimenti c'è qualche altra operazione di I/O su un altro socket
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Controlla se si chiude e legge il messaggio
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Qualcuno si è disconnesso, stampo il messaggio con le informazioni
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnesso , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //Chiudo il socket e lo metto a 0 per poterlo riutilizzare
                    close( sd );
                    client_socket[i] = 0;
                }
                  
                //Faccio l'ECHO del messaggio in ingresso
                else
                {
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }
    }
      
    return 0;
} 