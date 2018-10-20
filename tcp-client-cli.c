/******************************************************************************
 *
 * Name File : tcp-client-cli.c
 * Authors   : OLIVIER Thomas & ROBERT DE ST VINCENT Guillaume
 * Location  : UPSSITECH - University Paul Sabatier
 * Date      : October 2018
 *
 *                        This work is licensed under a 
 *              Creative Commons Attribution 4.0 International License.
 *                                    (CC BY)
 *
 *****************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#define MSG_SIZE 80

/******************************************************************************
 * Fonction qui récupère les informations du serveur en mode datagramme.
 * Prend en paramètre :
 *     - serverName    Pointeur vers une chaine de caractère pour le nom ou
 *                       l'adresse IP du serveur.
 *     - serverPort    Pointeur vers une chaine de caractère pour le numéro de
 *                       port du serveur.
 * Renvoie la structure d'information addrinfo.
 *****************************************************************************/
struct addrinfo get_info(char *serverName, char *serverPort) {
  int status;
  struct addrinfo hints;
  struct addrinfo *servInfo;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;

  /* Ouverture et récupération des informations de connexion au serveur */
  status = getaddrinfo(serverName, serverPort, &hints, &servInfo);
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

  socketDescriptor = socket(servInfo->ai_family, servInfo->ai_socktype,
                            servInfo->ai_protocol);
  if ( socketDescriptor == -1) {
    perror("Error with socket");
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
 * Fonction qui permet de créer le flux TCP entre le client et le serveur.
 * Prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - servInfo            Pointeur vers les informations récupérées par la
 *                             fonction 'get_info'.
 *****************************************************************************/
void client_connect(int socketDescriptor, struct addrinfo *servInfo) {
  if (connect(socketDescriptor, servInfo->ai_addr, servInfo->ai_addrlen) == -1) {
    close(socketDescriptor);
    perror("Error with connect");
    exit(EXIT_FAILURE);
  }
}

/******************************************************************************
 * Fonction qui envoie un message sur le flux du client.
 * Prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - msg                 Pointeur vers la chaine de caractère à envoyer.
 * Renvoie le code de la fonction sendTo.
 *****************************************************************************/
int message_send(int socketDescriptor, struct addrinfo *servInfo, char *msg) {
  int status;

  status = sendto(socketDescriptor, msg, strlen(msg), 0, servInfo->ai_addr, servInfo->ai_addrlen);
  if ( status == -1 ) {
    perror("Error with sendto");
    close(socketDescriptor);
    exit(EXIT_FAILURE);
  }

  return status;
}

/******************************************************************************
 * Fonction qui reçoit un message du flux du client.
 * Il prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - msg                 Pointeur vers la chaine de caractère à récupérer.
 * Renvoie le code de la fonction recv.
 *****************************************************************************/
int message_receive(int socketDescriptor, char *msg) {
  int status;

  status = recv(socketDescriptor, msg, MSG_SIZE, 0);
  if ( status == -1 ) {
    perror("Error with recv");
    close(socketDescriptor);
    exit(EXIT_FAILURE);
  }

  return status;
}

/******************************************************************************
 * Client CLI TCP, envoie une chaine de caractère à un serveur echo
 *   et reçoit la chaine de caractère envoyé.
 *   Le programme prend en paramètre :
 *     - host : Adresse de destination (adresse IP ou nom de domaine)
 *     - port : Port du serveur de destination
 *     - msg : Message à envoyer au serveur
 *****************************************************************************/
int main(int argc, char *argv[]) {
  struct addrinfo servInfo;
  int socketDescriptor;
  char msg[MSG_SIZE];


  /* Vérification des paramètres du programme */
  if ( argc < 4 ) {
    fprintf(stderr, "Usage %s host port msg\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  printf("\n ****      Welcome to the TCP Client.      ****\n\n");

  /* Récupération des informations du serveur */
  servInfo = get_info(argv[1], argv[2]);

  /* Ouverture du socket */
  socketDescriptor = socket_open(&servInfo);

  /* Connexion au serveur */
  client_connect(socketDescriptor, &servInfo);
  printf("Connected to the server.\n");

  /* Envoie du message */
  message_send(socketDescriptor, &servInfo, argv[3]);
  printf("Message sent : %s\n", argv[3]);

  /* Reception du message envoyé par le serveur echo */
  message_receive(socketDescriptor, msg);
  printf("Message received : %s\n", msg);

  close(socketDescriptor);

  exit(EXIT_SUCCESS);
}
