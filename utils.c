/**
 * Haha polyfill cheat sorry dear W3.org
 */
#include <ctype.h>
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

#pragma GCC diagnostic ignored "-Wunused-function"

void spth(char *dest, char *input) {
  char pth[PATH_MAX], spth[PATH_MAX] = "", *t;
  int level = 0;

  strcpy(pth, input);

  t = strtok(pth, "/");
  while (t != NULL) {
    if (strcmp(t, "..") == 0) {
      if (level > 0) {
        level--;
        char *last_slash = strrchr(spth, '/');
        if (last_slash != NULL) {
          *last_slash = '\0';
        }
      }
    } else if (strcmp(t, ".") != 0) {
      strcat(spth, "/");
      strcat(spth, t);
      level++;
    }
    t = strtok(NULL, "/");
  }

  strcpy(dest, spth);
}

char *get_mime_type(char *name) {
  char *dot;

  dot = strrchr(name, '.');
  if (dot == (char *)0)
    return "text/plain; charset=UTF-8";
  if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
    return "text/html; charset=UTF-8";
  if (strcmp(dot, ".xhtml") == 0 || strcmp(dot, ".xht") == 0)
    return "application/xhtml+xml; charset=UTF-8";
  if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
    return "image/jpeg";
  if (strcmp(dot, ".gif") == 0)
    return "image/gif";
  if (strcmp(dot, ".png") == 0)
    return "image/png";
  if (strcmp(dot, ".css") == 0)
    return "text/css";
  if (strcmp(dot, ".xml") == 0 || strcmp(dot, ".xsl") == 0)
    return "text/xml; charset=UTF-8";
  if (strcmp(dot, ".au") == 0)
    return "audio/basic";
  if (strcmp(dot, ".wav") == 0)
    return "audio/wav";
  if (strcmp(dot, ".avi") == 0)
    return "video/x-msvideo";
  if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
    return "video/quicktime";
  if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
    return "video/mpeg";
  if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
    return "model/vrml";
  if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
    return "audio/midi";
  if (strcmp(dot, ".mp3") == 0)
    return "audio/mpeg";
  if (strcmp(dot, ".ogg") == 0)
    return "application/ogg";
  if (strcmp(dot, ".pac") == 0)
    return "application/x-ns-proxy-autoconfig";
  return "text/plain; charset=UTF-8";
}

int hexit(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0; /* shouldn't happen, we're guarded by isxdigit() */
}

void strdecode(char *to, char *from) {
  for (; *from != '\0'; ++to, ++from) {
    if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {
      *to = hexit(from[1]) * 16 + hexit(from[2]);
      from += 2;
    } else
      *to = *from;
  }
  *to = '\0';
}

void strencode(char *to, size_t tosize, const char *from) {
  int tolen;

  for (tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from) {
    if (isalnum(*from) || strchr("/_.-~", *from) != (char *)0) {
      *to = *from;
      ++to;
      ++tolen;
    } else {
      (void)sprintf(to, "%%%02x", (int)*from & 0xff);
      to += 3;
      tolen += 3;
    }
  }
  *to = '\0';
}
