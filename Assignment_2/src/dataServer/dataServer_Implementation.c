#include <stdio.h>   

#include "dataServer_Interface.h"

// extern variables
List Q;      // Queue
pthread_t *worker_threads;    // Workers array
int block_size, w;     // w --->> worker counter

void* Communication_Threads(void *fd){
	char buf[512], full_path[512], str[512], *full_path_server="./Server";
	int fd_sock, i, flag_1, flag_2, letter_count, j, relative_path_len;
	
	for(i=0; i < sizeof(buf); i++){ buf[i]='\0'; full_path[i]='\0'; str[i]='\0'; }     // initialize -->> arrays
	
	fd_sock=*(int *)fd;
	
	printf("[ Thread: %ld ]:  About to scan directory Server\n", pthread_self ());    // optional: print message
	
	
	// read the client request -->> folder path to copy
	if(recv(fd_sock, buf, sizeof(buf), 0) < 0 ){
		perror("ERROR: recv() failed");\
		exit(EXIT_FAILURE);	
	}
	
	flag_1=0; flag_2=0 , j=0; letter_count=0;
	relative_path_len=strlen(buf);

	// Get the ultimate path (full path)
	for(i=0; i< relative_path_len; i++){

		if(flag_2== 1){
			str[j++]=buf[i];
		}
		else{    
			if(buf[i]=='S'){ flag_1=1; letter_count++; }
			else if(buf[i]=='e' && flag_1==1){  letter_count++; }
			else if(buf[i]=='r' && flag_1==1){  letter_count++; }
			else if(buf[i]=='v' && flag_1==1){  letter_count++; }
			else if(buf[i]=='e' && flag_1==1){  letter_count++; }
			else if(buf[i]=='r' && flag_1==1){  letter_count++; }
			else{  
				flag_1=0;
				letter_count=0; 
			}
		}
		
		if(letter_count==6){ flag_2=1;	}
		
		
	}
	
	strcpy(full_path, full_path_server);     // full_path = "../Server"
	strcat(full_path, str);       // e.g. if str = "floder1"  --- then --->> full_path = "../Server/floder1"
    
	FnS_file_paths(fd_sock, full_path);
	
	free(fd);
	
	return 0;
}

// FnS_file_paths() function: uses code from lecture notes
// https://cgi.di.uoa.gr/~antoulas/k24/lectures/l06.pdf --->> p.37
// https://cgi.di.uoa.gr/~antoulas/k24/lectures/l13.pdf --->> p.65 & p.66 & p.67
void FnS_file_paths(int fd, char* path){
    DIR *directory=NULL;
    struct dirent *current_directory_entry;
    struct stat stat_buffer;                          
    char path_d_name[100];
    int i, isDir, error;
    
    // Open directory (path) and get the file descriptor
	if ( (directory= opendir(path)) == NULL ){ 
		perror("ERROR: opendir() failed\n");
		exit(EXIT_FAILURE); 
	}
   
    // Read all the current directory 
    errno=0;
    while((current_directory_entry = readdir(directory)) != NULL){
        for(i=0; i<100; i++){ path_d_name[i]='\0'; }
		strcpy(path_d_name,path);
        strcat(path_d_name,"/");
        strcat(path_d_name,current_directory_entry->d_name);
		
		if( lstat(path_d_name,&stat_buffer) == -1){  // get info for current file
			perror("ERROR: lstat() failed\n");
			exit(EXIT_FAILURE); 	
		}
        
        isDir=S_ISDIR(stat_buffer.st_mode);   // get if current file is directory or not
        
        // Case 1: the current file is directory:
		if( isDir ){
			// I do not need: "." (current catalog) or ".." (parent catalog) 
            if( strcmp(current_directory_entry ->d_name,".") == 0 || strcmp(current_directory_entry ->d_name,"..")== 0 ){ 
	         	continue;
         	}
         	else{ FnS_file_paths(fd, path_d_name); }  // call FnS_file_paths() for the current path
		}
		
		// Case 2: the current file isn't directory:
		if( !isDir ){
			    
				// lock the queue so that no other process interferes with it while the FnS_file_paths is processing it
			if( ( error=pthread_mutex_lock(&Mutex) ) != 0 ){
				fprintf(stderr , "ERROR: pthread_mutex_lock() failed : %s \n", strerror( error ));
				exit(EXIT_FAILURE); 
			}
				
				// while a queue is full --> wait for the signal that the queue is not full
			while(IsFull(Q)==1){ 
				if( ( error=pthread_cond_wait(&Cond_nonFull, &Mutex) ) != 0){
					fprintf(stderr , "ERROR: pthread_cond_wait() failed : %s \n", strerror( error ));
					exit(EXIT_FAILURE); 
				}
			}
	        
			printf("[ Thread: %ld ]:  Adding file %s to the queue\n", pthread_self (), path);    // optional: print message
			
			// add the path file to the end of a queue
			AddLast(Q, make(fd, path_d_name)); 
			
			// triggered the signal that the queue is no more empty
			if( ( error=pthread_cond_signal(&Cond_nonEmpty) ) != 0){
				fprintf(stderr , "ERROR: pthread_cond_signal() failed : %s \n", strerror( error ));
				exit(EXIT_FAILURE); 
			}
				
			// Unlock the queue 	
			if( (error=pthread_mutex_unlock(&Mutex) ) != 0){
				fprintf(stderr , "ERROR: pthread_mutex_unlock() failed : %s \n", strerror( error ));
				exit(EXIT_FAILURE); 
			}
			usleep(300000);  // sleep to receive thread_pool the paths  
		}
		
	}
	
	if(directory != NULL){
      	closedir(directory);   // close directory
	}
}

