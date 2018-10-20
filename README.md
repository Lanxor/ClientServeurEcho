# Présentation
# Utilisation
Le programme se décompense en deux parties. Une utilisant le mode datagramme et une seconde utilisant le mode connecté.

Chaque mode est décompensé en deux sous-programme, un sous-programme simple, où l'utilisateur doit fournir les informations une par une. Et un mode en ligne de commande.

Pour compiler le projet en entier il suffit d'exécuter la commande suivante :
 ```
 $ make
```

## Mode UDP
### Programme simple
Compilation :
```
$ make udp                # Compile les programmes client et serveur.
$ make udpClient          # Compile le programme client.
$ make udpServer          # Compile le programme serveur.
```
Exécution :
```
$ ./udp-client          # Exécute le programme client.
$ ./udp-server          # Exécute le programme serveur.
```

### Programme en ligne de commande
Compilation
```
$ make udpCLI                # Compile les programme client et serveur.
$ make udpClientCLI          # Compile le programme client.
$ make udpServerCLI          # Compile le programme serveur.
```
Exécution :
```
$ ./udp-client-cli host port message          # Exécute le programme client
$ ./udp-server-cli port                       # Exécute le programme serveur
```

## Mode TCP
### Programme simple
Compilation :
```
$ make tcp                # Compile les programmes client et serveur.
$ make tcpClient          # Compile le programme client.
$ make tcpServer          # Compile le programme serveur.
```
Exécution :
```
$ ./tcp-client          # Exécute le programme client.
$ ./tcp-server          # Exécute le programme serveur.
```
### Programme en ligne de commande
Compilation
```
$ make tcpCLI                # Compile les programme client et serveur.
$ make tcpClientCLI          # Compile le programme client.
$ make tcpServerCLI          # Compile le programme serveur.
```
Exécution :
```
$ ./tcp-client-cli host port message          # Exécute le programme client
$ ./tcp-server-cli port                       # Exécute le programme serveur
```

# Exemple d'utilisation
Voici un exemple d'un client/serveur en mode connecté en ligne de commande.

## Côté client :
```
$ ./tcp-client-cli localhost 25555 "Hello world !"

 ****      Welcome in the program TCP Client.      ****

Connected on the server.
Message send : Hello world !
Message receive : Hello world !
```
## Côté serveur :
```
$ ./tcp-server-cli 25555
Listen on 25555

Wainting to connect on server.
localhost:45364 connected.
>> Hello world !
>> # Same message send.

Wainting to connect on server.
```
