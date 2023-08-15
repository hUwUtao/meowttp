/*
 * MeowTTP - A HTTP Server, with even smaller footprint than MicroHTTPd
 * Key feats
 * - Low level file streaming && buffering
 * - Very small
 * - Very fast, around 4ms local just for streaming a hello world!
 * TODO
 * - It should work with continious streaming for medias.
 * */

// standards
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
// system
#include <linux/limits.h> // haha what
#include <time.h>
#include <sys/stat.h>
// socket
#include <sys/socket.h>
#include <netinet/in.h>

// ugh
#define WORKDIR "/www" 
#define BSIZE 128 // reasonable system line right?

void header(int sockfd, unsigned long size){
    time_t rawtime = time(NULL);
    struct tm *tm = gmtime(&rawtime);
    char buf[BSIZE];
    strftime(buf, sizeof buf, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", tm);
    send(sockfd, buf, strlen(buf), 0);
    
    sprintf(buf, "Content-Length: %ld\r\n", size);
    send(sockfd, buf, strlen(buf), 0);

    sprintf(buf, "Server: mttp/%s\r\n", __BUILD__);
    send(sockfd, buf, strlen(buf), 0);

    sprintf(buf, "\r\n"); 
    send(sockfd, buf, strlen(buf), 0);
    return;
}

void handle_request(int sockfd, char *path, struct sockaddr_in addr){
  char buf[BSIZE];
  struct stat sb; 
  FILE *fp = fopen(path, "r");

  if(stat(path, &sb) == -1){
    sprintf(buf, "HTTP/1.1 404 Not Found\r\n"); 
    send(sockfd, buf, strlen(buf), 0);
    header(sockfd, 0);
  }
  // weird?
  else if(fp == NULL){
    sprintf(buf, "HTTP/1.1 403 Forbidden\r\n");
    send(sockfd, buf, strlen(buf), 0);
    header(sockfd, 0);
  }
  else{

    sprintf(buf, "HTTP/1.1 200 OK\r\n");
    send(sockfd, buf, strlen(buf), 0);

    header(sockfd, sb.st_size);
    
    while(fgets(buf, sizeof buf, fp) != NULL){
      send(sockfd, buf, strlen(buf), 0); 
    }

    fclose(fp);
  }
}

void sim(char *path) {
    int n = strlen(path), top = -1;
    char *stack[PATH_MAX];
    for (int i = 0; i < n; i++) {
        if (path[i] == '/') 
            continue;
        if (path[i] == '.') {
            if (i + 1 < n && path[i + 1] == '.') {
                if (top >= 0) {
                    top--;
                }
                i++;
            }
            continue;
        }
        int j = i;
        while (j < n && path[j] != '/') {
            j++;
        }
        char *dir = (char *)malloc(j - i + 1);
        strncpy(dir, path + i, j - i);
        dir[j - i] = '\0';
        stack[++top] = dir;
        i = j;
    }
    if (top == -1) {
        path = "/";
    } else {
        for (int i = 0; i <= top; i++) {
	    strcat(path, "/");
            strcat(path, stack[i]);
            free(stack[i]);
        }
    }
}


void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

int main(){

  int socket_desc , client_sock , c, port;
  struct sockaddr_in server , client;
  char client_message[PATH_MAX + 16], workdir[PATH_MAX]; // pretty standard tbh

  //make sock, why?
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1) {
    printf("can't make a socket, why is that?\r\n");
  }

  //vars
  char *prt = getenv("MTTP_PORT");
  if(prt != NULL) port = strtoul(prt, NULL, 0); else port = 8080;
  char *cwd = getenv("MTTP_ROOT");
  if(cwd) memcpy(workdir, cwd, strlen(cwd)); else getcwd(workdir, sizeof(workdir)); // what could be wooorse?

  //cook
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( port );
   
  unsigned int ret = 1;

  //bind
  while( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0 ){
    printf("bind failed %d\r\n", ret);
    ret++;
    if(ret > 32) return 1;
  }
   
  //hear
  listen(socket_desc , 3);
  c = sizeof(struct sockaddr_in);
  printf("listening on port %d\r\n", port);
  
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) ){
    // pipin
    memset(client_message, '\0', sizeof client_message);

    // it might useful anyway
    recv(client_sock , client_message , 2000 , 0);
    
    // get only
    if(strncmp("GET", client_message, 3) == 0){
      char *path = strtok(client_message, " ");
      path = strtok(NULL, " ");
      //sim(path);

      prepend(path, workdir);
      if(getenv("MTTP_LOG") != NULL) printf("/%s\n", path); // TODO: Implement sockaddr for this (probably not a goverment's request XD)

      handle_request(client_sock, path, client);  
    }

    //close(client_sock);
  }
   
  return 0;
}