// Thread_Pool(): uses code from lecture notes
// https://cgi.di.uoa.gr/~antoulas/k24/lectures/l13.pdf --->> p.65 & p.66 & p.67
void* Thread_Pool(void * size){
	int ThreadPool_sz, file_descr, i, error;
	ListNode a;
	char str[512];
		
	for(i=0; i<512; i++){ str[i]='\0'; }
	
    ThreadPool_sz=*(int*)size;

	while(1){
		
		usleep(1000000);  // clients-server synchronization
		
		// lock the queue so that no other process interferes with it while the thread_pool is processing it
		if( ( error=pthread_mutex_lock(&Mutex) ) != 0 ){
			fprintf(stderr , "ERROR: pthread_mutex_lock() failed : %s \n", strerror( error ));
			exit(EXIT_FAILURE); 
		}

        // while a queue is empty --> wait for the signal that the queue is not empty
		while(IsEmpty(Q)== 1){
			if( ( error=pthread_cond_wait(&Cond_nonEmpty, &Mutex) ) != 0){
				fprintf(stderr , "ERROR: pthread_cond_wait() failed : %s \n", strerror( error ));
				exit(EXIT_FAILURE); 
			}
		}

		a=GetFirst(Q);  // Get first node from the queue 
		
		// prepare the struct info for workers
		args_worker_thread=malloc(sizeof(struct arguments_worker_thread));

		// 1. save the path file
		args_worker_thread->File_Path=malloc(512*sizeof(char)); 
		file_descr=item(a, str);  
        strcpy(args_worker_thread->File_Path, str); 
		// 2. save the client's file descriptor
        args_worker_thread->File_Descriptor=file_descr;
        
		// Remove first node from the queue
		Remove(Q,a);
        
        // triggered the signal that the queue is no more full
		if( ( error=pthread_cond_signal(&Cond_nonFull) ) != 0){
			fprintf(stderr , "ERROR: pthread_cond_signal() failed : %s \n", strerror( error ));
			exit(EXIT_FAILURE); 
		}

    	// Start the worker no. w  
		if ( pthread_create(worker_threads+w , NULL , Worker_Thread , ( void *)args_worker_thread ) < 0 ){
             perror ( "ERROR: pthread_create() failed" );
             exit(EXIT_FAILURE);
        }
		
        usleep(2000000);     // sleep to add paths to queue FnS_file_paths()
        
        if( (error=pthread_detach(*(worker_threads+w)) ) == -1){
			fprintf(stderr , "ERROR: pthread_detach() failed : %s \n", strerror( error ));
            exit(EXIT_FAILURE);	
		} 
        
	    if(w == ThreadPool_sz-1){ w=0;}
	    else{ w++; }
        
		free(args_worker_thread->File_Path);   
	    free(args_worker_thread);
		
		// // Unlock the queue  &  Unlock the critical section of current worker 
		if( (error=pthread_mutex_unlock(&Mutex) ) != 0){
			fprintf(stderr , "ERROR: pthread_mutex_unlock() failed : %s \n", strerror( error ));
			exit(EXIT_FAILURE); 
		}
		
	}

}

