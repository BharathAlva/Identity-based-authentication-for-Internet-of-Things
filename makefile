all:
	g++ udp_client2.cpp -o client2 -lcrypto -w
	g++ udp_client.cpp -o client -lcrypto -w
	g++ udp_server.cpp -o server -lcrypto -w
	sudo chmod 777 server.sh
	sudo chmod 777 client.sh



