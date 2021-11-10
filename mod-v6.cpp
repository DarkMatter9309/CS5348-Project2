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

  if( access( file, F_OK ) == 0 ) {
        printf("File Already exists!! Please select a new filename \n");
    } else {
        file_descriptor=open(file, O_RDWR |  O_CREAT);
  
      if (file_descriptor ==-1) 
          { 
              perror("Error creating/opening file ");         
          } else{
            printf("Successfully created/opened the file %s", file);
            printf("\n");
          }
      } 
  return file_descriptor;
}

void initfs(int file_descriptor, int n1, int n2) {
  // n2 = blocks for inodes
  // n1 = fs size in # of blocks
  superBlock.isize = n2;
  superBlock.fsize = n1;
  // data blocks = total blocks - inode blocks - super block - block 0
  int first_data_block = 2 + n2;
  int free_data_blocks = n1-n2-2;
  for(int i = 0; i < 250; i++) {
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
  write(file_descriptor, &superBlock, 1024);

  // create a blank buffer for blank data blocks
  char buffer[1024];
  for(int i = 0; i < 1024; i++) {
    buffer[i] = 0;
  }
  // add free_data_blocks number of free blocks starting from byte 2048
  int block_number = first_data_block;
  int location = 2048;
  for(int i = 0; i<free_data_blocks; i++) {
    if(superBlock.nfree == 251) {
      int data_block[256];
      data_block[0] = 251;
      for(int i = 0; i < 256; i++) {
        if(i < 251) {
          data_block[i+1] = superBlock.free[i];
        }
        else {
          data_block[i+1] = 0;
        }
      }
      lseek(file_descriptor, block_number * 1024, SEEK_SET);
      write(file_descriptor, data_block, 1024);
    }
    else {
      lseek(file_descriptor, block_number*1024, SEEK_SET);
      write(file_descriptor, buffer, 1024);
    }
    superBlock.free[superBlock.nfree] = block_number;
    superBlock.nfree++;
    block_number++;
  }
  //closing the file_descriptor

    if (close(file_descriptor) < 0)
    {
        perror("Error Closing file");
    }
    else{
      close(file_descriptor);
      printf("Successfully closed the File.\n");
      return;
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
				initfs(file_descriptor, n1,n2);
			}else{
				cout << "Valid openfs command needs to be give before initfs. Please try again" << endl;
			}
			
		}else{
			cout << "Invalid Input! Please try again." << endl;
		}
	}
	return 0;
}


