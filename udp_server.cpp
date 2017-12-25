#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <iostream>
#include <time.h>
#include <cstring>
#include "openssl/aes.h"
#include <fstream>


#define true 1
#define NodeKey "alsjeuxm982m!ae3"


using namespace std;
void displayError(char* errorMsg);


struct requestStruct
{
	int numeric;
	std::string message;
};


int generateRandom(int min, int max)
{
   static bool first = true;
   if ( first ) 
   {  
      srand(time(NULL)); 
      first = false;
   }
   return min + rand() % (max - min);
}

string aesEncrypt(string buf)
{
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[100];
    unsigned char Finalin_[100];
    unsigned char out_[100];
    strcpy((char*)userKey_,NodeKey);
    strcpy((char*)in_,buf.c_str());

    //fprintf(stdout,"Original message: %s\n", in_);
	//cout<<"AES input msg = "<<buf<<endl;
    AES_set_encrypt_key(userKey_, 128, &aesKey_);
    AES_encrypt(in_, out_, &aesKey_);
    //fprintf(stdout,"Encrypted message: %s\n", out_);
    cout<<"Encrypted message = "<< out_<<endl;

    return string((const char*)out_);
}


std::string generateIdentity(std::string MAC, std::string serial)
{
	int macLength = strlen(MAC.c_str());
	int serialLength = strlen(serial.c_str());

	int l;
	int bl;
	bool flag = false;
	if (macLength == serialLength)
		l = bl = macLength;
	else if (macLength < serialLength)
	{
		l = macLength;
		bl = serialLength;
		flag = true; // serialLength is longer
	}
	else
		l = serialLength;

	char macBuf[1000];
	char serialBuf[1000];
	char idBuf[1000];
	strcpy(macBuf, MAC.c_str());
	strcpy(serialBuf, serial.c_str());
	int i;
	/*for (i = 0; i <= l; i++)
	{
		idBuf[i] =itoa(atoi(macBuf[i]) + atoi(serialBuf[i]) + (rand() % 10));
		
	}*/

	// for 128 bits
	i = 0;
	while ( i != 16)
	{
		//srand(getpid());
		//int k = (rand() % 13);
		int k = generateRandom(0,12);
		//cout<<k<<endl;
		int q = ((int)(macBuf[k]) + (int)(serialBuf[k]));
		
		while (q > 127 || q < 32)
			q = rand() % 100;
		idBuf[i++] = (char)q;
		//cout<<q<< " " << (char)q << " ";
	}
	//cout<<endl;
	idBuf[i] = '\0';
	return string(idBuf);
}


int main(int argc, char* argv[])
{
	int n;
	int socket1;
	int serverSize;
	struct sockaddr_in server;
	struct sockaddr_in client;
	char buf[1000];
	socklen_t clientLength;
	clientLength = sizeof(struct sockaddr_in);

	if (argc < 2)
	{
		displayError("Port number is not provided\n");
	}

	socket1 = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (socket1 < 0)
		displayError("Error in opening socket");

	serverSize = sizeof(server);
	// Initialize the server structure with 0
	bzero(&server, serverSize);
	

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[1]));

	if (bind(socket1, (struct sockaddr*)&server, serverSize) < 0)
		displayError("Error while binding");
	
	while (true)
	{
		requestStruct req;
		n = recvfrom(socket1, buf, 1000, 0, (struct sockaddr*)&client, &clientLength);

		if (n < 0)
			displayError("Error while receving data");

		cout<<"Received Message : "<<buf<<endl;

		
	 	char* str = strtok(buf, " ");
		bool flag1 = true;
		char* arr[2];
		string mac;
		string serial;
		while (str != NULL)
		{
			//cout<<str<<endl;
			if (flag1)
			{
				mac  =  std::string(str);
	//			cout<<"MAC "<<mac<<endl;
				flag1 = false;
			}
			else 
				serial = std::string(str);
			str = strtok(NULL, " ");
		}
		/*cout<<"DEBUG"<<endl;
		cout<<mac<<" "<<serial<<endl;*/
		string id = generateIdentity(mac,serial);
		cout<<"Generated Identity = "<<id<<" Identity length = "<<id.size()<<endl;
		
		ofstream Sfile;
		/*Sfile.open("S.txt");
		
		Sfile << id << " " << mac << " " << serial;
		Sfile.close();*/
		//write(1,buf,strlen(buf));
		string encMsg = aesEncrypt(id);
		//cout<<"After encryption "<<encMsg<<" "<<encMsg.size()<<endl;
		char buf2[1000];
		bzero(buf2,1000);
		strcpy(buf2,encMsg.c_str());
		buf2[encMsg.size() + 1] = '\0';
		//cout<<"Buf2 "<<buf2<<" "<<strlen(buf2)<<endl;
		n = sendto(socket1, buf2, strlen(buf2), 0, (struct sockaddr*)&client, clientLength);
		
		if (n < 0)
			displayError("error while sending message");
		
		return 0;

	}
	return 0;
}


void displayError(char* errorMsg)
{
	perror(errorMsg);
	exit(0);
}
