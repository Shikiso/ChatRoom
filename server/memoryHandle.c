// allClients and allUsernames will share indexs with the corosponding clients
int *allClients = NULL;
char **allUsernames = NULL;
char **allAddresses = NULL;

// Where the next data is to be stored
int clientPos = 0; // Both client and username use this
int maxAmountOfClientsStored = 2;
int memoryIncreaseSize = 2;
int usernameSize = 20;

int alteringClientMemory = 0;

void alterClientMemorySize(){
  allClients = realloc(allClients, maxAmountOfClientsStored * sizeof(*allClients));
  allUsernames = realloc(allUsernames, maxAmountOfClientsStored * sizeof(*allUsernames));
}

void addClientToMemory(int clientSocket, char *username, char *IpAddress){
  // In place so that memory is only altered by one client at a time
  while (alteringClientMemory){
    sleep(0.1);
  }
  alteringClientMemory = 1;

  // Checking if enough memory has been allocated
  if (clientPos == maxAmountOfClientsStored){
    maxAmountOfClientsStored = maxAmountOfClientsStored + memoryIncreaseSize;
    alterClientMemorySize();
  }

  allClients[clientPos] = clientSocket;
  allUsernames[clientPos] = (char*)calloc(usernameSize+1, sizeof(char));
  allAddresses[clientPos] = (char*)calloc(16, sizeof(char));
  
  // Last case method to ensure no memory is overwritten
  if (strlen(username) > usernameSize){
    sprintf(allUsernames[clientPos], "User");
  }
  else{
    sprintf(allUsernames[clientPos], "%s", username);
  }

  sprintf(allAddresses[clientPos], "%s", IpAddress);
  clientPos++;
  alteringClientMemory = 0;
}

void removeClientFromMemory(int clientSocket){
  int index;
  for (int i=0; i<maxAmountOfClientsStored; i++){
    if (allClients[i] == clientSocket){
      index = i;
    }
  }

  allClients[index] = 0;
  allAddresses[index] = "000.000.000.000"; // Don't want addresses saved for no reason
  clientPos--;

  // Freeing up memory thats not needed (leaves 2 positions free)
  if (maxAmountOfClientsStored - clientPos > 2){
    maxAmountOfClientsStored = memoryIncreaseSize;
    alterClientMemorySize();
  }
}

