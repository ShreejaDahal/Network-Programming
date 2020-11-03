// EchoServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<string>
#include<vector>
using namespace std;
#include<WinSock2.h>
#include<time.h>
#pragma comment(lib, "Ws2_32.lib")

/*Function to prepare to receive connectins for a service. */
static SOCKET passiveTCP(const char* service, int qlen);

int main()
{
    WORD WVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;

    int err = WSAStartup(WVersionRequested, &wsaData);
    if (err != 0) {
        cout << "[WARNING] Winsock dll missing! " << endl;
    }
    else {
        cout << "[INFO] Winsock dll loaded successfully! " << endl;
    }

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1) {
        cerr << "[WARNING] Useable DLL missing! " << endl;
        return 1;
    }


    const int BUFFSZ = 256;
    const char* service = "7007"; //the default port number for the service to be provided.
    vector<SOCKET>soc;
    int rv;

    SOCKET ls = passiveTCP(service, SOMAXCONN);
    int largestSocket = ls;

    while (true) {

        struct sockaddr_in address; //address of the client
        int alen = sizeof(sockaddr_in); //length of client address
        char buff[256];
       

        fd_set readmap;
        
        FD_ZERO(&readmap);
        FD_SET(ls, &readmap);

        for (int is = 0; is < (int)soc.size(); is++) {
            if (soc[is] == 0) {
                continue;
            }
            FD_SET(soc[is], &readmap);
            if (int(soc[is]) > largestSocket) {
                largestSocket = soc[is];
            }
        }

        int nsoc = select(largestSocket + 1, &readmap, NULL, NULL, NULL);
        if (nsoc == SOCKET_ERROR) {
            cerr << "Error in select" << endl;
            return 1;
        }

        if (FD_ISSET(ls, &readmap)) {
            SOCKET s = accept(ls, (sockaddr*)&address, (int*)&alen);
            if (s == INVALID_SOCKET) {
                int errorCode = WSAGetLastError();
                cerr << "Error in accept. " << errorCode << endl;
            }
            else {
                soc.push_back(s);
                continue;
            }
        }
        for (int i = 0; i < soc.size(); i++) {
            if (FD_ISSET(soc[i], &readmap)) {
                rv = recv(soc[i],buff, BUFFSZ, 0);
                if (rv == 0) {
                    closesocket(soc[i]);
                    soc.erase(soc.begin() + i);
                    i--;
                }
                else {
                    buff[rv] = '\0';
                    if(send(soc[i], buff, (int)strlen(buff), 0) == INVALID_SOCKET) {
                        int errorCode = WSAGetLastError();
                        cerr << "send to client failed " << errorCode << endl;
                        closesocket(soc[i]);
                        continue;
                    }
                    
                }
            }
        }
    }
}
static SOCKET passiveTCP(const char* service, int qlen) {

    struct sockaddr_in address; //internet end point address
    struct servent* serv; //information about the requested service
    SOCKET soc;

    memset(&address, 0, sizeof(address)); //sets the address to zero
    address.sin_family = AF_INET; //ipv4
    address.sin_addr.s_addr = INADDR_ANY; //s_addr is an on-wire format structure that describes how the information looks like when transmitted
    //address.sin_port = htons(8888);


    //getting the port number for the service
    if ((serv = getservbyname(service, "tcp")) != NULL) {
        address.sin_port = (u_short)serv->s_port;

    }
    else if ((address.sin_port = htons((u_short)stoi(service))) == 0) {
        perror("Port number");
        exit(1);
    }

        //cout << "[INFO] The IP address is " << address.sin_addr.s_addr << endl;


        //allocating a socket 

        soc = socket(PF_INET, SOCK_STREAM, 0);
        if (soc ==  INVALID_SOCKET) {
            cerr << "[WARNING] Could not allocate a socket ";
            exit(1);
        }

        //Bind the address to the socket
        int b = bind(soc, (struct sockaddr*) & address, sizeof(address));
        if (b == INVALID_SOCKET) {
            int errorCode = WSAGetLastError();
            cerr << "[WARNING] Bind call failed: " << errorCode << endl;
            exit(1);
        }

        //Indicate that we are ready to receive connections
        int listenReady = listen(soc, qlen);
        if (listenReady == INVALID_SOCKET) {
            int errorCode = WSAGetLastError();
            cerr << "[WARNING] Listen call failed: " << errorCode << endl;
            exit(1);
        }

        //return the listening socket
        return soc;
    }
