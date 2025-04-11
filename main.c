#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for customer data
struct customer
{
    int c_id;
    char c_name[50];
    float price;
};

// Structure for bill data
struct bill
{
    int b_id;
    int c_id;
    char c_name[50];
    float price;
    int units;
};

// Function to generate customer ID
int customer_id()
{
    int a;
    FILE *fp;
    fp = fopen("cus.txt", "r");
    if (fp == NULL)
        a = 1;
    else
    {
        fscanf(fp, "%d", &a);
        a++;
    }
    fclose(fp);
    fp = fopen("cus.txt", "w");
    fprintf(fp, "%d", a);
    fclose(fp);
    return a;
}

// Function to generate bill ID
int bill_id()
{
    int y;
    FILE *fp;
    fp = fopen("bi.txt", "r");
    if (fp == NULL)
        y = 1;
    else
    {
        fscanf(fp, "%d", &y);
        y++;
    }
    fclose(fp);
    fp = fopen("bi.txt", "w");
    fprintf(fp, "%d", y);
    fclose(fp);
    return y;
}

void send_email(const char *email, const char *subject, const char *message)
{
    char command[4096]; // Increased buffer size for long messages

    // Replace newlines with ` (PowerShell escape sequence)
    char formatted_message[3000];
    int i, j = 0;
    for (i = 0; message[i] != '\0'; i++)
    {
        if (message[i] == '\n')
        {
            formatted_message[j++] = '`'; // PowerShell escape character
            formatted_message[j++] = 'n';
        }
        else
        {
            formatted_message[j++] = message[i];
        }
    }
    formatted_message[j] = '\0'; // Null-terminate the string

    // Format command to call PowerShell script with escaped arguments
    snprintf(command, sizeof(command),
             "powershell.exe -ExecutionPolicy Bypass -File send_email.ps1 \"%s\" \"%s\" \"%s\"",
             email, subject, formatted_message);

    system(command);
}

// Function to add a new customer
void add_customer()
{
    FILE *ptr;
    struct customer c;
    ptr = fopen("cust.txt", "a");
    fflush(stdin);
    printf("\n Enter customer details :- \n Name - ");
    fgets(c.c_name, sizeof(c.c_name), stdin); // Safer input handling
    c.c_name[strcspn(c.c_name, "\n")] = 0;    // Remove newline character from input
    fflush(stdin);
    printf("\n Price of each unit - ");
    scanf("%f", &c.price);
    c.c_id = customer_id();
    printf("\n Record added successfully. ");
    printf("\n Your customer id is %d", c.c_id);
    fwrite(&c, sizeof(c), 1, ptr);
    fclose(ptr);
}

// Function to generate a bill
void generate_bill()
{
    int find, flag = 0;
    FILE *ptr, *ptr1;
    struct customer c;
    struct bill b;
    printf("\n Enter your customer id: ");
    scanf("%d", &find);

    ptr = fopen("cust.txt", "r");
    while (!feof(ptr))
    {
        fread(&c, sizeof(c), 1, ptr);
        if (c.c_id == find)
        {
            flag = 1;
            ptr1 = fopen("bill1.txt", "a");
            b.b_id = bill_id();
            strcpy(b.c_name, c.c_name);
            b.price = c.price;
            b.c_id = c.c_id;
            printf("\n Enter number of units consumed this month: ");
            scanf("%d", &b.units);
            fflush(stdin);
            printf("\n Bill has been generated successfully. ");
            printf("\n Your bill id is %d", b.b_id);
            fwrite(&b, sizeof(b), 1, ptr1);
            fclose(ptr1);

            // Send email after generating bill
            char email[100];
            printf("\n Enter the email address to send the bill: ");
            scanf("%s", email);
            char subject[100] = "Electricity Bill Notification";
            char message[1000]; // Increased message length for more detailed content

            // Construct the email body with actual newlines
            snprintf(message, sizeof(message),
                     "Dear %s, Your electricity bill for the current month has been generated successfully. "
                     "Bill ID: %d,  Customer ID: %d, Units Consumed: %d,  Price per Unit: %.2f,  "
                     "Total Amount: %.2f.Thank you for using our Service. Best regards, Electricity Billing System",
                     b.c_name, b.b_id, b.c_id, b.units, b.price, b.price * b.units);

            // Send the email
            send_email(email, subject, message);

            break;
        }
    }
    if (flag == 0)
        printf("\n Error! No such customer with id no. %d exists.", find);
    fclose(ptr);
}

void display_bill()
{
    int flag = 0, billid, custid;
    FILE *ptr1;
    struct bill b;
    ptr1 = fopen("bill1.txt", "r");
    printf("\n Enter your customer id: ");
    scanf("%d", &custid);
    fflush(stdin);
    printf("\n Enter bill id: ");
    scanf("%d", &billid);
    while (!feof(ptr1))
    {
        fread(&b, sizeof(b), 1, ptr1);
        if ((b.b_id == billid) && (b.c_id == custid))
        {
            flag++;
            printf("\n Electricity Bill");
            printf("\n Bill id: %d", b.b_id);
            printf("\n Name: %s", b.c_name);
            printf("\n Customer id: %d", b.c_id);
            printf("\n Price per unit: %.2f", b.price);
            printf("\n Units consumed: %d", b.units);
            printf("\n Total amount: %.2f", b.price * b.units);
            break;
        }
    }
    if (flag == 0)
        printf("\n Error! No such customer id no. %d OR bill no. %d exists.", custid, billid);
    fclose(ptr1);
}

