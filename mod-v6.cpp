/*
Group-9 Project2-Part2

GITHUB: https://github.com/DarkMatter9309/CS5348-Project2
Team Members and Contributions

Ameya Potey (ANP200000) - Worked on mkdir, cd file operations
Li Wang (li.wang2@utdallas.edu) - Worked on cpin, cpout file operations
Tejo Vardhan (txm200002) -  Worked on main, rm, also worked on integrating all the modules.

Instructions to compiler and run the program on cslab:

Go to the file location

>run g++ -o result mod-v6.cpp //you can replace result with a word of your choice
>run ./result to start

You should see the below message once you start the program succesfully:

Welcome User!
You can exit anytime by typing q

You can give the following commands to try this program. Order needs to be followed for successful execution.

1>openfs filename //filename should not be an existing file name
2>initfs n1 n2 //where n1 and n2 need to be positive integers, invalid n1 and n2 values will lead to error
3>cpin text.txt v6_file //text.txt should be a valid file name
4>cpout v6_file new_text.txt//v6_file should be a valid file name
5>rm v6_file //v6_file should be a valid file name
6>mkdir dirname //dirname should be a valid directory name
7>cd dirname //dirname should be a valid directory name
8>q //or q to quit Enter user command!)

For example:
>openfs project2part2
>initfs 100 50
>cpin text.txt v6_file
>cpout new_text.txt v6_file
>rm v6_file
>mkdir dirname1
>cd dirname1
>q //to quit the program

You should see the files created in the appropriate directories after the program has completed executing.
*/

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

