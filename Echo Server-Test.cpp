//Program Description: Use the echo service to determine how long it would take to send and receive 1000 messages of 2000 bytes from a remote site.  
//Receive each message fully before sending the next one.  Test on the Ramapo site cs.ramapo.edu.
//Author: Shreeja Dahal 


#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<Winsock2.h>
#include<cstdlib>
#include<iostream>
#include<string>
#include<chrono>
using namespace std;

#define BUFFSZ 2000
static SOCKET TCPConnection(const char* host, const char* service);


int main() {

	WORD WVersionRequested = MAKEWORD(1, 1);
	WSADATA wsaData;
	string messages;
	int totalNumberOfMessages = 1000;
	char buffer[BUFFSZ + 100];
	char rbuff[BUFFSZ + 1000];

	int err = WSAStartup(WVersionRequested, &wsaData);

	if (err != 0) {
		cout << "[WARNING] Winsock dll missing! " << endl;
		return 1;
	}

	else
	{
		cout << "[INFO] Winsock dll loaded successfully! " << endl;
	}

	if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		cerr << "[WARNING] Useable DLL missing! " << endl;
		return 1;
	}

	const char* host = "cs.ramapo.edu";
	const char* service = "echo";

	SOCKET soc = TCPConnection(host, service);
	//Asking the client for a message


	cout << "Enter a message! " << endl;
	cin >> messages;


	const auto start_time = chrono::system_clock::now();

	for (int i = 0; i < totalNumberOfMessages; i++) {

		int rv = 0; //data receive
		int nb = 0; //num of data sent
		if ((nb = send(soc, messages.c_str(), BUFFSZ, 0)) < 0) {
			cerr << "[WARNING] " << WSAGetLastError << " error found" << endl;
			return 0;

		}
		//resetting the total received byte for each i 
		int tnb = 0; 

		while (tnb < BUFFSZ) {

			if ((rv = recv(soc, rbuff, BUFFSZ, 0)) > 0) {
				tnb += rv;
				continue;
			}
			else {
				cerr << "[WARNING] Error " << WSAGetLastError << " found! " << endl;
				return 0;
			}
		}
		buffer[rv] = '\0';

	}
	closesocket(soc);
	const auto end_time = chrono::system_clock::now();
	

	cout << "The total time taken was: " << chrono::duration_cast<chrono::seconds>(end_time - start_time).count() << " seconds" << endl;
}


static SOCKET TCPConnection(const char* host, const char* service) {

	struct sockaddr_in sin; //internet end point address
	struct hostent* remote_host; //information about the remote host
	struct servent* serv; //information about the requested service 
	SOCKET soc;
	int connection_sucessful;


	memset((char*)&sin, 0, sizeof(sin)); //sets the address to zero
	sin.sin_family = AF_INET; //internet family of address, IP4


	//getting host name
	remote_host = gethostbyname(host);
	if (remote_host != NULL) {
		memcpy((char*)&sin.sin_addr, remote_host->h_addr, remote_host->h_length);
	}
	else {
		cerr << "[WARNING] Invalid host address!" << endl;
		exit(1);
	}

	//getting the port number
	serv = getservbyname(service, "tcp");
	if (serv != NULL) {
		sin.sin_port = serv->s_port;
		cout << "[INFO] Port number: " << (long int)ntohs(sin.sin_port) << endl;

	}
	else {

		cerr << "[WARNING] Invalid port number! " << endl;
		exit(1);
	}

	cout << "[INFO] The IP adress is " << sin.sin_addr.s_addr << endl;

	//allocating a socket from which the connection to the server will be made

	soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc < 0) {
		cerr << "[WARNING] Could not allocate the socket. ";
		exit(1);
	}
	//connect to the server
	connection_sucessful = connect(soc, (sockaddr*)&sin, sizeof(sin));
	if (connection_sucessful < 0) {
		cout << "[WARNING] Attempt to connect to the server failed" << endl;
	}
	else {
		cerr << "[INFO] Attempt to connect to the server was successful" << endl;
	}

	return soc;

}



