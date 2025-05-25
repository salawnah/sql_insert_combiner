# The problem 
While working on migrating data from database to another specially when using different database systems, the sql dump will have a separate insert statement for each record in the table like the following: 
`INSERT INTO CATS (name,age) VALUES ('Taki',2); 
 INSERT INTO CATS (name,age) VALUES ('Meaw',3) 
 `
