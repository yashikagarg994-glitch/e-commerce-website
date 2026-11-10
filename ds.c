#include <stdio.h>

// Linear Search Function
int linearSearch(int a[], int n, int key)
{
    int i;

    for(i = 0; i < n; i++)
    {
        if(a[i] == key)
            return i + 1;
    }

    return -1;
}

// Binary Search Function
int binarySearch(int a[], int n, int key)
{
    int low = 0, high = n - 1, mid;

    while(low <= high)
    {
        mid = (low + high) / 2;

        if(a[mid] == key)
            return mid + 1;

        else if(a[mid] < key)
            low = mid + 1;

        else
            high = mid - 1;
    }

    return -1;
}

int main()
{
    int a[100], n, key, i, result;

    printf("Enter number of elements: ");
    scanf("%d", &n);

    printf("Enter array elements in sorted order: ");

    for(i = 0; i < n; i++)
        scanf("%d", &a[i]);

    printf("Enter element to search: ");
    scanf("%d", &key);

    // Linear Search
    result = linearSearch(a, n, key);

    if(result != -1)
        printf("\nLinear Search: Element found at position %d", result);
    else
        printf("\nLinear Search: Element not found");

    // Binary Search
    result = binarySearch(a, n, key);

    if(result != -1)
        printf("\nBinary Search: Element found at position %d", result);
    else
        printf("\nBinary Search: Element not found");

    return 0;
}