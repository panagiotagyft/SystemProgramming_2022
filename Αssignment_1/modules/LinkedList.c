#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/LinkedList_Interface.h"

//   Node Type
struct listnode
{
	int counter;
	char *location_str;
	struct listnode *next;
	struct listnode *prev;
};

typedef struct listnode ListNodestr;

//   The first and last node (head and tail)
struct headertail
{
	struct listnode *first;
	struct listnode *last;
};

// Create an empty doubly linked list
List create()
{
	ListHeaderTail *h = malloc(sizeof(ListHeaderTail)); // I create node.The node include two structs.The type of structs is the same of the type the others nodes.
														// The difference here is that i use the node as a pointer.
	h->first = NULL;									// I initialize the head (FIRDT)
	h->last = NULL;										// I initialize the tail (LAST)

	return h; // I return the empty doubly linked list
}

// Insert node
void insert(List l, ListNode newnode)
{
	if (l->last == NULL)
	{						  // If tail pointer point to NULL
		l->last = newnode;	  // Tail pointer point to newnode
		l->first = newnode;	  // Cead pointer point to newnode
		newnode->prev = NULL; // The previous pointer of the newnode point to NULL
		newnode->next = NULL; // The next pointer of the newnode point to NULL
	}
	else
	{
		l->last->next = newnode; // The next pointer of the last node point to newnode
		newnode->prev = l->last; // The previous pointer of the newnode point to the l->last (Tail)
		l->last = newnode;		 // The tail pointer point to newnode
		l->last->next = NULL;	 // The next pointer of the last node point to NULL.
	}
}

int Update(List l, char *location)
{
	ListNodestr *node = l->first;
	int ok = 0;

	while (node != NULL)
	{
		if (strcmp(location, node->location_str) == 0)
		{					 // If location exists
			node->counter++; // Increase the counter of num of appearances
			ok = 1;
		}
		node = node->next;
	}

	return ok; // if ok=1 -->> The location exists else if ok=0 -->> The location isn't exists then insert the location in list
}

ListNode Make(char *location)
{
	ListNodestr *newnode = malloc(sizeof(ListNodestr));
	newnode->counter = 1; // I initialize the num of appearances equal with 1
	newnode->location_str = malloc((strlen(location) + 1) * sizeof(char));
	strcpy(newnode->location_str, location);
	newnode->next = NULL; // I initialize the next pointer of the newnode point to NULL
	newnode->prev = NULL; // I initialize the previous pointer of the newnode point to NULL
	return newnode;		  // I return the node
}

// Delete List
void remove_list(List l)
{
	ListNode node, nxt, a;
	// remove one node at a time
	while (size(l) != 0)
	{
		a = Get_First(l);
		if (a->next != NULL)
		{
			nxt = a->next;
			nxt->prev = NULL;
		}
		l->first = l->first->next;
		free(a); // free first node
	}
	free(l); // free list
}

char *get_loacation(List l, ListNode v)
{
	char str[500];

	sprintf(str, "%d", v->counter); // conver num of appearances (integer) to string
	char *location = malloc((strlen(v->location_str) + sizeof(str) + 1) * sizeof(char));
	strcpy(location, v->location_str); // e.g  location=di.uoa.gr
	strcat(location, " ");			   // add space after the location
	strcat(location, str);			   // if str="2" then location=di.uoa.gr 2
	return location;
}

int size(List l)
{
	int c = 0;
	ListNodestr *node = l->first;

	while (node != NULL)
	{
		c++; // I cross the list and count the nodes
		node = node->next;
	}
	return c;
}

// Return the first node
ListNode Get_First(List l)
{
	return l->first;
}

ListNode Get_Next(List l, ListNode a)
{
	ListNode node = a;
	node = node->next; // i return the next node
	return (node);
}

void print(List l)
{
	ListNodestr *node = l->first;
	while (node != NULL)
	{
		printf("%s | %d -->\n", node->location_str, node->counter); // I cross the list and i print the nodes
		node = node->next;
	}
}
