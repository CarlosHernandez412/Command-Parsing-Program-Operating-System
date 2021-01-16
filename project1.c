#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>



/* these need to be global if you want the handler to see them */
pid_t parent, cpid;
//int ret;
int pipefd[2];
int status; 

void ctrlc_handler(int sig){
		
    
	
   /* CHILD */
		if (cpid == 0) {
			printf("Child process exiting...\n");
			close(pipefd[0]);
			exit(0);
		}
	 /* PARENT */
	 else {
			printf("Parent process exiting.\n");
			close(pipefd[1]);
			exit(0);
		}
}


int main(int argc, char *argv[]) { 
	char buf[20];
	int child_status;
	parent = getpid(); 
	
   /* block all signals except SIGINT before the fork */
   sigset_t mask;
   sigfillset(&mask);
   sigdelset(&mask,SIGINT);
   sigprocmask(SIG_BLOCK, &mask, NULL);
   
   /* allocate a structure for the signal handler */
   struct sigaction sa;

   /* setup handler before the fork */ 
   sa.sa_handler = ctrlc_handler; /* Name of the signal handler function */
   sa.sa_flags = SA_RESTART;    /* restart system calls if interrupted */ 
   sigfillset(&sa.sa_mask);     /* mask all signals while in the handler */
  
   /* register the handler  */
   sigaction(SIGINT, &sa, NULL) ; 
     
/*print parent PID after setting up sig handler */
	printf(" Parent PID is %d\n",getpid()); 
   
	while(1){      /* repeat forever */
	
			  
			if (pipe(pipefd) < 0) {
			perror("pipe: ");
			exit(1);
			}
			cpid = fork(); 
	/*CHILD */
		
	if (cpid == 0) {
  
     close(pipefd[1]);// close write end of pipe
	 read(pipefd[0], buf, sizeof(buf));//read command string from pipe and print
	 
	  
	  
	  /*PARSE THE COMMAND STRING */
	  if(strcmp("quit", buf) ==0){
		 printf("Received command '%s'\n", buf);
		  printf ("Child sending special exit code to parent. \n");
		  exit(6);
		}
	  else if(strcmp( "list", buf) == 0)  {
		  char *listargv[] = { "/bin/ls", "-al", ".", NULL }; 
		 execve(listargv[0], listargv, NULL);
		 exit(0);
		}
	  else if(strcmp( "cat", buf) == 0)  {
		  printf("Received command '%s'\n", buf);
		  printf("Enter filename: "); //prompt the user
		  scanf ("%s", buf);
			char *catargv[] = { "/bin/cat", buf , NULL };
			execve(catargv[0], catargv, NULL);
			exit(0);
		}
		else {
			printf("Received command '%s'\n", buf);
			//if a command entered is not accepted print that it is unrecognized 
			printf ("Unrecognized command.\n");
			exit(1);
		}
		}
	 else {
		/*PARENT PROCESS STEPS*/
		close (pipefd[0]);
		printf ("Child PID is %d\n", cpid);
		printf ("Enter command: "); //prompt user
		scanf("%s", buf);//read users response into string variable
		write (pipefd[1], buf, sizeof(buf));
		wait(&child_status);
	
		close(pipefd[1]);//close write end of the pipe after status
		if (WIFEXITED(child_status))
		printf("child exited with exit code %d\n",WEXITSTATUS(child_status));
	
		//If the child exit code says the "quit" command was received by the child
		//break out of the loop and exit the program with success.
			if (WEXITSTATUS(child_status)==6){
				printf("Parent exiting..\n");
				exit (EXIT_SUCCESS);
			}
		}
	}
}

	
	
  
  
	
