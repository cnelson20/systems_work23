#include "pipe_networking.h"
#include <time.h>

unsigned int child_id = 0;

/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  struct addrinfo *hints, *results;

  printf("[server] Creating socket...\n");

  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
	
  int sd;
  
  getaddrinfo(NULL,"5000",hints,&results);
  if (results == NULL) {
	  printf("Error: getaddrinfo() error\n");
	  free(hints);
	  return -1;
  }
  
  sd = socket(results->ai_family,results->ai_socktype,results->ai_protocol);
  if (sd == -1) {
	  printf("Error: Could not create socket!\n");
	  freeaddrinfo(results);
	  free(hints);
	  return -1;
  }
  printf("[server]: binding socket descriptor %d\n",sd);
  if (bind(sd, results->ai_addr, results->ai_addrlen) == -1) {
	printf("Error: could not bind socket to port\n");
	return -1;
  }
  
  freeaddrinfo(results);
  free(hints);
  return sd;
}

/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int sd) {
	int client_socket;
	socklen_t sock_size;
	struct sockaddr_storage client_address;
	sock_size = sizeof(client_address);
	printf("[server]: waiting to accept connection\n");
	client_socket = accept(sd,(struct sockaddr *)&client_address, &sock_size);
	
	printf("[server]: connection! client_socket = %d\n",client_socket);
	return client_socket;
}

/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  struct addrinfo *hints, *results;

  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
	
  int sd;

  getaddrinfo("localhost","5000",hints,&results);
  if (results == NULL) {
	  printf("Can't find server!\n");
	  free(hints);
	  return -1;
  }
  sd = socket(results->ai_family,results->ai_socktype,results->ai_protocol);
  if (sd == -1) {
	  printf("Could not create socket!\n");
	  freeaddrinfo(results);
	  free(hints);
	  return -1;
  }
  
  if (connect(sd,results->ai_addr,results->ai_addrlen) == -1) {
	printf("Couldn't connect to server\n");
	freeaddrinfo(results);
	free(hints);
	return -1;
  }
  
  freeaddrinfo(results);
  free(hints);
  
  *to_server = sd;
  return sd;
  
}
