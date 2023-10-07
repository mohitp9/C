#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PRODUCTS 100
#define MAX_EMPLOYEES 50
#define MAX_CUSTOMERS 100
#define MAX_SALES 100

#define INVENTORY_FILE "inventory.txt"
#define EMPLOYEE_FILE "employees.txt"

enum Category {
    GROCERY,
    ELECTRONICS,
    CLOTHING,
    COSMETICS,
};

struct Product {
    char name[50];
    enum Category category;
    float price;
    int quantity;
};

struct Employee {
    char name[50];
    int role; // 0-Cashier, 1-Manager, 2-Stock Clerk, etc.
    int hours_worked;
};

struct Customer {
    char name[50];
    char email[100];
    int loyalty_points;
};

struct SaleItem {
    struct Product product;
    int quantity;
};

struct Sale {
    struct Customer customer;
    struct SaleItem items[MAX_PRODUCTS];
    int num_items;
    float total_cost;
};
#define RESTOCK_THRESHOLD 30
void restockProduct(struct Product *product) {
    if (product->quantity < RESTOCK_THRESHOLD) {
        printf("Restocking %s...\n", product->name);
        product->quantity += 20;
    }
}

void displayProduct(const struct Product *product) {
    printf("Name: %s\n", product->name);
    printf("Category: %d\n", product->category);
    printf("Price: %.2f\n", product->price);
    printf("Quantity: %d\n", product->quantity);
}

struct Product* searchProductByName(const char *name, struct Product *inventory, int size, int index) {
    if (index >= size) {
        return NULL;
    }
    if (strcmp(name, inventory[index].name) == 0) {
        return &inventory[index];
    }
    return searchProductByName(name, inventory, size, index + 1);
}

void processSale(struct Sale *sale, struct Product *inventory, int numProducts) {
    printf("Enter customer name: ");
    scanf("%s", sale->customer.name);

    printf("Enter customer email: ");
    scanf("%s", sale->customer.email);

    sale->num_items = 0;
    sale->total_cost = 0;

    int continueShopping = 1;
    while (continueShopping) {
        printf("\nProducts available for sale:\n");
        for (int i = 0; i < numProducts; i++) {
            printf("%d. %s - $%.2f\n", i + 1, inventory[i].name, inventory[i].price);
        }

        int choice, quantity;
        printf("Enter the product number (0 to finish shopping): ");
        scanf("%d", &choice);
        if (choice == 0) {
            continueShopping = 0;
            break;
        } else if (choice < 1 || choice > numProducts) {
            printf("Invalid product choice.\n");
            continue;
        }

        printf("Enter quantity: ");
        scanf("%d", &quantity);

        if (quantity <= 0 || quantity > inventory[choice - 1].quantity) {
            printf("Invalid quantity or insufficient stock.\n");
        } else {
            sale->items[sale->num_items].product = inventory[choice - 1];
            sale->items[sale->num_items].quantity = quantity;
            sale->total_cost += inventory[choice - 1].price * quantity;
            inventory[choice - 1].quantity -= quantity;
            sale->num_items++;
        }
    }

    // Update customer loyalty points based on total cost
    if (sale->total_cost >= 100) {
        sale->customer.loyalty_points += 10;
    }

    // Print the receipt
    printf("\nReceipt:\n");
    printf("Customer: %s\n", sale->customer.name);
    printf("Email: %s\n", sale->customer.email);
    printf("Items Purchased:\n");
    for (int i = 0; i < sale->num_items; i++) {
        printf("%s x%d - $%.2f\n", sale->items[i].product.name, sale->items[i].quantity, sale->items[i].product.price);
    }
    printf("Total Cost: $%.2f\n", sale->total_cost);
    printf("Loyalty Points Earned: %d\n", sale->customer.loyalty_points);
}

