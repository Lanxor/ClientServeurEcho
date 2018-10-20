C=gcc
OPT= -W -Wall -pedantic

all: udp udpCLI tcp clean

udp: udpClient udpServer

udpCLI : udpClientCLI udpServerCLI

udpClient: udp-client.o
	$(CC) $^ -o udp-client $(OPT)

udpClientCLI: udp-client-cli.o
	$(CC) $^ -o udp-client-cli $(OPT)

udpServer: udp-server.o
	$(CC) $^ -o udp-server $(OPT)

udpServerCLI: udp-server-cli.o
	$(CC) $^ -o udp-server-cli $(OPT)

tcp: tcpClient tcpServer

tcpCLI: tcpClientCLI tcpServerCLI

tcpClient: tcp-client.o
	$(CC) $^ -o tcp-client $(OPT)

tcpClientCLI: tcp-client-cli.o
	$(CC) $^ -o tcp-client-cli $(OPT)

tcpServer: tcp-server.o
	$(CC) $^ -o tcp-server $(OPT)

tcpServerCLI: tcp-server-cli.o
	$(CC) $^ -o tcp-server-cli $(OPT)

%.o: %.c
	$(CC) -o $@ -c $< $(OPT)

clean:
	rm -rf *.o

mrproper: clean
	rm -f udp-client udp-client-cli udp-server udp-server-cli
	rm -f tcp-client tcp-client-cli tcp-server tcp-server-cli
