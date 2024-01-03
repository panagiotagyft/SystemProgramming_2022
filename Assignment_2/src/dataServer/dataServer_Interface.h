#pragma once

#include <stdio.h>   
#include <sys/types.h> 	  // socket(), listen(), bind(), accept(), send(), recv(), opendir(), lstat(), closedir(), open(), lseek()
#include <sys/socket.h>   // socket(), listen(), bind(), accept(), send(), recv()
#include <netinet/in.h>   // internet sockets 
#include <netdb.h> 		  // Defines the value of the highest reserved Internet port number
#include <unistd.h> 	  // close(), _exit(), write(), lstat(), usleep(), lseek(), read()
#include <stdlib.h>       // exit(), _exit(), free(), atoi(), malloc(), calloc(), realloc()
#include <signal.h>       // sigaction()
#include <arpa/inet.h>    // htonl
#include <sys/stat.h>     // lstat(), open()  ----//
#include <fcntl.h>        // open() ----//
#include <pthread.h>	  // pthread_create(), pthread_self(), pthread_join()
#include <dirent.h>       // opendir(), readdir(), closedir()    ----//
#include <string.h>       // memset(), strcmp(), strlen(), strcpy(), strcat()
#include <errno.h>        // errno

#include "DoublyList_Interface.h"

struct arguments_worker_thread{
   char* File_Path;
   int  File_Descriptor;
} *args_worker_thread;


extern List Q;
extern pthread_t *worker_threads;
extern int block_size;
extern int w;

static pthread_mutex_t Mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t Cond_nonFull = PTHREAD_COND_INITIALIZER;
static pthread_cond_t Cond_nonEmpty = PTHREAD_COND_INITIALIZER;

void* Communication_Threads(void *);
void FnS_file_paths(int , char* );
void* Thread_Pool(void * );
void* Worker_Thread(void * );

