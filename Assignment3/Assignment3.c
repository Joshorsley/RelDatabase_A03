#include <mysql.h>
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>



void printSQLError(MYSQL* conn, const char* functionName);


void addNewRental(MYSQL* databaseObject);
void viewRentalHistory(MYSQL* databaseObject, int cust_id, char* startDate, char* endDate);
void showMenu();




int main()
{
	const char* server = "localhost";
	const char* username = "root";
	const char* password = "root";
	const char* defaultDatabase = "sakila";

	// step 1. Initialize the MySQL Object
	MYSQL* databaseObject = mysql_init(NULL);

	if (databaseObject == NULL)
	{
		printf("Error Initializing the Database Object\n");
		return EXIT_FAILURE;
	}

	// step 2. Connect to the MySQL Server
	if (!mysql_real_connect(databaseObject, server, username, password, defaultDatabase, 3306, NULL, 0))
	{
		printf("Error Connecting to the MySQL Server\n");
		return EXIT_FAILURE;
	}

	while (1)
	{
		int choice = 0;

		showMenu();
		if (GetInt(&choice) != SUCCESS)
		{
			printf("Invalid input. Please enter a number.\n");
			continue;
		}

		switch (choice)
		{
			case 1:
				addNewRental(databaseObject);
				break;
			case 2:
				// Update customer info goes here
				break;
			case 3:
				viewRentalHistory(databaseObject, 1, "2005-05-25 00:00:00", "2010-01-01 00:00:00");
				break;
			case 4:
				// Delete customer goes here
				break;
			case 5: 
				mysql_close(databaseObject);
				exit(EXIT_SUCCESS);
			default:
				printf("Invalid choice. Please try again.\n");

		}
	}

	mysql_close(databaseObject);
	return EXIT_SUCCESS;
}

// Function: addNewRental
// Description: This function prompts the user for input of a customer_id, inventory_id, and staff_id to enter a new rental record into the sakila database.
//				Before creating the record this function queries the inventory table to make sure the film is available for rent.
//				If the film is available the record is inserted into the table along with the rental_date and a NULL return_date.
//				If the film is unavailable then the user is offered the option of joining a waitlist
// Parameters:
// databaseObject - a pointer to a MYSQL database object
// Returns: void

void addNewRental(MYSQL* databaseObject)
{
	printf("You have chosen add new rental. please follow the instructions below:\n\n");

	int customer_id = 0;
	int inventory_id = 0;
	int staff_id = 0;

	printf("Enter Customer ID: ");
	if (GetInt(&customer_id) != SUCCESS)
	{
		printf("Invalid input for Customer ID.  Please enter a valid number.\n");
		return;
	}
	printf("Enter Inventory ID: ");
	if (GetInt(&inventory_id) != SUCCESS)
	{
		printf("Invalid input for Inventory ID.  Please enter a valid number.\n");
		return;
	}
	printf("Enter Staff ID: ");
	if (GetInt(&staff_id) != SUCCESS)
	{
		printf("Invalid input for Staff ID.  Please enter a valid number.\n");
		return;
	}

	char query[512];
	sprintf(query,
		"SELECT COUNT(*) AS available_count" 
		"FROM inventory i"
		"LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL" 
		"WHERE i.inventory_id = %d AND r.rental_id IS NULL",
		inventory_id);

	if (mysql_query(databaseObject, query) != 0)
	{
		printf("Error Executing the Query\n");
		return;
	}
	MYSQL_RES* result = mysql_store_result(databaseObject);
	if (result == NULL)
	{
		printSQLError(databaseObject, "mysql_store_result");
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(result);
	int available_count = atoi(row[0]);

	printf("%d are available for rent\n", available_count);
	mysql_free_result(result);

	if (available_count > 0)
	{
		sprintf(query,
			"INSERT INTO rental (rental_date, inventory_id, customer_id, staff_id, return_date) "
			"VALUES (NOW(), %d, %d, %d, NULL);",
			inventory_id, customer_id, staff_id);

		if (mysql_query(databaseObject, query) != 0)
		{
			printSQLError(databaseObject, "mysql_query");
			return;
		}

		printf("Rental record added successfully.\n");

	}
	else
	{
		char choice = NULL;
		printf("Rental not available. Add customer to waitlist? (y/n)");
		choice = getchar();
		if (choice == 'y' || choice == 'Y')
		{
			printf("Customer added to the waitlist.");
		}
		else
		{
			printf("Customer not added to the waitlist.\n");
		}
	}
}


void showMenu()
{
	printf("\n ----- MENU -----\n");
	printf("1. Add A New Rental\n");
	printf("2. Update Customer Information\n");
	printf("3. View Rental History\n");
	printf("4. Delete Customer Record\n");
	printf("5. EXIT\n");
	printf("-----------------\n");
	printf("Enter your choice: ");
}

//Looks ugly as heck cuz of the big long date strings
//Should probably try to format better..
void viewRentalHistory(MYSQL* databaseObject, int cust_id, char* startDate, char* endDate) {

	char query[256];
	sprintf(query,
		"SELECT * FROM rental WHERE customer_id = %d AND rental_date >= %s 00:00:00' AND return_date <= '%s 00:00:00';",
		cust_id, startDate, endDate);

	//Send query
	if (mysql_query(databaseObject, query) != 0)
	{
		printSQLError(databaseObject, "mysql_query");
		return;
	}
	//Grab the results
	MYSQL_RES* result = mysql_store_result(databaseObject);
	if (result == NULL)
	{
		printSQLError(databaseObject, "mysql_store_result");
		return;
	}

	//Get column count for the result
	int num_fields = mysql_num_fields(result);
	MYSQL_ROW row;

	//Print column names
	MYSQL_FIELD* fields = mysql_fetch_fields(result);
	for (int i = 0; i < num_fields; i++) {
		printf("%s\t", fields[i].name);
	}
	printf("\n");

	//Print each row
	while ((row = mysql_fetch_row(result)) != NULL) {
		for (int i = 0; i < num_fields; i++) {
			printf("%s\t", row[i] ? row[i] : "NULL");
		}
		printf("\n");
	}
}

void printSQLError(MYSQL* conn, const char* functionName)
{
	printf("%s failed:\nError %u: %s\n",
		functionName, mysql_errno(conn), mysql_error(conn));
}