#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <fstream>
#include "openssl/aes.h"
#define MAC "1E-57-E7-F0-0A-22"
#define SERIAL_NUMBER "9221335947"
#define NodeKey "alsjeuxm982m!ae3"



using namespace std;
struct requestStruct
{
	int numeric;
	std::string message;
};
string encrypt(char* buf)
{
	ifstream f;
	f.open("A.txt");
	string key;
	getline(f,key);
	cout<<"Encrypt using Client1's Identity "<<key<<endl;
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[100];
    unsigned char Finalin_[100];
    unsigned char out_[100];
    strcpy((char*)userKey_,key.c_str());
    strcpy((char*)in_,buf);
//	cout<<"in_   "<<in_<<endl;


    AES_set_encrypt_key(userKey_, 128, &aesKey_);
    AES_encrypt(in_, out_, &aesKey_);
    //Finalin_[strlen(Finalin_)] = '\0';
    //cout<<"AES enc msg = "<< out_<<endl;
    f.close();

    return string((const char*)out_);
}
bool decrypt(char* buf)
{
	ifstream f;
	f.open("B.txt");
	string key;
	getline(f,key);
	cout<<"Decrypt using Client2's Identity "<<key<<endl;
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[1000];
    unsigned char Finalin_[100];
    unsigned char out_[100];
    strcpy((char*)userKey_,key.c_str());
    strcpy((char*)in_,buf);
	//cout<<"in_   "<<in_<<endl;


    AES_set_decrypt_key(userKey_, 128, &aesKey_);
    AES_decrypt((unsigned char*)buf,Finalin_,&aesKey_);
    //Finalin_[strlen(Finalin_)] = '\0';
    cout<<"Decrypted message = "<<Finalin_<<endl;
    f.close();
    return true;
}
string aesDecrypt(char* buf)
{
	//cout<<"BUF "<<buf<<endl;
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[1000];
    unsigned char Finalin_[16];
    unsigned char out_[100];
    strcpy((char*)userKey_,NodeKey);
    strcpy((char*)in_,buf);
	//cout<<"in_  "<<in_<<endl;


    AES_set_decrypt_key(userKey_, 128, &aesKey_);
    AES_decrypt((unsigned char*)buf,Finalin_,&aesKey_);
    //Finalin_[strlen(Finalin_)] = '\0';
    //cout<<Finalin_<<endl;

    return string((const char*)Finalin_);
}

void displayError(char* errorMsg);

int main(int argc, char *argv[])
{	
	//cout<<argv[3]<<" "<<argc<<endl;
	if (argc < 3)
		displayError("Provide server details ");
	
	
	// 0 - client, 1 - server
	string str1;
	if (argc == 3)
	{
		string str2(argv[2]);
		str1 = str2;
	}
	
	else
	{
		string str2(argv[3]);
		str1 = str2;
	}
	//cout<<"STR@ "<<str1<<endl;
	if (!strcmp("0",str1.c_str()))
	{
		//cout<<"Inside client code of client2\n";
		int n;
		ofstream Afile;
		int socket1;
		struct sockaddr_in server;
		struct sockaddr_in client;
		struct hostent *hn;
		char buf[1000];
		unsigned int size;

		 size = sizeof(struct sockaddr_in);
		

		socket1 = socket(AF_INET, SOCK_DGRAM, 0);
		if (socket1 < 0)
			displayError("Error while socket creation");
		
		server.sin_family = AF_INET;
		hn = gethostbyname(argv[1]);
		if (hn == 0)
			displayError("Hostname error");
		
		bcopy((char *)hn->h_addr, (char *)&server.sin_addr, hn->h_length);
		
		server.sin_port = htons(atoi(argv[2]));
		
		bzero(buf, 1000);

		std::string str = MAC;
		str = str + " " + SERIAL_NUMBER;

		strcpy(buf, str.c_str());
		
		cout<<"Message sent to Server = "<<buf<<endl;
		n = sendto(socket1, buf, sizeof(buf), 0, (const struct sockaddr*)&server, size);

		if (n < 0)
			displayError("error while sending message");

		bzero(buf, 1000);
		char buf2[1000];
		//bzero(buf2,1000);
		n = recvfrom(socket1, buf2, sizeof(buf2), 0, (struct sockaddr*)&client, &size);
		if (n < 0)
			displayError("error while receivng message");

		cout<<"Response from Server = "<<endl<<buf2<<" Size = "<<strlen(buf2)<<endl;
	//	buf2[strlen(buf2)] = '\0';

		string decMsg = aesDecrypt(buf2);
		cout<<"Decrypted message = "<<decMsg<<endl;
		Afile.open("B.txt");
		Afile << decMsg;
		Afile.close();
		close(socket1);

		return 0;
	}
	else
	{
//		cout<<"Server function\n";
		int n;
		int socket1;
		int serverSize;
		struct sockaddr_in server;
		struct sockaddr_in client;
		char buf[1000];
		socklen_t clientLength;
		clientLength = sizeof(struct sockaddr_in);

		if (argc < 3)
		{
			displayError("Port number or client/server is not provided\n");
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

			cout<<"Received Message = "<<buf<<endl;

			
			ofstream Sfile;
			
			if (decrypt(buf))
			{
				cout<<"Able to decrypt confiming authenticity of the client\n";
			}
			else
			{
				cout<<"Not able to decypt/unauthenticated client\n";
			}
			cout<<"Message sent to Client1 = ACK"<<endl;
			string encMsg = encrypt("ACK");
			cout<<endl<<"After encryption "<<encMsg<<" Size = "<<encMsg.size()<<endl;
			char buf2[1000];
		bzero(buf2,1000);
		strcpy(buf2,encMsg.c_str());
		buf2[encMsg.size() + 1] = '\0';
			n = sendto(socket1, encMsg.c_str(), encMsg.size() + 1, 0, (struct sockaddr*)&client, clientLength);
			
			if (n < 0)
				displayError("error while sending message");
			
			return 0;

		}
		return 0;
	}
}



void displayError(char* msg)
{
	perror(msg);
	exit(0);
}
