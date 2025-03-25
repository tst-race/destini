//
// Put stuff here to help start the Sanic server for MJPG, as well as
// client-side stuff.

#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "mjpg_transport.h"

#if defined (USE_FORK)
#include "popenRWE.c"
#else
#include "popenPSpawn.c"
#endif

#define UNLINK 1

// size_t file_size
// Video choices:
//    /root/veil/videos/husky.mjpg
//    /root/veil/videos/osaka-full.mjpg

int startMJPGServer(char *video, char *port) {
  int fd;
  int _rwepipe[3];
  char * args[11];

  // fd = open("/tmp/streamer.log", O_CREAT | O_WRONLY, 0777);

  _rwepipe[0] = 0;
  _rwepipe[1] = 1;
  _rwepipe[2] = 2;

  args[0] = "/usr/bin/python";
  args[1] = "/root/streamer/streamer.py";
  args[2] = "--video";
  args[3] = (char *) video;
  args[4] = "--port";
  args[5] = (char *) port;
  args[6] = "--fps";
  args[7] = "10";
  args[8] = "--repeat";
  args[9] = "5";
  args[10] = NULL;

  printf("startMJPGServer: serving video %s on port %s.\n", video, port);
  
  fd = popenRWE(_rwepipe, "/usr/bin/python", args);
  printf("startMJPGServer: popenRWE returns %d.\n", fd);

  return fd;
}

int startMJPGClient(char *URL) {
  int fd;
  int _rwepipe[3];
  char * args[5];

  _rwepipe[0] = 0;
  _rwepipe[1] = 1;
  _rwepipe[2] = 2;

  args[0] = "/usr/bin/python";
  args[1] = "/root/streamer/client.py";
  args[2] = "--url";
  args[3] = URL;
  args[4] = NULL;

  printf("startMJPGClient: polling url %s.\n", URL);

  fd = popenRWE(_rwepipe, "/usr/bin/python", args);
  return fd;
}


#define USE_KILLPG 0


#if USE_KILLPG
int stopMJPGServer(int pid) {
  int rc;
  int pg;

  pg = getpgid(pid);
  rc = killpg(pg, SIGKILL);
  if (rc < 0) 
    perror("stopMJPGServer ");
  return rc;
}


int stopMJPGClient(int pid) {
  int rc;
  int pg;

  pg = getpgid(pid);
  rc = killpg(pg, SIGKILL);
  if (rc < 0) 
    perror("stopMJPGClient ");
  return rc;
}

#else

int stopMJPGServer(int pid) {
  int rc;

  rc = kill(pid, SIGKILL);
  if (rc < 0) 
    perror("stopMJPGServer ");
  else
    printf("Killing Server pid %d\n", pid);
  return rc;
}


int stopMJPGClient(int pid) {
  int rc;

  rc = killpg(pid, SIGKILL);
  if (rc < 0) 
    perror("stopMJPGClient ");
  else
    printf("Killing Client pid %d\n", pid);
  return rc;
}
#endif

int checkMJPGStream(char *url) {
  printf("URL: %s\n", url);
  return MJPG_OK;
}


static size_t message_size(char *file) {
  int rc;
  struct stat info;
  rc = stat(file, &info);
  if (rc < 0) {
    printf("Could not fstat %s\n", file);
    return 0;
  }

  return (size_t) info.st_size;
}


int getMJPGPostBuf(int index, char *buf, unsigned long buflen, const char *user) {
  struct stat info;
  int fd;
  int rc;
  char *fname = ".inbound/0001____";
  size_t nbytes;
  printf("Index: %d; user: %s\n", index, user);

  nbytes = message_size(fname);

  if (nbytes == 0) {
    printf("getMJPGPostBuf: no message found.\n");
    return 0;
  }
  
  if (nbytes >= buflen) {
    printf("buflen (%ld) too small (< %ld).  Truncating message.\n", buflen, (unsigned long) info.st_size);
    nbytes = (unsigned long) buflen-1;
  }

  printf("Reading %ld bytes.\n", (long) nbytes);

  if (nbytes > 0) {

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
      printf("Could not open %s\n", fname);
      return 0;
    }

    buf[nbytes] = 0;
    rc = read(fd, buf, nbytes);
    printf("getMJPGPostBuf: read returns %d.\n", rc);
    close(fd);
  }
