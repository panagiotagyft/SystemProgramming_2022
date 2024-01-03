#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../include/Worker_Interface.h"

void worker(char *fifo_name, pid_t pid)
{
	int fd_fifo, file_d, prod_files, i, j, k, counter, flag, flag1;
	char buf[512], location[512], buf_l[512], c;

	List l = create();

	// Initialize -->> buffers
	for (i = 0; i < 512; i++)
	{
		buf_l[i] = '\0';	// buffer -->> help to save the location
		location[i] = '\0'; // buffer -->> help to save the location
		buf[i] = '\0';		// reading buffer
	}

	// Open the fifo
	if ((fd_fifo = open(fifo_name, O_RDONLY)) < 0)
	{
		perror("ERROR: worker_read -> open() failed\n");
		exit(EXIT_FAILURE);
	}

	// Read the fifo and save the message
	if (read(fd_fifo, buf, 512) <= 0)
	{
		perror("ERROR: read() failed\n");
		exit(EXIT_FAILURE);
	}

	sleep(5); // sleep 5 sec

	// Open the file which sent the manager
	if ((file_d = open(buf, O_RDONLY)) == -1)
	{
		perror("ERROR: open() failed\n");
		exit(EXIT_FAILURE);
	}

	flag = 0;
	counter = 0;
	i = 0;
	while (read(file_d, &c, 1) > 0)
	{ // read the file  (read one character at a time)

		// find urls with protocol http://
		if (i == 0 && c == 'h')
		{
			i++;
			continue;
		}
		else if (i == 1 || i == 2 && c == 't')
		{
			i++;
			continue;
		}
		else if (i == 3 && c == 'p')
		{
			i++;
			continue;
		}
		else if (i == 4 && c == ':')
		{
			i++;
			continue;
		}
		else if (i == 5 || i == 6 && c == '/')
		{
			i++;
			flag = 1;
			j = 0;
			continue;
		}
		else
		{
			if (flag == 0)
			{
				continue;
			}
		}

		// If find urls	with protocol http:// save the location
		if (flag == 1 && i > 6)
		{

			if (c != '/' && c != ' ')
			{
				buf_l[j++] = c;
			}
			else
			{

				if (buf_l[0] == 'w' && buf_l[1] == 'w' && buf_l[2] == 'w' && buf_l[3] == '.')
				{ // remove www. from location
					k = 0;
					for (i = 4; i < sizeof(buf_l); i++)
					{
						location[k++] = buf_l[i];
					}
					// If the location exists in list -->> increase the counter of appearances else insert the location in list
					if (Update(l, location) == 0)
					{
						counter++;
						insert(l, Make(location));
					}

					for (i = 0; i < sizeof(buf_l); i++)
					{
						buf_l[i] = '\0';
					} // Initialize -->> buffer for the next url
					flag = 0;
					i = 0; // Initialize helpful variables
				}
				else
				{
					strcpy(location, buf_l);

					// If the location exists in list -->> increase the counter of appearances else insert the location in list
					if (Update(l, location) == 0)
					{
						counter++;
						insert(l, Make(location));
					}

					for (i = 0; i < sizeof(buf_l); i++)
					{
						buf_l[i] = '\0';
					} // Initialize -->> buffer for the next url
					flag = 0;
					i = 0; // Initialize helpful variables
				}
			}
		}
	}

	char buf_filename[100];
	flag1 = 0;
	k = 0;
	for (i = 0; i < sizeof(buf_filename); i++)
	{
		buf_filename[i] = '\0';
	} // Initialize -->> buffer

	// Isolate the file name from the path
	for (i = sizeof(buf) - 1; i >= 0; i--)
	{ // save the name upside down

		if (buf[i] != '/')
		{
			if (flag1 == 1)
			{
				buf_filename[k++] = buf[i];
			}
			if (buf[i] == '.')
			{
				flag1 = 1;
			}
			buf[i] = '\0';
		}
		else
		{
			break;
		}
	}

	char path_dir[512];

	for (i = 0; i < 512; i++)
	{
		path_dir[i] = '\0';
	} // Initialize -->> buffer path dir

	// Create the path of the new file
	strcpy(path_dir, "./Producted_Files/");

	for (i = sizeof(buf_filename) - 1; i >= 0; i--)
	{
		path_dir[strlen(path_dir)] = buf_filename[i];
	} // �ligned the name

	strcat(path_dir, ".out"); // Set the file format

	// Create the new file
	if ((prod_files = open(path_dir, O_RDWR | O_CREAT, 0777)) == -1)
	{
		perror("ERROR: open() failed\n");
		exit(EXIT_FAILURE);
	}

	ListNode a;
	char *buffer;
	counter = 0;

	// Write in the file all the locations with the number of their appearances
	a = Get_First(l);
	while (size(l) > counter)
	{								  // Cross all list
		buffer = get_loacation(l, a); // Get the location with the number of appearances as a string
		strcat(buffer, "\n");		  // add the "\n" to change line

		// Write in the file the location with the number of appearances
		if (write(prod_files, buffer, strlen(buffer)) != strlen(buffer))
		{
			perror("ERROR: write() failed\n");
			exit(EXIT_FAILURE);
		}
		counter++;
		if (counter < size(l))
		{
			a = Get_Next(l, a);
		} // Get the next location
	}

	remove_list(l); // Delete the list

	kill(pid, SIGSTOP); // Sleep, worker !!!!
}
