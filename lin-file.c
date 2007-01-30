// lin-file.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/dir.h>

char command[1024];
FILE *DirFiles;
char DirRead[1024];

//File Routines-----------------------------------------------------------
void make_directory(char *dirname)
{
    // ZZ> This function makes a new directory
    snprintf(command, sizeof(command), "mkdir -p %s > /dev/null 2>&1\n",dirname);
    system(command);
}

void delete_file(char *filename)
{
    // ZZ> This function deletes a file
    snprintf(command, sizeof(command), "rm -f %s > /dev/null 2>&1\n",filename);
    system(command);
}

void copy_file(char *source, char *dest)
{
    // ZZ> This function copies a file on the local machine
    snprintf(command, sizeof(command), "cp -f %s %s > /dev/null 2>&1\n",source,dest);
    system(command);
}

void delete_directory(char *dirname)
{
    // ZZ> This function deletes all files in a directory,
    //     and the directory itself
    snprintf(command, sizeof(command), "rm -rf %s > /dev/null 2>&1\n",dirname);
    system(command);
}

void copy_directory(char *dirname, char *todirname)
{
    // ZZ> This function copies all files in a directory
    snprintf(command, sizeof(command), "cp -fr %s %s > /dev/null 2>&1\n",dirname,todirname);
    system(command);
}

void empty_import_directory(char* home)
{
    // ZZ> This function deletes all the TEMP????.OBJ subdirectories in the IMPORT directory
    snprintf(command, sizeof(command), "rm -rf %s/temp*.obj > /dev/null 2>&1\n", home);
    system(command);
}

// Read the first directory entry
char *DirGetFirst(char *search)
{
	fprintf(stderr, "searching : '%s'\n", search);
  snprintf(command, sizeof(command),
#	ifdef GP2X
		"ls -1d %s|xargs basename"
#	else
		"ls -1d %s|xargs -n1 basename"
#	endif
		, search);
  DirFiles=popen(command,"r");
  if(!feof(DirFiles))
  {
    fscanf(DirFiles,"%s\n",DirRead);
    return(DirRead);
  }
  else
  {
    return(NULL);
  }
}

// Read the next directory entry (NULL if done)
char *DirGetNext(void)
{
  if(!feof(DirFiles))
  { 
    fscanf(DirFiles,"%s\n",DirRead);
    return(DirRead);
  }
  else
  {
    return(NULL);
  }
}

// Close anything left open
void DirClose()
{
  pclose(DirFiles);
}

int ClockGetTick()
{
//	struct timeval tv;
//	(void)gettimeofday(&tv, NULL);
//	return tv.tv_sec*1000000 + tv.tv_usec;
	return(clock());
}

int DirGetAttrib(char *fromdir)
{
  //  int tmp;
#define FILE_ATTRIBUTE_DIRECTORY 0x10
#define FILE_ATTRIBUTE_NORMAL 0x0
#define FILE_ATTRIBUTE_ERROR 0xffffffff

  return(0);
}
