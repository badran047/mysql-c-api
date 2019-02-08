#include <my_global.h>
#include <mysql.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//set your preferred time here
int expire_processing_days = 35;

typedef struct
{
    char host[20];
    char user[25];
    char pass[50];
    char db[25];
}DB_CONN_PARAMS;

MYSQL * connect_db(DB_CONN_PARAMS *params)
{
    //init connection
    MYSQL *connection = mysql_init(NULL);

    if (connection == NULL)
    {
        //check init worked
        fprintf(stderr, "%s\n", mysql_error(connection));
        exit(EXIT_FAILURE);
    }
    //connect:
    if (mysql_real_connect(
            connection,
            params->host,
            params->user,
            params->pass,
            params->db,
            0,NULL,0)==NULL)
    {
        //connection failed?
        fprintf(stderr, "%s\n", mysql_error(connection));
        mysql_close(connection);
        exit(EXIT_FAILURE);
    }
    return connection;
}

//if an error occurred, we print the error message, close connection and terminate the application
void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s\n", mysql_error(con));
    mysql_close(con);
    exit(1);        
}

void remove_rows(MYSQL *con)
{
    char selection_query[256];
    char deletion_query[256];
    
    sprintf(selection_query, "SELECT id FROM <table_name> WHERE started < DATE(NOW()) - INTERVAL %d DAY", expire_processing_days);
    // mysql_query return value: zero for success. Nonzero if an error occurred
    if (mysql_query(con, selection_query))
    {
        finish_with_error(con);
    }
  
    MYSQL_RES *result = mysql_store_result(con);
  
    if (result == NULL) 
    {
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) 
    {
        for(int i = 0; i < num_fields; i++) 
        { 
            printf("Deleting %s ", row[i] ? row[i] : "NULL");
            sprintf(deletion_query, "DELETE FROM <table_name> WHERE id = %s", row[i]);
            // mysql_query return value: zero for success. Nonzero if an error occurred
            if (mysql_query(con, deletion_query))
            {
                finish_with_error(con);
            }
        } 
            printf("\n"); 
    }
  
    mysql_free_result(result);
}

int main()
{
    MYSQL *con;
    DB_CONN_PARAMS *params = calloc(1,sizeof(DB_CONN_PARAMS));

    //just an alternative way of passing connection params, find a struct easier
    strcpy(params->host, <your_host>);
    strcpy(params->user, <your_user>);
    strcpy(params->pass, <your_password>);
    strcpy(params->db, <your_database>);

    MYSQL * connect_db(DB_CONN_PARAMS *params);
    con = connect_db(params);

    //we don't need the struct anymore
    free(params);
    params = NULL;

    //kill processes that are incomplete
    remove_rows(con);

    //close connection, of course!
    mysql_close(con);

    return EXIT_SUCCESS;
}