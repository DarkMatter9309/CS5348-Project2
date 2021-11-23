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

#include <libgen.h>

#include <string.h>

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
char currPath[80];

int currInode = 1;

unsigned int getFreeBlock();
void mkDir( const char * dirName, int iNodeNumber);
unsigned int getInode();



void createDirectory(int currInode,int prev, const char * dirName,  int iNodeNumber){
  int startAdd;
  dir_type dir;
  unsigned int inode = currInode;
  char tempFileName[28];
  int flag =1;
    lseek(file_descriptor, 2048 + (inode-1)*64 +20, SEEK_SET);
    read(file_descriptor,&startAdd, 4);
    lseek(file_descriptor,startAdd*1024 , SEEK_SET);
    int runningInode ;
    int rcount=0;
    read(file_descriptor,&runningInode, 4);
    
    while(runningInode > 0){
      //lseek(file_descriptor, 4, SEEK_CUR);
      read(file_descriptor,&tempFileName,28);
      // printf("File Name : %s\n",tempFileName);
      if(strcmp(tempFileName,dirName)==0){
        printf("Directory Already Exists!!\n");
        flag =0;
      }
      rcount++;
      read(file_descriptor,&runningInode, 4);
    }
    if(flag ==1){
       unsigned int dirBlock = getFreeBlock();
      strcpy(dir.filename,dirName);
     dir.inode = iNodeNumber;
     lseek(file_descriptor,startAdd*1024 +rcount*32 , SEEK_SET);
     write(file_descriptor,&dir, 32);

     inode_type dirInode;

    dirInode.flags = 0b1100110000000000;
    dirInode.nlinks = 2;
    dirInode.uid = 0;
    dirInode.gid = 0;
    dirInode.size0 = 0;
    dirInode.size1 = 0;
    dirInode.addr[0] = dirBlock;
    dirInode.actime = time(0);
    dirInode.modtime = time(0);

    lseek(file_descriptor, 2048 + (iNodeNumber-1)*64, SEEK_SET);
    int bytes = write(file_descriptor, &dirInode, 64);

    dir.filename[0] = '.';
    dir.filename[1] = '\0';
    dir.inode = iNodeNumber;

    lseek(file_descriptor, dirBlock*1024, SEEK_SET);
    write(file_descriptor, &dir, 32);

    dir.filename[0] = '.';
    dir.filename[1] = '.';
    dir.filename[2] = '\0';
    dir.inode = prev;

    write(file_descriptor, &dir, 32);
    printf("Directory Created Successfully!!\n");
    }
     
    
}
unsigned int getInode(char * filename, unsigned int prevInode){
  char tempFileName[28];
  unsigned int  startAdd;
    lseek(file_descriptor, 2048 + (prevInode-1)*64 +20, SEEK_SET);
    read(file_descriptor,&startAdd, 4);
    lseek(file_descriptor,startAdd*1024 , SEEK_SET);
    unsigned int runningInode ;
    read(file_descriptor,&runningInode, 4);
     while(runningInode > 0){
      read(file_descriptor,&tempFileName,28);
      if(strcmp(tempFileName,filename)==0){
        return runningInode;
      }
      read(file_descriptor,&runningInode, 4);
     }
      printf("No directory called %s found!! \n",filename);
    return -1;
}
void chanegDirectory(const char * dirName){
  char str[80];
  strcpy(str, dirName);
  if(strcmp(dirName, "/")==0){
    strcpy(currPath,"/");
    currInode=1;
    printf("Directory changed to root!!\n");
  }
  else if(str[0] != '/'){
    char* finalDirName = basename(str);
      char* filename = strtok(str, "/");
     unsigned int currNode = getInode(filename,currInode);
     unsigned int prevNode = currNode;
     if( currNode != -1){
        while (filename != NULL && currNode != -1) {
          filename = strtok(NULL, "/");
            if(filename != NULL)
            currNode = getInode(filename,currNode);
        }
        if(currNode != -1){
          strcpy(currPath,dirName);
          printf("current directory succesfully changed to %s\n",currPath);
         currInode = currNode;
        }
     }
  }
  else{
      char* finalDirName = basename(str);
      char* filename = strtok(str, "/");
     unsigned int currNode = getInode(filename,1);
     unsigned int prevNode = currNode;
     if( currNode != -1){
        while (filename != NULL && currNode != -1) {
          filename = strtok(NULL, "/");
            if(filename != NULL)
            currNode = getInode(filename,currNode);
        }
        if(currNode != -1){
          strcpy(currPath,dirName);
          printf("current directory succesfully changed to %s\n",currPath);
         currInode = currNode;
        }
     }
  }
}
void mkDir( const char * dirName, int iNodeNumber){
  int num;
  char str[80];
  char tempFileName[28];
  
  char relPath[80];
  strcpy(str, dirName);
  
  if(str[0] != '/'){
          if(strchr(str,'/')!=NULL){
              char* finalDirName = basename(str);
              char* filename = strtok(str, "/");
             unsigned int currNode = getInode(filename,currInode);
             unsigned int prevNode = currNode;
             if( currNode != -1){
                while (filename != NULL) {
                  filename = strtok(NULL, "/");
                    if(strcmp(filename,finalDirName)==0)
                      break;
                    prevNode = currNode;
                    currNode = getInode(filename,currNode);
                }
                if(currNode != -1){
                  createDirectory(currNode,prevNode,finalDirName,iNodeNumber);
                  
                }
             }
          }
          else{
            createDirectory(currInode,currInode, dirName,iNodeNumber); 
          }
           
    }
  
  else{
      char* finalDirName = basename(str);
      char* filename = strtok(str, "/");
     unsigned int currNode = getInode(filename,1);
     unsigned int prevNode = currNode;
     if( currNode != -1){
        while (filename != NULL) {
          filename = strtok(NULL, "/");
            if(strcmp(filename,finalDirName)==0)
              break;
            prevNode = currNode;
            currNode = getInode(filename,currNode);
        }
        if(currNode != -1){
          createDirectory(currNode,prevNode,finalDirName,iNodeNumber);
        }
     }
    
  }

}

