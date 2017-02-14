#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 5000
#define DEFAULT_PORT "8080"
#define IP "192.168.1.123"
#define COPY 'c'
#define PASTE 'p'

#define DEBUG 1

#if DEBUG
#define PR(a) printf(a)
#define ERROR(b) printf(b);\
				 getchar();\
				 exit(1)
#else
#define PR(a) 
#define ERROR(a) 
#endif

void BailOut(char *msg)
{
	//fprintf(stderr, "Exiting: %s\n", msg);
	exit(1);
}

int __cdecl main(int argc, char **argv)
{
	/*HANDLE h;
	/char *sendbuf;

	if (!OpenClipboard(NULL))
	{
		ERROR("Error opening clipboard");
	}
		

	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		sendbuf = (char *)GetClipboardData(CF_TEXT);
	}else if (IsClipboardFormatAvailable(CF_TIFF))
	{
		PR("tagged image");
	}*/
		
		
	char sendbuf []= { 'h','e','l','l','o','\n'};
	//printf("%s\n", sendbuf);
	//getchar();
	CloseClipboard();

	char sendInfo[DEFAULT_BUFLEN];
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	
	char in;

	in = getchar();
	if(in == COPY)
		sendInfo[0] = COPY;
	else if(in == PASTE)
		sendInfo[0] = PASTE;

	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		PR("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		PR("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			PR("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		PR("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendInfo, (int)strlen(sendInfo), 0);
	if (iResult == SOCKET_ERROR) {
		PR("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	PR("Bytes Sent: %ld\n", iResult);

	if(in == 'c')
	{
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendInfo), 0);
		if (iResult == SOCKET_ERROR) {
			PR("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
	}else if(in == 'p')
	{
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			printf("Data %s \n", recvbuf);
			getchar();
		}
		else if (iResult == 0)
		{
			printf("Data %s \n", recvbuf);
			PR("Connection closed\n");
			getchar();
		}
		else
		{
			PR("recv failed with error: %d\n", WSAGetLastError());
		}
	}

	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		PR("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	/*
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			printf("Bytes received: %d\n", iResult);

			printf("Data %s \n", recvbuf);
			getchar();
		}else if (iResult == 0)
		{
			printf("Data %s \n", recvbuf);
			PR("Connection closed\n");
			getchar();
		}else
		{
			PR("recv failed with error: %d\n", WSAGetLastError());
		}
		

	} while (iResult > 0);*/

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();
	getchar();
	return 0;
}