// Function to display employee details
void displayEmployees(const struct Employee *employees, int numEmployees) {
    printf("Employee Details:\n");
    for (int i = 0; i < numEmployees; i++) {
        printf("Name: %s\n", employees[i].name);
        printf("Role: %d\n", employees[i].role);
        printf("Hours Worked: %d\n", employees[i].hours_worked);
        printf("----------------------------\n");
    }
}

// Function to save inventory data to a file
void saveInventoryToFile(const struct Product *inventory, int numProducts) {
    FILE *file = fopen(INVENTORY_FILE, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    for (int i = 0; i < numProducts; i++) {
        fprintf(file, "%s|%d|%.2f|%d\n", inventory[i].name, inventory[i].category, inventory[i].price, inventory[i].quantity);
    }

    fclose(file);
}

// Function to load inventory data from a file
int loadInventoryFromFile(struct Product *inventory, int maxProducts) {
    FILE *file = fopen(INVENTORY_FILE, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return 0;
    }

    int numProducts = 0;
    while (numProducts < maxProducts && fscanf(file, "%49[^|]|%d|%f|%d\n", inventory[numProducts].name, &inventory[numProducts].category, &inventory[numProducts].price, &inventory[numProducts].quantity) == 4) {
        numProducts++;
    }

    fclose(file);
    return numProducts;
}
// Function to save employee data to a file
void saveEmployeesToFile(const struct Employee *employees, int numEmployees) {
    FILE *file = fopen("employees.txt", "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    for (int i = 0; i < numEmployees; i++) {
        fprintf(file, "%s|%d|%d\n", employees[i].name, employees[i].role, employees[i].hours_worked);
    }

    fclose(file);
}
// Function to load employee data from a file
int loadEmployeesFromFile(struct Employee *employees, int maxEmployees) {
    FILE *file = fopen("employees.txt", "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return 0;
    }

    int numEmployees = 0;
    while (numEmployees < maxEmployees && fscanf(file, "%49[^|]|%d|%d\n", employees[numEmployees].name, &employees[numEmployees].role, &employees[numEmployees].hours_worked) == 3) {
        numEmployees++;
    }

    fclose(file);
    return numEmployees;
}

int main() {
    struct Product inventory[MAX_PRODUCTS];
    int numProducts = 0;
     numProducts = loadInventoryFromFile(inventory, MAX_PRODUCTS);

    struct Employee employees[MAX_EMPLOYEES];
    int numEmployees = 0;

    struct Customer customers[MAX_CUSTOMERS];
    int numCustomers = 0;

    struct Sale sales[MAX_SALES];
    int numSales = 0;

    int choice;
    while (1) {
       printf("\n\033[1;36mSupermarket Management Menu:\033[0m\n");
        printf("\033[1;32m1. Add Product\033[0m\n");
        printf("\033[1;32m2. Display Product Details\033[0m\n");
        printf("\033[1;32m3. Restock Product\033[0m\n");
        printf("\033[1;32m4. Search Product by Name\033[0m\n");
        printf("\033[1;32m5. Add Employee\033[0m\n");
        printf("\033[1;32m6. Display Employees\033[0m\n"); 
        printf("\033[1;32m7. Process Sale\033[0m\n");
        printf("\033[1;32m8. Display Sales Report\033[0m\n");
        printf("\033[1;32m9. Save Inventory to File\033[0m\n");
        printf("\033[1;32m10. Load Inventory from File\033[0m\n");
        printf("\033[1;32m11. Save Employees to File\033[0m\n");
        printf("\033[1;32m12. Load Employees from File\033[0m\n");
        printf("\033[1;31m13. Exit\033[0m\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
    case 1:
    {
        // Add Product
        if (numProducts < MAX_PRODUCTS) {
            printf("Enter product details:\n");
            printf("Name: ");
            scanf("%s", inventory[numProducts].name);
            printf("Category (0-Grocery, 1-Electronics, 2-Clothing, 3-Cosmetics): ");
            scanf("%d", &inventory[numProducts].category);
            printf("Price: ");
            scanf("%f", &inventory[numProducts].price);
            printf("Quantity: ");
            scanf("%d", &inventory[numProducts].quantity);
            numProducts++;
        }
        else {
            printf("Inventory is full. Cannot add more products.\n");
        }
        break;
    }
    case 2:
    {
        // Display Product Details
        printf("Enter the index of the product to display details: ");
        int index;
        scanf("%d", &index);
        if (index >= 0 && index < numProducts) {
            displayProduct(&inventory[index]);
        }
        else {
            printf("Invalid index.\n");
        }
        break;
    }
    case 3:
    {
        // Restock Product
        int index;
        printf("Enter the index of the product to restock: ");
        scanf("%d", &index);
        if (index >= 0 && index < numProducts) {
            restockProduct(&inventory[index]);
        }
        else {
            printf("Invalid index.\n");
        }
        break;
    }
    case 4:
    {
        // Search Product by Name
        printf("Enter the name of the product to search for: ");
        char searchName[50];
        scanf("%s", searchName);
        struct Product* foundProduct = searchProductByName(searchName, inventory, numProducts, 0);
        if (foundProduct != NULL) {
            printf("\n%s found in inventory.\n", searchName);
            displayProduct(foundProduct);
        }
        else {
            printf("\n%s not found in inventory.\n", searchName);
        }
        break;
    }
    case 5:
    {
        // Add Employee
        if (numEmployees < MAX_EMPLOYEES) {
            printf("Enter employee details:\n");
            printf("Name: ");
            scanf("%s", employees[numEmployees].name);
            printf("Role (0-Cashier, 1-Manager, 2-Stock Clerk, etc.): ");
            scanf("%d", &employees[numEmployees].role);
            employees[numEmployees].hours_worked = 0;
            numEmployees++;
        }
        else {
            printf("Employee list is full. Cannot add more employees.\n");
        }
        break;
    }
    case 6:
    {
        // Display Employees
        printf("Employees:\n");
        for (int i = 0; i < numEmployees; i++) {
            printf("Employee %d:\n", i + 1);
            printf("Name: %s\n", employees[i].name);
            printf("Role: %d\n", employees[i].role);
            printf("Hours Worked: %d\n", employees[i].hours_worked);
        }
        break;
    }
    case 7:
    {
        // Process Sale
        if (numProducts == 0) {
            printf("No products available for sale. Add products first.\n");
            break;
        }
        struct Sale newSale;
        processSale(&newSale, inventory, numProducts);
        sales[numSales] = newSale;
        numSales++;
        printf("Sale processed successfully.\n");
        break;
    }
    case 8:
    {
        // Display Sales Report
        printf("Sales Report:\n");
        for (int i = 0; i < numSales; i++) {
            printf("Sale %d:\n", i + 1);
            printf("Customer: %s\n", sales[i].customer.name);
            printf("Total Cost: $%.2f\n", sales[i].total_cost);
        }
        break;
    }
    case 9:
    {
        // Save Inventory to File
        saveInventoryToFile(inventory, numProducts);
        printf("Inventory saved to file.\n");
        break;
    }
    case 10:
    {
        // Load Inventory from File
        numProducts = loadInventoryFromFile(inventory, MAX_PRODUCTS);
        printf("Inventory loaded from file.\n");
        break;
    }
    case 11:
    {
        // Save Employees to File
        saveEmployeesToFile(employees, numEmployees);
        printf("Employees saved to file.\n");
        break;
    }
    case 12:
    {
        // Load Employees from File
        numEmployees = loadEmployeesFromFile(employees, MAX_EMPLOYEES);
        printf("Employees loaded from file.\n");
        break;
    }
    case 13:
    {
        printf("Exiting the program.\n");
        return 0;
    }
    default:
    {
        printf("Invalid choice. Please try again.\n");
    }
}


		}
    
	   

    return 0;
}

