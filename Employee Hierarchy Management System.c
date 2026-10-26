#include <stdio.h>

#define MAX_EMPLOYEES 100
#define NAME_LENGTH 50

// Status codes returned by functions
typedef enum {
    STATUS_SUCCESS,
    STATUS_INVALID_INPUT,
    STATUS_INVALID_ID,
    STATUS_DUPLICATE_ID,
    STATUS_INVALID_TEXT,
    STATUS_MANAGER_NOT_FOUND,
    STATUS_EMPLOYEE_NOT_FOUND,
    STATUS_CANNOT_DELETE_CEO,
    STATUS_NO_SIBLING,
    STATUS_MEMORY_ERROR
} StatusCode;

// Menu options
typedef enum {
    ADD_EMPLOYEE = 1,
    REMOVE_EMPLOYEE =2 ,
    DISPLAY_EMPLOYEE_HIERARCHY=3,
    DISPLAY_COMPLETE_HIERARCHY=4,
    EXIT_PROGRAM=5
} MenuOption;

// Structure to store employee details
typedef struct {
    int id;
    char name[NAME_LENGTH];
    char designation[NAME_LENGTH];
    int managerId;
} Employee;

// Global array to store employees
Employee employees[MAX_EMPLOYEES];
int employeeCount = 0;

// Function declarations
void clearInput(void);
int readText(char text[], int size);
int readInteger(int *number);
int findEmployeeIndex(int id);
int countDirectReports(int employeeId);
int countSubordinates(int employeeId);
int displayHierarchy(int employeeId, int level);
int displayCompleteHierarchy(void);
StatusCode addEmployee(void);
StatusCode removeEmployee(int employeeId);
void showMenu(void);

// Clear extra input from the input buffer
void clearInput(void) {
    int character;

    while ((character = getchar()) != '\n' &&
           character != EOF) {
    }
}

// Read text with spaces
int readText(char text[], int size) {
    int index = 0;
    int character;

    if (fgets(text, size, stdin) == NULL) {
        return 0;
    }

    while (text[index] != '\0' && text[index] != '\n') {
        index++;
    }

    if (text[index] == '\n') {
        text[index] = '\0';
    } else {
        while ((character = getchar()) != '\n' && character != EOF) {
        }
    }

    return text[0] != '\0';
}

// Read an integer safely
int readInteger(int *number) {
    if (scanf("%d", number) != 1) {
        clearInput();
        return 0;
    }

    clearInput();
    return 1;
}

// Find employee by ID
int findEmployeeIndex(int id) {
    int index;

    for (index = 0; index < employeeCount; index++) {
        if (employees[index].id == id) {
            return index;
        }
    }

    return -1;
}

// Count direct reports of an employee
int countDirectReports(int employeeId) {
    int index;
    int count = 0;

    for (index = 0; index < employeeCount; index++) {
        if (employees[index].managerId == employeeId) {
            count++;
        }
    }

    return count;
}

// Count all indirect and direct subordinates recursively
int countSubordinates(int employeeId) {
    int index;
    int total = 0;

    for (index = 0; index < employeeCount; index++) {
        if (employees[index].managerId == employeeId) {
            total++;

            total += countSubordinates(employees[index].id);
        }
    }

    return total;
}

// Display one employee and their complete hierarchy recursively
int displayHierarchy(int employeeId, int level) {
    int index;
    int childIndex;
    int total = 0;

    index = findEmployeeIndex(employeeId);

    if (index == -1) {
        return 0;
    }

    for (childIndex = 0; childIndex < level; childIndex++) {
        printf("    ");
    }

    printf("ID: %d | Name: %s | Designation: %s\n",
           employees[index].id,
           employees[index].name,
           employees[index].designation);

    for (childIndex = 0; childIndex < employeeCount; childIndex++) {
        if (employees[childIndex].managerId == employeeId) {
            total++;

            total += displayHierarchy(
                employees[childIndex].id,
                level + 1
            );
        }
    }

    return total;
}

// Display the complete company hierarchy



int displayCompleteHierarchy(void) {
    int index;
    int total = 0;

    if (employeeCount == 0) {
        printf("No employees available.\n");
        return 0;
    }

    printf("\nComplete Employee Hierarchy:\n");

    for (index = 0; index < employeeCount; index++) {
        if (employees[index].managerId == 0) {
            displayHierarchy(employees[index].id, 0);
            total = countSubordinates(employees[index].id) + 1;
        }
    }

    printf("\nTotal employees: %d\n", total);

    return total;
}


