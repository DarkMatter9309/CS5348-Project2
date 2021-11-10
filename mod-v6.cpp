#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/stdc++.h>
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
} superblock_type; // Block size is 1024 Bytes; only 1023 Bytes are used 
 
superblock_type superBlock; 

// block structure

typedef struct {
  int next_block;
  char data[1024];
} block_type;
 
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
} inode_type; //64 Bytes in size 
 
typedef struct { 
  unsigned int inode; 
  char filename[28]; 
} dir_type;//32 Bytes long 

// global constants
#define BLOCK_SIZE = 1024;
#define ISIZE = 64;
int file_descriptor;

int openfs(const char *file)
{
  file_descriptor=open(file, O_RDWR);
  if (file_descriptor == -1) {
	  file_descriptor=open(file, O_CREAT);
	  printf("File does not already exist. Successfully created file %s\n", file);
  }
  else {
	  printf("Successfully opened the file %s\n", file);
  }
  if (file_descriptor ==-1) 
    { 
        perror("Error creating/opening file ");         
    } 
  return file_descriptor;
}

void initfs(int n1, int n2) {
  block_type block_zero; // ignore
  // n2 = blocks for inodes
  // n1 = fs size in # of blocks
  superBlock.isize = n2;
  superBlock.fsize = n1;
  // create n2/16 blocks for inodes
  int number_of_inode_blocks = 0;
  if(n2 % 16 == 0) {
    number_of_inode_blocks = n2/16;
  } else {
    number_of_inode_blocks = ceil(n2/16);
  }
  // data blocks = total blocks - inode blocks - super block - block 0
  int number_of_data_blocks = n1 - number_of_inode_blocks - 2;
  block_type memory[n1];
  
  /*
  int number_of_inode_blocks = ceil(n2/16);
  block_type inodes[number_of_inode_blocks];
  int number_of_data_blocks = n1 - number_of_inode_blocks;
  block_type data[number_of_data_blocks];
  */

  // set all blocks to free
  for(int i = 1; i < 251; i++) {
    superBlock.free[i] = 0;
  }

  // write superblock to fs
  /*
  off_t lseek(int fildes, off_t offset, int whence)
fildes = file descriptor
offset = position file offset to a particluar location in file
whence:
  SEEK_SET: offset is set to offset bytes
  SEEK_CUR: offset is set to its current location + offset bytes
  SEEK_END: offset is set to size of file + offset bytes
  */
  //closing the file_descriptor

    if (close(file_descriptor) < 0) 
    { 
        perror("Error Closing file");
    }
    else{
      printf("Successfully closed the File.\n");
    }
}

int main(){
	int finished = 0;
	vector<string> userInputsVector;
	string userInput;
	int openfsValid = 0;
	while(!finished){
		cout << "Enter user command! (you can exit by typing q)" << endl;
		getline (cin, userInput);
		userInputsVector.push_back(userInput);
		if(userInput == "q"){
			cout << "The program will now exit" << endl;
			exit(0);
		}
		
    	istringstream iss(userInput);
    	vector<string> inputVector;
    	for(string s;iss>>s;)
        	inputVector.push_back(s);
        if(inputVector[0] == "openfs"){
			openfsValid = openfs(inputVector[1].c_str());
		}else if(inputVector[0] == "initfs"){
			if(openfsValid != -1){
				int n1 = atoi(inputVector[1].c_str());
				int n2 = atoi(inputVector[2].c_str());
				initfs(n1,n2);
			}else{
				cout << "Valid openfs command needs to be give before initfs. Please try again" << endl;
			}
			
		}else{
			cout << "Invalid Input! Please try again." << endl;
		}
	}
	return 0;
}


