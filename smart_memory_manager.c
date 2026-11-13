#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_POOL_SIZE 1000000
#define INITIAL_CAPACITY 4

// Block status
enum Status
{
    FREE = 0,
    ALLOCATED = 1
};

// Allocation strategies
enum Strategy
{
    FIRST_FIT = 1,
    BEST_FIT = 2,
    WORST_FIT = 3
};

// Store memory block details
struct Block
{
    int start;
    int size;
    int id;
    int status;
};

// Global variables
unsigned char *memoryPool = NULL;
struct Block *blocks = NULL;

int poolSize = 0;
int blockCount = 0;
int capacity = 0;
int nextID = 1;
int strategy = FIRST_FIT;

// Function declarations
int readNumber(const char message[], int *number);
int initialize(void);
int growBlocks(int required);
int findSuitableBlock(int size);
int findID(int id);

void insertAllocation(int index, int size, int id);
void allocateMemory(void);
void releaseMemory(void);
void mergeBlocks(void);
void resizeMemory(void);

void displayMap(void);
void displayStats(void);
void changeStrategy(void);
void showMenu(void);
void cleanup(void);

// Read and validate integer input
int readNumber(const char message[], int *number)
{
    char input[100];
    int i = 0;
    int sign = 1;
    int digitFound = 0;
    int ch;
    long long value = 0;
    long long limit;

    printf("%s", message);

    if (fgets(input, sizeof(input), stdin) == NULL)
        return -1;

    while (input[i] != '\0' && input[i] != '\n')
        i++;

    if (input[i] == '\0' && i == 99)
    {
        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("Invalid input. Please enter a valid number.\n");
        return 0;
    }

    i = 0;

    // Skip leading spaces
    while (input[i] == ' ' || input[i] == '\t')
        i++;

    // Check sign
    if (input[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (input[i] == '+')
    {
        i++;
    }

    if (sign == -1)
        limit = -(long long)INT_MIN;
    else
        limit = INT_MAX;

    // Read number digit by digit
    while (input[i] >= '0' && input[i] <= '9')
    {
        int digit = input[i] - '0';

        if (value > (limit - digit) / 10)
        {
            printf("Invalid input. Number is out of range.\n");
            return 0;
        }

        value = value * 10 + digit;
        digitFound = 1;
        i++;
    }

    // Skip trailing spaces
    while (input[i] == ' ' || input[i] == '\t' ||
           input[i] == '\n' || input[i] == '\r')
    {
        i++;
    }

    if (!digitFound || input[i] != '\0')
    {
        printf("Invalid input. Please enter a valid number.\n");
        return 0;
    }

    *number = (int)(value * sign);

    return 1;
}

// Initialize memory pool and metadata
int initialize(void)
{
    int choice;
    int result;
    int i;

    // Read memory pool size
    while (1)
    {
        result = readNumber("Enter memory pool size: ", &poolSize);

        if (result == -1)
            return 0;

        if (result == 0)
            continue;

        if (poolSize >= 1 && poolSize <= MAX_POOL_SIZE)
            break;

        printf("Invalid pool size. Enter between 1 and 1000000.\n");
    }

    // Allocate memory pool
    memoryPool = (unsigned char *)malloc(poolSize);

    if (memoryPool == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    for (i = 0; i < poolSize; i++)
        memoryPool[i] = 0;

    // Allocate metadata array
    capacity = INITIAL_CAPACITY;

    blocks = (struct Block *)malloc(
        capacity * sizeof(struct Block));

    if (blocks == NULL)
    {
        printf("Metadata allocation failed.\n");
        free(memoryPool);
        memoryPool = NULL;
        return 0;
    }

    // Initially complete memory is free
    blockCount = 1;

    blocks[0].start = 0;
    blocks[0].size = poolSize;
    blocks[0].id = 0;
    blocks[0].status = FREE;

    // Select allocation strategy
    while (1)
    {
        result = readNumber(
            "Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ",
            &choice);

        if (result == -1)
            return 0;

        if (result == 0)
            continue;

        if (choice >= FIRST_FIT && choice <= WORST_FIT)
            break;

        printf("Invalid strategy. Please select 1, 2 or 3.\n");
    }

    strategy = choice;

    if (strategy == FIRST_FIT)
        printf("Strategy set to First Fit.\n");
    else if (strategy == BEST_FIT)
        printf("Strategy set to Best Fit.\n");
    else
        printf("Strategy set to Worst Fit.\n");

    return 1;
}

// Increase metadata capacity using realloc
int growBlocks(int required)
{
    int newCapacity;
    struct Block *temp;

    if (required <= capacity)
        return 1;

    newCapacity = capacity;

    while (newCapacity < required)
        newCapacity *= 2;

    temp = (struct Block *)realloc(
        blocks, newCapacity * sizeof(struct Block));

    if (temp == NULL)
    {
        printf("Metadata expansion failed.\n");
        return 0;
    }

    blocks = temp;
    capacity = newCapacity;

    return 1;
}

// Find suitable free block
int findSuitableBlock(int size)
{
    int i;
    int selected = -1;

    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == FREE &&
            blocks[i].size >= size)
        {
            // First Fit
            if (strategy == FIRST_FIT)
                return i;

            if (selected == -1)
                selected = i;

            // Best Fit
            else if (strategy == BEST_FIT &&
                     blocks[i].size < blocks[selected].size)
                selected = i;

            // Worst Fit
            else if (strategy == WORST_FIT &&
                     blocks[i].size > blocks[selected].size)
                selected = i;
        }
    }

    return selected;
}

// Insert allocation into free block
void insertAllocation(int index, int size, int id)
{
    int i;
    int oldStart = blocks[index].start;
    int oldSize = blocks[index].size;

    // Split block if extra space is available
    if (oldSize > size)
    {
        for (i = blockCount; i > index; i--)
            blocks[i] = blocks[i - 1];

        // Remaining free block
        blocks[index + 1].start = oldStart + size;
        blocks[index + 1].size = oldSize - size;
        blocks[index + 1].id = 0;
        blocks[index + 1].status = FREE;

        blockCount++;
    }

    // Mark block as allocated
    blocks[index].start = oldStart;
    blocks[index].size = size;
    blocks[index].id = id;
    blocks[index].status = ALLOCATED;
}

// Allocate memory
void allocateMemory(void)
{
    int size;
    int index;
    int result;

    if (nextID <= 0)
    {
        printf("Allocation failed: no more allocation IDs available.\n");
        return;
    }

    result = readNumber("Requested size: ", &size);

    if (result != 1)
        return;

    if (size <= 0 || size > poolSize)
    {
        printf("Allocation failed: invalid requested size.\n");
        return;
    }

    index = findSuitableBlock(size);

    if (index == -1)
    {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        return;
    }

    // Increase metadata capacity if splitting is needed
    if (blocks[index].size > size)
    {
        if (!growBlocks(blockCount + 1))
            return;
    }

    insertAllocation(index, size, nextID);

    printf("Allocation successful. ID: %d, Start: %d, Size: %d\n",
           nextID, blocks[index].start, blocks[index].size);

    if (nextID == INT_MAX)
        nextID = 0;
    else
        nextID++;
}

// Find allocation using ID
int findID(int id)
{
    int i;

    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == ALLOCATED &&
            blocks[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

// Merge adjacent free blocks
void mergeBlocks(void)
{
    int i;
    int j;

    i = 0;

    while (i < blockCount - 1)
    {
        if (blocks[i].status == FREE &&
            blocks[i + 1].status == FREE)
        {
            blocks[i].size += blocks[i + 1].size;

            for (j = i + 1; j < blockCount - 1; j++)
                blocks[j] = blocks[j + 1];

            blockCount--;
        }
        else
        {
            i++;
        }
    }
}

// Release memory
void releaseMemory(void)
{
    int id;
    int index;

    if (readNumber("Allocation ID: ", &id) != 1)
        return;

    if (id <= 0)
    {
        printf("Invalid allocation ID or allocation already released.\n");
        return;
    }

    index = findID(id);

    if (index == -1)
    {
        printf("Invalid allocation ID or allocation already released.\n");
        return;
    }

    blocks[index].status = FREE;
    blocks[index].id = 0;

    mergeBlocks();

    printf("Allocation released.\n");
}

// Resize allocation
void resizeMemory(void)
{
    int id;
    int newSize;
    int index;
    int next;
    int target;
    int oldStart;
    int oldSize;
    int targetStart;
    int targetSize;
    int i;
    int extra;
    int result;

    result = readNumber("Allocation ID: ", &id);

    if (result != 1)
        return;

    if (id <= 0)
    {
        printf("Invalid allocation ID.\n");
        return;
    }

    index = findID(id);

    if (index == -1)
    {
        printf("Invalid allocation ID.\n");
        return;
    }

    result = readNumber("New requested size: ", &newSize);

    if (result != 1)
        return;

    if (newSize <= 0 || newSize > poolSize)
    {
        printf("Invalid resize size.\n");
        return;
    }

    oldStart = blocks[index].start;
    oldSize = blocks[index].size;

    if (newSize == oldSize)
    {
        printf("Allocation size is already the same.\n");
        return;
    }

    // Case 1: Shrink allocation
    if (newSize < oldSize)
    {
        if (!growBlocks(blockCount + 1))
            return;

        for (i = blockCount; i > index + 1; i--)
            blocks[i] = blocks[i - 1];

        blocks[index].size = newSize;

        blocks[index + 1].start = oldStart + newSize;
        blocks[index + 1].size = oldSize - newSize;
        blocks[index + 1].id = 0;
        blocks[index + 1].status = FREE;

        blockCount++;

        mergeBlocks();

        printf("Allocation resized successfully.\n");
        return;
    }

    // Case 2: Expand into next free block
    next = index + 1;

    if (next < blockCount &&
        blocks[next].status == FREE)
    {
        int available = oldSize + blocks[next].size;

        if (available >= newSize)
        {
            extra = newSize - oldSize;

            if (blocks[next].size == extra)
            {
                for (i = next; i < blockCount - 1; i++)
                    blocks[i] = blocks[i + 1];

                blockCount--;
            }
            else
            {
                blocks[next].start += extra;
                blocks[next].size -= extra;
            }

            blocks[index].size = newSize;

            printf("Allocation resized successfully.\n");
            return;
        }
    }

    // Case 3: Move allocation to another free block
    target = findSuitableBlock(newSize);

    if (target == -1)
    {
        printf("Resize failed: no suitable free block available. Original allocation unchanged.\n");
        return;
    }

    targetStart = blocks[target].start;
    targetSize = blocks[target].size;

    if (targetSize > newSize)
    {
        if (!growBlocks(blockCount + 1))
            return;
    }

    // Copy old data to new location
    for (i = 0; i < oldSize; i++)
        memoryPool[targetStart + i] = memoryPool[oldStart + i];

    insertAllocation(target, newSize, id);

    // Release old block
    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == ALLOCATED &&
            blocks[i].id == id &&
            blocks[i].start == oldStart)
        {
            blocks[i].status = FREE;
            blocks[i].id = 0;
            break;
        }
    }

    mergeBlocks();

    printf("Allocation resized successfully.\n");
}

// Display memory map
void displayMap(void)
{
    int i;

    printf("Memory Map (%d bytes):\n", poolSize);

    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == FREE)
        {
            printf("[Free: %d bytes, Start: %d]\n",
                   blocks[i].size, blocks[i].start);
        }
        else
        {
            printf("[Allocated #%d: %d bytes, Start: %d]\n",
                   blocks[i].id, blocks[i].size, blocks[i].start);
        }
    }
}