int openfs(const char * file) {

  if (access(file, F_OK) == 0) {
    printf("File Already exists!! Please select a new filename \n");
  } else {
    file_descriptor = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_descriptor == -1) {
      perror("Error creating/opening file ");
    } else {
      printf("Successfully created/opened  the file %s", file);
      printf("\n");
    }
  }

  return file_descriptor;
}

void create_root() {


  unsigned int rootBlock = getFreeBlock();
  rootDir.filename[0] = '.';
  rootDir.filename[1] = '\0';
  rootDir.inode = 1;

  root_inode.flags = 0b1100110000000000;
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

unsigned int getNewInode(){
  int count=1;
  unsigned short buf;
  lseek(file_descriptor, 2050, SEEK_SET);
  read(file_descriptor, &buf, 2);

  while( buf>=1 && count<= (superBlock.isize*1024)/64){
    lseek(file_descriptor, 62, SEEK_CUR);
    read(file_descriptor, &buf, 2);
    count = count+1;
  }
  if(count ==  (superBlock.isize*1024)/64){
    return -1;
  }
  else{
    return count;
  }
  
}

unsigned int getFreeBlock() {
  unsigned int freeBlock;
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
  strcpy(currPath,"/");
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
          unsigned int dirInode = getNewInode();
          // printf("Inode number: %d\n",dirInode);
          if(dirInode == -1){
            printf("No inodes left!! Please try again !\n");
          }
          else{
          mkDir(inputVector[1].c_str(),dirInode);
          }
        }

    }
    else if (inputVector[0] == "cd"){
        if (openfsValid != -1) {
          chanegDirectory(inputVector[1].c_str());
          }
        }
    else {
      cout << "Invalid Input! Please try again.!" << endl;
    }
  }
  return 0;
}
