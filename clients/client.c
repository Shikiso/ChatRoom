#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 3333

int client_fd;

void *recieveMessages(void* args){
  char buffer[1000];
  char senderUsername[20];
  while (client_fd){
    recv(client_fd, senderUsername, sizeof(senderUsername), 0);
    recv(client_fd, buffer, sizeof(buffer), 0);
    printf("[%s] %s\n", senderUsername, buffer);
  }
}

int main(int argc, char *argv[]){
  struct sockaddr_in server_addr;

  client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd < 0){
    printf("\nFailed to create socket!\n");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0){
    printf("\nInvalid address!\n");
    return -1;
  }

  if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
    printf("\nFailed to connect!\n");
    return -1;
  }

  char username[20];
  printf("Enter your username: ");
  fgets(username, sizeof(username), stdin);
  username[strlen(username)-1] = 0;

  send(client_fd, username, sizeof(username), 0);
  sleep(0.5);
  
  pthread_t recieverThread;
  pthread_create(&recieverThread, NULL, recieveMessages, NULL);

  char buffer[1000];
  char recievedUsername[20];
  while(client_fd){
    fgets(buffer, sizeof(buffer), stdin);

    send(client_fd, buffer, sizeof(buffer), 0);
    
    if (!strcmp(buffer, "exit\n")){
      close(client_fd);
      break;
    }
  }

  return 0;
}
