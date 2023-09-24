#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <assert.h>

#ifdef _WIN32
	#include <windows.h>
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <Wspiapi.h>
#else //for linux
	#include <sys/socket.h>
	#include <stdio.h>
	#include <netdb.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <ifaddrs.h>
	#include <net/if.h>
	#include <stdarg.h>
	#include <string.h>
	#include <fcntl.h>
#endif


_Static_assert(sizeof(char) == 1);

#define SERVER_NAME "127.0.0.1"
#define PORT "12500"


int main() {
	#if _WIN32
		WORD version_wanted = MAKEWORD(1, 1);
		WSADATA wsa_data;
		WSAStartup(version_wanted, &wsa_data); //Initialize Winsock
	#endif

	//----------

	int connection_fd = 0;
	struct addrinfo connection_configs;
	struct addrinfo* server_info;
	char* msg_buff = "Test message";

	memset(&connection_configs, 0, sizeof(struct addrinfo));
	connection_configs.ai_family = AF_INET;
	connection_configs.ai_socktype = SOCK_STREAM;
	connection_configs.ai_protocol = IPPROTO_TCP;

	//get server possible addresses
	getaddrinfo(SERVER_NAME, PORT, &connection_configs, &server_info);
	
	struct addrinfo* addr = 0;

	//try to to connect to one of them
	for (addr = server_info; addr != 0; addr = addr->ai_next)
	{
		connection_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
				
		if ((connect(connection_fd, addr->ai_addr, (int)addr->ai_addrlen)) == -1)
		{
			printf("Error connecting to server\n");
			return -1;
		}

		printf("Successfully conected\n");
	}

	freeaddrinfo(server_info);

	if ((send(connection_fd, msg_buff, (int)strlen(msg_buff), 0)) == -1)
	{
		printf("Error while sending message to server\n");
		return -1;
	}

	//receive the response from the server;
	int buff_size = 128;
	char buff[buff_size];

	#ifdef _WIN32
		int bytes_read = recv(connection_fd, buff, buff_size, 0);
	#else
		int bytes_read = read(connection_fd, buff, buff_size);
	#endif

	if (bytes_read > 0) 
	{
		printf("Received from server: %s\n", buff);
	}
	else
	{
		printf("Failed to receive response from server\n");
		return -1;
	}
	

	printf("Press any key to exit");
	char ch;
	scanf("%c", &ch);
}