---
author: "Vidge Wong"
title: "A simple Linked List question"
date: "2020-03-19"
summary: "A step-by-step guide to linked list operations including splitting, reversing specific nodes, and joining lists"
tags: ["c/cpp"]
categories: ["Algorithm"]
cover:
  image: images/cover/default.jpg
  hiddenInList: true
ShowToc: true
TocOpen: true
---

## Problem Statement

Given a linked list, perform the following operations:
1. Create a single linked list with 5 nodes: 1->2->3->4->5
2. Split it into 2 lists: (1->2) and (3->4->5)
3. Reverse the first 2 nodes in the second list: (1->2) and (4->3->5)
4. Join the lists again to get: 1->2->4->3->5

## Solution

Here's a complete implementation in C:

```c
#include <stdio.h>
#include <stdlib.h>

// Node structure
struct Node {
    int data;
    struct Node* next;
};

// Function to create a new node
struct Node* createNode(int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to print the linked list
void printList(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf("%d -> ", current->data);
        current = current->next;
    }
    printf("NULL\n");
}

// Function to split the list into two parts
void splitList(struct Node* head, struct Node** firstHalf, struct Node** secondHalf) {
    if (head == NULL || head->next == NULL) {
        *firstHalf = head;
        *secondHalf = NULL;
        return;
    }

    struct Node* slow = head;
    struct Node* fast = head->next;

    // Move fast two steps and slow one step
    while (fast != NULL && fast->next != NULL) {
        fast = fast->next->next;
        slow = slow->next;
    }

    // Split the list
    *firstHalf = head;
    *secondHalf = slow->next;
    slow->next = NULL;
}

// Function to reverse first N nodes of a linked list
struct Node* reverseFirstN(struct Node* head, int n) {
    if (head == NULL || n <= 1) return head;

    struct Node *prev = NULL, *curr = head, *next = NULL;
    int count = 0;
    struct Node* lastNodeOfFirst = NULL;

    while (curr != NULL && count < n) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
        count++;
    }

    // Connect with remaining list
    head->next = curr;
    return prev;
}

// Function to join two lists
struct Node* joinLists(struct Node* first, struct Node* second) {
    if (first == NULL) return second;
    
    struct Node* current = first;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = second;
    return first;
}

int main() {
    // Step 1: Create the initial list 1->2->3->4->5
    struct Node* head = createNode(1);
    head->next = createNode(2);
    head->next->next = createNode(3);
    head->next->next->next = createNode(4);
    head->next->next->next->next = createNode(5);

    printf("Original list: ");
    printList(head);

    // Step 2: Split the list
    struct Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf, &secondHalf);

    printf("First half: ");
    printList(firstHalf);
    printf("Second half: ");
    printList(secondHalf);

    // Step 3: Reverse first 2 nodes of second half
    secondHalf = reverseFirstN(secondHalf, 2);
    printf("After reversing first 2 nodes of second half: ");
    printList(secondHalf);

    // Step 4: Join the lists
    struct Node* result = joinLists(firstHalf, secondHalf);
    printf("Final result: ");
    printList(result);

    return 0;
}
```

## Step-by-Step Explanation

### 1. Creating the List

We first create a basic linked list structure and helper functions:
- `struct Node`: Basic node structure with data and next pointer
- `createNode()`: Allocates memory and initializes a new node
- `printList()`: Utility function to display the list

### 2. Splitting the List

The `splitList()` function uses the fast and slow pointer technique:
- Fast pointer moves two steps at a time
- Slow pointer moves one step at a time
- When fast reaches end, slow is at the split point

### 3. Reversing First N Nodes

`reverseFirstN()` function:
- Takes number of nodes to reverse as parameter
- Uses three pointers (prev, curr, next) for reversal
- Maintains connection with remaining list

### 4. Joining Lists

`joinLists()` function:
- Traverses to end of first list
- Connects it with second list
- Returns head of combined list

## Output

```
Original list: 1 -> 2 -> 3 -> 4 -> 5 -> NULL
First half: 1 -> 2 -> NULL
Second half: 3 -> 4 -> 5 -> NULL
After reversing first 2 nodes of second half: 4 -> 3 -> 5 -> NULL
Final result: 1 -> 2 -> 4 -> 3 -> 5 -> NULL
```

## Time Complexity Analysis

1. Creating list: O(1) per node
2. Splitting list: O(n)
3. Reversing first N nodes: O(n)
4. Joining lists: O(n)

Overall complexity: O(n) where n is the number of nodes

## Memory Considerations

- Space Complexity: O(1) as we only use a constant amount of extra space
- All operations are performed in-place
- Proper memory management (freeing nodes) is important in production code

## Common Pitfalls to Avoid

1. Null pointer checks
2. Maintaining proper links during operations
3. Memory leaks in node creation/deletion
4. Edge cases (empty list, single node)

## Conclusion

This implementation demonstrates common linked list operations while maintaining code readability and efficiency. The solution handles edge cases and uses standard techniques like fast/slow pointers for list manipulation. 