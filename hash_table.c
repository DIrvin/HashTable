/***********************************************************************************
	Programmed By: DEREK IRVIN
	Professor: Busovaca
	Class: CSC60
	FileName: hash_table.c
*****************************************************************************/

#include "hash_table.h"
#include "variables.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>

// Hash Table Creation
enum {BUCKET_COUNT = 1024};

struct Node
{
	char *key;	// Key Value
	char *value;	// Value... Value
	struct Node *next;
};

struct Table
{
	 struct Node *array[BUCKET_COUNT];
};

// Table Creation
struct Table *Table_create(void)
{
	struct Table *t;
	t=(struct Table*)
		calloc(1, sizeof(struct Table));
	return t;
};

unsigned int hash(const char *x)
{
	int i;
	unsigned int h = 0U;
	for(i = 0; x[i]!='\0'; i++)
		h = h*65599 + (unsigned char)x[i];
	return h % 1024;
}

// Hash Table Addition

void Table_add(struct Table *t,char *key, char *value)
{
	struct Node *p = (struct Node*)malloc(sizeof(struct Node));
	int h = hash(key);
	
	p->key=(char*)malloc(strlen(key) + 1);
	strcpy(p->key, key);
	
	p->value =(char*)malloc(strlen(value) + 1);
	strcpy(p->value, value);
	p->next = t->array[h];
	t->array[h] = p;
}

// Hash Table Search. Which Sucked. Alot. Like Alot Alot

int Table_search(struct Table *t, char *key, char **resultValue)
{
	struct Node *p;
	
	int h = hash(key);
	for (p = t-> array[h]; p!=NULL; p = p->next)
		if(strcmp(p->key, key)==0)
		{
			*resultValue=p->value;
			return 1;
		}
	return 0;
}

// Table Free FREEEDOM!
void Table_free(struct Table *t)
{
	struct Node *p;
	struct Node *nextp;

	int b;

	for(b=0; b < BUCKET_COUNT; b++)
	for(p= t->array[b]; p != NULL; p = nextp)
	{
		nextp = p->next;
		free(p);
	}
	free(t);
}


	
