# The problem 
While working on migrating data from one database to another, especially when using different database systems, the SQL dump will have a separate insert statement for each record in the table, like the following: 

`INSERT INTO CATS (NAME, AGE) VALUES ('Taki',2);`

`INSERT INTO CATS (NAME, AGE) VALUES ('Meaw',3);`

`INSERT INTO CATS (NAME, AGE) VALUES ('Foodi',1.5);`

When dealing with huge tables, the insertion process will be very slow. Maybe some DB clients will need a separate transaction/connection for each statement. To accelerate the import, we need to minimize the number of SQL statements to be executed. 

# The Solution 
Combining many SQL INSERT statements into a single statement is an easy process. Add the values tuple second one after the first with a preceding `,` and so on, like the following: 

`INSERT INTO CATS (NAME, AGE) VALUES ('Taki',2), ('Meaw',3),('Foodi',1.5);`

This way, only one SQL statement will be executed much faster than 3. 

I wrote simple C++ code to perform this text manipulation task. To make it easy for the end user, I also developed a simple GUI using the Qt Framework. 
