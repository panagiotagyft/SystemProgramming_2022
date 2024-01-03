#include <stdio.h>

#include "dataServer_Interface.h"

// extern variables
List Q;      // Queue
pthread_t *worker_threads;    // Workers array
int *socket_descriptors;  // File descriptors array
int block_size, w;  // w --->> worker counter

int fd_socket, new_alloc, *pool_size;

static void ctrl_c_fun(int signalnum){
	int i;
	ListNode a;
	
	// Delete Queue
	while(IsEmpty(Q)!=1){
		a=GetFirst(Q); 
		Remove(Q,a);
		Print(Q);
	}
	free(Q);  // deallocate
	 
	// Close sockets descriptors of all client connections
	for( i=0; i <  new_alloc; i++ ){
		close( socket_descriptors[i] );
	} 
    free( socket_descriptors );  // deallocate
    
    free( worker_threads );    // deallocate

    // If the pthread mutex is locked  -- then -->>  unlock mutex and destroy it  
    if( pthread_mutex_unlock( &Mutex ) != 0 ){
		(void)write(STDOUT_FILENO,"ERROR: pthread_mutex_unlock() failed\n", 37);    
		_exit(EXIT_FAILURE);
    } 
    
    // Destroy condition variables
    pthread_cond_destroy( &Cond_nonFull );
    pthread_cond_destroy( &Cond_nonEmpty );
    
	// Destroy mutex
    pthread_mutex_destroy( &Mutex ); 
    
    free( pool_size );    // delete a pointer
    
	close( fd_socket );   // close server socket descriptor

	_exit(EXIT_FAILURE);
}

