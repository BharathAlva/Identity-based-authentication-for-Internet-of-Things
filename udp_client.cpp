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
#define MAC "08:00:27:f9:80:0f"
#define SERIAL_NUMBER "9827365987"
#define NodeKey "alsjeuxm982m!ae3"



using namespace std;
struct requestStruct
{
	int numeric;
	std::string message;
};

string decrypt(char* buf)
{
	//cout<<"BUF "<<buf<<endl;
	ifstream f;
	string key;
	f.open("A.txt");
	getline (f,key);
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[1000];
    unsigned char Finalin_[100];
    unsigned char out_[100];
    cout<<"Decrypt using Client1's identity "<<key<<endl;
    strcpy((char*)userKey_,key.c_str());
    strcpy((char*)in_,buf);
	//cout<<"in_"<<in_<<endl;


    AES_set_decrypt_key(userKey_, 128, &aesKey_);
    AES_decrypt((unsigned char*)buf,Finalin_,&aesKey_);

    //cout<<Finalin_<<endl;

    return string((const char*)Finalin_);
}

string encrypt(string buf)
{
	
	ifstream f;
	f.open("B.txt");
	string key;
	getline(f,key);
	cout<<"Encrypt using Client2's identity "<<key<<endl;
	AES_KEY aesKey_;
    unsigned char userKey_[100];
    unsigned char in_[100];
    unsigned char Finalin_[100];
    unsigned char out_[100];
    strcpy((char*)userKey_,key.c_str());
    strcpy((char*)in_,buf.c_str());


	//cout<<"AES input msg = "<<buf<<endl;
    AES_set_encrypt_key(userKey_, 128, &aesKey_);
    AES_encrypt(in_, out_, &aesKey_);

    //cout<<"AES enc msg = "<< out_<<endl;

    f.close();
    return string((const char*)out_);
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
	//cout<<"in_"<<in_<<endl;


    AES_set_decrypt_key(userKey_, 128, &aesKey_);
    AES_decrypt((unsigned char*)buf,Finalin_,&aesKey_);

    //cout<<Finalin_<<endl;

    return string((const char*)Finalin_);
}

void displayError(char* errorMsg);

int main(int argc, char *argv[])
{
	int n;
	ofstream Afile;
	int socket1;
	struct sockaddr_in server;
	struct sockaddr_in client;
	struct hostent *hn;
	char buf[1000];
	unsigned int size;

	 size = sizeof(struct sockaddr_in);
	 //cout<<"ARGC "<<argc<<endl;
	if (argc < 4)
		displayError("Provide server details");

	string st2(argv[3]);

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

	// 0- message from server, 1 - message from client1
	if (!strcmp("0",st2.c_str()))
	{
		strcpy(buf, str.c_str());
	
	cout<<"Message sent to Server = "<<buf<<endl;
	n = sendto(socket1, buf, sizeof(buf), 0, (const struct sockaddr*)&server, size);
	}
	
	else
	{
		cout<<"Message sent to Server = Client1-Hello"<<endl;
		string st = encrypt("Client1-Hello");
		char buf2[1000];
		bzero(buf2,1000);
		strcpy(buf2,st.c_str());
		buf2[st.size() + 1] = '\0';
		//strcpy(buf, st.c_str());
	
	cout<<"Encrypted message = "<<buf2<<endl;
		n = sendto(socket1, st.c_str(), st.size() + 1, 0, (const struct sockaddr*)&server, size);
	}

	if (n < 0)
		displayError("error while sending message");

	bzero(buf, 1000);
	char buf2[1000];
	//bzero(buf2,1000);
	n = recvfrom(socket1, buf2, sizeof(buf2), 0, (struct sockaddr*)&client, &size);
	if (n < 0)
		displayError("error while receivng message");

	//int bufL = strlen(buf2);
	cout<<"Response from Server "<<buf2<<" Size = "<<strlen(buf2)<<endl;
//	buf2[strlen(buf2)] = '\0';
	string decMsg;
	if (!strcmp("0",st2.c_str()))
	decMsg = aesDecrypt(buf2);
	else
	decMsg = decrypt(buf2);

	cout<<"Decrypted message = "<<decMsg<<endl;
	if (!strcmp("0",st2.c_str()))
	{
		Afile.open("A.txt");
		Afile << decMsg;
		Afile.close();
	}
	
	close(socket1);

	return 0;	
}


void displayError(char* msg)
{
	perror(msg);
	exit(0);
}
