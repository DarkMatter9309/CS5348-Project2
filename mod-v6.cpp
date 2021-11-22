/*
Group-9 Project2-Part1

Team Members and Contributions

Ameya Potey (ANP200000) - Worked on appending the root directory and inodes along with a few file operations
Li Wang (li.wang2@utdallas.edu) - Worked on defining the file system and all its internals with Tejo Vardhan
Tejo Vardhan (txm200002) -  Worked a bit on defining the file system and all its internals with Li and also worked on integrating all the modules.

Instructions to compiler and run the program on cslab:

Go to the file location

>run g++ -o result mod-v6.cpp //you can replace result with a word of your choice
>run ./result to start

You should see the below message once you start the program succesfully:

Welcome User!
You can exit anytime by typing q
You cannot give initfs before a valid openfs command is given

You can give the following commands to try this program. Order needs to be followed

1>openfs //filename should not be an existing file name
2>initfs n1 n2 //where n1 and n2 need to be positive integers, invalid n1 and n2 values will lead to error
3>openfs //and try initfs n1 n2 again for your new file
3>q //or q to quit Enter user command!)

For example:
>openfs project2part1
>initfs 100 50
>q


You should see a file with the given filename in the openfs command, in the same directory after the program has completed executing.
*/
#include <stdio.h>

#include <stdlib.h>

#include <math.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <unistd.h>

#include <fcntl.h>

#include <bits/stdc++.h>

#include <time.h>

using namespace std;

typedef struct {
  int isize;
  int fsize;
  int nfree;
  unsigned int free[251];
  char flock;
  char ilock;
  char fmod;
  unsigned int time;
}
superblock_type; // Block size is 1024 Bytes; only 1023 Bytes are used 

superblock_type superBlock;

// i-node Structure 

typedef struct {
  unsigned short flags;
  unsigned short nlinks;
  unsigned int uid;
  unsigned int gid;
  unsigned int size0;
  unsigned int size1;
  unsigned int addr[9];
  unsigned int actime;
  unsigned int modtime;
}
inode_type; //64 Bytes in size 

typedef struct {
  unsigned int inode;
  char filename[28];
}
dir_type; //32 Bytes long 

int file_descriptor;
inode_type root_inode; // to store inode for the root
dir_type rootDir; //to stpr
unsigned int i = 0; //loop counter

unsigned short getFreeBlock();
// void createDirectory(const char * dirName);
unsigned int getInode();



int openfs(const char * file) {

  if (access(file, F_OK) == 0) {
    printf("File Already exists!! Please select a new filename \n");
  } else {
    file_descriptor = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_descriptor == -1) {
      perror("Error creating/opening file ");
    } else {
      printf("Successfully created/opened the file %s", file);
      printf("\n");
    }
  }

  return file_descriptor;
}

void create_root() {

  unsigned short rootBlock = getFreeBlock();
  rootDir.filename[0] = '.';
  rootDir.filename[1] = '\0';
  rootDir.inode = 1;

  root_inode.flags = 00;
  root_inode.nlinks = 2;
  root_inode.uid = 0;
  root_inode.gid = 0;
  root_inode.size0 = 0;
  root_inode.size1 = 0;
  root_inode.addr[0] = rootBlock;
  root_inode.actime = time(0);
  root_inode.modtime = time(0);

  lseek(file_descriptor, 2048, SEEK_SET);
  write(file_descriptor, &root_inode, 64);
  lseek(file_descriptor, rootBlock * 1024, SEEK_SET);

  //filling . as the first entry in root;
  write(file_descriptor, & rootDir, 32);

  // Filling .. as the next entry in root
  rootDir.filename[0] = '.';
  rootDir.filename[1] = '.';
  rootDir.filename[2] = '\0';
  write(file_descriptor, & rootDir, 32);

  printf("Initailized the root directory and its inode Successfully!!\n");


}

unsigned int getInode(){
  int count=1;
  unsigned short buf;
  lseek(file_descriptor, 2050, SEEK_SET);
  read(file_descriptor, &buf, 2);
  while( buf>=1){
    lseek(file_descriptor, 64, SEEK_SET);
    read(file_descriptor, &buf, 2);
    count = count+1;
  }
  return count;
}

unsigned short getFreeBlock() {
  unsigned short freeBlock;
  freeBlock = superBlock.free[--superBlock.nfree];
  superBlock.free[superBlock.nfree] = 0;
  return freeBlock;
}

