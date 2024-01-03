#pragma once

typedef struct listnode ListNodestr;

typedef struct headertail ListHeaderTail;

typedef ListHeaderTail *List;
typedef ListNodestr *ListNode;

List create();			 // Create an empty doubly linked list
void insert(List , ListNode);   // Insert node
int Update(List , char *);      // Retuen if location exists and if exists update the num of 
ListNode Make(char*);           // Return a node 
void remove_list(List);  		// Delete the Doubly Linked List
char* get_loacation(List , ListNode );      // Get the location with the number of appearances as a string
int size(List );               // This function return the length of Doubly Linked List
ListNode Get_First(List );     // Get the FIRST node of the Doubly Linked List
void print(List );             // Print nodes of Doubly Linked List
ListNode Get_Next(List , ListNode);        // Return the next node from node a
