#include "functions.h"
#include <pthread.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

int desc;

void terminate(){
	close(desc);
	exit(0);
}

int main(int argc, char* argv[]){
	signal(SIGINT, terminate);

	pthread_t thread1;

	//Initialisation
	struct sockaddr_in adressServer;
	struct sockaddr_in adressClient;
	struct sockaddr_in adressData;
	int sizeResult = sizeof(adressData);
	int descData;
	socklen_t adressClientLength;
	int nbClients = 0;
	int port, port_data;
	char buffer[RCVSIZE];
	char bufferACK[11];
	char bufferFIN[4] = "FIN\0";
	int msg, try, ACK;
	FILE* file;
	int nbChar = 0, seq=1;
	struct timeval start, end, RTTtimeval, waiting_time;
	struct timezone tz;

	//Test the number of args
	testArg(&argc);
	port = atoi(argv[1]);
	port_data = port;

	//Socket initialisation
	openSocketUDP(&desc);
	editStructurAdress(&adressServer, port, INADDR_ANY);
	bindServer(&desc, &adressServer);

	while(1){
		if(handShakeServer(&desc,&adressClient, port_data+1) == 1){
			port_data++;
			// Initialize new socket
			openSocketUDP(&descData);
			printf("port_data : %d\n", port_data);
			editStructurAdress(&adressData, port_data, INADDR_ANY);
			bindServer(&descData, &adressData);

			//int pid=fork();
			//if(pid==0){ // Processus fils : send data
			void *thread_1(void *arg)
			{
				int slowStartCounter = 0, cwnd = 1, counterACK=0;
				char purData[RCVSIZE-6];
				char filename[100];
				receiveFileName(descData, adressClient, filename);
				file = fopen(filename, "rb");

				// Calcul file size and send it to the server
				fseek (file , 0 , SEEK_END);
				int size = ftell(file);
				printf("size : %d\n", size);
				fseek(file,0,SEEK_SET);

				// Create file descriptor set in order to test if ACK is received or not
				fd_set set;
				FD_ZERO(&set);

				//initialize RTT just for the first paquet, before real RTT is calculated
				RTTtimeval.tv_usec = 30000;
				RTTtimeval.tv_sec = 0;

				// send paquets
				while((size-nbChar)>(RCVSIZE-6)){
					resetTIMEVAL(&start, &end);
					memset(purData, '\0', 1018);
					memset(buffer, '\0', 1024);
					int res = fread(purData, 1, RCVSIZE-6, file);

					usleep(1);
					startRTT(&start, &tz);
					try = sendData(seq, buffer, purData, descData, adressClient, sizeof(adressClient), RCVSIZE);
					//printf("%d bytes sent\n", try);
					// test reception ACK
					ACK = receiveACK_Segment(bufferACK, descData, adressClient, &sizeResult, set, &RTTtimeval, &waiting_time);
					endRTT(&end, &tz, &start, &RTTtimeval);
					while(ACK==-1){
						// Resend the paquet
						try = sendData(seq, buffer, purData, descData, adressClient, sizeof(adressClient), RCVSIZE);
						ACK = receiveACK_Segment(bufferACK, descData, adressClient, &sizeResult, set, &RTTtimeval, &waiting_time);
					}
					seq++;
					printf("ACK received : %d\n", ACK);
					nbChar+=(try-6);
				}

				// read last part (less than RCSIZE-6) and send it
				int bytesAtEnd = size-nbChar;
				char bufferEnd[bytesAtEnd];
				char bufferEndWithSeq[bytesAtEnd+6];
				int resultat = fread(bufferEnd, 1, bytesAtEnd, file);

				usleep(1);
				try = sendData(seq, bufferEndWithSeq, bufferEnd, descData, adressClient, sizeof(adressClient), (bytesAtEnd+6));

				// test reception ACK
				int sizeResult;
				ACK=receiveACK_Segment(bufferACK, descData, adressClient, &sizeResult, set, &RTTtimeval, &waiting_time);

				while(ACK==-1){
					// Resend the paquet
					try = sendData(seq, bufferEndWithSeq, bufferEnd, descData, adressClient, sizeof(adressClient), (bytesAtEnd+6));
					ACK=receiveACK_Segment(bufferACK, descData, adressClient, &sizeResult, set, &RTTtimeval, &waiting_time);
				}

				printf("ACK received : %d\n", ACK);

				// send 'FIN'
				sendto(descData, bufferFIN, sizeof(bufferFIN), 0, (struct sockaddr*)&adressClient, sizeof(adressClient));
				printf("File sent !\n");

				fflush(file);
				fclose(file);
				close(descData);
				(void) arg;
				pthread_exit(NULL);
			}//else{
				// Processus pere
				//close(descData);
 			//}
 			if (pthread_create(&thread1, NULL, thread_1, NULL))
			{
				perror("pthread_create");
				return EXIT_FAILURE;
			
			}
			if (pthread_join(thread1, NULL))
			{
				perror("pthread_join");
				return EXIT_FAILURE;
			}
			printf("Après la création du thread.\n");
			return EXIT_SUCCESS;
		}
	}
	close(desc);
	return 1;
}
