# mysql-c-api
Access mysql database to do stuff, here I remove rows from a table in a database. This could be helpfull in killing unfinished processes and so on.

## to compile
gcc remove_rows.c -std=c99 `mysql_config --cflags --libs` -o remove_rows