typedef struct
{
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

typedef struct
{
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

typedef struct
{
  unsigned int inode;
  char filename[28];
} dir_type; //32 Bytes long

void out_smallfile(char *externalfile, char *v6_file, int needed_blocks);
void in_smallfile(char *externalfile, char *v6_file, int needed_blocks, int file_size);
unsigned int allocateFreeBlock();

int file_descriptor;
inode_type root_inode; // to store inode for the root
dir_type rootDir;      //to stpr
unsigned int i = 0;    //loop counter
char currPath[80];

int currInode = 1;

unsigned int getFreeBlock();
void mkDir(const char *dirName, int iNodeNumber);
unsigned int getInode();

void createDirectory(int currInode, int prev, const char *dirName, int iNodeNumber)
{
  int startAdd;
  dir_type dir;
  unsigned int inode = currInode;
  char tempFileName[28];
  int flag = 1;
  lseek(file_descriptor, 2048 + (inode - 1) * 64 + 20, SEEK_SET);
  read(file_descriptor, &startAdd, 4);
  lseek(file_descriptor, startAdd * 1024, SEEK_SET);
  int runningInode;
  int rcount = 0;
  read(file_descriptor, &runningInode, 4);

  while (runningInode > 0)
  {
    //lseek(file_descriptor, 4, SEEK_CUR);
    read(file_descriptor, &tempFileName, 28);
    // printf("File Name : %s\n",tempFileName);
    if (strcmp(tempFileName, dirName) == 0)
    {
      printf("Directory Already Exists!!\n");
      flag = 0;
    }
    rcount++;
    read(file_descriptor, &runningInode, 4);
  }
  if (flag == 1)
  {
    unsigned int dirBlock = getFreeBlock();
    strcpy(dir.filename, dirName);
    dir.inode = iNodeNumber;
    lseek(file_descriptor, startAdd * 1024 + rcount * 32, SEEK_SET);
    write(file_descriptor, &dir, 32);

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

    lseek(file_descriptor, 2048 + (iNodeNumber - 1) * 64, SEEK_SET);
    int bytes = write(file_descriptor, &dirInode, 64);

    dir.filename[0] = '.';
    dir.filename[1] = '\0';
    dir.inode = iNodeNumber;

    lseek(file_descriptor, dirBlock * 1024, SEEK_SET);
    write(file_descriptor, &dir, 32);

    dir.filename[0] = '.';
    dir.filename[1] = '.';
    dir.filename[2] = '\0';
    dir.inode = prev;

    write(file_descriptor, &dir, 32);
    printf("Directory Created Successfully!!\n");
  }
}

unsigned int getInode(char *filename, unsigned int prevInode)
{
  char tempFileName[28];
  unsigned int startAdd;
  lseek(file_descriptor, 2048 + (prevInode - 1) * 64 + 20, SEEK_SET);
  read(file_descriptor, &startAdd, 4);
  lseek(file_descriptor, startAdd * 1024, SEEK_SET);
  unsigned int runningInode;
  read(file_descriptor, &runningInode, 4);
  while (runningInode > 0)
  {
    read(file_descriptor, &tempFileName, 28);
    if (strcmp(tempFileName, filename) == 0)
    {
      return runningInode;
    }
    read(file_descriptor, &runningInode, 4);
  }
  printf("No directory/file called %s found!! \n", filename);
  return -1;
}

void changeDirectory(const char *dirName)
{
  char str[80];
  strcpy(str, dirName);
  if (strcmp(dirName, "/") == 0)
  {
    strcpy(currPath, "/");
    currInode = 1;
    printf("Directory changed to root!!\n");
  }
  else if (str[0] != '/')
  {
    char *finalDirName = basename(str);
    char *filename = strtok(str, "/");
    unsigned int currNode = getInode(filename, currInode);
    unsigned int prevNode = currNode;
    if (currNode != -1)
    {
      while (filename != NULL && currNode != -1)
      {
        filename = strtok(NULL, "/");
        if (filename != NULL)
          currNode = getInode(filename, currNode);
      }
      if (currNode != -1)
      {
        strcpy(currPath, dirName);
        printf("current directory succesfully changed to %s\n", currPath);
        currInode = currNode;
      }
    }
  }
  else
  {
    char *finalDirName = basename(str);
    char *filename = strtok(str, "/");
    unsigned int currNode = getInode(filename, 1);
    unsigned int prevNode = currNode;
    if (currNode != -1)
    {
      while (filename != NULL && currNode != -1)
      {
        filename = strtok(NULL, "/");
        if (filename != NULL)
          currNode = getInode(filename, currNode);
      }
      if (currNode != -1)
      {
        strcpy(currPath, dirName);
        printf("current directory succesfully changed to %s\n", currPath);
        currInode = currNode;
      }
    }
  }
}

void mkDir(const char *dirName, int iNodeNumber)
{
  int num;
  char str[80];
  char tempFileName[28];

  char relPath[80];
  strcpy(str, dirName);

  if (str[0] != '/')
  {
    if (strchr(str, '/') != NULL)
    {
      char *finalDirName = basename(str);
      char *filename = strtok(str, "/");
      unsigned int currNode = getInode(filename, currInode);
      unsigned int prevNode = currNode;
      if (currNode != -1)
      {
        while (filename != NULL)
        {
          filename = strtok(NULL, "/");
          if (strcmp(filename, finalDirName) == 0)
            break;
          prevNode = currNode;
          currNode = getInode(filename, currNode);
        }
        if (currNode != -1)
        {
          createDirectory(currNode, prevNode, finalDirName, iNodeNumber);
        }
      }
    }
    else
    {
      createDirectory(currInode, currInode, dirName, iNodeNumber);
    }
  }

  else
  {
    char *finalDirName = basename(str);
    char *filename = strtok(str, "/");
    unsigned int currNode = getInode(filename, 1);
    unsigned int prevNode = currNode;
    if (currNode != -1)
    {
      while (filename != NULL)
      {
        filename = strtok(NULL, "/");
        if (strcmp(filename, finalDirName) == 0)
          break;
        prevNode = currNode;
        currNode = getInode(filename, currNode);
      }
      if (currNode != -1)
      {
        createDirectory(currNode, prevNode, finalDirName, iNodeNumber);
      }
    }
  }
}

int openfs(const char *file)
{

  if (access(file, F_OK) == 0)
  {
    printf("File Already exists!! Please select a new filename \n");
  }
  else
  {
    file_descriptor = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (file_descriptor == -1)
    {
      perror("Error creating/opening file ");
    }
    else
    {
      printf("Successfully created/opened  the file %s", file);
      printf("\n");
    }
  }

  return file_descriptor;
}

void create_root()
{

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
  write(file_descriptor, &rootDir, 32);

  // Filling .. as the next entry in root
  rootDir.filename[0] = '.';
  rootDir.filename[1] = '.';
  rootDir.filename[2] = '\0';
  write(file_descriptor, &rootDir, 32);

  printf("Initailized the root directory and its inode Successfully!!\n");
}

unsigned int getNewInode()
{
  int count = 1;
  unsigned short buf;
  lseek(file_descriptor, 2050, SEEK_SET);
  read(file_descriptor, &buf, 2);

  while (buf >= 1 && count <= (superBlock.isize * 1024) / 64)
  {
    lseek(file_descriptor, 62, SEEK_CUR);
    read(file_descriptor, &buf, 2);
    count = count + 1;
  }
  if (count == (superBlock.isize * 1024) / 64)
  {
    return -1;
  }
  else
  {
    return count;
  }
}

void cpout(char *v6_file, char *externalfile)
{
  struct stat stats;
  stat(v6_file, &stats);
  int needed_blocks = stats.st_blocks;
  //out_smallfile(externalfile, v_6file, needed_blocks);
  out_smallfile(externalfile, v6_file, 3);
}

void out_smallfile(char *externalfile, char *v6_file, int needed_blocks)
{
  unsigned short buffer[512];
  int v6_fd = open(v6_file, O_RDONLY, S_IRUSR | S_IWUSR);
  unsigned short addr[8];
  lseek(v6_fd, 1024, SEEK_SET);
  read(v6_fd, &addr, 16);
  close(v6_fd);
  int externalfile_fd = creat(externalfile, 0775);
  externalfile_fd = open(externalfile, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
  v6_fd = open(v6_file, O_RDONLY, S_IRUSR | S_IWUSR);
  for (int i = 0; i < needed_blocks; i++)
  {
    lseek(v6_fd, i * 1024, SEEK_SET);
    read(v6_fd, &buffer, 1024);
    lseek(externalfile_fd, i * 1024, SEEK_SET);
    write(externalfile_fd, &buffer, 1024);
  }
  printf("Small file copied and exported\n");
  close(externalfile_fd);
  close(v6_fd);
}

void cpin(char *externalfile, char *v6_file)
{
  // stat struct provides information about file such as number of blocks needed and file size
  struct stat stats;
  stat(externalfile, &stats);
  int needed_blocks = stats.st_blocks;
  int file_size = stats.st_size;
  //in_smallfile(externalfile, needed_blocks, v_6file);
  in_smallfile(externalfile, v6_file, 3, file_size);
}

unsigned int allocateFreeBlock()
{
  unsigned int free_address;
  unsigned int buffer[512];
  superBlock.nfree--;
  if (superBlock.nfree > 0)
  {
    if (superBlock.free[superBlock.nfree] == 0)
    {
      printf("Error, file system is full");
      return 1;
    }
    else
    {
      free_address = superBlock.free[superBlock.nfree];
    }
  }
  else
  {
    int next_free_block = superBlock.free[superBlock.nfree];
    int free_block_address = next_free_block * 1024 + 2048 + superBlock.isize * 1024;
    lseek(file_descriptor, free_block_address, 1024);
    read(file_descriptor, buffer, 1024);
    superBlock.nfree = buffer[0];
    for (int i = 0; i < 251; i++)
    {
      superBlock.free[i] = buffer[i + 1];
    }
    free_address = superBlock.free[superBlock.nfree];
  }
  return free_address;
}

void addEntryToCD(int currInode, const char *filename, int inodeNumber)
{
  int startAdd;
  dir_type dir;
  unsigned int inode = currInode;
  char tempFileName[28];
  int flag = 1;
  lseek(file_descriptor, 2048 + (inode - 1) * 64 + 20, SEEK_SET);
  read(file_descriptor, &startAdd, 4);
  lseek(file_descriptor, startAdd * 1024, SEEK_SET);
  int runningInode;
  int rcount = 0;
  read(file_descriptor, &runningInode, 4);
  while (runningInode > 0)
  {
    //lseek(file_descriptor, 4, SEEK_CUR);
    read(file_descriptor, &tempFileName, 28);
    // printf("File Name : %s\n",tempFileName);
    if (strcmp(tempFileName, filename) == 0)
    {
      printf("Directory Already Exists!!\n");
      flag = 0;
    }
    rcount++;
    read(file_descriptor, &runningInode, 4);
  }
  if (flag == 1)
  {
    unsigned int dirBlock = getFreeBlock();
    strcpy(dir.filename, filename);
    dir.inode = inodeNumber;
    lseek(file_descriptor, startAdd * 1024 + rcount * 32, SEEK_SET);
    write(file_descriptor, &dir, 32);
  }
}

void in_smallfile(char *externalfile, char *v6_file, int needed_blocks, int file_size)
{
  inode_type new_inode;
  for (int i = 0; i < needed_blocks; i++)
  {
    new_inode.addr[i] = allocateFreeBlock();
  }
  unsigned int inode_number = getNewInode();
  unsigned int free_address = new_inode.addr[0];
  new_inode.flags = 0100000 | 000000 | 000077;
  new_inode.size0 = 0;
  new_inode.size1 = file_size;
  new_inode.actime = 0;
  new_inode.modtime = 0;
  lseek(file_descriptor, 2048 + (inode_number - 1) * 64, SEEK_SET);
  write(file_descriptor, &new_inode, 64);
  addEntryToCD(currInode, v6_file, inode_number);
  unsigned short buffer[512];
  int externalfile_fd = open(externalfile, O_RDONLY, S_IRUSR | S_IWUSR);
  for (int i = 0; i <= needed_blocks; i++)
  {
    lseek(externalfile_fd, 1024 * i, SEEK_SET);
    read(externalfile_fd, &buffer, 1024);
    lseek(file_descriptor, 1024 * (new_inode.addr[i]), SEEK_SET);
    write(file_descriptor, &buffer, 1024);
  }
  printf("Small file copied\n");
  close(externalfile_fd);
}

unsigned int getFreeBlock()
{
  unsigned int freeBlock;
  freeBlock = superBlock.free[--superBlock.nfree];
  superBlock.free[superBlock.nfree] = 0;
  return freeBlock;
}

void initfs(int file_descriptor, int n1, int n2)
{
  if (file_descriptor != 3)
  {
    printf("Please specify a file to intialize the modified V-6 file system in!!\n");
    return;
  }
  if (n1 == 0 || n2 == 0 || n1 < n2)
  {
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
  for (int i = 0; i < 250; i++)
  {
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
  for (int i = 0; i < 1024; i++)
  {
    buffer[i] = 0;
  }
  // add free_data_blocks number of free blocks starting from byte 2048
  int block_number = first_data_block;
  int location = 2048;
  for (int i = 0; i < free_data_blocks; i++)
  {
    if (superBlock.nfree == 251)
    {
      int data_block[256];
      data_block[0] = 251;
      for (int i = 0; i < 256; i++)
      {
        if (i < 251)
        {
          data_block[i + 1] = superBlock.free[i];
        }
        else
        {
          data_block[i + 1] = 0;
        }
      }
      lseek(file_descriptor, block_number * 1024, SEEK_SET);
      write(file_descriptor, data_block, 1024);
    }
    else
    {
      lseek(file_descriptor, block_number * 1024, SEEK_SET);
      write(file_descriptor, buffer, 1024);
    }
    superBlock.free[superBlock.nfree] = block_number;
    superBlock.nfree++;
    block_number++;
  }
  //closing the file_descriptor
  create_root();
  strcpy(currPath, "/");
  // if (close(file_descriptor) < 0) {
  //   perror("Error Closing file");
  // } else {
  //   close(file_descriptor);
  //   printf("Successfully completed Initializing the modified V-6 file system!!\n");
  //   return;
  // }
}

//rm v6 file command and its methods

void addFreeBlock(int blockNumber)
{
  superBlock.free[superBlock.nfree] = blockNumber;
  superBlock.nfree++;
}

void writeToBlock (int blockNumber, void * buffer, int nbytes)
{
        lseek(file_descriptor,1024 * blockNumber, SEEK_SET);
        write(file_descriptor,buffer,nbytes);
}

void rm(char *filename)
{
  struct stat stats;
  stat(filename, &stats);
  int needed_blocks = stats.st_blocks;
  unsigned int fileInode = getInode(filename, currInode);
  unsigned short buffer[512];
  if (fileInode == -1)
  {
    printf("File does not exist!!\n");
    return;
  }
  else
  {
    unsigned short addr[8];
    int v6_fd = open(filename, O_RDWR);
    lseek(v6_fd, 1024, SEEK_SET);
    read(v6_fd, &addr, 16);
    close(v6_fd);
    unsigned short buffer[512]; //empty buffer
    v6_fd = open(filename, O_RDONLY);
    for (int i = 0; i < needed_blocks; i++)
    { //free the blocks
      lseek(v6_fd, 1024 * (addr[i]), SEEK_SET);
      addFreeBlock(addr[i]);
      writeToBlock(addr[i], buffer, 1024);
    }
    printf("Removed file %s successfully\n", filename);
    
    return;
  }
}


int main()
{
  cout << "Welcome User!" << endl;
  cout << "You can exit anytime by typing q" << endl;
  cout << "You need to give a valid openfs command before any other command is given." << endl;
  cout << "You can give the following commands to try this program!" << endl;
  cout << "1. openfs <filename>" << endl;
  cout << "2. initfs n1 n2   - where n1 and n2 need to be positive integers, invalid n1 and n2 values will lead to error" << endl;
  cout << "**text.txt is shipped alond with the project." << endl;
  cout << "3. cpin text.txt v6_file" << endl;
  cout << "4. cpout v6_file newtext.txt" << endl;
  cout << "5. rm v6_file" << endl;
  cout << "6. mkdir <dirname>" << endl;
  cout << "7. cd <dirname>" << endl;
  cout << "8. q to quit" << endl;
  int finished = 0;
  vector<string> userInputsVector;
  string userInput;
  int openfsValid = -1;
  while (!finished)
  {
    cout << "Enter user command!" << endl;
    getline(cin, userInput);
    userInputsVector.push_back(userInput);
    if (userInput == "q")
    {
      cout << "The program will now exit!" << endl;
      exit(0);
    }

    istringstream iss(userInput);
    vector<string> inputVector;
    for (string s; iss >> s;)
      inputVector.push_back(s);
    if (inputVector[0] == "openfs")
    {
      openfsValid = openfs(inputVector[1].c_str());
    }
    else if (inputVector[0] == "initfs")
    {
      if (openfsValid != -1)
      {
        int n1 = atoi(inputVector[1].c_str());
        int n2 = atoi(inputVector[2].c_str());
        if (n1 <= 0 || n2 <= 0)
        {
          cout << "Valid n1,n2 values are not given. Please try again!" << endl;
        }
        else
        {
          initfs(file_descriptor, n1, n2);
        }
      }
      else
      {
        cout << "Valid openfs command needs to be give before initfs. Please try again!" << endl;
      }
    }
    else if (inputVector[0] == "mkdir")
    {
      if (openfsValid != -1)
      {
        unsigned int dirInode = getNewInode();
        // printf("Inode number: %d\n",dirInode);
        if (dirInode == -1)
        {
          printf("No inodes left!! Please try again !\n");
        }
        else
        {
          mkDir(inputVector[1].c_str(), dirInode);
        }
      }
      else
      {
        cout << "File System has to be initialized before mkdir!!" << endl;
      }
    }
    else if (inputVector[0] == "cd")
    {
      if (openfsValid != -1)
      {
        changeDirectory(inputVector[1].c_str());
      }
      else
      {
        cout << "File System has to be initialized before cd!!" << endl;
      }
    }
    else if (inputVector[0] == "cpin")
    {
      if (openfsValid != -1)
      {
        //const_cast<char*>(data.str().c_str())
        //char *cstr = &str[0];
        char *input1 = &inputVector[1][0];
        char *input2 = &inputVector[2][0];
        cpin(input1, input2);
      }
      else
      {
        cout << "File System has to be initialized before cpin!!" << endl;
      }
    }

    else if (inputVector[0] == "cpout")
    {
      if (openfsValid != -1)
      {
        char *input1 = &inputVector[1][0];
        char *input2 = &inputVector[2][0];
        cpout(input1, input2);
      }
      else
      {
        cout << "File System has to be initialized before cpout!!" << endl;
      }
    }
    else if (inputVector[0] == "rm")
    {
      if (openfsValid != -1)
      {
        char *input1 = &inputVector[1][0];
        rm(input1);
      }
      else
      {
        cout << "File System has to be initialized before rm!!" << endl;
      }
    }
    else
    {
      cout << "Invalid Input! Please try again.!" << endl;
    }
  }
  return 0;
}
