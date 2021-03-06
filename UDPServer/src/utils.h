/*
 * utils.h
 *
 *  Created on: 11 dic 2021
 *      Author: Ester Molinari, Tommaso Perniola
 */

#ifndef UTILS_H_
#define UTILS_H_

#if defined WIN32
#include <winsock2.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define ECHOMAX 255
#define EQUALS '='

// default address and port
#define DEFAULT_NAME "localhost"
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 45000

// error message handler
void errorhandler(char* errorMessage)
{
	printf(errorMessage);
}

// for windows sockets
void clearwinsock() {
#if defined WIN32
	WSACleanup();
#endif
}

// math functions
int add(int a, int b)
{
	return (a + b);
}

int  mult(int a, int b)
{
	return (a * b);
}

int sub(int a, int b)
{
	return (a - b);
}

float division(float a, float b) {
	return (a / b);
}

#endif /* UTILS_H_ */
