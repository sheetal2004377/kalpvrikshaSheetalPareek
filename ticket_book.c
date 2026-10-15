#include <stdio.h>

// Defining constants for ticket prices
#define STANDARD_PRICE 150
#define PREMIUM_PRICE 250
#define RECLINER_PRICE 400

// Defining constants for ticket limits and discount
#define MIN_TICKETS 1
#define MAX_TICKETS 20
#define DISCOUNT_LIMIT 5
#define DISCOUNT_RATE 10

// Defining enum for ticket categories
typedef enum {
    STANDARD = 1,
    PREMIUM,
    RECLINER
} TicketCategory;

// Function declarations
int validateName(char name[]);
int isValidTickets(int tickets);
int isValidChoice(int choice);
int getPrice(TicketCategory choice);
int takeInputs(char name[], int *tickets, TicketCategory *category);
float getDiscount(int tickets, float subtotal);
void showCategory(TicketCategory choice);
void displayTicketDetails(char name[], int tickets,
                          TicketCategory category,
                          float subtotal, float discount,
                          float finalBill);

// Function to validate customer name
int validateName(char name[])
{
    int i = 0;

    if (name[0] == '\0')
        return 0;

    while (name[i] != '\0')
    {
        if (!((name[i] >= 'A' && name[i] <= 'Z') ||
              (name[i] >= 'a' && name[i] <= 'z')))
            return 0;

        i++;
    }

    return 1;
}

// Function to check ticket count
int isValidTickets(int tickets)
{
    if (tickets >= MIN_TICKETS && tickets <= MAX_TICKETS)
        return 1;
    else
        return 0;
}

// Function to check category choice
int isValidChoice(int choice)
{
    if (choice >= STANDARD && choice <= RECLINER)
        return 1;
    else
        return 0;
}

// Function to get ticket price
int getPrice(TicketCategory choice)
{
    switch (choice)
    {
        case STANDARD:
            return STANDARD_PRICE;

        case PREMIUM:
            return PREMIUM_PRICE;

        case RECLINER:
            return RECLINER_PRICE;

        default:
            return 0;
    }
}

// Function to take and validate inputs
int takeInputs(char name[], int *tickets, TicketCategory *category)
{
    int choice;

    // Taking customer name
    printf("Enter customer name: ");
    if (scanf("%49s", name) != 1 || !validateName(name))
    {
        printf("Invalid customer name\n");
        return 1;
    }

    // Taking number of tickets
    printf("Enter number of tickets: ");
    if (scanf("%d", tickets) != 1)
    {
        printf("Invalid input\n");
        return 1;
    }

    if (!isValidTickets(*tickets))
    {
        printf("Invalid number of tickets\n");
        return 1;
    }

    // Taking ticket category
    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner): ");
    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input\n");
        return 1;
    }

    if (!isValidChoice(choice))
    {
        printf("Invalid ticket category\n");
        return 1;
    }

    // Storing selected category
    *category = (TicketCategory)choice;

    return 0;
}

// Function to calculate discount
float getDiscount(int tickets, float subtotal)
{
    if (tickets >= DISCOUNT_LIMIT)
        return subtotal * DISCOUNT_RATE / 100.0;
    else
        return 0;
}

// Function to display ticket category
void showCategory(TicketCategory choice)
{
    switch (choice)
    {
        case STANDARD:
            printf("Ticket Category: Standard\n");
            break;

        case PREMIUM:
            printf("Ticket Category: Premium\n");
            break;

        case RECLINER:
            printf("Ticket Category: Recliner\n");
            break;
    }
}

// Function to display booking details
void displayTicketDetails(char name[], int tickets,
                          TicketCategory category,
                          float subtotal, float discount,
                          float finalBill)
{
    printf("Customer: %s\n", name);
    showCategory(category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: Rs. %.2f\n", subtotal);
    printf("Discount: Rs. %.2f\n", discount);
    printf("Final Bill: Rs. %.2f\n", finalBill);
}

// Main function: program execution starts here
int main()
{
    // Declaring variables for booking details
    char customerName[50];
    int tickets, price;
    TicketCategory category;
    float subtotal, discount, finalBill;

    // Taking inputs and checking status
    if (takeInputs(customerName, &tickets, &category) != 0)
        return 1;

    // Calculating ticket cost and discount
    price = getPrice(category);
    subtotal = price * tickets;
    discount = getDiscount(tickets, subtotal);
    finalBill = subtotal - discount;

    // Displaying booking details
    displayTicketDetails(customerName, tickets, category,
                         subtotal, discount, finalBill);

    return 0;
}
