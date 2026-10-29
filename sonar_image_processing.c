#include <stdio.h>
#include <stdlib.h>

#define MIN_SIZE 2
#define MAX_SIZE 10
#define MAX_VAL 255
//FUNCTION DECLARATIONS (Prototypes)
int read_size(void);
int* allocate_values(int size);
int** allocate_matrix(int size);
void link_pointers(int **matrix, int *values, int size);
void make_matrix(int **matrix, int size);
void show_matrix(int **matrix, int size);
void swap_addresses(int **ptr1, int **ptr2);
void transpose_matrix(int **matrix, int size);
void reverse_rows(int **matrix, int size);
void rotate_matrix(int **matrix, int size);
void copy_row(int *source, int *destination, int size);
int get_average(int **matrix, int size, int *previous, int *current, int row, int column);
void smooth_matrix(int **matrix, int size);
void free_memory(int *values, int **matrix);

//FUNCTION DEFINITIONS (Implementations)

int read_size(void) {
    int size, result, ch;

    while(1){
        printf("Enter matrix size (%d-%d): ", MIN_SIZE, MAX_SIZE);
        result = scanf("%d", &size);

        if(result == EOF){
            exit(1);
        }

        if(result == 1 && size >= MIN_SIZE && size <= MAX_SIZE){
            return size;
        }

        printf("Invalid input. Enter a whole number between %d and %d.\n", MIN_SIZE, MAX_SIZE);

        while((ch = getchar()) != '\n' && ch != EOF){
        }

        if(ch == EOF){
            exit(1);
        }
    }
}

int* allocate_values(int size) {
    return (int *)malloc(size * size * sizeof(int));
}

int** allocate_matrix(int size) {
    return (int **)malloc(size * size * sizeof(int *));
}

void link_pointers(int **matrix, int *values, int size) {
    for(int i = 0; i < size * size; i++){
        *(matrix + i) = values + i;
    }
}

void make_matrix(int **matrix, int size) {
    unsigned long long seed = 12345ULL;
    int *cell;

    for(cell = *matrix; cell < *matrix + size * size; cell++){
        seed = (seed * 1103515245ULL + 12345ULL);
        *cell = (int)((seed / 65536ULL) % (MAX_VAL + 1));
    }
}

void show_matrix(int **matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size; j++){
            printf("%4d", **(matrix + i * size + j));
        }
        printf("\n");
    }
}

void swap_addresses(int **ptr1, int **ptr2) {
    int *temp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = temp;
}

void transpose_matrix(int **matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = i + 1; j < size; j++){
            swap_addresses((matrix + i * size + j), (matrix + j * size + i));
        }
    }
}

void reverse_rows(int **matrix, int size) {
    int i, j;

    for(i = 0; i < size; i++){
        for(j = 0; j < size / 2; j++){
            swap_addresses((matrix + i * size + j), (matrix + i * size + (size - 1 - j)));
        }
    }
}

void rotate_matrix(int **matrix, int size) {
    transpose_matrix(matrix, size);
    reverse_rows(matrix, size);
}

void copy_row(int *source, int *destination, int size) {
    int i;

    for(i = 0; i < size; i++){
        *(destination + i) = *(source + i);
    }
}

int get_average(int **matrix, int size, int *previous, int *current, int row, int column) {
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
                    sum += **(matrix + x * size + y);
                }

                count++;
            }
        }
    }

    return sum / count;
}

void smooth_matrix(int **matrix, int size) {
    int previous[MAX_SIZE] = {0};
    int current[MAX_SIZE] = {0};
    int temp_row[MAX_SIZE];
    int row, column;

    for(row = 0; row < size; row++){
        for(int j = 0; j < size; j++) {
            *(current + j) = **(matrix + row * size + j);
        }

        for(column = 0; column < size; column++){
            *(temp_row + column) = get_average(matrix, size, previous, current, row, column);
        }

        for(int j = 0; j < size; j++) {
            **(matrix + row * size + j) = *(temp_row + j);
        }

        copy_row(current, previous, size);
    }
}

void free_memory(int *values, int **matrix) {
    if (values) free(values);
    if (matrix) free(matrix);
}


  // MAIN FUNCTION 

int main(void) {
    int size = read_size();

    int *values = allocate_values(size);
    int **matrix = allocate_matrix(size);

    if (!values || !matrix) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    link_pointers(matrix, values, size);

    make_matrix(matrix, size);
    printf("\nOriginal :\n");
    show_matrix(matrix, size);

    rotate_matrix(matrix, size);
    printf("\nRotated:\n");
    show_matrix(matrix, size);

    smooth_matrix(matrix, size);
    printf("\nFinal Output:\n");
    show_matrix(matrix, size);

    free_memory(values, matrix);

    return 0;
}
