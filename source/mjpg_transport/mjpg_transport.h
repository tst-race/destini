/* Version 0 model: Start the streamer before running race-cli --send,
   and start client.py before running race-cli --recv

   This means that checkMJPGStream and startMJPStream are no-ops that
   return OK.  Define the put / get functions to 1) run stcp for
   'put', and 2) examine the .inbound directory for items for the
   named user.

*/

#define MJPG_OK   1
#define MJPG_FAIL 0

#ifdef __cplusplus
extern "C" {
#endif

// Start the MJPG stream by spawning a process that runs streamer.py:
int startMJPGServer(char *video, char *port);

// Start the MJPG stream by spawning a process that runs streamer.py:
int startMJPGClient(char *url);

int stopMJPGServer(int pid);
int stopMJPGClient(int pid);

// Stub: Return 1 if stream is active, 0 otherwise:
int checkMJPGStream(char *url);

// Check to see if the user has a file in .inbound, returns the number
// of bytes found in message:
int getMJPGPostBuf(int index, char *buf, unsigned long buflen, const char *user);
char ** getMJPGPost(int index, const char *user);  

// Invoke stcp to packetize a file and place it into the .outbound
// directory, returns the wait return code (pid?):
int putMJPGPost(int index, char *msg, unsigned long msglen, const char *user);

#ifdef __cplusplus
}
#endif
