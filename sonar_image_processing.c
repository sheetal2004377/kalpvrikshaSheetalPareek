#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define MAX_VAL 255

//FUNCTION DECLARATIONS (Prototypes)
int readSize(void);
int* allocateValues(int size);
void makeMatrix(int *matrix, int size);
void showMatrix(int *matrix, int size);
void swapValues(int *ptr1, int *ptr2);
void transposeMatrix(int *matrix, int size);
void reverseRows(int *matrix, int size);
void rotateMatrix(int *matrix, int size);
void copyRow(int *source, int *destination, int size);
int getAverage(int *matrix, int size, int *previous, int *current, int row, int column);
void smoothMatrix(int *matrix, int size);
void freeMemory(int *values);

//FUNCTION DEFINITIONS (Implementations)

int readSize(void) {
    int size, ch;

    while(1) {
        printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SIZE);

        if(scanf("%d", &size) != 1) {
            printf("Invalid input!\n");

            while((ch = getchar()) != '\n' && ch != EOF);

            if(ch == EOF) {
                return -1;
            }

            continue;
        }

        ch = getchar();

        while(ch == ' ' || ch == '\t' || ch == '\r') {
            ch = getchar();
        }

        if(ch != '\n' && ch != EOF) {
            printf("Invalid input!\n");

            while(ch != '\n' && ch != EOF) {
                ch = getchar();
            }

            if(ch == EOF) {
                return -1;
            }

            continue;
        }

        if(size >= MIN_SIZE && size <= MAX_SIZE) {
            return size;
        }

        printf("Invalid size!\n");

        if(ch == EOF) {
            return -1;
        }
    }
}

//Memory Allocation
int* allocateValues(int size) {
    return malloc((size_t)size * size * sizeof(int));
}

//create matrix
void makeMatrix(int *matrix, int size) {
    int *cell;

    for(cell = matrix; cell < matrix + size * size; cell++) {
        *cell = rand() % (MAX_VAL + 1);
    }
}

//display matrix
void showMatrix(int *matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            printf("%4d", *(matrix + i * size + j));
        }
        printf("\n");
    }
}

//Swap Values
void swapValues(int *ptr1, int *ptr2) {
    int temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

//transpose matrix
void transposeMatrix(int *matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = i + 1; j < size; j++){
            swapValues((matrix + i * size + j), (matrix + j * size + i));
        }
    }
}

//reverse rows
void reverseRows(int *matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size / 2; j++){
            swapValues((matrix + i * size + j), (matrix + i * size + (size - 1 - j)));
        }
    }
}

//rotate Matrix
void rotateMatrix(int *matrix, int size) {
    transposeMatrix(matrix, size);
    reverseRows(matrix, size);
}

//copy rows
void copyRow(int *source, int *destination, int size) {
    int i;

    for(i = 0; i < size; i++){
        *(destination + i) = *(source + i);
    }
}

//calculate average
int getAverage(int *matrix, int size, int *previous, int *current, int row, int column) {
    int sum = 0, count = 0, x, y;

    for(x = row - 1; x <= row + 1; x++){
        for(y = column - 1; y <= column + 1; y++){
            if(x >= 0 && x < size && y >= 0 && y < size){
                if(x < row){
                    sum += *(previous + y);
                }
                else if(x == row){
                    sum += *(current + y);
                }
                else{
                    sum += *(matrix + x * size + y);
                }

                count++;
            }
        }
    }

    return sum / count;
}

void smoothMatrix(int *matrix, int size) {
    int previous[MAX_SIZE] = {0};
    int current[MAX_SIZE] = {0};
    int tempRow[MAX_SIZE];
    int row, column,j;

    for(row = 0; row < size; row++){
        for(j = 0; j < size; j++) {
            *(current + j) = *(matrix + row * size + j);
        }

        for(column = 0; column < size; column++){
            *(tempRow + column) = getAverage(matrix, size, previous, current, row, column);
        }

        for(j = 0; j < size; j++) {
            *(matrix + row * size + j) = *(tempRow + j);
        }

        copyRow(current, previous, size);
    }
}

void freeMemory(int *values) {
    free(values);
}

  // MAIN FUNCTION 

int main(void) {
    int size = readSize();

    if(size == -1) {
        return 1;
    }

    int *values = allocateValues(size);

     if(values == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    makeMatrix(values, size);

   printf("\nOriginal:\n");
    showMatrix(values, size);

    rotateMatrix(values, size);

    printf("\nRotated:\n");
    showMatrix(values, size);

    smoothMatrix(values, size);

    printf("\nFinal Output:\n");
    showMatrix(values, size);

    // Free allocated memory
    freeMemory(values);
    values = NULL;

    return 0;

}