#if UNLINK
  rc = unlink(fname);
  printf("unlink(%s) returns %d\n", fname, rc);
#endif
  return (int) nbytes;
}

// Horrible model, as it precludes the appearance of NUL in the
// message:
char ** getMJPGPost(int index, const char *user) {
  unsigned long nbytes;
  int n;
  // char fname[512];
  // Hardwired for now, but wrong in general:
  char *fname = ".inbound/0001____";
  char **blist;
  char *buf;

  blist = malloc(sizeof(char*));
  blist[0] = NULL;

  nbytes = message_size(fname);
  if (nbytes == 0) {
    printf("In getMJPGPost, no message found.\n");
    return blist;
  }

  buf = malloc(nbytes+32);
  n = getMJPGPostBuf(index, buf, nbytes+32, user);
  if (n > 0) {
    blist[0] = buf;
    printf("\n\n=====>>>>>   getMJPGPost found a message:\n%s\n========\n\n", buf);
  }
  
  printf("In getMJPGPost, got %d bytes.\n", n);
  return blist;
}
  


int putMJPGPost(int index, char *msg, unsigned long msglen, const char *user) {
  int fd, pid, rc;
  int _rwepipe[3];
  char * args[5];
  char * tmpfile;

  printf("putMJPGPost:  Index %d, user %s\n", index, user);
  tmpfile = malloc(64);

  // Must be improved:
  sprintf(tmpfile, "/tmp/%04d.txt", index);
  printf("Saving message into %s\n", tmpfile);
  fd = open(tmpfile, O_CREAT | O_WRONLY);
  printf("open returns %d\n", fd);
  rc = write(fd, msg, (size_t) msglen);
  close(fd);
  printf("write returned %d\n", rc);

  _rwepipe[0] = 0;
  _rwepipe[1] = 1;
  _rwepipe[2] = 2;

  args[0] = "/usr/bin/python";
  args[1] = "/root/streamer/stcp";
  args[2] = tmpfile;
  args[3] = NULL;
  args[4] = NULL;

  sleep(15);
  pid = popenRWE(_rwepipe, "/usr/bin/python", args);
  rc = waitpid(pid, NULL, 0);
  // Hack: introduces latency to allow the streamer time to pick up
  // the chunks and send them:
  sleep(15);
  return rc;
}


/* ---------------------------------------------------------------------- */

#ifdef TEST2

// gcc -o mt -DTEST2 mjpg_transport.c

/* This version (-DTEST2) tests cross-user posting and retrieval of
 * media (jpg or video)
 */

int main(int argc, char **argv) {
  int streamer_pid, client_pid, rc, nbytes, i;
  char port[32];
  char url[256];
  //char out[512];
  char **out;
  char * message = "==>>  Now is the time, the time is now!\n";
  int msglen = strlen(message);

  char *video = "/root/veil/videos/husky.mjpg";
  if (argc > 1)
    sprintf(port, "%s", argv[1]);
  else
    sprintf(port, "%d", 8080);

  sprintf(url, "http://localhost:%s/streamer", port);

  streamer_pid = startMJPGServer(video, port);
  printf("Streamer pid = %d\n", streamer_pid);

  // For testing, just use the same URL:
  client_pid = startMJPGClient(url);
  printf("client pid = %d\n", client_pid);

  sleep(10);
  

  rc = putMJPGPost(1, message, msglen, "Fred");
  printf("putMJPGPost returns %d\n", rc);
  // out should be properly null terminated:
  i = 0;
  out = NULL;
  while (i < 10 && out == NULL) {
    out = getMJPGPost(1, "Fred");
    i++;
    sleep(3);
  }

  if (out) {
    printf("Message:\n%s", out[0]);
  }

  printf("Quitting...\n");

  // These will kill the current process as well.  Need a better way
  // to tell the subprocess to exit:
  rc = stopMJPGServer(streamer_pid);
  printf("stopMJPGServer returns %d\n", rc);

  rc = stopMJPGClient(client_pid);
  printf("stopMJPGClient returns %d\n", rc);
}

#endif