void* Worker_Thread(void *argumnts){
	char *buf, buf_filename[4096], read_buf[block_size];
	int fd, fd_client, i, j, number, n;
	off_t file_size_in_bytes;
	struct arguments_worker_thread *args_worker_thread = argumnts;
 
    buf=malloc(4096 * sizeof(char));

	for(i=0; i < 4096; i++){ buf[i]='\0'; buf_filename[i]='\0';}     // initialize -->> arrays
    
    // Get info 
	fd_client=args_worker_thread->File_Descriptor;  
	strcpy(buf, args_worker_thread->File_Path);
   

	printf("[ Thread: %ld ]:  Received task: <%s, %d>\n", pthread_self (), buf, fd_client);   // optional: print message
     
	// Open the file 
	if((fd=open(buf , O_RDONLY)) == -1){
		perror("ERROR: open() failed\n");
		exit(EXIT_FAILURE);
	}
	
	strcpy(buf_filename, buf);
    
    printf("[ Thread: %ld ]:  About to read file %s\n", pthread_self(), buf);   // optional: print message

// --- A. send file name. ---

    if ( send(fd_client, buf_filename, sizeof(buf_filename) , 0) < 0){
		perror("ERROR: send1() failed\n");
		exit(EXIT_FAILURE); 	
	}
	usleep(10000);  // client-server synchronization

// --- B. send file size. ---
	
	// lseek() with whence=SEEK_END & pos=0: Get the end of the file --->> total bytes 
	if((file_size_in_bytes = lseek(fd, 0, SEEK_END)) == -1){
		perror("ERROR: lseek() failed\n");
		exit(EXIT_FAILURE);			
	}
	
	// lseek() with whence=SEEK_CUR & pos=-file_size_in_bytes. I set the file from the beginning 
	if(lseek(fd, -file_size_in_bytes, SEEK_CUR) == -1){
		perror("ERROR: lseek() failed\n");
		exit(EXIT_FAILURE);			
	} 
	
	number=htonl(file_size_in_bytes);
		
    if ( send(fd_client, &number, sizeof(number) , 0) < 0){
		perror("ERROR: send2() failed\n");
		exit(EXIT_FAILURE); 	
	}


// --- C. send file. --- 
	
	// e.g   file_size_in_bytes = 4303   &   block_size = 2000
	float variable1 = (float)file_size_in_bytes/(float)block_size;   //    4303.00/2000.00 = 2.1515 (= variable1)
	int variable2=(int)variable1;                                    //    variable2 = 2
	float variable3=variable1-(float)variable2;                      //    2.1515-2.0000=0.1515 (=variable3)
	
	// if variable3=0.00 -->> ok the number of repetitions is exactly e.g. 2! Do not increase the variable2 and continue
	// if variable3!=0 and variable2==0 ( because e.g. variable1=0.1515) --- then --->> increase the variable2
	if(variable3!=0.00 || variable2==0){ variable2++; }                               
	
    for(i=0; i < variable2; i++){
    	
	 	for(j=0; j<sizeof(read_buf); j++){ read_buf[j]='\0';} // initialize read_buf for the next repetition
		
		n=read(fd, read_buf, sizeof(read_buf));     // send block_size bytes at a time
	    if ( send(fd_client, read_buf, n , 0) < 0){ 
			perror("ERROR: send3() failed\n");
			exit(EXIT_FAILURE); 	
		}	
	}
	
	free(buf);
	
	return 0;
}

