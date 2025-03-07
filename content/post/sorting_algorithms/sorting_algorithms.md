---
author: "Vidge Wong"
title: "Simple Sort"
date: "2020-02-19"
summary: "Implementation and analysis of basic sorting algorithms including bubble sort, insertion sort, and quick sort"
tags: ["c/cpp"]
categories: ["Algorithm"]
image: img/cover/math.png
ShowToc: true
TocOpen: true
---

## Introduction

This article demonstrates the implementation of three fundamental sorting algorithms: Bubble Sort, Insertion Sort, and Quick Sort. Each algorithm has its own characteristics and use cases.

## Implementation

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Utility function to swap two elements
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Bubble Sort Implementation
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        int swapped = 0;
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
                swapped = 1;
            }
        }
        // If no swapping occurred, array is already sorted
        if (swapped == 0)
            break;
    }
}

// Insertion Sort Implementation
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

// Quick Sort Implementation
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// Utility function to print array
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

// Test function with random array generation
void testSort(void (*sortFunc)(int[], int), const char* name, int size) {
    int* arr = (int*)malloc(size * sizeof(int));
    
    // Generate random array
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 100;
    
    printf("\nTesting %s\n", name);
    printf("Original array: ");
    printArray(arr, size);
    
    clock_t start = clock();
    sortFunc(arr, size);
    clock_t end = clock();
    
    printf("Sorted array: ");
    printArray(arr, size);
    printf("Time taken: %f seconds\n", 
           (double)(end - start) / CLOCKS_PER_SEC);
    
    free(arr);
}
```

## Algorithm Analysis

### 1. Bubble Sort

```c
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}
```

**Characteristics:**
- Time Complexity: O(n²)
- Space Complexity: O(1)
- Stable: Yes
- In-place: Yes
- Best for: Small datasets and nearly sorted arrays
- Optimization: Early termination if no swaps needed

### 2. Insertion Sort

```c
void insertionSort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}
```

**Characteristics:**
- Time Complexity: O(n²)
- Space Complexity: O(1)
- Stable: Yes
- In-place: Yes
- Best for: Small datasets and partially sorted arrays
- Adaptive: Performance improves for partially sorted arrays

### 3. Quick Sort

```c
void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}
```

**Characteristics:**
- Time Complexity: O(n log n) average, O(n²) worst case
- Space Complexity: O(log n)
- Stable: No
- In-place: Yes
- Best for: Large datasets
- Widely used in practice due to good average-case performance

## Performance Comparison

Here's a comparison of the three algorithms:

| Algorithm | Best Case | Average Case | Worst Case | Space | Stable |
|-----------|-----------|--------------|------------|-------|---------|
| Bubble Sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Insertion Sort | O(n) | O(n²) | O(n²) | O(1) | Yes |
| Quick Sort | O(n log n) | O(n log n) | O(n²) | O(log n) | No |

## Usage Example

```c
int main() {
    srand(time(NULL));
    int size = 10;
    
    // Test Bubble Sort
    testSort(bubbleSort, "Bubble Sort", size);
    
    // Test Insertion Sort
    testSort(insertionSort, "Insertion Sort", size);
    
    // Test Quick Sort
    int* arr = (int*)malloc(size * sizeof(int));
    for (int i = 0; i < size; i++)
        arr[i] = rand() % 100;
    
    printf("\nTesting Quick Sort\n");
    printf("Original array: ");
    printArray(arr, size);
    
    clock_t start = clock();
    quickSort(arr, 0, size-1);
    clock_t end = clock();
    
    printf("Sorted array: ");
    printArray(arr, size);
    printf("Time taken: %f seconds\n", 
           (double)(end - start) / CLOCKS_PER_SEC);
    
    free(arr);
    return 0;
}
```

## When to Use Each Algorithm

1. **Bubble Sort**:
   - Small datasets
   - Nearly sorted data
   - When simplicity is preferred
   - Teaching/learning purposes

2. **Insertion Sort**:
   - Small datasets
   - Online sorting (sorting as data arrives)
   - Nearly sorted data
   - When stability is required

3. **Quick Sort**:
   - Large datasets
   - When average case performance is important
   - When in-place sorting is needed
   - Standard library implementations

## Common Pitfalls

1. **Bubble Sort**:
   - Not optimizing with early termination
   - Using on large datasets

2. **Insertion Sort**:
   - Not considering it for small arrays
   - Overlooking its advantages for nearly sorted data

3. **Quick Sort**:
   - Poor pivot selection
   - Not handling duplicates efficiently
   - Stack overflow in recursive implementation

## Conclusion

Understanding these sorting algorithms and their characteristics helps in choosing the right algorithm for specific use cases. While Quick Sort is generally the most efficient for large datasets, Bubble Sort and Insertion Sort have their places in specific scenarios, particularly with small or nearly sorted datasets. 