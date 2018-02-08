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

#define SERVER_PORT 1337
#define MAX_PENDING 1
#define MAX_LINE 1000

void sendMessage(int, int, string);

int main(int argcount, char* argvector[])
{


	struct sockaddr_in sockAddress;
	socklen_t addressLength;
	int bufferIndex = 0;
	int bufferLength;
	int serverSocket;
	int clientSocket;
	int id;
	char idCopy[4];
	string idCopy2;
	string compareId;
	string storeId;
	int listCount = 0;
	int tempid = 0;
	int modid = 0;
	char rBuffer[MAX_LINE];
	char sBuffer[MAX_LINE];
	char addBuffer[32] = { '2','0','0',' ','O','K','\n',
		'T','h','e',' ','n','e','w',' ',
		'r','e','c','o','r','d',' ',
		'I','D',' ','i','s',' ' };
	string add;
	string command;
	string line;
	string deleteline;
	FILE* file;
	ifstream findId;
	ifstream inFile;
	ofstream outFile;
	size_t fileSize;
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
			while (rBuffer[bufferIndex] != ' ' && rBuffer[bufferIndex] != '\n')
			{
				command += toupper(rBuffer[bufferIndex]);
				++bufferIndex;
			}

			if (command == "SHUTDOWN")
			{
				sendMessage(clientSocket, 0, command);
				close(clientSocket);
				close(serverSocket);
				exit(1);
			}
			else if (command == "QUIT")
			{
				sendMessage(clientSocket, 0, command);
			}

			else if (command == "ADD")
			{
				// get user id
				findId.open("./addressBook.txt");
				if (findId.fail())
					cout << "ERROR: File failed to open." << endl;
				else
				{
					findId.seekg(0, ios::end);
					fileSize = findId.tellg();
					findId.close();
				}
				if (fileSize == 0)
					id = 1000;
				else
				{
					findId.open("./addressBook.txt");
					if (findId.fail())
						cout << "ERROR: File failed to open." << endl;
					while (!findId.eof())
					{
						findId >> id >> firstName >> lastName >> phoneNumber;
					}
					++id;
					findId.close();
				}
				// open address book file
				file = fopen("./addressBook.txt", "a+");
				if (file == NULL)
					perror("ERROR: The address book file failed to open.");
				
				sscanf(rBuffer, "%s %s %s %s", command2, firstName, lastName, phoneNumber);
				fprintf(file, "%d\t%s %s\t%s\n", id, firstName, lastName, phoneNumber);
				sendMessage(clientSocket, id, command);
				++bufferIndex;
				fclose(file);
			}
			else if (command == "LIST")
			{
				sendMessage(clientSocket, 0, command);
			}
			else if (command == "DELETE")
			{
				inFile.open("./addressBook.txt");
				if (inFile.fail())
					cout << "in file failed." << endl;
				outFile.open("./newFile.txt");
				if (outFile.fail())
					cout << "out file failed." << endl;
				sscanf(rBuffer, "%s %s", command2, idCopy);
				while (!inFile.eof())
				{
					getline(inFile, line);
					for (int i = 0; i < 4; ++i)
					{
						idCopy2 += idCopy[i];
						compareId += line.at(i);
					}
					if (compareId == idCopy2)
					{
						inFile.seekg(0, ios::beg);
						while (!inFile.eof())
						{
							getline(inFile, line);
							if (line.size() > 2)
							{
								for (int i = 0; i < 4; ++i)
								{
									storeId += line.at(i);
								}
								if (storeId != idCopy2)
								{
									outFile << line << endl;
								}
								storeId.clear();
							}
						}
						inFile.close();
						remove("addressBook.txt");
						rename("newFile.txt", "addressBook.txt");
						outFile.close();
						outFile.open("./newFile.txt");
						outFile.close();
						sendMessage(clientSocket, 0, command);
					}
					else
					{
						idCopy2.clear();
						compareId.clear();
					}
				}

			}
			else
			{
				send(clientSocket, "300 invalid command.", 20, 0);
			}
			command.clear();
			bufferIndex = 0;
		}
	}
}

void sendMessage(int socketNum, int id, string request)
{
	char* listBuffer;
	FILE* tempFile;
	int tempId = 0;
	int modularId = 0;
	int bufferSize = 0;
	char addBuffer[33] = { '2','0','0',' ','O','K','\n',
						   'T','h','e',' ','n','e','w',' ',
						   'r','e','c','o','r','d',' ',
						   'I','D',' ','i','s',' ' };

	if (request == "LIST")
	{
		tempFile = fopen("./addressBook.txt", "r");
		fseek(tempFile, 0, SEEK_END);
		bufferSize = ftell(tempFile);
		rewind(tempFile);
		listBuffer = (char*)calloc(1, bufferSize + 1);
		fread(listBuffer, bufferSize, 1, tempFile);
		send(socketNum, listBuffer, bufferSize + 1, 0);
		delete[] listBuffer;
		fclose(tempFile);
	}
	else if (request == "ADD")
	{
		modularId = id;
		for (int i = 31; i > 27; --i)
		{
			tempId = modularId % 10;
			addBuffer[i] = tempId + 48;
			modularId = modularId / 10;
		}
		send(socketNum, addBuffer, 33, 0);
	}
	else if (request == "SHUTDOWN" || request == "QUIT"|| request == "DELETE")
	{
		send(socketNum, "200 OK", 7, 0);
	}

}