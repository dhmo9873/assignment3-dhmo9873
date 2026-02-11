//CODE USED FROM - https://beej.us/guide/bgnet/html/#acceptthank-you-for-calling-port-3490.
//https://www.devzery.com/post/your-ultimate-guide-to-c-handlers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "9000"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue holds

//int exit_flag = 0
volatile sig_atomic_t exit_flag = 0;;

void handle_signal(int sig) {

	printf("Caught signal %d\n", sig);
	exit_flag = 1;
}

int main(void)
{

	struct sigaction sa;
	sa.sa_handler = handle_signal;
	sa.sa_flags = 0; // or SA_RESTART
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction SIGINT");
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sa, NULL) == -1) {
		perror("sigaction SIGTERM");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int sockfd, new_fd, write_fd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;  
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;     

	int status = getaddrinfo(NULL, MYPORT, &hints, &res);
	if (status != 0){
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(status));
		return -1;
	}
	// make a socket, bind it, and listen on it:

	sockfd = socket(res->ai_family, res->ai_socktype,res->ai_protocol);
	if (sockfd < 0){
		perror("socket");
		return -1;
	}

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0){
		perror("bind");
		return -1;
	}

	if (listen(sockfd, BACKLOG) < 0){
		perror("socket");
		return -1;
	}
	freeaddrinfo(res);

	// now accept an incoming connection:

	addr_size = sizeof (their_addr);
	while (!exit_flag) {
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&addr_size);

		if (new_fd < 0) {
			perror("accept");
			return -1;
		}

		char buffer[1024];
		char *packet = NULL;
		int packet_len = 0;

		while (!exit_flag) {
			int bytes = recv(new_fd, buffer, sizeof(buffer), 0);
			if (bytes <= 0)
				break;

			char *temp = realloc(packet, packet_len + bytes);
			if (!temp){
				free(packet);
				packet = NULL;
				break;
			}
			packet = temp;
			memcpy(packet + packet_len, buffer, bytes);
			packet_len += bytes;

			if (memchr(packet, '\n', packet_len) != NULL)
			{
				/* Append to file */
				int fd = open("/var/tmp/aesdsocketdata", O_WRONLY | O_CREAT | O_APPEND, 0644);
				if (fd >= 0) {
					write(fd, packet, packet_len);
					close(fd);
				}

				/* Send full file back */
				fd = open("/var/tmp/aesdsocketdata", O_RDONLY);
				if (fd >= 0) {
					ssize_t r;
					while ((r = read(fd, buffer, sizeof(buffer))) > 0) {
						send(new_fd, buffer, r, 0);
					}
					close(fd);
				}

				free(packet);
				packet = NULL;
				packet_len = 0;
			}
		}
		free(packet);
		close(new_fd);
	}
	close(sockfd);
	if (remove("/var/tmp/aesdsocketdata") == 0) {
        printf("File '%s' deleted successfully.\n", "/var/tmp/aesdsocketdata");
    } else {
        fprintf(stderr, "Error deleting file: ");
        perror(""); 
    }

	return 0;
}	
