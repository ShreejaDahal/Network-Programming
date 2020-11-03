// getaddrinfo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[]) {

	if (argc != 2) {
		cerr << "[Warning] argument missing! " << endl;
	}

	WORD wVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;

	int err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		cerr << "[WARNING] Winsock dill missing " << endl;
	}
	else {
		cout << "INFO: Winsock dll successfully loaded! " << endl;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		cerr << "[WARNING] Useable dll missing! " << endl;
		return 1;
	}

	struct addrinfo hints, *res; //a point to an addrinfo structure that provides hints
	//about the type of socket caller supports and *res a pointer to the linked list
	//of one or more addrinfo structures that contains response information about the host.
	
	
	memset(&hints, 0, sizeof(hints)); //setting the hints to zero

	hints.ai_family = AF_INET; //ipv4
	hints.ai_socktype = SOCK_STREAM; //stream socket


	if (getaddrinfo(argv[1], "12345", &hints, &res) != 0) {
		cerr << "[WARNING] Error occured! " << endl;
	}

	struct sockaddr_in* addr; //internet end point address 
	//contains, short sin_family, sin_port, sin_addr,

	struct addrinfo* rp = res;  //describes address information for use with TCP/IP

	//get the pointer to the address itself, different fields in IPv4
	for (rp = res; rp != NULL; rp = rp->ai_next) {
		addr = (struct sockaddr_in*)rp->ai_addr;
		 
		cout << " Port number: " << ntohs(addr->sin_port) << endl; //the use of ntohs is to convert the network byte ordering to the host byte order, ntohs is network to host short
		cout << "Address: " << inet_ntoa((struct in_addr)addr->sin_addr); //inet_ntoa converts an IPV4 adrress into an ASCII string in Internet standard dotted-decimal format
		
	}

	WSACleanup();

}