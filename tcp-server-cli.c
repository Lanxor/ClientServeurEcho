/******************************************************************************
 *
 * Name File : tcp-server-cli.c
 * Authors   : OLIVIER Thomas & ROBERT DE ST VINCENT Guillaume
 * Location  : UPSSITECH - University Paul Sabatier
 * Date      : October 2018
 *
 *                        This work is licensed under a 
 *              Creative Commons Attribution 4.0 International License.
 *                                    (CC BY)
 *
 *****************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_SIZE 80
#define SIZE_WATING_LIST 5

/******************************************************************************
 * Fonction qui récupère les informations du serveur en mode datagramme.
 * Prend en paramètre :
 *     - serverName    Pointeur vers une chaine de caractère pour le nom ou
 *                       l'adresse IP du serveur.
 *     - serverPort    Pointeur vers une chaine de caractère pour le numéro de
 *                       port du serveur.
 * Renvoie la structure d'information addrinfo.
 *****************************************************************************/
struct addrinfo get_info(char *serverPort) {
  int status;
  struct addrinfo hints;
  struct addrinfo *servInfo;

  /* Configuration des paramètre du socket pour le serveur */
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;		/* IPv4 et IPv6 ou autre protocole */
  hints.ai_socktype = SOCK_STREAM;	/* socket TCP */
  hints.ai_flags = AI_PASSIVE;		/* écoute sur toute les interfaces */
  hints.ai_protocol = 0;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  /* Ouverture et récupération des informations de connexion au serveur */
  status = getaddrinfo(NULL, serverPort, &hints, &servInfo);
  if ( status != 0 ) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(EXIT_FAILURE);
  }

  return *servInfo;
}

/******************************************************************************
 * Fonction qui permet d'ouvrir le socket.
 * Prend en paramètre la structure récupérée par la fonction 'get_info'.
 * Renvoie le descripteur du socket.
 *****************************************************************************/
int socket_open(struct addrinfo *servInfo) {
  int socketDescriptor;
  struct addrinfo *rp;

  /* Ouverture du socket sur le port d'écoute passé en paramètre */
  for ( rp = servInfo; rp != NULL; rp = rp->ai_next ) {
    socketDescriptor = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if ( socketDescriptor == -1 )
      continue;
    if ( bind(socketDescriptor, rp->ai_addr, rp->ai_addrlen) == 0 )
      break;
    close(socketDescriptor);
  }

  if ( rp == NULL ) {
    fprintf(stderr, "Could not bind\n");
    fprintf(stderr, "Port number is already used.\n");
    exit(EXIT_FAILURE);
  }

  if (listen(socketDescriptor, SIZE_WATING_LIST) == -1) {
    perror("Error with listen");
    exit(EXIT_FAILURE);
  }

  return socketDescriptor;
}

/******************************************************************************
 * Fonction qui ferme le socket.
 * Prend en paramètre le descripteur du socket.
 *****************************************************************************/
void socket_close(int socketDescriptor) {
  close(socketDescriptor);
}

/******************************************************************************
 * Fonction qui permet attend de recevoir un flux d'un client.
 * Prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - servInfo            Pointeur vers les informations récupérées par la
 *                             fonction 'get_info'.
 * Renvoie le numéro de flux du client connecté.
 *****************************************************************************/
int client_connect(int socketDescriptor, struct addrinfo *servInfo) {
  int streamClient;

  /* Action bloquante */
  streamClient = accept(socketDescriptor, (struct sockaddr *) &servInfo->ai_addr,
                         &servInfo->ai_addrlen);
  if ( streamClient == -1) {
    perror("Error with accept");
    close(socketDescriptor);
    exit(EXIT_FAILURE);
  }

  return streamClient;
}

/******************************************************************************
 * Fonction qui reçoit un message du flux du client.
 * Il prend en paramètre :
 *     - streamClient    Numéro du flux du client.
 *     - msg             Pointeur vers la chaine de caractère à récupérer.
 *****************************************************************************/
int message_receive(int streamClient, char *msg) {
  int status;

  status = recv(streamClient, msg, MSG_SIZE, 0);
  if ( status == -1 ) {
    perror("Error with recv");
  }
  if ( msg[strlen(msg)-1] == '\n' || strlen(msg) > MSG_SIZE ) {
    msg[strlen(msg)-1] = '\0';
  }

  return status;
}

/******************************************************************************
 * Fonction qui envoie un message sur le flux du client.
 * Prend en paramètre :
 *     - streamClient    Numéro du flux du client.
 *     - msg                 Pointeur vers la chaine de caractère à envoyer.
 * Renvoie 1 si le message à bien été envoyé, 0 sinon.
 *****************************************************************************/
int message_send(int streamClient, char *msg) {
  int status;

  status = send(streamClient, msg, MSG_SIZE, 0);
  if ( status == -1 ) {
    perror("Error with send");
    return 0;
  }
  return 1;
}

/******************************************************************************
 * Fonction qui affiche les informations du client connecter au serveur.
 * Prend en paramètre :
 *     - servInfo    Pointeur vers les informations récupérées par la
 *                     fonction 'get_info'.
 *****************************************************************************/
void printClient(struct addrinfo *servInfo) {
  int status;
  char host[NI_MAXHOST], service[NI_MAXHOST];

  status = getnameinfo((struct sockaddr *) &servInfo->ai_addr,
                         servInfo->ai_addrlen, host, NI_MAXHOST, 
                         service, NI_MAXSERV, NI_NUMERICSERV);
  if ( status == 0 ) {
    printf("%s:%s connected.\n", host, service);
  } else {
    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(status));
  }
}

/******************************************************************************
 * Serveur CLI TCP, reçoit une chaine de caractère d'un client et lui renvoie.
 *   Le programme prend en paramètre :
 *     - port : Port d'écoute du serveur
 *****************************************************************************/

int main(int argc, char *argv[]) {
  struct addrinfo servInfo;
  int socketDescriptor;
  int streamClient;
  char msg[MSG_SIZE];


  printf("\n ****      Welcome to the TCP Server.      ****\n\n");

  /* Vérification des paramètres du programme */
  if ( argc != 2 ) {
    fprintf(stderr, "Usage: %s port\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  /* Récupération des informations du serveur */
  servInfo = get_info(argv[1]);

  /* Ouverture du socket */
  socketDescriptor = socket_open(&servInfo);

  printf("Listen on %s\n", argv[1]);

  /* Traitement de tous message reçu, renvoie au client le message reçu */
  while ( 1 ) {
    printf("\nWainting to connect to server.\n");

    streamClient = client_connect(socketDescriptor, &servInfo);
    printClient(&servInfo);

    memset(&msg, 0, sizeof(msg));
    while ( message_receive(streamClient, msg) > 0 ) {
      printf(">> %s\n", msg);
      if ( message_send(streamClient, msg) )
        printf(">> # Same message sent.\n");
      memset(&msg, 0, sizeof(msg));
    }
  }

  socket_close(socketDescriptor);

  exit(EXIT_SUCCESS);
}  
