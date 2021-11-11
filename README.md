# CS5348-Project2

Groups-9 Project2 Part 1

Steps to execute the project (On a Unix machine)

Navigate to the folder containg the source file and type the following commands.

1. ```make mod-v6```
2. ```./mod-v6```

Then give inputs as desired. Here is an example of a sequence of commands that could be given:

1. ```openfs filesys```
2. ```initfs 100 50```
3. ```q```

You should see a file with the given filename in the openfs command, in the same directory after the program has completed executing.

Steps to execute the project (On a windows command prompt)
1. Go to the file location
2. run g++ -o result mod-v6.cpp //you can replace result with a word of your choice
3. run ./result to start

You should see the below message once you start the program succesfully:

Welcome User!
You can exit anytime by typing q
You cannot give initfs before a valid openfs command is given
You can give the following commands to try this program. Order needs to be followed
1. openfs <filename> //filename should not be an existing file name
2. initfs n1 n2 //where n1 and n2 need to be positive integers, invalid n1 and n2 values will lead to error
3. openfs <newfilename> //and try initfs n1 n2 again for your new file 
3. q //or q to quit
Enter user command!)

