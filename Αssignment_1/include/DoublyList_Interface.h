#include "DoublyList_Types.h"

List Create();      // Create an empty doubly linked list
int Size(List);     // This function return the length of Doubly Linked List
int IsEmpty(List);  // I check if a Doubly Linked List is empty
void Print(List );  // Print nodes of Doubly Linked List
void Remove(List , ListNode );  // Delete a node from the Doubly Linked List

//  GetNode Functions 
ListNode GetFirst(List);     // Get the FIRST node of the Doubly Linked List
ListNode GetLast(List);      // Get the LAST node of the Doubly Linked List
ListNode GetNext(List , ListNode );   // Return the next node from node a
ListNode GetPrev(List , ListNode );   // Return the previous node from node a

//Add Functions
void AddFirst(List ,ListNode );   // Insert node at START of Doubly Linked List
void AddLast(List , ListNode);    // Insert node at the END of Doubly Linked List
void AddAfter(List , ListNode, ListNode);   // Insert node AFTER from node v
void AddBefore(List ,ListNode, ListNode );  // Insert node BEFORE from node v

//Auxiliary Function
ListNode make(Item);   // Return a node 
int item(ListNode);    // Return the item of one node