// main(): uses code from lecture notes
// https://cgi.di.uoa.gr/~antoulas/k24/lectures/l10.pdf --->> p.31 & p.32 & p.33
int main(int argc,char *argv[]){
	int fd_client, *new_fd_client, port, thread_pool_size, number, queue_size, error;
	socklen_t client_len;
	//char buf[100];
	struct sigaction sigint_act;
	
	// The Internet domain uses struct sockaddr_in for struct sockaddr. 
	// struct sockaddr_in : socket address under the internet environment (internet socket address)
	// struct sockaddr: universal socket address
	struct sockaddr_in Server, Client;
	struct sockaddr * server_ptr = ( struct sockaddr *)&Server;
	struct sockaddr * client_ptr = ( struct sockaddr *)&Client;


    // Signal Handler for SIGINT
	memset(&sigint_act, 0, sizeof(sigint_act));
	sigint_act.sa_handler=ctrl_c_fun;
	if(sigaction(SIGINT, &sigint_act, NULL)==-1){
		perror("ERROR: sigaction() failed\n"); 
		exit(EXIT_FAILURE);					
	}

    // Edit command line arguments
	if(argc < 9){
		write(STDOUT_FILENO,"Missing arguments...\n", 21);
		exit(EXIT_FAILURE);
	}
	if(argc == 9){
		if( strcmp(argv[1], "-p") > 0){ 
			printf("Incorrect argument: %s .The correct argument is: -p\n", argv[1]);
			exit(EXIT_FAILURE);    
		}
		if( strcmp(argv[3], "-s") > 0 ){ 
			printf("Incorrect argument: %s .The correct argument is: -s\n", argv[3]);
			exit(EXIT_FAILURE);    
		}
		if( strcmp(argv[5], "-q") > 0 ){ 
			printf("Incorrect argument: %s .The correct argument is: -q\n", argv[5]);
			exit(EXIT_FAILURE);    
		}
		if( strcmp(argv[7], "-b") > 0){ 
			printf("Incorrect argument: %s .The correct argument is: -b\n", argv[7]);
			exit(EXIT_FAILURE);    
		}
	}
		
	if(argc == 9){
		port=atoi(argv[2]);
		thread_pool_size=atoi(argv[4]);
		queue_size=atoi(argv[6]);
		block_size=atoi(argv[8]);
	}
	
	printf("Server's parametrs are:\n");
    printf("thread_pool_size: %d\n", thread_pool_size);
    printf("queue_size: %d\n", queue_size);
    printf("Block_size: %d\n", block_size);

    // socket(): creates a communication endpoint
	if((fd_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0 ){
		perror("ERROR: socket() failed");
		exit(EXIT_FAILURE); 
	}
    
    Server.sin_family = AF_INET;     // For internet communication, sin_family is AF_INET
    Server.sin_addr.s_addr = htonl(INADDR_ANY);   // INADDR_ANY: the socket will accept connection requests on any of the server's network interfaces
    Server.sin_port = htons(port);     // Port number
    
    // bind(): associates the socket with a unique external name, to be used by other processes
    if(bind(fd_socket, server_ptr, sizeof(Server)) < 0){
    	perror("ERROR: bind() failed");
    	exit(EXIT_FAILURE);
	}
	
	printf("Server was successfully initialized...\n");    // optional: print message
	
	// listen(): indicates the socket as ready to accept connections from other sockets
	if(listen(fd_socket, 5) < 0){
		perror("ERROR: listen() failed");
		exit(EXIT_FAILURE);
	}
	
	printf("Listening for connections on port %d \n", port); 

	Q=Create(queue_size);  // Create Queue
	
	// Create worker threads
	if( ( worker_threads = malloc(thread_pool_size* sizeof(pthread_t)) ) == NULL ){
			perror("ERROR: malloc() failed");
			exit(EXIT_FAILURE);		
	}

    pool_size=malloc(1);
	*pool_size = thread_pool_size;
	
	socket_descriptors=NULL;      // socket_descriptors array stores the file descriptions of clients sockets
	
    new_alloc=0; w=0;    // Initialize the variables to zero 
	

	// accept(): waiting for incoming connections
	while( (fd_client= accept(fd_socket, client_ptr, &client_len)) ){

		printf("Accepted connection from localhost\n\n");    // optional: print message
        
		// realloc socket_descriptors array -->> adding one more client socket file descriptor 
		socket_descriptors=(int*)realloc(socket_descriptors,(++new_alloc)*sizeof(int));
		socket_descriptors[new_alloc-1]=fd_client; 
        	    
	    // Send block size to the client
		number=htonl(block_size);  
	    if ( send(fd_client, &number, sizeof(number) , 0) < 0){ 
			perror("ERROR: send2() failed\n");
			exit(EXIT_FAILURE); 	
		}
		
		// Thread definition
        pthread_t communication_thread;
        pthread_t thread_pool;
        
        new_fd_client = malloc(1);
		*new_fd_client = fd_client;
		
        // Start the communication_thread
		if ( ( error=pthread_create(&communication_thread, NULL ,  Communication_Threads, ( void *)new_fd_client) ) < 0 ){
			fprintf(stderr , "ERROR: pthread_create() failed : %s \n", strerror( error ));
            exit(EXIT_FAILURE);
        }
		if( (error=pthread_detach(communication_thread) ) == -1){
			fprintf(stderr , "ERROR: pthread_detach() failed : %s \n", strerror( error ));
            exit(EXIT_FAILURE);	
		}
		
		sleep(2);
		
		// Start the thread pool
		if ( (error=pthread_create(&thread_pool, NULL , Thread_Pool, ( void *)pool_size) ) < 0 ){
            fprintf(stderr , "ERROR: pthread_create() failed : %s \n", strerror( error ));
            exit(EXIT_FAILURE);
        }
        if( (error=pthread_detach(thread_pool) ) == -1){
			fprintf(stderr , "ERROR: pthread_detach() failed : %s \n", strerror( error ));
            exit(EXIT_FAILURE);	
		}

	}
    
    // If accept() failed
   	if(fd_client < 0 ){   
		perror("ERROR: accept() failed");
		exit(EXIT_FAILURE);
	}
	
	close(fd_socket);   // close server socket descriptor
	
	exit(EXIT_SUCCESS); 
}
