/******************************************************************************
 *
 * Name File : udp-client.c
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
#define NAME_ARRAY_SIZE 80
#define PORT_ARRAY_SIZE 8

/******************************************************************************
 * Fonction qui demande à l'utilisateur de saisir une chaine de caractère.
 * Prend en paramètre :
 *     - string        Un pointeur vers une chaine de caractère.
 *     - sizeString    Un nombre maximal de caractères pour la chaine saisie.
 *****************************************************************************/
int input(char *string, unsigned int sizeString) {
  memset(string, 0, sizeof(*string));
  if ( fgets(string, sizeString+1, stdin) == NULL )
   return -1;
  if ( strlen(string) < sizeString )
    string[strlen(string)-1] = '\0';
  return 0;
}

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
  hints.ai_socktype = SOCK_DGRAM;
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
 * Fonction qui envoie un message sur le socket passé en paramètre.
 * Prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - servInfo            Pointeur vers les informations récupérées par la
 *                             fonction 'get_info'.
 *     - msg                 Pointeur vers la chaine de caractère à envoyer.
 *****************************************************************************/
void message_send(int socketDescriptor, struct addrinfo *servInfo, char *msg) {
  int status;

  status = sendto(socketDescriptor, msg, strlen(msg), 0, servInfo->ai_addr, servInfo->ai_addrlen);
  if ( status == -1 ) {
    perror("Error with sendto");
    close(socketDescriptor);
    exit(EXIT_FAILURE);
  }
}

/******************************************************************************
 * Fonction qui reçoit un message du descripteur de socket.
 * Il prend en paramètre :
 *     - socketDescriptor    Numéro du descripteur de socket.
 *     - msg                 Pointeur vers la chaine de caractère à récupérer.
 *****************************************************************************/
void message_receive(int socketDescriptor, char *msg) {
  int status;

  status = recv(socketDescriptor, msg, MSG_SIZE, 0);
  if ( status == -1 ) {
    perror("Error with recv");
    close(socketDescriptor);
    exit(EXIT_FAILURE);
  }
}

/******************************************************************************
 * Client simple UDP, envoie une chaine de caractère à un serveur echo
 *   et reçoit la chaine de caractère envoyé.
 *   Demande à l'utilisateur les paramètres suivants :
 *     - name    Adresse de destination (adresse IP ou nom de domaine)
 *     - port    Port du serveur de destination
 *     - msg     Message à envoyer au serveur
 *****************************************************************************/
int main() {
  int socketDescriptor;
  struct addrinfo servInfo;
  char msg[MSG_SIZE];
  char serverName[NAME_ARRAY_SIZE];
  char serverPort[PORT_ARRAY_SIZE];


  printf("\n ****      Welcome to the UDP Client.      ****\n\n");

  /* Demande de l'adresse de destination à l'utilisateur */
  printf("Enter the server name or IP address : ");
  input(serverName, NAME_ARRAY_SIZE);

  /* Demande du numéro de port du serveur à l'utilisateur */
  printf("Enter the port number : ");
  input(serverPort, PORT_ARRAY_SIZE);

  /* Récupération des informations du serveur */
  servInfo = get_info(serverName, serverPort);
  /* Ouverture du socket */
  socketDescriptor = socket_open(&servInfo);

  printf("\n **** Enter the character '.' to stop the program  ****\n\n");
  /* Demande du message à envoyer au serveur */
  printf("Write your message : ");
  input(msg, MSG_SIZE);

  while ( strcmp(msg, ".") ) {
    /* Envoie du message */
    message_send(socketDescriptor, &servInfo, msg);
    printf("Message sent : %s\n", msg);

    /* Reception du message envoyé par le serveur echo */
    message_receive(socketDescriptor, msg);
    printf("Message received : %s\n\n", msg);

    /* Demande du message à envoyer au serveur */
    printf("Write your message : ");
    input(msg, MSG_SIZE);
  }

  printf("You are leaving the program, good bye.\n");

  socket_close(socketDescriptor);


  exit(EXIT_SUCCESS);
}
