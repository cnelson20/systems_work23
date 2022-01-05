#define BASIC_SERVER_C
#include "pipe_networking.h"

extern unsigned int child_id;

int strtoupper(char *string) {
	while (*string) {
		if (*string >= 'a' && *string <= 'z') {
		*string -= ('a' - 'A');
		}
		string++;
	}
}

int main() {
	
	int listen_socket;
	int to_client, from_client;
	
	 
	listen_socket = server_setup();
	printf("[server]: setup listen on listen_socket %d\n",listen_socket);
	if (listen_socket == -1) {
		printf("[server]: error: Could not create socket\n");
		return 1;
	}
	if (listen(listen_socket,0) == -1) {
		printf("Could not create socket\n");
		return 1;
	}
	
	while (1) {
		if (from_client != 0) {
			if (!fork()) {
				child_id++;
				break;
			}
			close(from_client);
		}
		
		from_client = server_connect(listen_socket);
	}
	// Subserver part 
	printf("[subserver %u]: init\n",child_id);
	int data_length;
	char *data_to_process;
	
	to_client = from_client;	
	while (1) {
		if (read(from_client,&data_length,sizeof(int)) == 0) {break;}
		data_to_process = malloc(data_length);
		printf("[subserver %u]: about to read %d bytes\n",child_id,data_length);
		if (read(from_client,data_to_process,data_length) == 0) {
			free(data_to_process);
			break;
		}
		printf("[subserver %u]: recieved data '%s'\n",child_id,data_to_process);
		strtoupper(data_to_process);
		printf("[subserver %u]: sending response '%s'\n",child_id,data_to_process);
		write(to_client,&data_length,sizeof(int));
		write(to_client,data_to_process,data_length);
	
		free(data_to_process);
	}
	printf("[subserver %u]: client terminated connection, exiting\n",child_id);
}
