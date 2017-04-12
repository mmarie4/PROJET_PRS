#ifndef CONNEXION_H
#define CONNEXION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define RCVSIZE 1024

void openSocketUDP(int* descripteur);
void editStructurAdress(struct sockaddr_in* structure, int port, int ip);
int bindServer(int* soc, struct sockaddr_in* ptrAdress);

void sendSYN(int* soc, struct sockaddr_in* ptrAdress);
void sendSYNACK(int* soc, struct sockaddr_in* ptrAdress, int new_port);
void sendACK(int* soc, struct sockaddr_in* ptrAdress);
int handShakeServer(int* desc, struct sockaddr_in* ptrAdress, int port_data);
int sendData(int* seq, char buffer[], char purData[], int descData, struct sockaddr_in adressClient, socklen_t adressClientLength);
void receiveFileName(int descData, struct sockaddr_in adressClientData, char fileName[]);

void testArg(int* arg);

void refreshBuffer(char buf[], int size);
void handleError(int val, char* error);

void sendSEQ(int* seq, char bufferImage[], int descClient, struct sockaddr_in adressServer);
void receiveSEQ(int* seq, char bufferImage[], int descData, struct sockaddr_in adressServerData, int* sizeResult);
int receiveACK_Segment(char bufferACK[], int descClient, struct sockaddr_in adressClient, int* sizeResult);

#endif