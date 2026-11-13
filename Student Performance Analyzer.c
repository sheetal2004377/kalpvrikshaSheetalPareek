#include<stdio.h>
#include<ctype.h>
//define constants
#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 50
#define TOTAL_SUBJECTS 3

#define MIN_MARKS 0
#define MAX_MARKS 100

#define GRADE_A 85
#define GRADE_B 70
#define GRADE_C 50
#define GRADE_D 35

//structure defination
struct student{
    int roll_number;
    char name[MAX_NAME_LENGTH];
    int marks1;
    int marks2;
    int marks3;
};

//declare functions
int calculate_total(struct student s);
float calculate_average(int total);
char calculate_grade(float average);
void print_pattern(char grade);
void print_roll_numbers(struct student students[],int n,int index);

int roll_number_exists(struct student students[], int n, int roll);
int valid_marks(int marks);
int valid_name(char name[]);

void input_students(struct student students[], int n);
void display_results(struct student students[], int n);


//calculate total marks
int calculate_total(struct student s){
    int total;

    total=s.marks1+s.marks2+s.marks3;

    return total;
}

//calculate average
float calculate_average(int total){
    float average;

    average=total/(float)TOTAL_SUBJECTS;

    return average;
}

//calculate grade
char calculate_grade(float average){
    if(average>=GRADE_A){
        return 'A';
    }
    else if(average>=GRADE_B){
        return 'B';
    }
    else if(average>=GRADE_C){
        return 'C';
    }
    else if(average>=GRADE_D){
        return 'D';
    }
    else{
        return 'F';
    }
}

//print performnce pattern using switch case
void print_pattern(char grade){
    int stars=0;

    switch(grade){
        case 'A':
          stars=5;
          break;
        case 'B':
          stars=4;
          break;
        case 'C':
          stars=3;
          break;
        case 'D':
          stars=2;
          break;
        default:
          stars=0;
    }
    for(int i=1;i<=stars;i++){
        printf("*");
    }
}

//validation check if roll_number already exits
int roll_number_exists(struct student students[], int n, int roll) {
    for (int i = 0; i < n; i++) {
        if (students[i].roll_number == roll) {
            return 1;//exists
        }
    }
    return 0;//does not exist
}

//Validate marks
int valid_marks(int marks) {
    if (marks >= MIN_MARKS && marks <= MAX_MARKS) {
        return 1;
    } else {
        return 0;
    }
}

// Validate a single-word alphabetic name
int valid_name(char name[]) {
    if (name[0] == '\0') {
        return 0;
    }

    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i])) {
            return 0;
        }
    }

    return 1;
}

//take input for all students
void input_students(struct student students[], int n) {
    for (int i = 0; i < n; i++) {

        // Input and validate roll number
        scanf("%d", &students[i].roll_number);

        while (students[i].roll_number <= 0 ||
               roll_number_exists(students, i,
                                  students[i].roll_number)) {
            scanf("%d", &students[i].roll_number);
        }

        // Input and validate name
        scanf("%49s", students[i].name);

        while (!valid_name(students[i].name)) {
            scanf("%49s", students[i].name);
        }

        // Input and validate marks
        scanf("%d %d %d",
              &students[i].marks1,
              &students[i].marks2,
              &students[i].marks3);

        while (!valid_marks(students[i].marks1) ||
               !valid_marks(students[i].marks2) ||
               !valid_marks(students[i].marks3)) {

            scanf("%d %d %d",
                  &students[i].marks1,
                  &students[i].marks2,
                  &students[i].marks3);
        }
    }
}
// Display student results

void display_results(struct student students[], int n) {
    for (int i = 0; i < n; i++) {
        int total = calculate_total(students[i]);
        float average = calculate_average(total);
        char grade = calculate_grade(average);

        if (i > 0) {
            printf("\n");
        }

        printf("Roll: %d\n", students[i].roll_number);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);

        if (average < GRADE_D) {
            continue;
        }

        printf("Performance: ");
        print_pattern(grade);
        printf("\n");
    }
}

// Print roll numbers using recursion
void print_roll_numbers(struct student students[], int n, int index) {
    if (index == n) {
        return;
    }

    printf("%d ", students[index].roll_number);

    print_roll_numbers(students, n, index + 1);
}

// Main function

int main() {
    struct student students[MAX_STUDENTS];
    int n;

    scanf("%d", &n);

    if (n <= 0 || n > MAX_STUDENTS) {
        return 0;
    }

    input_students(students, n);
    display_results(students, n);

    printf("List of Roll Numbers (via recursion): ");
    print_roll_numbers(students, n, 0);
    printf("\n");

    return 0;
}
