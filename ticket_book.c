#include<stdio.h>

//definig ticket prices as constants
#define STANDARD 150
#define  PREMIUM 250
#define RECLINER 400

// Function declarations
int getPrice(int choice);
int isValidTickets(int tickets);
int isValidChoice(int choice);
void showCategory(int choice);
float getDiscount(int tickets, float subtotal);

//selecting ticket prices according to category
int getPrice(int choice)
{
    switch(choice){
        case 1:
         return STANDARD;
        case 2:
         return PREMIUM;
        case 3:
         return RECLINER;
        default:
         return 0;
    }
}

//function to check ticket count
int isValidTickets(int tickets){
    if(tickets >=1 && tickets <=20)
      return 1;
    else
      return 0;
}


//function to check category choice
int isValidChoice(int choice){
    if(choice >=1 && choice <= 3)
     return 1;
    else
     return 0;
}

//displaying the selected ticket category
void showCategory(int choice){
    if(choice==1){
        printf("Ticket Category: Standard\n");
    }
    else if(choice==2){
        printf("Ticket Category: Premium\n");
    }
    else{
        printf("Ticket Category: Recliner\n");
    }
}

//calculating discount for 5 or more tickets
float getDiscount(int ticket ,float subtotal){
    if(ticket>=5){
        return subtotal*10/100;
    }
    else{
        return 0;
    }
}

int main(){
    //declaring variables for booking tickets
    char name[50];
    int tickets,choice,price;
    float subtotal,discount,finalBill;

    //Taking customer details
    printf("Enter customer name: ");
    scanf("%49s",name);

    printf("Enter number of tickets: ");
    scanf("%d", &tickets);

    //checking ticket count
    if(!isValidTickets(tickets)){
        printf("Invalid number of tickets\n");
        return 1;
    }
    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ");
    scanf("%d", &choice);

    //checking category choice
    if(!isValidChoice(choice)){
        printf("Invalid ticket category\n");
        return 1;
    }

    //calculating ticket cost and discount
    price=getPrice(choice);
    subtotal=price*tickets;
    discount=getDiscount(tickets,subtotal);
    finalBill=subtotal-discount;

    //displaying booking details
    printf("\nCustomer: %s\n",name);
    showCategory(choice);
    printf("Tickets: %d\n",tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);

    return 0;
}