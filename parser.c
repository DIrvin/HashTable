/***************************************************
	Programmer Name: Derek Irvin
	FileName: parser.c
	Purpose: Create a separate set of functions that can be used as modular
        code for common C features related to shell processing.
	Description: Basic shell functions
***************************************************/
#include "parser.h"
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

/* Static Variables Referenced only in this file */
static int pipefd[2];
static int background;
void parse(char *cmdline)
{
  char *cmdpart[2];

  pipefd[0] = PARSE_NOPIPE;    /* Init: default is no pipe */

  background = checkbackground(cmdline);
  
  /* Separate into individual commands if there is a pipe symbol. */

  if (strstr(cmdline, "|"))
    pipefd[0] = PARSE_USEPIPE;

  /* Must do the strtok() stuff before calling parse_cmd because
     strtok is used in parse_cmd or the functions parse_cmd calls. */

  cmdpart[0] = strtok(cmdline, "|");
  cmdpart[1] = strtok((char *)NULL, "|");
  parse_cmd(cmdpart[0]);
  if (cmdpart[1]) parse_cmd(cmdpart[1]);
}

/* parse_cmd will do what is necessary to separate out cmdpart and run
   the specified command. */

void parse_cmd(char *cmdpart)
{
  int setoutpipe = 0;        /* TRUE if need to set up output pipe
                   after forking */
  int pid;            /* Set to pid of child process */
  int fd;            /* fd to use for input redirection */

  char *args[MAXARGS + 5];
  char *filename;            /* Filename to use for I/O redirection */

  splitcmd(cmdpart, args);

  if (pipefd[0] == PARSE_USEPIPE) {
    pipe(pipefd);
    setoutpipe = 1;
  }

  pid = fork();
  if (!pid) {            /* child */
    if (setoutpipe) {
      dup2(pipefd[1], 1);    /* connect stdout to pipe if necessary */
    }
    if (!setoutpipe && (pipefd[0] > -1)) {
      /* Need to set up an input pipe. */
      dup2(pipefd[0], 0);
    }

    filename = parseredir('<', args);

    if (filename) {    /* Input redirection */
      fd = open(filename, O_RDONLY);
      if (!fd) {
       fprintf(stderr, "Couldn't redirect from %s", filename);
       exit(255);
      }
      dup2(fd, 0);
    }

    if ((filename = parseredir('>', args))) { /* Output redirection */
      fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      if (!fd) {
       fprintf(stderr, "Couldn't redirect to %s\n", filename);
       exit(255);
      }
      dup2(fd, 1);
    }

    if (!args[0]) {
      fprintf(stderr, "No program name specified.\n");      
      exit(255);
    }
    
    execvp(args[0], args);
    /* If failed, die. */
    exit(255);
  } else {            /* parent */
    if ((!background) &&
       (!setoutpipe))
      waitpid(pid, (int *)NULL, 0);
    else
      if (background)
       fprintf(stderr, "BG process started: %d\n", (int) pid);
    if (pipefd[0] > -1) {    /* Close the pipe if necessary. */
      if (setoutpipe)
        close(pipefd[1]);
      else                     
        close(pipefd[0]);
    }                           
  } /* if (!pid) */
  freeargs(args);
} /* parse_cmd()  */

/* splitcmd() will split a string into its component parts.

   Since splitcmd() uses strdup, freeargs() should be called on the
   args array after it is not used anymore. */

void splitcmd(char *cmdpart, char *args[])
{
  int counter = 0;
  char *tempstr;

  tempstr = strtok(cmdpart, " ");
  args[0] = (char *)NULL;
  while (tempstr && (counter < MAXARGS - 1)) {
    args[counter] = strdup(expandtilde(tempstr));
    args[counter + 1] = (char *)NULL;
    counter++;
    tempstr = strtok(NULL, " ");
  }
  if (tempstr) {         /* Broke out of loop because of num of args */
    fprintf(stderr, "WARNING: argument limit reached, command may be truncated.\n");
  }
}

/* expandtilde() will perform tilde expansion on str if necessary. */

char *expandtilde(char *str)
{
  static char retval[MAXINPUTLINE];
  char tempstr[MAXINPUTLINE];
  char *homedir;
  char *tempptr;
  int counter;
  

  if (str[0] != '~') return str;      /* No tilde -- no expansion. */
  strcpy(tempstr, (str + 1));          /* Make a temporary copy of the string */
  if ((tempstr[0] == '/') || (tempstr[0] == 0))
    tempptr = (char *)NULL;
  else {                  /* Only parse up to a slash */
    /* strtok() cannot be used here because it is being used in the function
      that calls expandtilde().  Therefore, use a simple substitute. */
    if (strstr(tempstr, "/"))
      *(strstr(tempstr, "/")) = 0;
    tempptr = tempstr;
  }
  
  if ((!tempptr) || !tempptr[0]) {    /* Get user's own homedir */
    homedir = gethomedir();
  } else {                  /* Get specified user's homedir */
    homedir = getuserhomedir(tempptr);
  }

  /* Now generate the output string in retval. */

  strcpy(retval, homedir);          /* Put the homedir in there */

  /* Now take care of adding in the rest of the parameter */

  counter = 1;
  while ((str[counter]) && (str[counter] != '/')) counter++;

  strcat(retval, (str + counter));

  return retval;
}
  
/* freeargs will free up the memory that was dynamically allocated for the
   array */
    
void freeargs(char *args[])
{
  int counter = 0;

  while (args[counter]) {
    free(args[counter]);
    counter++;
  }
}

/* Calculates number of arguments in args */

void calcargc(char *args[], int *argc)
{
  *argc = 0;
  while (args[*argc]) {
    (*argc)++;            /* Increment while non-null */
  }
  (*argc)--;            /* Decrement after finding a null */
}
  
/* parseredir will see if it can find a redirection operator oper
   in the array args[], and, if so, it will return the parameter (filename)
   to that operator. */

char *parseredir(char oper, char *args[])
{
  int counter;
  int argc;
  static char retval[MAXINPUTLINE];

  calcargc(args, &argc);

  for (counter = argc; counter >= 0; counter--) {
    fflush(stderr);
    if (args[counter][0] == oper) {
      if (args[counter][1]) {    /* Filename specified without a space */
       strcpy(retval, args[counter] + 1);
       argsdelete(args + counter);
       return retval;
      } else {            /* Space seperates oper from filename */
       if (!args[counter+1]) {    /* Missing filename */
         fprintf(stderr, "Error: operator %c without filename", oper);
         exit(255);
       }
       strcpy(retval, args[counter+1]);
       argsdelete(args + counter + 1);
       argsdelete(args + counter);
       return retval;    
      }
    }
  }
  return NULL;            /* No match */
}

/* Argsdelete will remove a string from the array */

void argsdelete(char *args[])
{
  int counter = 0;
  if (!args[counter]) return;    /* Empty argument list: do nothing */
  free(args[counter]);
  while (args[counter]) {
    args[counter] = args[counter + 1];
    counter++;
  }
}
