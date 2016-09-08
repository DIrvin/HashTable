/***************************************************
Programmer Name: Derek Irvin
FileName: shutil.c
Purpose: Provide additional necessary utilities for the main processing shell
commands
Description: Contains additional necessary functions necessary to creating
a working shell from the executable.
***************************************************/
#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>

void stripcrlf(char *temp)
{
  while (temp[0] && 
        ((temp[strlen(temp)-1] == 13) || (temp[strlen(temp)-1] == 10))) {
    temp[strlen(temp)-1] = 0;
  }
}

char *gethomedir(void)
{
  static char homedir[_POSIX_PATH_MAX * 2]; /* Just to be safe. */
  struct passwd *pws;

  pws = getpwuid(getuid());
  if (!pws) {
    fprintf(stderr, "getpwuid() on %d failed", (int) getuid());
    exit(255);
  }

  strcpy(homedir, pws->pw_dir);
  return homedir;
}

char *getuserhomedir(char *user)
{
  static char homedir[_POSIX_PATH_MAX * 2]; /* Just to be safe. */
  struct passwd *pws;

  pws = getpwnam(user);
  if (!pws) {
    fprintf(stderr, "getpwnam() on %s failed", user);
    exit(255);
  }

  strcpy(homedir, pws->pw_dir);
  return homedir;
}

void signal_c_init(void)
{
  struct sigaction act;
  
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;

  act.sa_handler = (void *)waitchildren;
  sigaction(SIGCHLD, &act, NULL);
}

void waitchildren(int signum)
{
  while (wait3((int *)NULL,
               WNOHANG,
               (struct rusage *)NULL) > 0) {}
}

/* Check to see whether or not we should run in background */

int checkbackground(char *cmdline)
{
  /* First, strip off any trailing spaces (this has not yet been run
     through strtok) */

  striptrailingchar(cmdline, ' ');

  /* We are looking for an ampersand at the end of the command. */

  if (cmdline[strlen(cmdline)-1] == '&') {
    cmdline[strlen(cmdline)-1] = 0; /* Remove the ampersand from the command */
    return 1;            /* Indicate that this is background mode */
  }
  return 0;
}

void striptrailingchar(char *temp, char tc)
{
  while (temp[0] && (temp[strlen(temp)-1] == tc)) {
    temp[strlen(temp)-1] = 0;
  }
}