void delete_bill()
{
    int flag = 0, billid;
    FILE *ptr, *ptr1;
    struct bill b;
    ptr = fopen("bill1.txt", "r");
    ptr1 = fopen("temp.txt", "w");
    printf("\n Enter bill id to delete: ");
    scanf("%d", &billid);
    while (!feof(ptr))
    {
        fread(&b, sizeof(b), 1, ptr);
        if (b.b_id == billid)
        {
            printf("\n Bill with id no. %d deleted successfully.", b.b_id);
            flag = 1;
        }
        else
        {
            fwrite(&b, sizeof(b), 1, ptr1);
        }
    }
    fclose(ptr);
    fclose(ptr1);
    if (flag == 0)
        printf("\n Error! No such bill with id no. %d exists.", billid);
    remove("bill1.txt");
    rename("temp.txt", "bill1.txt");
}

void view_customers()
{
    FILE *ptr;
    struct customer c;
    ptr = fopen("cust.txt", "r");
    if (ptr == NULL)
    {
        printf("No customers found.\n");
        return;
    }

    printf("\n%15s %25s %15s\n", "Customer ID", "Customer Name", "Price per Unit");
    printf("-------------------------------------------------------------\n");

    while (fread(&c, sizeof(c), 1, ptr) == 1)
    {
        printf("%15d %25s %15.2f\n", c.c_id, c.c_name, c.price);
    }

    fclose(ptr);
}

void export_to_csv()
{
    FILE *custFile, *billFile;
    struct customer c;
    struct bill b;

    custFile = fopen("cust.txt", "r");
    if (custFile == NULL)
    {
        printf("No customers found.\n");
        return;
    }

    billFile = fopen("bill1.txt", "r");
    if (billFile == NULL)
    {
        printf("No bills found.\n");
        fclose(custFile);
        return;
    }

    // Store customer and bill data in memory
    struct bill bills[100]; // Max 100 bills (can increase if needed)
    int billCount = 0;

    // Read existing bill data into memory
    while (fread(&b, sizeof(b), 1, billFile) == 1)
    {
        bills[billCount++] = b;
    }
    fclose(billFile);

    struct customer customers[100];
    int custCount = 0;
    while (fread(&c, sizeof(c), 1, custFile) == 1)
    {
        customers[custCount++] = c;
    }
    fclose(custFile);

    FILE *csvfile = fopen("data.csv", "w");
    if (csvfile == NULL)
    {
        printf("Error opening the CSV file.\n");
        return;
    }

    fprintf(csvfile, "Bill id,Name,Customer id,Price per unit,Units consumed,Total amount\n");

    // Write data from customer and bill structures
    for (int i = 0; i < billCount; i++)
    {
        for (int j = 0; j < custCount; j++)
        {
            if (bills[i].c_id == customers[j].c_id)
            {
                fprintf(csvfile, "%d,%s,%d,%.2f,%d,%.2f\n",
                        bills[i].b_id,
                        customers[j].c_name,
                        customers[j].c_id,
                        customers[j].price,
                        bills[i].units,
                        customers[j].price * bills[i].units);
            }
        }
    }

    fclose(csvfile);

    printf("Data exported to data.csv successfully.\n");
}

int main()
{
    char username[20], password[20];
    int choice;

    printf("Please enter the username: ");
    scanf("%s", username);
    printf("Please enter the password: ");
    scanf("%s", password);

    if (strcmp(username, "admin") == 0)
    {
        if (strcmp(password, "12345") == 0)
        {
            printf("\nWelcome, login successful!\n");
        }
        else
        {
            printf("\nInvalid password\n");
            exit(0);
        }
    }
    else
    {
        printf("\nInvalid username\n");
        exit(0);
    }

    while (1)
    {
        printf("\n**************************************\n");
        printf("Electricity Bill Calculator\n");
        printf("**************************************\n");

        printf("\nMAIN MENU\n");
        printf("1. Add new customer\n");
        printf("2. Bill generation\n");
        printf("3. Display Bill\n");
        printf("4. Delete Bill\n");
        printf("5. View all customers\n");
        printf("6. Export data to CSV\n");
        printf("7. Exit\n");

        printf("\nPlease select an option: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            add_customer();
            break;
        case 2:
            generate_bill();
            break;
        case 3:
            display_bill();
            break;
        case 4:
            delete_bill();
            break;
        case 5:
            view_customers();
            break;
        case 6:
            export_to_csv();
            break;
        case 7:
            printf("Exiting system...\n");
            exit(0);
        default:
            printf("\nInvalid choice. Please try again.\n");
        }
    }
    return 0;
}
