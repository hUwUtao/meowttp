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
#include <sys/stat.h>
#include <time.h>
// socket
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "utils.h"

// ugh
#ifdef __BUILD__
#else
#define __BUILD__ "undefined"
#endif
#define BSIZE 8192

// macros for chaos
#define snd send(sck, buf, strlen(buf), 0);
#define header(size) send_header(sck, size);

#pragma GCC diagnostic ignored "-Wformat-extra-args"
#define bprintf(...)                                                           \
  sprintf(buf, __VA_ARGS__);                                                   \
  snd;
#define logup if (getenv("MTTP_LOG") != NULL)

void send_header(int sck, unsigned long size) {
  time_t rawtime = time(NULL);
  struct tm *tm = gmtime(&rawtime);
  char buf[BSIZE];

  strftime(buf, BSIZE, "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", tm);
  snd;
  bprintf("Content-Length: %ld\r\n", size);
  bprintf("Server: mttp/%s\r\n", __BUILD__);
  bprintf("Connection: close\r\n");
}

void handle_request(int sck, char *path, struct sockaddr_in addr) {
  char buf[BSIZE];
  struct stat sb;
  FILE *fp = fopen(path, "r");

  if (stat(path, &sb) == -1) {
    bprintf("HTTP/1.1 404 Not Found\r\n");
    header(0);
  }
  // weird?
  else if (fp == NULL || !(sb.st_mode & __S_IFREG)) {
    bprintf("HTTP/1.1 403 Forbidden\r\n");
    header(0);
  } else {

    bprintf("HTTP/1.1 200 OK\r\n");

    bprintf("Content-Type: %s\r\n", get_mime_type(path));
    header(sb.st_size);

    bprintf("\r\n");

    while (fread(buf, 1, BSIZE, fp)) {
      send(sck, buf, BSIZE, 0);
      logup printf("BUFFERING at %ld\r\n", ftell(fp));
    }

    fclose(fp);
  }
}

void prepend(char *s, const char *t) {
  size_t len = strlen(t);
  memmove(s + len, s, strlen(s) + 1);
  memcpy(s, t, len);
}

int main() {

  int socket_desc, client_sock, c, port;
  struct sockaddr_in server, client;
  char client_message[PATH_MAX + 16], workdir[PATH_MAX]; // pretty standard tbh

  // make sock, why?
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_desc == -1) {
    printf("can't make a socket, why is that?\r\n");
  }

  // vars
  char *prt = getenv("MTTP_PORT");
  if (prt != NULL)
    port = strtoul(prt, NULL, 0);
  else
    port = 8080;
  char *cwd = getenv("MTTP_ROOT");
  if (cwd)
    memcpy(workdir, cwd, strlen(cwd));
  else
    getcwd(workdir, sizeof(workdir)); // what could be wooorse?

  // cook
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);

  unsigned int ret = 1;

  // bind
  while (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("bind failed %d times\r\n", ret);
    ret++;
    if (ret > 32)
      return 1;
  }

  // hear
  listen(socket_desc, 3);
  c = sizeof(struct sockaddr_in);
  printf("listening on port %d\r\n", port);

  while ((client_sock = accept(socket_desc, (struct sockaddr *)&client,
                               (socklen_t *)&c))) {
    // pipin
    memset(client_message, '\0', sizeof client_message);

    recv(client_sock, client_message, PATH_MAX, 0);

    // get only
    if (strncmp("GET", client_message, 3) == 0) {
      char *path = strtok(client_message, " ");
      path = strtok(NULL, " ");
      logup printf("%s//%s\n", inet_ntoa(client.sin_addr), path);
      strdecode(path, path); // not to chaos later
      spth(path, path);

      prepend(path, workdir);

      handle_request(client_sock, path, client);
    }

    // close(client_sock);
  }

  return 0;
}
