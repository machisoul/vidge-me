---
author: "Vidge Wong"
title: "Add Two Numbers"
date: "2020-02-20"
summary: "A solution to add two numbers represented by linked lists in reverse order"
tags: ["c/cpp", "algorithm", "data structure"]
categories: ["Algorithm"]
image: images/cover/default.jpg
hiddenInList: true
ShowToc: true
TocOpen: true
---

## Problem Statement

You are given two non-empty linked lists representing two non-negative integers. The digits are stored in reverse order, and each of their nodes contains a single digit. Add the two numbers and return the sum as a linked list.

You may assume the two numbers do not contain any leading zero, except the number 0 itself.

### Examples

1. Input: l1 = [2,4,3], l2 = [5,6,4]
   Output: [7,0,8]
   Explanation: 342 + 465 = 807

2. Input: l1 = [0], l2 = [0]
   Output: [0]

3. Input: l1 = [9,9,9,9,9,9,9], l2 = [9,9,9,9]
   Output: [8,9,9,9,0,0,0,1]

### Constraints

- The number of nodes in each linked list is in the range [1, 100]
- 0 <= Node.val <= 9
- It is guaranteed that the list represents a number that does not have leading zeros

## Solution

Here's the implementation in C:

```c
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    int carry = 0;

    struct ListNode* resultHead = (struct ListNode*)malloc(sizeof(struct ListNode));
    resultHead->val = 0;
    resultHead->next = NULL;

    struct ListNode* tempNode = resultHead;

    while(l1 || l2){
        int tempVal = carry;

        if(l1){
            tempVal += l1->val;
            l1 = l1->next;
        }

        if(l2){
            tempVal += l2->val;
            l2 = l2->next;
        }

        tempNode->val = tempVal % 10;
        carry = tempVal / 10;

        if(l1 || l2){
            tempNode->next = (struct ListNode*)malloc(sizeof(struct ListNode));
            tempNode = tempNode->next;
            tempNode->val = 0;
            tempNode->next = NULL;
        }else{
            break;
        }
    }

    if(carry){
        tempNode->next = (struct ListNode*)malloc(sizeof(struct ListNode));
        tempNode = tempNode->next;
        tempNode->val = carry;
        tempNode->next = NULL;
    }

    return resultHead;
}
```

## Algorithm Explanation

### Step-by-Step Process

1. **Initialize Variables**:
   - `carry`: Keeps track of the carry value
   - `resultHead`: Points to the head of the result list
   - `tempNode`: Used to traverse and build the result list

2. **Main Loop**:
   - Continue while either list has nodes or there's a carry
   - Add values from both lists and carry
   - Calculate new digit and carry
   - Create new node if needed

3. **Handle Carry**:
   - If carry exists after processing all digits
   - Create one more node with carry value

### Key Points

1. **Reverse Order**:
   - Numbers are stored in reverse order
   - Makes addition simpler as we start from least significant digit

2. **Carry Handling**:
   - Carry is calculated as sum/10
   - New digit is calculated as sum%10

3. **List Length**:
   - Solution handles lists of different lengths
   - Continues until both lists are exhausted

## Time and Space Complexity

- **Time Complexity**: O(max(n,m))
  - Where n and m are the lengths of the input lists
  - We need to traverse the longer list

- **Space Complexity**: O(max(n,m))
  - We create a new list of length max(n,m) + 1
  - Extra space for potential carry digit

## Edge Cases

1. **Different Length Lists**:
   - Solution handles lists of different lengths
   - Continues processing until both lists are exhausted

2. **Carry at End**:
   - Handles case where final sum produces a carry
   - Creates additional node for carry value

3. **Zero Values**:
   - Correctly handles lists with single zero value
   - Maintains proper list structure

## Testing

Here's a test function to verify the solution:

```c
void printList(struct ListNode* head) {
    while (head) {
        printf("%d -> ", head->val);
        head = head->next;
    }
    printf("NULL\n");
}

int main() {
    // Test Case 1: [2,4,3] + [5,6,4] = [7,0,8]
    struct ListNode* l1 = createList(new int[]{2,4,3}, 3);
    struct ListNode* l2 = createList(new int[]{5,6,4}, 3);
    
    printf("Test Case 1:\n");
    printf("Input: ");
    printList(l1);
    printf("      + ");
    printList(l2);
    printf("Output: ");
    printList(addTwoNumbers(l1, l2));
    
    // Add more test cases...
    
    return 0;
}
```

## Common Pitfalls

1. **Memory Management**:
   - Always allocate memory for new nodes
   - Consider memory leaks in production code

2. **Carry Handling**:
   - Don't forget to handle final carry
   - Properly propagate carry through all digits

3. **List Traversal**:
   - Check for NULL pointers
   - Handle lists of different lengths

## Conclusion

This solution efficiently adds two numbers represented by linked lists. The reverse order storage makes the addition process straightforward, while proper handling of carry and different list lengths ensures correct results for all test cases. 