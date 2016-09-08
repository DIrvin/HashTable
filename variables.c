#include "parser.h"
#include "shell.h"
#include "hash_table.h"
#include "variables.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

void equal_case(struct Table *t, char *input)
{
	// Variable Declarations for the key and value
	char keyValue[40];
	char valueSquared[40];
	
	// const check case for the = in the string
	const char s[2] = "=";
	char *token;
	
	// Tokenize the input till the = sign to get the Key value from the string
	token = strtok(input, s);
	strcpy(keyValue, token);
	
	// Tokenize the input from NULL to s to find the value from the string
	token = strtok(NULL, s);	
	strcpy(valueSquared, token);	
	
	// Call Table Addition with the table, key value, and the value of the key
	Table_add(t, keyValue, valueSquared);
	return;
}

void read_case(struct Table *t, char *input)
{
	// Same concept as the equal case
	char keyValue[40];
	char valueSquared[40];
	
	// take the key value
	char *token=(char*)malloc(sizeof input);
	token = strtok(input, " ");	// tokenize first for the read
	token = strtok(NULL, " ");	// tokenize again for the key value
	stripcrlf(token);		
	

	strcpy(keyValue, token);	// copy key value to keyValue
	printf("enter the value: ");		
	char *tokenDos = (char*) malloc(sizeof input);	// Get the value input
	if(fgets(tokenDos, 100, stdin) != NULL)	
	{
		stripcrlf(tokenDos);
	}

	strcpy(valueSquared, tokenDos);
	Table_add(t, keyValue, valueSquared);
	return;
}

void search_case(struct Table *t, char *input)
{
	// for breaking down between words
	const char s[2] = " ";
	char *token;

	// the final output after $
	char finalString[100];

	// Set finalString to nothing
	strcpy(finalString, "");

	// tokenize the input 
	token = strtok(input, s);
	
	//while the token value is not Null. 
	while(token != NULL)
	{
		// if the value is $
		if(token[0]=='$')
		{
			// for key value
			char keyValue[40];

			// copy the value of token + 1 for the space
			strcpy(keyValue, token + 1);
			char *valueSquared;

			
			int k;
			for (k = 0; k <strlen(keyValue); k++)	// while we are not at the end of the keyValue length
				if(keyValue[k]=='\n')
					keyValue[k]=0;

			
			Table_search(t, keyValue, &valueSquared);
			int len;
			len = strlen(valueSquared);	

			if(valueSquared[len-1]=='\n')
				valueSquared[len-1]=0;
	
			// add the value to finalString piece by piece
			strcat(finalString, valueSquared);
			strcat(finalString, " ");
		}
		else
		{
			// otherwise just add the string value and a space if there is no $ case
			strcat(finalString, token);
			strcat(finalString, " ");
		}

		token = strtok(NULL, s);
	}
	
	strcpy(input, finalString);	// output the finalString Value
	return;
}
