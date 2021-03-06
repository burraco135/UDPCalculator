/*
 ============================================================================
 Name        : UDPClient.c
 Author      : Ester Molinari
 Version     : 1.6
 Copyright   : Your copyright notice
 Description : A simple UDP calculator in C (client)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void checkingServerInfo(char name[], int port);

int main(int argc, char *argv[]) {
	// output stream
	setvbuf(stdout, NULL, _IONBF, 0);

#if defined WIN32
	WSADATA wsa_data;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsa_data);

	if (iResult != 0) {
		errorhandler("Error at WSAStartup()\n");
		return EXIT_FAILURE;
	}
#endif

	int client_socket = 0;
	int echoStringLen = 0;
	int respStringLen = 0;
	int fromSize = 0;
	int server_port = 0;
	int a = 0, b = 0;
	struct sockaddr_in echoServerAddr;
	struct sockaddr_in fromAddr;
	struct hostent* server;
	struct in_addr* serverAddr;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	char server_name[ECHOMAX];
	char server_ip[16];
	char sign[0];
	char* token;

	// client's socket
	if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		errorhandler("socket() failed");
		return EXIT_FAILURE;
	}

	// server's address and port from input
	if (argc > 2) {
		strcpy(server_name, argv[1]);
		server_port = atoi(argv[2]);
		checkingServerInfo(argv[1], atoi(argv[2]));
	} else {
		printf("Insert server's name and port (localhost:45000)\n");
		scanf(" %[^\n]", echoBuffer);
	}

	token = strtok(echoBuffer, ":");
	strcpy(server_name, token);
	token = strtok(NULL, ":");
	server_port = atoi(token);

	// checking input with default values
	checkingServerInfo(server_name, server_port);

	// server's address resolution
	server = gethostbyname(server_name);

	if (server == NULL) {
		errorhandler("gethostbyname() failed\n");
		return EXIT_FAILURE;
	} else {
		serverAddr = (struct in_addr*)server->h_addr_list[0];
		strcpy(server_ip, inet_ntoa(*serverAddr));
	}

	printf("Connection established with %s, address: %s\n\n", server_name, server_ip);

	// setting server's address and port
	memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = PF_INET;
	echoServerAddr.sin_port = htons(DEFAULT_PORT);
	echoServerAddr.sin_addr.s_addr = inet_addr(DEFAULT_IP);

	// sending a message to server
	printf("Welcome to UDPCalculator!"
			"\n There's a list of queries example:"
			"\n Addition:\t\t+ 23 45"
			"\n Subtraction:\t\t- 23 45"
			"\n Division:\t\t/ 23 45"
			"\n Multiplication:\t* 23 45"
			"\n Close calculator:\t=\n\n");
	while(1) {
		memset(echoString, 0, sizeof(echoString));
		printf("Insert a math query (+ 23 45):\n");
		scanf(" %[^\n]", echoString);
		echoStringLen = strlen(echoString);

		if (echoString[0] != '=') {
			if (echoStringLen > ECHOMAX) {
				errorhandler("echo word too long");
			} else {
				sscanf(echoString, "%s %d %d", sign, &a, &b);
			}

			if (sendto(client_socket, echoString, echoStringLen, 0, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != echoStringLen) {
				errorhandler("sento() sent different number of bytes than expected");
			}

			// receiving result from server
			fromSize = sizeof(fromAddr);
			memset(echoBuffer, 0, sizeof(echoBuffer));
			respStringLen = recvfrom(client_socket, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

			if (echoServerAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
				fprintf(stderr, "Error: received a packet from unknown source.\n");
				return EXIT_FAILURE;
			}

			printf("Result received from server %s, ip %s: %s\n\n", server_name, inet_ntoa(*serverAddr), echoBuffer);
		} else {
			if (sendto(client_socket, echoString, echoStringLen, 0, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != echoStringLen) {
				errorhandler("echo word too long");
			}

			// closing socket
			printf("Closing client\n");
			closesocket(client_socket);
			clearwinsock();
			system("pause");
			return EXIT_SUCCESS;
		}
	}
}

void checkingServerInfo(char name[], int port) {
	// checking loopback canonical name
	if (strcmp(name, DEFAULT_NAME) != 0) {
		strcpy(name, DEFAULT_NAME);
	}
	if (port != DEFAULT_PORT) {
		port = DEFAULT_PORT;
	}
}
