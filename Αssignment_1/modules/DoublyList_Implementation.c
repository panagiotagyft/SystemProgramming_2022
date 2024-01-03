#include <stdio.h>
#include <stdlib.h>
#include "../include/DoublyList_Interface.h"

//   Node Type
struct listnode
{
	Item item;
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
List Create()
{
	ListHeaderTail *h = malloc(sizeof(ListHeaderTail)); // I create node.The node include two structs.The type of structs is the same of the type the others nodes.
														// The difference here is that i use the node as a pointer.
	h->first = NULL;									// I initialize the head (FIRDT)
	h->last = NULL;										// I initialize the tail (LAST)

	return h; // I return the empty doubly linked list
}

// This function return the length of Doubly Linked List
int Size(List l)
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

// I check if a Doubly Linked List is empty
int IsEmpty(List l)
{
	if (l->first == NULL)
	{ // If head pointer point to NULL --> return 1 (TRUE). The list is empty.
		return 1;
	}
	else
	{ // Else return 0; (FALSE).
		return 0;
	}
}

// Print nodes of Doubly Linked List
void Print(List l)
{
	ListNodestr *node = l->first;
	while (node != NULL)
	{
		printf("%d -->", node->item); // I cross the list and i print the nodes
		node = node->next;
	}
	printf("\n");
}

// Delete a node from the Doubly Linked List
void Remove(List l, ListNode v)
{
	ListNode pr, nxt, temp;
	if (l->first == v)
	{ // If the node which i want to delete is the first node
		if (v->next != NULL)
		{
			nxt = v->next;
			nxt->prev = NULL; // I initialize the previous pointer of the next node point to NULL
		}
		l->first = l->first->next; // The pointer point to the next node
	}
	else if (l->last == v)
	{							 // If the node which i want to delete is the last node
		pr = v->prev;			 //
		l->last = l->last->prev; // The pointer point to the previous node
		pr->next = NULL;		 // I initialize the next pointer of the next node point to NULL
	}
	else
	{
		pr = v->prev;
		nxt = v->next;
		pr->next = nxt; // The next pointer of the node pr point to the node nxt
		nxt->prev = pr; // The previous pointer of the node nxt point to the node pr
	}
	free(v);
}

// Get the FIRST node of the Doubly Linked List
ListNode GetFirst(List l)
{
	return l->first;
}

// Get the LAST node of the Doubly Linked List
ListNode GetLast(List l)
{
	return l->last;
}

// Return the next node from node a
ListNode GetNext(List l, ListNode a)
{
	ListNode node = a;
	node = node->next; // i return the next node
	return (node);
}

// Return the previous node from node a
ListNode GetPrev(List l, ListNode a)
{
	ListNode node = a;
	node = node->prev; // i return the previous node
	return (node);
}

// Insert node at START of Doubly Linked List
void AddFirst(List l, ListNode newnode)
{
	if (l->first == NULL)
	{						  // If head pointer point to NULL
		l->first = newnode;	  // �ead pointer point to newnode
		l->last = newnode;	  // Tail pointer point to newnode
		newnode->prev = NULL; // The previous pointer of the newnode point to NULL
		newnode->next = NULL; // The next pointer of the newnode point to NULL
	}
	else
	{
		newnode->next = l->first; // The next pointer of the newnode point to the l->first (Head)
		l->first->prev = newnode; // The previous pointer of the first node point to newnode
		l->first = newnode;		  // The head pointer point to newnode
		l->first->prev = NULL;	  // The previous pointer of the first node point to NULL.
	}
}

// Insert node at the END of Doubly Linked List
void AddLast(List l, ListNode newnode)
{
	if (l->last == NULL)
	{						  // If tail pointer point to NULL
		l->last = newnode;	  // Tail pointer point to newnode
		l->first = newnode;	  // �ead pointer point to newnode
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

// Insert node AFTER from node v
void AddAfter(List l, ListNode newnode, ListNode v)
{
	ListNode u;
	if (l->last == v)
	{						 // If last node is equal with v
		AddLast(l, newnode); // --> AddLast function
	}
	else
	{
		u = v->next;
		newnode->next = v->next; // The next pointer of the newnode point to where it point and the next pointer of the node v
		v->next = newnode;		 // The next pointer of the v point to newnode
		newnode->prev = v;		 // The previous pointer of the newnode point to the node v
		u->prev = newnode;		 // The previous pointer of the u point to newnode
	}
}

// Insert node BEFORE from node v
void AddBefore(List l, ListNode newnode, ListNode v)
{
	ListNode u;
	if (l->first == v)
	{						  // If first node is equal with v
		AddFirst(l, newnode); // --> AddFirst function
	}
	else
	{
		u = v->prev;
		newnode->next = v;		 // The next pointer of the newnode point to the node v
		newnode->prev = v->prev; // The previous pointer of the newnode point to where it point and the previous pointer of the node v
		v->prev = newnode;		 // The previous pointer of the node v point to newnode
		u->next = newnode;		 // The next pointer of the node u point to newnode
	}
}

// Return a node
ListNode make(Item i)
{
	ListNodestr *newnode = malloc(sizeof(ListNodestr));
	newnode->item = i;	  // I initialize the item of the newnode equal with i
	newnode->next = NULL; // I initialize the next pointer of the newnode point to NULL
	newnode->prev = NULL; // I initialize the previous pointer of the newnode point to NULL
	return newnode;		  // I return the node
}

int item(ListNode v)
{
	int x;
	x = v->item;
	return x;
}
