/***************************************************
        Programmer: Derek Irvin
	FileName: sshell.c
	Purpose: Execute all unix shell commands through the executable file.
	Description: Ties in all the individual pieces of the shell utility
        that we are creating.
***************************************************/

#include "parser.h"
#include "shell.h"
#include "hash_table.h"	// contains table creation and variables
#include "variables.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main(void){
  char input[MAXINPUTLINE]; // User Input
  char last[MAXINPUTLINE]; // Last user command for !! case

  signal_c_init();

  printf("Welcome to the sample shell!  You may enter commands here, one\n");
  printf("per line.  When you're finished, press Ctrl+D on a line by\n");
  printf("itself.  I understand basic commands and arguments separated by\n");
  printf("spaces, redirection with < and >, up to two commands joined\n");
  printf("by a pipe, tilde expansion, and background commands with &.\n\n");
  
  printf("dish\n$ "); /* Print out D(derek) I(irvin) S(Simple) H(Shell) */

// Code For Assignment 5

// Table initialization
// Make sure it is out of the while loop or you are pretty much screwed
struct Table *t;
t = Table_create();

while(fgets(input, sizeof(input), stdin))
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assignment 4 Code
	// History Code Test if the user's input is history and if so open the file and print out the string of commands in order
	if (strstr(input, "history") !=NULL){
		FILE* file=fopen(".simpleshell_history", "a+");	// Open the simpleshell history file
		char c[MAXINPUTLINE];
		int i = 1; 	// Counter for displaying the comments
		while(fgets(c, 10240, file) !=NULL) {	// while there are still values in the history take the value into C and print to the string
			printf( "%d %s", i, c);
			i++;	// increase value of 1 to represent command we are on
		}
		fputs(input, file);	// put the value of f into input
		strcpy(last, input);	// copy the value of input to last for the !! case scenario
		fclose(file);		// trivial, close the file
	}

   // !# Case of input
    if (input[0] == '!' && isdigit(input[1]))
    {
 	char *temp = input + 1;
	FILE* file=fopen(".simpleshell_history", "r");
	char t[MAXINPUTLINE];
	int i;
	int k = atoi(temp);
	
	for(i = 0; fgets(t, 10240, file); i++)
	{
		if(i == k)
		{
			strcpy(input, t);
			break;
		}
	}

	printf("%s", input);

        fclose(file);
    }

   // !!
   if(input[0] == '!' && input[1] == '!')
   {
	strcpy(input, last);
	printf("%s", input);
   }
   else if(input[0] == '!' && isalpha(input[1]))
   {
 	int i;
	char j[MAXINPUTLINE];
	FILE* file=fopen(".simpleshell_history", "a+");
	for( i = 0; fgets(j, 10240, file); i++)
	{
		if(input[1]==j[0] && input[2]==j[1])
		{
			strcpy(input, j);
			break;
		}
	}
     }

    else
    {
	FILE* file=fopen(".simpleshell_history", "a+");
	fputs(input, file);
	strcpy(last, input);
	fclose(file);
     }	

// Assignment 5 Code

// "=" Case
// Strstr scans the user input string searching for the second string "="

     if(strstr(input,"=")) //check the input to see if we are assigning a value to the hash table
     {
	equal_case(t, input);
 
     }

// "read" Case

    else if (strstr(input, "read "))	// compare the user's input for "read " 
    {
	read_case(t, input);
    }

// '$' inputcase check the string for $ 
   else if (strstr(input, "$"))
   {
	search_case(t, input);
   }

   else	// if it does not meet any of these then add it to the history file. 
   {
		FILE *f=fopen(".simpleshell_history", "a+");
		fputs(input, f);
		strcpy(last, input);
		fclose(f);
   }
	stripcrlf(input);
	parse(input);
	printf("\ndish$");

}
  Table_free(t);	// FREEDOM!!!
  return 0;

}
