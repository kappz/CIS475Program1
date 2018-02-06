#include <stdio.h>
#include <iostream>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

#define SERVER_PORT 6011
#define MAX_PENDING 1
#define MAX_LINE 1000

int main(int argcount, char* argvector[])
{
	

	struct sockaddr_in sockAddress;
	socklen_t addressLength;
	int bufferIndex = 0;
	int bufferLength;
	int serverSocket;
	int clientSocket;
	int id = 1000;
	int listCount = 0;
	int tempid = 0;
	int modid = 0;
	int bufferSize = 0;
	char rBuffer[MAX_LINE];
	char sBuffer[MAX_LINE];
	char *listBuffer;
	char addBuffer[28] = {'2','0','0',' ','O','K','\n',
						  'T','h','e',' ','n','e','w',' ',
						  'r','e','c','o','r','d',' ',
		                  'I','D',' ','i','s',' '};
	string add;
	string command;
	FILE* file;
	FILE* tempFile;
	char phoneNumber[12];
	char command2[8];
	char firstName[8];
	char lastName[8];

	bzero((char*)&sockAddress, sizeof(sockAddress));  // set all socket structures to NULL
	sockAddress.sin_family = AF_INET;
	sockAddress.sin_addr.s_addr = INADDR_ANY;
	sockAddress.sin_port = htons(SERVER_PORT);

	// create socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("ERROR: Failed to create socket.");
		exit(1);
	}
	// bind socket to port
	if ((bind(serverSocket, (struct sockaddr*) &sockAddress, sizeof(sockAddress))) < 0)
	{
		perror("ERROR: Failed to bind socket.");
		exit(1);
	}

	// set socket to listen 
	listen(serverSocket, MAX_PENDING);
	addressLength = sizeof(sockAddress);
	cout << "The server is running and waiting for a connection.." << endl;
	
	// wait for connection
	while (1)
	{
		// accept first connection
		if ((clientSocket = accept(serverSocket, (struct sockaddr*)&sockAddress, &addressLength)) < 0)
		{
			perror("ERROR: Acception failed.");
			exit(1);
		}
		cout << "New connection from " << inet_ntoa(sockAddress.sin_addr) << endl;

		while (bufferLength = recv(clientSocket, rBuffer, sizeof(rBuffer), 0))
		{
			while (rBuffer[bufferIndex] != ' ')
			{
				command += toupper(rBuffer[bufferIndex]);
				++bufferIndex;
			}
			if (command == "SHUTDOWN")
			{
				send(clientSocket, "200 OK", 8, 0);
				close(clientSocket);
				close(serverSocket);
				exit(1);
			}
			else if (command == "QUIT")
			{
				send(clientSocket, "200 OK", 8, 0);
			}
			
			else if (command == "ADD")
			{
				// open address book file
				file = fopen("./addressBook.txt", "a+");
				if (file == NULL)
					perror("ERROR: The address book file failed to open.");
				modid = id;
				/*
				for (int i = 31; i > 27; --i)
				{
					tempid = modid % 10;
					addBuffer[i] = tempid;
					modid = modid % 10;
				}
				*/
				sscanf(rBuffer, "%s %s %s %s", command2, firstName, lastName, phoneNumber);
				fprintf(file, "%d\t%s %s\t%s\n", id, firstName, lastName, phoneNumber);
				send(clientSocket, "200 OK", 8, 0);
				++id;
				++bufferIndex;
				fclose(file);
			}
			else if (command == "LIST")
			{ 
				tempFile = fopen("./addressBook.txt", "rb");
				fseek(tempFile, 0, SEEK_END);
				bufferSize = ftell(tempFile);
				rewind(tempFile);
				listBuffer = (char*) calloc(1, bufferSize + 1);
				fread(listBuffer, bufferSize, 1, tempFile);
				send(clientSocket, listBuffer, bufferSize, 0);
				fclose(tempFile);
			}
			command.clear();
			bufferIndex = 0;
		}
	}
}