// Display memory statistics
void displayStats(void)
{
    int i;
    int totalFree = 0;
    int totalAllocated = 0;
    int largestFree = 0;
    int freeBlocks = 0;

    for (i = 0; i < blockCount; i++)
    {
        if (blocks[i].status == FREE)
        {
            totalFree += blocks[i].size;
            freeBlocks++;

            if (blocks[i].size > largestFree)
                largestFree = blocks[i].size;
        }
        else
        {
            totalAllocated += blocks[i].size;
        }
    }

    printf("Total memory: %d bytes\n", poolSize);
    printf("Allocated: %d bytes\n", totalAllocated);
    printf("Free: %d bytes\n", totalFree);
    printf("Free blocks: %d\n", freeBlocks);
    printf("Largest free block: %d bytes\n", largestFree);
}

// Change allocation strategy
void changeStrategy(void)
{
    int choice;
    int result;

    while (1)
    {
        result = readNumber(
            "Select allocation strategy (1. First Fit, 2. Best Fit, 3. Worst Fit): ",
            &choice);

        if (result == -1)
            return;

        if (result == 0)
            continue;

        if (choice >= FIRST_FIT && choice <= WORST_FIT)
            break;

        printf("Invalid strategy. Please select 1, 2 or 3.\n");
    }

    strategy = choice;

    if (strategy == FIRST_FIT)
        printf("Strategy set to First Fit.\n");
    else if (strategy == BEST_FIT)
        printf("Strategy set to Best Fit.\n");
    else
        printf("Strategy set to Worst Fit.\n");
}

// Display menu
void showMenu(void)
{
    printf("Menu:\n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");
}

// Free allocated memory
void cleanup(void)
{
    free(memoryPool);
    free(blocks);

    memoryPool = NULL;
    blocks = NULL;
}

// Main function
int main(void)
{
    int choice;
    int result;

    if (!initialize())
    {
        cleanup();
        return 1;
    }

    while (1)
    {
        showMenu();

        result = readNumber("Choice: ", &choice);

        if (result == -1)
            break;

        if (result == 0)
            continue;

        switch (choice)
        {
            case 1:
                allocateMemory();
                break;

            case 2:
                releaseMemory();
                break;

            case 3:
                resizeMemory();
                break;

            case 4:
                displayMap();
                break;

            case 5:
                displayStats();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                cleanup();
                printf("Program exited successfully.\n");
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    cleanup();

    printf("Program exited successfully.\n");

    return 0;
}
