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


}