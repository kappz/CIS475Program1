#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstring>

using namespace std;

#define SERVER_PORT 1337
#define MAX_LINE 1000

void printMessage(int);
int main(int argcount, char* argvector[])
{
	struct sockaddr_in sockaddress;
	int length;
	int socketfd;
	int bufferIndex = 0;
	string command;
	char sBuffer[MAX_LINE];
	char rBuffer[MAX_LINE];

	// verify client provided host IP address.
	if (argcount < 2)
	{
		cout << "usage: client <Server IP Address>" << endl;
		exit(1);
	}
	// create socket
	if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR: Failed to create socket.");
		exit(1);
	}

	bzero((char*)&sockaddress, sizeof(sockaddress)); // set all socket data structures to NULL
	sockaddress.sin_family = AF_INET;
	sockaddress.sin_addr.s_addr = inet_addr(argvector[1]);
	sockaddress.sin_port = htons(SERVER_PORT);

	// connect to server
	if (connect(socketfd, (struct sockaddr*)&sockaddress, sizeof(sockaddress)))
	{
		perror("ERROR: Failed to connect to server.");
		close(socketfd);
		exit(1);
	}
	else
		cout << "Connection to server successfull." << endl;

	while (fgets(sBuffer, sizeof(sBuffer), stdin))
	{
		while (sBuffer[bufferIndex] != ' ' && sBuffer[bufferIndex] != '\n')
		{
			command += toupper(sBuffer[bufferIndex]);
			++bufferIndex;
		}

		if (command == "QUIT")
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
			exit(1);
		}
		else if  (command == "ADD")
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
		}
		else if (command == "SHUTDOWN")
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
		}
		else if (command == "LIST")
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
		}
		else if (command == "DELETE")
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
		}
		else
		{
			send(socketfd, sBuffer, sizeof(sBuffer), 0);
			printMessage(socketfd);
		}
		command.clear();
		bufferIndex = 0;
	}
}

void printMessage(int socketNum)
{
	char recBuffer[1000];
	recv(socketNum, recBuffer, sizeof(recBuffer), 0);
	cout << recBuffer << endl;
}