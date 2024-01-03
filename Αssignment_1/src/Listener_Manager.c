#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "../include/DoublyList_Interface.h"
#include "../include/Worker_Interface.h"

void Unlink_Fifo(int);
char *fifo_names(int);

char *FIFO_NAME = "/tmp/fifo_wm";
static List Q;

static void ctrl_c_fun(int signalnum)
{
	ListNode a;
	pid_t pid;

	Unlink_Fifo(Size(Q)); // Delete all named_pipes (aka fifos)

	// Delete pids from queue AND kill pids with signal -> SIGKILL
	while (IsEmpty(Q) != 0)
	{
		// Delete pid
		a = GetFirst(Q);
		pid = item(a);
		Remove(Q, a);

		// Kill pid
		if (kill(pid, SIGKILL) == -1)
		{
			(void)write(STDOUT_FILENO, "ERROR: kill() failed\n", 21);
			_exit(EXIT_FAILURE);
		}
	}
	free(Q); // free -->> queue

	_exit(EXIT_FAILURE);
}

static void wait_sigchld(int signalnum)
{
	pid_t sid;
	int STATUS;

	//  WNOHANG-->> waitpid return even if the status of a child process isn't immediately available AND
	//  WUNTRACED -->> waitpid to report the status of a child process that -> stopped
	while ((sid = waitpid(-1, &STATUS, WNOHANG | WUNTRACED)) > 0)
	{

		AddLast(Q, make(sid)); // if current child process -> stopped then add pid to the end of the queue
	}

	// if waitpid() failed
	if (sid < 0)
	{
		(void)write(STDOUT_FILENO, "ERROR: waitpid() failed\n", 24);
		_exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[])
{
	char *array[7];
	char buf[512], c, *fifo_name;
	int p_fd[2], fd_fifo;
	pid_t pid, wid, pids[4096];
	int counter = 0, n, s, i, size = 0, flag, directory;
	ListNode a;
	struct sigaction sigchld_act, sigint_act;

	// Watch -->> current directory
	if (argc == 1)
	{
		array[0] = "inotifywait";
		array[1] = ".";
		array[2] = "-m";
		array[3] = "-e";
		array[4] = "create";
		array[5] = "-e";
		array[6] = "moved_to";
	}

	// Watch -->> other directory
	if (argc == 3)
	{
		array[0] = "inotifywait";
		array[1] = argv[2];
		array[2] = "-m";
		array[3] = "-e";
		array[4] = "create";
		array[5] = "-e";
		array[6] = "moved_to";
	}

	if (argc == 2)
	{
		write(STDOUT_FILENO, "Missing arguments...\n", 21);
		exit(EXIT_FAILURE);
	}

	// Signal Handler for SIGINT
	memset(&sigint_act, 0, sizeof(sigint_act));
	sigint_act.sa_handler = ctrl_c_fun;
	if (sigaction(SIGINT, &sigint_act, NULL) == -1)
	{
		perror("ERROR: sigaction() failed\n");
		exit(EXIT_FAILURE);
	}

	// Signal Handler for SIGCHLD
	memset(&sigchld_act, 0, sizeof(sigchld_act));
	sigchld_act.sa_handler = wait_sigchld;
	if (sigaction(SIGCHLD, &sigchld_act, NULL) == -1)
	{
		perror("ERROR: sigaction() failed\n");
		exit(EXIT_FAILURE);
	}

	// Create Folder: Producted_Files -->> The results will be saved in this folder
	if ((directory = mkdir("../Producted_Files", 0777)) == -1)
	{
		perror("ERROR: mkdir() failed\n");
		exit(EXIT_FAILURE);
	}

	Q = Create(); // Create Queue

	for (int i = 0; i < sizeof(buf); i++)
	{
		buf[i] = '\0';
	} // Initialize -->> reading buffer

	// Create a simple pipe
	if (pipe(p_fd))
	{
		perror("ERROR: pipe() failed\n"); // Error message
		exit(EXIT_FAILURE);
	}

	// Create --->>  Listener and Manager
	switch (pid = fork())
	{
	case -1:
		perror("ERROR: fork() failed\n");
		exit(EXIT_FAILURE);
	case 0:							  //  -----------------   Listener  ------------------   //
		close(p_fd[0]);				  // I do not need it
		dup2(p_fd[1], STDOUT_FILENO); // now p_fd[1] = STDOUT_FILENO
		close(p_fd[1]);				  // I do not need it anymore
		if (execvp(array[0], array) < 0)
		{										// Call -->> inotifywait
			perror("ERROR: execvp() failed\n"); // Error message
			exit(EXIT_FAILURE);
		}
	default:			//  -----------------   Manager  ------------------   //
		close(p_fd[1]); // I do not need it

		for (;;)
		{
			i = 0;
			flag = 0;

			while ((n = read(p_fd[0], &c, 1)) > 0)
			{ // read from stdout  (read one character at a time)

				if (c == '\n')
				{
					break;
				} //  if read all message -->> Stop

				// Save the path of file
				if (c != ' ' && flag == 0)
				{
					buf[i++] = c;
				}
				else if (c == ' ' && flag == 1)
				{
					flag = 0;
				}
				else
				{
					flag = 1;
				}
			}
			// If the read() is blocked by a signal -->> set errno=0 and continue to the next recurrence
			if (errno == EINTR && n == -1)
			{
				errno = 0;
				continue;
			}

			// If queue is empty -->> create new worker
			if (IsEmpty(Q) == 1)
			{

				// Create the name of fifo. If size=0 then fifo_name is /tmp/fifo_wm0 , size=1 then fifo_name is /tmp/fifo_wm1 .......
				fifo_name = fifo_names(size);

				// Create the fifo
				if ((mkfifo(fifo_name, 0777) < 0) && (errno != EEXIST))
				{
					perror("ERROR: mkfifo() failed\n");
					exit(EXIT_FAILURE);
				}

				// Open the fifo
				if ((fd_fifo = open(fifo_name, O_RDWR)) < 0)
				{
					perror("ERROR_named_pipe: open() failed\n");
					exit(EXIT_FAILURE);
				}

				// Write into the fifo the path of file
				if (write(fd_fifo, buf, strlen(buf)) != strlen(buf))
				{
					perror("ERROR_named_pipe: write() failed\n");
					exit(EXIT_FAILURE);
				}

				// Create -->> worker
				switch (pids[size++] = fork())
				{
				case -1:
					perror("ERROR: fork() failed\n");
					exit(EXIT_FAILURE);
				case 0: //  -----------------   Worker  ------------------   //
					worker(fifo_name, getpid());
				}
			}
			else
			{
				a = GetFirst(Q); // Get first free worker from queue
				wid = item(a);	 // Get the "worker number"

				// Find the position of the worker in the array pids  -->> and use the position to find the name of fifo
				for (s = 0; s < size; s++)
				{
					if (pids[s] == wid)
					{
						break;
					}
				}

				Remove(Q, a); // Remove the worker from queue

				kill(wid, SIGCONT); // Wake up, Worker!!!!!

				fifo_name = fifo_names(s); // Find the name of fifo. If possition=4 then fifo_name=/tmp/fifo_wm4

				// Open the fifo
				if ((fd_fifo = open(fifo_name, O_RDWR)) < 0)
				{
					perror("ERROR_named_pipe: open() failed\n");
					exit(EXIT_FAILURE);
				}

				// Write into the fifo the path of file
				if (write(fd_fifo, buf, strlen(buf)) != strlen(buf))
				{
					perror("ERROR_named_pipe: write() failed\n");
					exit(EXIT_FAILURE);
				}

				worker(fifo_name, wid); // call worker!
			}

			for (i = 0; i < sizeof(buf); i++)
			{
				buf[i] = '\0';
			} // Initialize -->> reading buffer for the next recurrence
		}
		close(p_fd[0]); // I do not need it anymore
	}

	exit(EXIT_SUCCESS);
}

char *fifo_names(int num)
{ // Create the name of fifo or find the name of fifo
	char str[20];
	sprintf(str, "%d", num); // convert int to string
	char *fifo_name = malloc((strlen(FIFO_NAME) + strlen(str) - 1) * sizeof(char));
	strcpy(fifo_name, FIFO_NAME); // fifo_name="/tmp/fifo_wm"
	strcat(fifo_name, str);		  // if str="1" -->>  fifo_name="/tmp/fifo_wm1"
	return fifo_name;
}

void Unlink_Fifo(int num)
{ // Delete all fifos
	int i;

	for (i = 0; i < num; i++)
	{ // num is the number of fifos
		if (unlink(fifo_names(i)) < 0)
		{
			perror("ERROR: unlink() failed\n");
			exit(EXIT_FAILURE);
		}
	}
}
