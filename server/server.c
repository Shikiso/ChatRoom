#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "memoryHandle.c"
#define PORT 3333

typedef struct {
  int clientSocket;
  char address[16];
} client_struct;

void sendAll(int clientSocket, char *username, char *text, int text_size){
 for (int i=0; i<maxAmountOfClientsStored; i++){
      if (allClients[i] != 0 && allClients[i] != clientSocket){
        send(allClients[i], username, sizeof(username), 0);
        sleep(0.3);
        send(allClients[i], text, text_size, 0);
        sleep(0.3);  
    }
  }
}

void* handleClient(void* args){
  char IpAddress[16];
  client_struct *client_args = args;
  int clientSocket = client_args->clientSocket;
  strncpy(IpAddress, client_args->address, 15);
  free(client_args);

  char username[usernameSize];
  recv(clientSocket, username, usernameSize, 0);
  
  sendAll(clientSocket, "Server", "User has joined!\n", sizeof("User has joined!\n"));

  addClientToMemory(clientSocket, username, IpAddress);

  char buffer[1000];
  char lastMsg[1000];

  while (clientSocket){
    recv(clientSocket, buffer, 1000, 0);
    if (!strcmp(buffer, lastMsg)){
      continue;
    }

    if (!strcmp(buffer, "exit\n")){
      sendAll(clientSocket, "Server", "User has left!\n", sizeof("User has left!\n"));
      close(clientSocket);
      removeClientFromMemory(clientSocket);
      break;
    }

    sendAll(clientSocket, username, buffer, sizeof(buffer));
    strcpy(lastMsg, buffer);
  }
  return NULL;
}

int main(int argc, char *argv[]){
  allClients = calloc(maxAmountOfClientsStored, sizeof(*allClients));
  allUsernames = (char**)calloc(maxAmountOfClientsStored, sizeof(char*));
  allAddresses = (char**)calloc(maxAmountOfClientsStored, sizeof(char*));
  
  int server_fd, clientSocket;
  struct sockaddr_in address;
  socklen_t addrlen = sizeof(address);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0){
    perror("Failed to create socket!\n");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
    perror("Failed to bind to host to socket!\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0){
    perror("Failed to listen for connection!\n");
    exit(EXIT_FAILURE);
  }

  while (1){
    clientSocket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    client_struct *args = malloc(sizeof(*args));
    args->clientSocket = clientSocket;
    inet_ntop(AF_INET, &(address.sin_addr), args->address, sizeof(args->address));

    pthread_t clientThread;
    pthread_create(&clientThread, NULL, handleClient, args);
  }
  close(server_fd);

  return 0;
}
