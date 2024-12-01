#include <mysql.h>
#include "Utils.h"



void printSQLError(MYSQL* conn, const char* functionName);


void addNewRental(MYSQL* databaseObject);
void viewRentalHistory(MYSQL* databaseObject, int cust_id, char* startDate, char* endDate);
void deleteCustomerRecord(MYSQL* databaseObject);
void showMenu();
void updateCustomerInfo(MYSQL* databaseObject);




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
				updateCustomerInfo(databaseObject);
				break;
			case 3:
				viewRentalHistory(databaseObject, 1, "2005-05-25 00:00:00", "2010-01-01 00:00:00");
				break;
			case 4:
				deleteCustomerRecord(databaseObject);
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

// Function:	addNewRental
// Description: This function prompts the user for input of a customer_id, inventory_id, and staff_id to enter a new rental record into the sakila database.
//				Before creating the record this function queries the inventory table to make sure the film is available for rent.
//				If the film is available the record is inserted into the table along with the rental_date and a NULL return_date.
//				If the film is unavailable then the user is offered the option of joining a waitlist
// Parameters:	databaseObject - a pointer to a MYSQL database object
// Returns:		void

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
		"SELECT COUNT(*) AS available_count "
		"FROM inventory i "
		"LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL "
		"WHERE i.inventory_id = %d AND r.rental_id IS NULL;",
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
		char choice[256]; // Buffer for input
		printf("Rental not available. Add customer to waitlist? (y/n): ");

		
		if (fgets(choice, sizeof(choice), stdin) != NULL)
		{
			//
			if (choice[0] == 'y' || choice[0] == 'Y')
			{
				printf("Customer added to the waitlist.\n");
			}
			else if (choice[0] == 'n' || choice[0] == 'N')
			{
				printf("Customer not added to the waitlist.\n");
			}
			else
			{
				printf("Invalid input. Please enter 'y' or 'n'.\n");
			}
		}
		else
		{
			printf("Input error. Please try again.\n");
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


void viewRentalHistory(MYSQL* databaseObject, int cust_id, char* startDate, char* endDate) {

	char query[256];
	sprintf(query,
		"SELECT * FROM rental WHERE customer_id = %d AND rental_date >= '%s' AND return_date <= '%s';",
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


// Function:	deleteCustomerRecord
// Description: This function is used to delete a customer record from the sakila database.  
//				The user is prompted for a customer ID and the function will check if the customer has any 
//				active rentals. If the customer has active rentals the function will not delete the record.
// 				If the customer has no active rentals the function will prompt the user to confirm the deletion. 
// Parameters:	databaseObject - a pointer to a MYSQL database object
// Returns:		void
void deleteCustomerRecord(MYSQL* databaseObject)
{
	//Prompt user for customer ID
	printf("You have chosen to delete a customer record. Please enter the customer ID: ");
	int customer_id = 0;
	if (GetInt(&customer_id) != SUCCESS)
	{
		printf("Invalid input for Customer ID.  Please enter a valid number.\n");
		return;
	}

	//Check if customer has any rentals, if so, do not delete
	char query[256];
	sprintf(query,
		"SELECT COUNT(*) AS rental_count FROM rental WHERE customer_id = %d AND return_date IS NULL;",
		customer_id);

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

	//Get the rental count
	MYSQL_ROW row = mysql_fetch_row(result);
	int rental_count = atoi(row[0]);
	mysql_free_result(result);

	//If there are active rentals, do not delete
	if (rental_count > 0)
	{
		printf("Unable to delete customer, there are still %d active rentals. \n", rental_count);
		return;
	}

	//Prompt user to confirm deletion
	printf("Are you sure you want to delete this customer record? (y/n): ");
	char choice[256];
	if (GetString(choice, sizeof(choice)) != SUCCESS)
	{
		printf("Input error. Deletion cancelled.\n");
		return;
	}

	if (choice[0] != 'y' && choice[0] != 'Y')
	{
		printf("Deletion cancelled.\n");
		return;
	}

	//Deleting the customer record
	sprintf(query, "DELETE FROM customer WHERE customer_id = %d;", customer_id);
	if (mysql_query(databaseObject, query) != 0)
	{
		printSQLError(databaseObject, "mysql_query");
		return;
	}

	printf("Customer record deleted successfully.\n");

}

void printSQLError(MYSQL* conn, const char* functionName)
{
	printf("%s failed:\nError %u: %s\n",
		functionName, mysql_errno(conn), mysql_error(conn));
}

// Function:	updateCustomerInfo
// Description: This function prompts the user for input for the customer ID they wish to update. After the customer ID has been
//				verified the user is promped to enter a new first name, last name, email, and address ID. The email entered is 
//				checked for proper formatting and the address ID is verified through the address table. If all information is valid
//				the customer with the customer ID entered by the user will be updated. 
// Parameters:	databaseObject - a pointer to a MYSQL database object
// Returns:		void

void updateCustomerInfo(MYSQL* databaseObject)
{
	int customer_id = 0;
	char first_name[100];
	char last_name[100];
	char email[100];
	int address_id = 0;
	printf("You have chosen to update customer information. Please enter the customer ID: ");
	if (GetInt(&customer_id) != SUCCESS)
	{
		printf("Invalid input for Customer ID. Please enter a valid number.\n");
		return;
	}

	char query[256];
	sprintf(query, "SELECT customer_id FROM customer WHERE customer_id = %d;", customer_id); 

	if (mysql_query(databaseObject, query) != 0)
	{
		printSQLError(databaseObject, "mysql_query");
		return;
	}

	MYSQL_RES* result = mysql_store_result(databaseObject);
	if (result == NULL)
	{
		printSQLError(databaseObject, "mysql_store_result");
		return;
	}

	if (mysql_num_rows(result) == 0)
	{
		printf("Customer ID %d does not exist.\n", customer_id);
		mysql_free_result(result);
		return;
	}
	mysql_free_result(result);

	printf("Enter new First Name: ");
	if (GetString(&first_name, sizeof(first_name)) != SUCCESS)
	{
		printf("Invlaid input for First Name. Please enter a valid name. \n");
		return;
	}

	printf("Enter new Last Name: ");
	if (GetString(&last_name, sizeof(last_name)) != SUCCESS)
	{
		printf("Invlaid input for Last Name. Please enter a valid name. \n");
		return;
	}

	printf("Enter new Email Address: ");
	if (GetString(&email, sizeof(email)) != SUCCESS)
	{
		printf("Invlaid input for Email Address. Please enter a valid email. \n");
		return;
	}
	else
	{
		if (ValidateEmail(email) != SUCCESS)
		{
			printf("Invalid email format. Please enter a valid email.\n");
			return;
		}
	}

	printf("Enter new Address ID: ");
	if (GetInt(&address_id) != SUCCESS)
	{
		printf("Invalid input for Address ID. Please enter a valid number. \n");
		return;
	}
	else
	{
		sprintf(query, "SELECT address_id FROM address WHERE address_id = %d;", address_id);
		if (mysql_query(databaseObject, query) != 0)
		{
			printSQLError(databaseObject, "mysql_query");
			return;
		}
		result = mysql_store_result(databaseObject);
		if (result == NULL)
		{
			printSQLError(databaseObject, "mysql_store_result");
			return;
		}

		if (mysql_num_rows(result) == 0)
		{
			printf("Address ID %d does not exist.\n", address_id);
			mysql_free_result(result);
			return;
		}
		mysql_free_result(result);
	}

	sprintf(query, "UPDATE customer SET "
		"first_name = '%s', "
		"last_name = '%s', "
		"email = '%s', "
		"address_id = %d "
		"WHERE customer_id = %d;",
		first_name, last_name, email,
		address_id, customer_id);

	if (mysql_query(databaseObject, query) != 0)
	{
		printSQLError(databaseObject, "mysql_query");
		return;
	}

	printf("Customer information updated successfully.\n");
}