/*
 ============================================================================
 Name        : UDPClient.c
 Author      : Ester Molinari
 Version     : 1.1
 Copyright   : Your copyright notice
 Description : A simple UDP calculator in C (client)
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int main(void) {
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

	int client_socket;
	int echoStringLen;
	int respStringLen;
	int fromSize;
	int server_port = 0;
	struct sockaddr_in echoServerAddr;
	struct sockaddr_in fromAddr;
	struct hostent* server;
	struct in_addr* serverAddr;
	char echoString[ECHOMAX];
	char echoBuffer[ECHOMAX];
	char server_name[40];
	char server_ip[16];

	// client's socket
	if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		errorhandler("socket() failed");
		return EXIT_FAILURE;
	}

	// server's address and port from input
	// FIXME doesn't recognise name:port format
	// TODO starting with standard values
	printf("Insert server's name and port like this: localhost 48000\n");
	scanf(" %[^\n]", echoBuffer);
	sscanf(echoBuffer, "%s %d", server_name, &server_port);

	// server's address resolution
	server = gethostbyname(server_name);

	if (server == NULL) {
		errorhandler("gethostbyname() failed\n");
		return EXIT_FAILURE;
	} else {
		serverAddr = (struct in_addr*)server->h_addr_list[0];
		strcpy(server_ip, inet_ntoa(*serverAddr));
	}

	printf("Name: %s, Address: %s\n", server_name, server_ip);

	// setting server's address and port
	memset(&echoServerAddr, 0, sizeof(echoServerAddr));
	echoServerAddr.sin_family = PF_INET;
	echoServerAddr.sin_port = htons(DEFAULT_PORT);
	echoServerAddr.sin_addr.s_addr = inet_addr(DEFAULT_IP);

	// sending a message to server
	memset(echoString, 0, sizeof(echoString));
	printf("Insert echo string:\n");
	scanf("%s", echoString);

	if ((echoStringLen = strlen(echoString)) > ECHOMAX) {
		errorhandler("echo word too long");
	}

	if (sendto(client_socket, echoString, echoStringLen, 0, (struct sockaddr*)&echoServerAddr, sizeof(echoServerAddr)) != echoStringLen) {
		errorhandler("sento() sent different number of bytes than expected");
	}

	// receiving result from server
	fromSize = sizeof(fromAddr);
	respStringLen = recvfrom(client_socket, echoBuffer, ECHOMAX, 0, (struct sockaddr*)&fromAddr, &fromSize);

	if (echoServerAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr) {
		fprintf(stderr, "Error: received a packet from unknown source.\n");
		return EXIT_FAILURE;
	}

	// TODO find server's canonical name and info

	// closing socket
	closesocket(client_socket);
	clearwinsock();

	return EXIT_SUCCESS;
}
