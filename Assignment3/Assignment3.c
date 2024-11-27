#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(disable: 4996)




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

	int customer_id = 0;
	int inventory_id = 0;
	int staff_id = 0;

	printf("Enter Customer ID: ");
	if (scanf("%d", &customer_id) != 1)
	{
		printf("Invalid input for Customer ID.  Please enter a valid number.\n");
		return EXIT_FAILURE;
	}
	printf("Enter Inventory ID: ");
	if (scanf("%d", &inventory_id) != 1)
	{
		printf("Invalid input for Inventory ID.  Please enter a valid number.\n");
		return EXIT_FAILURE;
	}
	printf("Enter Staff ID: ");
	if (scanf("%d", &staff_id) != 1)
	{
		printf("Invalid input for Staff ID.  Please enter a valid number.\n");
		return EXIT_FAILURE;
	}

	char query[256];
	printf(query,
		"SELECT COUNT(*) AS available_count FROM inventory i LEFT JOIN rental r ON i.inventory_id = r.inventory_id AND r.return_date IS NULL WHERE i.inventory_id = %d AND r.rental_id IS NULL",
		inventory_id);

	if (mysql_query(databaseObject, query) != 0)
	{
		printf("Error Executing the Query\n");
		mysql_close(databaseObject);
		return EXIT_FAILURE;
	}
	MYSQL_RES* result = mysql_store_result(databaseObject);
	if (result == NULL)
	{
		printf("Error Storing Result: %s\n", mysql_error(databaseObject));
		mysql_close(databaseObject);
		return EXIT_FAILURE;
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
			printf("Error inserting rental");
			mysql_close(databaseObject);
			return EXIT_FAILURE;
		}

		printf("Rental record added successfully.\n");

	}
	else
	{
		char choice = NULL;
		printf("Rental not available. Add customer to waitlist? (y/n)");
	}
}