void initfs(int file_descriptor, int n1, int n2) {
  if (file_descriptor != 3) {
    printf("Please specify a file to intialize the modified V-6 file system in!!\n");
    return;
  }
  if(n1==0 || n2 ==0 || n1<n2){
    printf("Invalid inputs for the initfs command!! Please give valid inputs!!\n");
    return;
   }
  // n2 = blocks for inodes
  // n1 = fs size in # of blocks
  superBlock.isize = n2;
  superBlock.fsize = n1;
  // data blocks = total blocks - inode blocks - super block - block 0
  int first_data_block = 2 + n2;
  int free_data_blocks = n1 - n2 - 2;
  for (int i = 0; i < 250; i++) {
    superBlock.free[i] = 0;
  }
  superBlock.nfree = 0;
  superBlock.flock = 'f';
  superBlock.ilock = 'i';
  superBlock.fmod = 0;
  superBlock.time = 0;

  // start writing from block 2
  // write superblock into block 2
  lseek(file_descriptor, 1024, SEEK_SET);
  write(file_descriptor, & superBlock, 1024);

  // create a blank buffer for blank data blocks
  char buffer[1024];
  for (int i = 0; i < 1024; i++) {
    buffer[i] = 0;
  }
  // add free_data_blocks number of free blocks starting from byte 2048
  int block_number = first_data_block;
  int location = 2048;
  for (int i = 0; i < free_data_blocks; i++) {
    if (superBlock.nfree == 251) {
      int data_block[256];
      data_block[0] = 251;
      for (int i = 0; i < 256; i++) {
        if (i < 251) {
          data_block[i + 1] = superBlock.free[i];
        } else {
          data_block[i + 1] = 0;
        }
      }
      lseek(file_descriptor, block_number * 1024, SEEK_SET);
      write(file_descriptor, data_block, 1024);
    } else {
      lseek(file_descriptor, block_number * 1024, SEEK_SET);
      write(file_descriptor, buffer, 1024);
    }
    superBlock.free[superBlock.nfree] = block_number;
    superBlock.nfree++;
    block_number++;
  }
  //closing the file_descriptor
  create_root();
  // if (close(file_descriptor) < 0) {
  //   perror("Error Closing file");
  // } else {
  //   close(file_descriptor);
  //   printf("Successfully completed Initializing the modified V-6 file system!!\n");
  //   return;
  // }
}

int main() {
  cout << "Welcome User!" << endl;
  cout << "You can exit anytime by typing q" << endl;
  cout << "You cannot give initfs before a valid openfs command is given" << endl;
  cout << "You can give the following commands to try this program. Order needs to be followed" << endl;
  cout << "1. openfs <filename>" << endl;
  cout << "2. initfs n1 n2 where n1 and n2 need to be positive integers, invalid n1 and n2 values will lead to error" << endl;
  cout << "3. openfs <newfilename> or q to quit" << endl;
  int finished = 0;
  vector < string > userInputsVector;
  string userInput;
  int openfsValid = 0;
  while (!finished) {
    cout << "Enter user command!" << endl;
    getline(cin, userInput);
    userInputsVector.push_back(userInput);
    if (userInput == "q") {
      cout << "The program will now exit!" << endl;
      exit(0);
    }

    istringstream iss(userInput);
    vector < string > inputVector;
    for (string s; iss >> s;)
      inputVector.push_back(s);
    if (inputVector[0] == "openfs") {
      openfsValid = openfs(inputVector[1].c_str());
    } else if (inputVector[0] == "initfs") {
      if (openfsValid != -1) {
        int n1 = atoi(inputVector[1].c_str());
        int n2 = atoi(inputVector[2].c_str());
        if (n1 <= 0 || n2 <= 0) {
        	cout << "Valid n1,n2 values are not given. Please try again!" << endl;
        } else {
        	initfs(file_descriptor, n1, n2);
        }
      } else {
        cout << "Valid openfs command needs to be give before initfs. Please try again!" << endl;
      }

    } 
    else if (inputVector[0] == "mkdir"){
        if (openfsValid != -1) {
          unsigned int dirInode = getInode();
          printf("Inode number: %d\n",dirInode);
          // if(dirInode < 0){
          //   printf("No inodes left!! Please try again !\n");
          // }
          if(access(inputVector[1].c_str(),R_OK)==0){
            printf("The Directory already exists! Please try again!\n");
          }
          else{
            //createDirectory(inputVector[1].c_str());
          }
        }

    }
    else {
      cout << "Invalid Input! Please try again.!" << endl;
    }
  }
  return 0;
}