// Add a new employee
StatusCode addEmployee(void) {
    Employee newEmployee;
    int managerIndex;
    int index;

    if (employeeCount >= MAX_EMPLOYEES) {
        printf("Employee limit reached.\n");
        return STATUS_MEMORY_ERROR;
    }

    printf("Enter employee ID: ");

    if (!readInteger(&newEmployee.id)) {
        printf("Invalid ID input.\n");
        return STATUS_INVALID_INPUT;
    }

    if (newEmployee.id <= 0) {
        printf("ID must be positive.\n");
        return STATUS_INVALID_ID;
    }

    if (findEmployeeIndex(newEmployee.id) != -1) {
        printf("This ID already exists.\n");
        return STATUS_DUPLICATE_ID;
    }

    printf("Enter employee name: ");

    if (!readText(newEmployee.name, NAME_LENGTH)) {
        printf("Name cannot be empty.\n");
        return STATUS_INVALID_TEXT;
    }

    printf("Enter designation: ");

    if (!readText(newEmployee.designation, NAME_LENGTH)) {
        printf("Designation cannot be empty.\n");
        return STATUS_INVALID_TEXT;
    }

    if (employeeCount == 0) {
        newEmployee.managerId = 0;

    } else {
        printf("Enter reporting manager ID: ");

        if (!readInteger(&newEmployee.managerId)) {
            printf("Invalid manager ID input.\n");
            return STATUS_INVALID_INPUT;
        }

        managerIndex = findEmployeeIndex(newEmployee.managerId);

        if (managerIndex == -1) {
            printf("Reporting manager not found.\n");
            return STATUS_MANAGER_NOT_FOUND;
        }

        if (newEmployee.managerId == newEmployee.id) {
            printf("An employee cannot manage themselves.\n");
            return STATUS_INVALID_ID;
        }
    }

    // Store the new employee in the array
    employees[employeeCount] = newEmployee;
    employeeCount++;

    printf("Employee added successfully.\n");

    return STATUS_SUCCESS;
}

// Remove an employee and transfer their direct reports
StatusCode removeEmployee(int employeeId) {
    int employeeIndex;
    int managerId;
    int siblingIndex = -1;
    int index;
    int minimumReports = MAX_EMPLOYEES + 1;
    int directReports;
    int removedIndex;

    employeeIndex = findEmployeeIndex(employeeId);

    if (employeeIndex == -1) {
        printf("Employee not found.\n");
        return STATUS_EMPLOYEE_NOT_FOUND;
    }

    // CEO cannot be removed
    if (employees[employeeIndex].managerId == 0) {
        printf("CEO cannot be removed.\n");
        return STATUS_CANNOT_DELETE_CEO;
    }

    managerId = employees[employeeIndex].managerId;

    // Find the sibling with the fewest direct reports
    for (index = 0; index < employeeCount; index++) {
        if (index != employeeIndex &&
            employees[index].managerId == managerId) {

            directReports = countDirectReports(employees[index].id);

            if (directReports < minimumReports) {
                minimumReports = directReports;
                siblingIndex = index;
            }
        }
    }

    // Cannot transfer reports if no sibling exists
    if (countDirectReports(employeeId) > 0 &&
        siblingIndex == -1) {
        printf("Cannot remove employee: no sibling manager available.\n");
        return STATUS_NO_SIBLING;
    }

    // Transfer direct reports to the selected sibling
    if (siblingIndex != -1) {
        for (index = 0; index < employeeCount; index++) {
            if (employees[index].managerId == employeeId) {
                employees[index].managerId =
                    employees[siblingIndex].id;
            }
        }
    }

    // Remove employee by shifting array elements
    for (removedIndex = employeeIndex;
         removedIndex < employeeCount - 1;
         removedIndex++) {

        employees[removedIndex] = employees[removedIndex + 1];
    }

    employeeCount--;

    printf("Employee removed successfully.\n");

    return STATUS_SUCCESS;
}

// Display menu
void showMenu(void) {
    printf("\n===== Employee Hierarchy System =====\n");
    printf("1. Add Employee\n");
    printf("2. Remove Employee\n");
    printf("3. Display Employee Hierarchy\n");
    printf("4. Display Complete Hierarchy\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main(void) {
    int choice;
    int employeeId;
    int index;
    StatusCode status;

    do {
        showMenu();

        if (!readInteger(&choice)) {
            printf("Please enter a valid menu number.\n");
            continue;
        }

        switch (choice) {
            case ADD_EMPLOYEE:
                status = addEmployee();
                break;

            case REMOVE_EMPLOYEE:
                printf("Enter employee ID to remove: ");

                if (!readInteger(&employeeId)) {
                    printf("Invalid ID input.\n");
                    break;
                }

                status = removeEmployee(employeeId);
                break;

            case DISPLAY_EMPLOYEE_HIERARCHY:
                printf("Enter employee ID: ");

                if (!readInteger(&employeeId)) {
                    printf("Invalid ID input.\n");
                    break;
                }

                index = findEmployeeIndex(employeeId);

                if (index == -1) {
                    printf("Employee not found.\n");
                } else {
                    displayHierarchy(employeeId, 0);

                    printf("Total subordinates: %d\n",
                           countSubordinates(employeeId));
                }
                break;

            case DISPLAY_COMPLETE_HIERARCHY:
                displayCompleteHierarchy();
                break;

            case EXIT_PROGRAM:
                printf("Program ended.\n");
                break;

            default:
                printf("Invalid menu choice.\n");
        }

    } while (choice != EXIT_PROGRAM);

    return 0;
}
