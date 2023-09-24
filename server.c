#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

#if _WIN32
	#include <winsock2.h>
#else
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

#include <assert.h>

_Static_assert(sizeof(char) == 1);

#define PORT 12500


int main() {
	#if _WIN32
		WORD version_wanted = MAKEWORD(1, 1);
		WSADATA wsa_data;
		WSAStartup(version_wanted, &wsa_data); //Initialize Winsock
	#endif

	//----------

	printf("Starting server\n");

	//create the TCP socket for listening
	struct sockaddr_in server_addr, client_addr;
	int socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	assert(socket_fd != -1);

	memset(&server_addr, 0, sizeof(struct sockaddr_in));

	//configure the server ip and port 
	server_addr.sin_family = AF_INET; //use IPv4
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(PORT);

	bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));

	//listen to the client
	while (1)
	{
		printf("Server listening...\n");

		listen(socket_fd, 1);
	
		int addr_len = sizeof(client_addr);
		int connection_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &addr_len);
		
		if (connection_fd == -1) {
			printf("Error when accepting client request\n");
			return -1;
		}

		printf("Connected to client\n");

		//create a buffer for the client message
		const int buff_size = 128;
		char buff[buff_size];
		memset(buff, '\0', buff_size); 

		//read the received message
		#ifdef _WIN32
			int bytes_read = recv(connection_fd, buff, buff_size, 0);
		#else
			int bytes_read = read(connection_fd, buff, buff_size);
		#endif
		

		//print it if successfully read
		if (bytes_read > 0) {
			buff[bytes_read] = '\0';
			printf("Message from client: %s\n", buff);

			//convert the message to uppercase
			int i;
			for (i = 0; i < strlen(buff); ++i)
				buff[i] = toupper(buff[i]);

			//send message to client
			send(connection_fd, buff, (int)(strlen(buff)) + 1, 0);
		}
		else
		{
			printf("Error reading client message\n");
			return -1;
		}

		#ifdef _WIN32
			closesocket(connection_fd);
		#else
			close(connection_fd);
		#endif


		printf("-----------------\n");
		printf("Terminate server? (y or n):");

		char ch = 'n';
		while ((ch = getchar()) == '\n') {};

		if (ch != 'n')
		{
			printf("Terminating...\n");
			break;
		}
	}
	
	#ifdef _WIN32
		closesocket(socket_fd);
	#else
		close(socket_fd);
	#endif

	
	return 0;
}
