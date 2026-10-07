#include<stdio.h>
struct student{
    int roll_number;
    char name[50];
    int marks1;
    int marks2;
    int marks3;
};

int calculate_total(struct student s){
    int total;

    total=s.marks1+s.marks2+s.marks3;

    return total;
}

float calculate_average(int total){
    float average;

    average=total/3.0;

    return average;
}

char calculate_grade(float average){

    if(average>=85){
        return 'A';
    }

    else if(average>=70){
        return 'B';
    }

    else if(average>=50){
        return 'C';
    }

    else if(average>=35){
        return 'D';
    }

    else{
        return 'F';
    }
}

void print_pattern(char grade){
     
    int stars=0;

    if(grade=='A'){
        stars=5;
    }
    else if(grade=='B'){
        stars=4;
    }
    else if(grade=='C'){
        stars=3;
    }
    else if(grade=='D'){
        stars=2;
    }
    for(int i = 1; i <= stars; i++) { 
        printf("*"); 
    }
}

void print_roll_numbers(struct student students[],int n,int index){
    if(n==0){
        return ;
    }
    printf("%d ",students[index].roll_number);

    print_roll_numbers(students ,n-1 ,index+1);

}

int main(){
    int n;
    struct student students[100];

    printf("Enter number of students: ");
    scanf("%d",&n);

    printf("\n%-10s %-15s %-10s %-10s %-10s\n", "Roll No.", "Name", "Marks1", "Marks2", "Marks3"); 
    printf("------------------------------------------------------------\n");

    //taking input
    for(int i=0;i<n;i++){
        scanf("%d %s %d %d %d",
        &students[i].roll_number,
        students[i].name,
        &students[i].marks1,
        &students[i].marks2,
        &students[i].marks3);
    }

    //display result for each student
    for(int i=0;i<n;i++){
        
        int total=calculate_total(students[i]);
        float average=calculate_average(total);
        char grade=calculate_grade(average);

        printf("\nRoll: %d\n", students[i].roll_number);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n",total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n" ,grade);
        
        //skip performance whwn average is below 35
        if(average<35){
            continue;
        }
        printf("Performance: ");
        print_pattern(grade);
        printf("\n");
    }

    //print actual roll number using recursion
   printf("\nList of Roll Numbers (via recursion): "); 
   print_roll_numbers(students, n, 0); printf("\n"); 

   return 0;
}