# include <stdio.h>
# include <sys/types.h> 
# include <sys/socket.h> 
# include <netinet/in.h> 
# include <unistd.h> 
# include <netdb.h> 
# include <stdlib.h> 
# include <string.h> 
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>


int Check_if_a_Directory_Exists(char* path){
    DIR *directory=NULL;
    
    errno=0;
	if ( (directory= opendir(path))){ 
   		 closedir(directory);
   		 return 1;
	}
    else{  return 0;  }
}

// main(): uses code from lecture notes
// https://cgi.di.uoa.gr/~antoulas/k24/lectures/l10.pdf --->> p.35 & p.36 
int main(int argc,char *argv[]){
	int server_port,fd_socket, number, file_size, path_file_len, i, slash, fd_file, fd_dir, block_size, l;
	struct sockaddr_in Server ;
	struct sockaddr * server_ptr = ( struct sockaddr *)&Server;
	struct hostent * rem ;
   	char buf[4096], PrevDirectories[100];
   
   // Edit command line arguments
	if(argc < 7){
		write(STDOUT_FILENO,"Missing arguments...\n", 21);
		exit(EXIT_FAILURE);
	}
	
	if(argc == 7){
		if( strcmp(argv[1], "-i") > 0 ){ 
			printf("Incorrect argument: %s .The correct argument is: -i\n", argv[1]);
			exit(EXIT_FAILURE);    
		}
		if( strcmp(argv[3], "-p") > 0 ){ 
			printf("Incorrect argument: %s .The correct argument is: -p\n", argv[3]);
			exit(EXIT_FAILURE);    
		}
		if( strcmp(argv[5], "-d") > 0 ){ 
			printf("Incorrect argument: %s .The correct argument is: -d\n", argv[5]);
			exit(EXIT_FAILURE);    
		}
	}
	
	int len_server_ip,  len_directory;
	
	len_server_ip=strlen(argv[2]);
	char server_ip[len_server_ip];
	
	len_directory=strlen(argv[6]);
	char directory[len_directory];	
	
	if(argc == 7){
		strcpy(server_ip,argv[2]);
		server_port=atoi(argv[4]);
		strcpy(directory,argv[6]);
	}
	
	
	printf("Client's parametrs are:\n");
    printf("serverIP: %s\n", server_ip);
    printf("port: %d\n", server_port);
    printf("directory: %s\n", directory);
    
	for(i=0; i < 100; i++){ PrevDirectories[i]='\0'; }
    
	int count_prev_directs=0, count_slash, flag_d=0, d=0;
    
    // Count previous catalogs
	for(i=0; i < len_directory; i++){
    	if(directory[i]=='/'){ count_prev_directs++; }
	}
	
	// Find previous directories
	for(i=0; i < len_directory; i++){
		if(directory[i]=='/'){ count_slash++; }
		if(count_prev_directs==count_slash && flag_d==0){
			flag_d=1;
			continue;
		}
		if(flag_d!=1){
			PrevDirectories[d++]=directory[i];
		}
	}
  		
	// socket(): creates a communication endpoint					
	if((fd_socket = socket(AF_INET , SOCK_STREAM , 0)) < 0 ){
		perror("ERROR: socket() failed"); 
		exit(EXIT_FAILURE);
	}

    // gethostbyname(): search for the host specified
	if (( rem = gethostbyname(server_ip)) == NULL ) {
		herror("ERROR: gethostbyname() failed"); 
		exit(EXIT_FAILURE);
	}

	
    Server.sin_family = AF_INET;     // For internet communication, sin_family is AF_INET
    memcpy(&Server.sin_addr, rem->h_addr, rem->h_length );
    Server.sin_port=htons(server_port);
    
    // connect(): connects to a socket that is blocked in an accept
	while(connect(fd_socket, server_ptr, sizeof(Server)) == -1){
		// p.558 -->> Programmatismos se Unix Marc J. Rochkind
		if(errno==ENOENT){ 
			sleep(1); 
			continue;
		}
		else{ 
			perror("ERROR: connect() failed"); 
			exit(EXIT_FAILURE);
		}
	}
	
	printf("Connecting to %s on port %d \n",server_ip, server_port);

    // Send the relevant path to the server ( request )
    if ( send(fd_socket, directory, len_directory , 0) < 0){
		perror("ERROR: send() failed\n");
		exit(EXIT_FAILURE); 	
	}
	
	// Read the block_size
	if(recv(fd_socket, &number, sizeof(number), 0) < 0 ){
		perror("ERROR: recv() failed");
		exit(EXIT_FAILURE);	
	}
	block_size=ntohl(number);
	

    int read__b;
    while(1){

// --- A. read file name. ---

		if((read__b=recv(fd_socket, buf, sizeof(buf), 0)) < 0 ){
			perror("ERROR: recv1() failed");
			exit(EXIT_FAILURE);	
		}
        
        // Server disconnected -->> if server close then break and terminate the client!
		if(read__b==0){ 
		    printf("Server disconnected -->> The Server is finished.The job is done.Bye!\n");
			break;
		}   
  		    
		printf("Received: %s\n",buf);
		
// --- B. read file size. ---
		
		if(recv(fd_socket, &number, sizeof(number), 0) < 0 ){
			perror("ERROR: recv2() failed");
			exit(EXIT_FAILURE);	
		}
		file_size=ntohl(number);

// --- C. send file. --- 
	
		// e.g   file_size_in_bytes = 4303   &   block_size = 2000
		float variable1 = (float)file_size/(float)block_size;   //    4303.00/2000.00 = 2.1515 (= variable1)
		int variable2=(int)variable1;                                    //    variable2 = 2
		float variable3=variable1-(float)variable2;                      //    2.1515-2.0000=0.1515 (=variable3)
		
		// if variable3=0.00 -->> ok the number of repetitions is exactly e.g. 2! Do not increase the variable2 and continue
		// if variable3!=0 and variable2==0 ( because e.g. variable1=0.1515) --- then --->> increase the variable2
		if(variable3!=0.00 || variable2==0){ variable2++; } 
	
	
		int len_prev_directs;
        char path_file[100], substr[100];
        
		for(i=0; i < sizeof(path_file); i++){ path_file[i]='\0'; substr[i]='\0';}
        strcpy(path_file,buf);
		path_file_len=strlen(path_file);
	    
		len_prev_directs=strlen(PrevDirectories);
		
	    char str[len_prev_directs+1]; 
	  	int flag=0, j=0;
        
        //  e.g.  if PrevDirectories = "Server/floder2" and path_file = "../server/floder2/floder2_1/test.txt" 
		for(i=0; i < path_file_len; i++){
			if(flag==0){
				memcpy(str, &path_file[i], len_prev_directs);    
				str[len_prev_directs]='\0';
				
				// if str is equal with PrevDirectories  -- then -->> set flag=1 and i=i+len_prev_directs (because i want from i.st position to 
				// save  relevant path.If path_file = "../server/floder2/floder2_1/test.txt  -- save -->> "floder2_1/test.txt" 
				if(strcmp(str,PrevDirectories)==0){    
					flag=1;
					i=i+len_prev_directs;   
				}
			}
			else{
				substr[j++]=path_file[i];
			}
		}	

        // Count how many folders need to be created
	    slash=0;
		for(i=0; i < sizeof(substr); i++){
			if(substr[i]=='/'){ 
				slash++; 
			}
			
		}
        
		// Create folders
		if(slash>=1){
            char string[100], path_dir[100], dir_name[100];  
            for(l=0; l<100; l++){ string[l]='\0'; path_dir[i]='\0'; dir_name[i]='\0';}
            l=0;
			for(i=0; i < sizeof(substr); i++){
				if(substr[i]=='/'){ 
  			        
					strcpy(path_dir, "./Output/");
    				strcat(path_dir,string);
    				
    				strcpy(dir_name, "Output/");
    				strcat(dir_name,string);
    				
    				// If current path is directory make dir
    				if(Check_if_a_Directory_Exists(path_dir)==0){  
    					if((fd_dir=mkdir(dir_name, 0755))==-1){
							perror("ERROR: mkdir() failed\n");
							exit(EXIT_FAILURE);
						}	
					}
					
				}
				string[l++]=substr[i];
			}

		}
		
		char path[100];
        for(i=0; i<100; i++){ path[i]='\0';}
		
		strcpy(path, "./Output/");
		strcat(path,substr);
 
        // Create file
		if((fd_file=open(path,  O_WRONLY | O_CREAT | O_TRUNC , S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH))==-1){
			perror("ERROR: open() failed\n"); 
			exit(EXIT_FAILURE);	
		}
		 
        char read_b[block_size];
	    int n;
		
		for(int i=0; i<variable2; i++){
			
			for(j=0; j<sizeof(read_b); j++){ read_b[j]='\0';}    // initialize read_buf for the next repetition
			
			if((n=recv(fd_socket, read_b, sizeof(read_b), 0) )< 0 ){
				perror("ERROR: recv3() failed");
				exit(EXIT_FAILURE);	
			}
			
			if(write(fd_file, read_b, n)!=n){     // write block_size bytes at a time
				perror("ERROR: write() failed\n");
				exit(EXIT_FAILURE);		
			}
		}
	}

	close(fd_socket);
    
	return 0;
}
