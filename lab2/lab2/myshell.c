/**
 * CS2106 AY22/23 Semester 2 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */

#include "myshell.h"

#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>	
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

struct PCBTable pcbTable[MAX_PROCESSES]; 
int numOfProc = 0; 

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void signal_handler(int signo) {

        // Use the signo to identy ctrl-Z or ctrl-C and print “[PID] stopped or print “[PID] interrupted accordingly.
        // Update the status of the process in the PCB table 

}



static void proc_update_status() {


       /******* FILL IN THE CODE *******/

        // Call everytime you need to update status and exit code of a process in PCBTable
        // check and update pcbTable if background child ended
		pid_t pid;
    	int status_code;
    		
    	for (int i = 0; i < numOfProc; i++) {
        	if (pcbTable[i].pid != -1) {
            	pid = waitpid(pcbTable[i].pid, &status_code, WNOHANG);
            	if (pid > 0) {
                	// Child process has exited
                	pcbTable[i].status = 1;
                	// update exitcode terminated by a signal
        			if (WIFSIGNALED(status_code)) {
            			pcbTable[i].exitCode = WTERMSIG(status_code);
        			} else {
            			// update exitcode from status_code
            			pcbTable[i].exitCode = WEXITSTATUS(status_code);
        			}
            	}
        	}
    	}

        // May use WIFEXITED, WEXITSTATUS, WIFSIGNALED, WTERMSIG, WIFSTOPPED


}


/*******************************************************************************
 * Built-in Commands
 ******************************************************************************/
static void update_child_status() {
	// check and update pcbTable if background child ended
	pid_t pid;
    int status_code;
    	
    for (int i = 0; i < numOfProc; i++) {
        if (pcbTable[i].pid != -1) {
            pid = waitpid(pcbTable[i].pid, &status_code, WNOHANG);
            if (pid > 0) {
                // Child process has exited
                pcbTable[i].status = 1;
                // update exitcode terminated by a signal
        		if (WIFSIGNALED(status_code)) {
            		pcbTable[i].exitCode = WTERMSIG(status_code);
        		} else {
            		// update exitcode from status_code
            		pcbTable[i].exitCode = WEXITSTATUS(status_code);
        		}
            }
        }
    }
}


static void command_info(char option) {

        /******* FILL IN THE CODE *******/
	int numOfExited = 0;
	int numOfRunning = 0;
	int numOfTerminating = 0;
	
	proc_update_status();
	
	
    // If option is 0
    if (option == '0') {
    	//printf("Option 0: print details of all processes\n");
        //Print details of all processes in the order in which they were run. You will need to print their process IDs, their current status (Exited, Running, Terminating, Stopped)
        // For Exited processes, print their exit codes.
        
        for (int i = 0; i < numOfProc; i++) {
        printf("[%d] ", pcbTable[i].pid);
    		if (pcbTable[i].status == 1) {
    			// If option is 1
    		    //Print the number of exited process.
    			printf("Exited %d\n", pcbTable[i].exitCode);
    		} else if (pcbTable[i].status == 2) {
    			// If option is 2
        		//Print the number of running process.
        		printf("Running\n");
        	} else if (pcbTable[i].status == 3) {
        		// If option is 3
        		//Print the number of terminating process.
        		printf("Terminating\n");
        	} else if (pcbTable[i].status == 4) {
    			// If option is 4
        		//Print the number of stopped process.
        		printf("Stopped\n");
        	}
        }
   	} else if (option == '1') {
   		//printf("Option 1: print num of exited processes\n");
   		for (int i = 0; i < numOfProc; i++) {
            if (pcbTable[i].status == 1) {
                numOfExited++;
            }
        }
    	printf("Total exited process: %d\n", numOfExited);
   	} else if (option == '2') {
   		//printf("Option 2: print num of running processes\n");
   		for (int i = 0; i < numOfProc; i++) {
            if (pcbTable[i].status == 2) {
                numOfRunning++;
            }
        }
    	printf("Total running process: %d\n", numOfRunning);
    } else if (option == '3') {
   		//printf("Option 3: print num of terminating processes\n");
   		for (int i = 0; i < numOfProc; i++) {
            if (pcbTable[i].status == 3) {
                numOfTerminating++;
            }
        }
    	printf("Total terminating process: %d\n", numOfTerminating);
   	} else {
   		//For all other cases print “Wrong command” to stderr.
   		fprintf(stderr, "Wrong command\n");
   	}
}

static void command_wait(pid_t pid) {

        /******* FILL IN THE CODE *******/

	
    // Find the {PID} in the PCBTable
    for (int i = 0; i < numOfProc; i++) {
        if (pcbTable[i].pid == pid) {
        	
        	//printf("Found pid: %d in table\n", pid);
        	// check if still running
        	if (kill(pid, 0) == 0) {
        		// If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
        		//printf("running, wait for it\n");
        		int status_code;
        		if (waitpid(pid, &status_code, WUNTRACED) == -1) {
        			fprintf(stderr, "Error waitpid, wait for child\n");
        			exit(EXIT_FAILURE);
        		}
        		// After the process terminate, update status and exit code (call proc_update_status())
        		int exit_status = WEXITSTATUS(status_code);
            	pcbTable[i].status = 1;
            	pcbTable[i].exitCode = exit_status;
        	}
        }
    }
    
    
    // Else, continue accepting user commands.

}


static void command_terminate(pid_t pid) {

        /******* FILL IN THE CODE *******/

    // Find the pid in the PCBTable
    for (int i = 0; i < numOfProc; i++) {
        if (pcbTable[i].pid == pid) {
        	//printf("Found pid: %d in table\n", pid);
        	if (kill(pid, 0) == 0) {
        		// If {PID} is RUNNING:
        		//Terminate it by using kill() to send SIGTERM
        		printf("Terminating [%d]\n", pcbTable[i].pid);
            	kill(pid, SIGTERM);
        		int status_code;
        		if (waitpid(pid, &status_code, WNOHANG) == -1) {
        			fprintf(stderr, "Error waitpid, continue to accept\n");
        			exit(EXIT_FAILURE);
        		}
        		// The state of {PID} should be “Terminating” until {PID} exits
        		pcbTable[i].status = 3;
        	}
        }
    }
    

}

static void command_fg(/* pass necessary parameters*/) {

        /******* FILL IN THE CODE *******/
        

    // if the {PID} status is stopped
        //Print “[PID] resumed”
        // Use kill() to send SIGCONT to {PID} to get it continue and wait for it
        // After the process terminate, update status and exit code (call proc_update_status())
}


/*******************************************************************************
 * Program Execution
 ******************************************************************************/

static void command_exec(char **cmd, size_t cmd_length) {

        /******* FILL IN THE CODE *******/


    // check if program exists and is executable : use access()
    if (access(cmd[0], X_OK) != 0) {
    	printf("%s not found\n", cmd[0]);
    	return;
	}
	//printf("Program exists and is executable\n");

    // fork a subprocess and execute the program

    pid_t pid;
    bool wait = true;
    if (strcmp(cmd[cmd_length - 1], "&") == 0) {
        	wait = false;
			cmd[cmd_length - 1] = NULL;
	}
    if ((pid = fork()) == 0) {
        // CHILD PROCESS


        // check file redirection operation is present : ex3
        
        // if < or > or 2> present: 
            // use fopen/open file to open the file for reading/writing with  permission O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_SYNC and 0644
            // use dup2 to redirect the stdin, stdout and stderr to the files
            // call execv() to execute the command in the child process

        // else : ex1, ex2 
            // call execv() to execute the command in the child process
        
        if (execv(cmd[0], cmd) == -1) {
        	fprintf(stderr, "execv() failed\n");
        	exit(EXIT_FAILURE);
    	}
    	
    	

        // Exit the child
        //exit(0);
		

    } else {


        // PARENT PROCESS
        // register the process in process table
        int index = -1;
        for (int i = 0; i < MAX_PROCESSES; i++) {
        	if (pcbTable[i].pid == -1) {
        		index = i;
        		break;
        	}
        }
        
        // Full pcbTable
        if (index == -1) {
        	fprintf(stderr, "Full pcbTable\n");
        	kill(pid, SIGTERM);
        	return;
        }
        // record entry
        pcbTable[index].pid = pid;
        pcbTable[index].status = 2;
        numOfProc++;

        // If  child process need to execute in the background  (if & is present at the end )
        //print Child [PID] in background
        int status_code;
        //printf("wait is currently: %s\n", wait?"true":"false");
		if (wait == false) {
			//printf("We dont wait for child\n");
            fprintf(stderr, "Child [%d] in background\n", pid);
            if (waitpid(pid, &status_code, WNOHANG) == -1) {
        		fprintf(stderr, "Error waitpid, continue to accept\n");
        		exit(EXIT_FAILURE);
        	}
        } else {
        	// else wait for the child process to exit 
        	//printf("We waiting for child\n");
        	if (waitpid(pid, &status_code, WUNTRACED) == -1) {
        		fprintf(stderr, "Error waitpid, wait for child\n");
        		exit(EXIT_FAILURE);
        	}
        	int exit_status = WEXITSTATUS(status_code);
            	pcbTable[index].status = 1;
            	pcbTable[index].exitCode = exit_status;
        }
        // Use waitpid() with WNOHANG when not blocking during wait and  waitpid() with WUNTRACED when parent needs to block due to wait 


    }
}



/*******************************************************************************
 * Command Processor
 ******************************************************************************/

//static void command(char **tokens, size_t numOfTokens) {
//static void command(char **cmd) {
static void command(char **cmd, size_t cmd_length) {

        /******* FILL IN THE CODE *******/
    
    // if command is "info" call command_info()             : ex1
    if (strcmp(cmd[0], "info") == 0) {
    	//printf("cmd[1] is: %c \n", *cmd[1]);
    	if (cmd_length != 2) {
    		fprintf(stderr, "Wrong command\n");
    	} else {
    		command_info(*cmd[1]);
    	}
    } else if (strcmp(cmd[0], "wait") == 0) {
    	// if command is "wait" call command_wait()             : ex2
    	if (cmd_length != 2) {
    		fprintf(stderr, "Wrong command\n");
    	} else {
    		//printf("Passing pid: %d to command_wait\n", atoi(cmd[1]));
    		pid_t pid = atoi(cmd[1]);
    		command_wait(pid);
    	}
    
    } else if (strcmp(cmd[0], "terminate") == 0) {
    	// if command is "terminate" call command_terminate()   : ex2
    	if (cmd_length != 2) {
    		fprintf(stderr, "Wrong command\n");
    	} else {
    		//printf("Passing pid: %d to command_terminate\n", atoi(cmd[1]));
    		pid_t pid = atoi(cmd[1]);
    		command_terminate(pid);
    	}
    } else {
    	// call command_exec() for all other commands           : ex1, ex2, ex3
    	command_exec(cmd, cmd_length);
    }
    
    
    
    // if command is "fg" call command_fg()                 : ex4

    


}

/*******************************************************************************
 * High-level Procedure
 ******************************************************************************/

void my_init(void) {

           /******* FILL IN THE CODE *******/
		//struct PCBTable pcbTable[MAX_PROCESSES]; 
		//int numOfProc = 0;   
		
		for (int i = 0; i < MAX_PROCESSES; i++) {
			pcbTable[i].pid = -1;
			pcbTable[i].status = -1;
			pcbTable[i].exitCode = -1;
		} 

        // use signal() with SIGTSTP to setup a signalhandler for ctrl+z : ex4
        // use signal() with SIGINT to setup a signalhandler for ctrl+c  : ex4

        // anything else you require
        

}

void my_process_command(size_t num_tokens, char **tokens) {


        /******* FILL IN THE CODE *******/

        // Split tokens at NULL or ; to get a single command (ex1, ex2, ex3, ex4(fg command))
		char ***commands_arr = malloc(num_tokens * sizeof(char**));
		size_t *commands_len_arr = malloc(num_tokens * sizeof(size_t));
    	size_t cmd_count = 0;
    	size_t start = 0;
    	for (size_t i = 0; i < num_tokens; i++) {
        	if (tokens[i] == NULL || strcmp(tokens[i], ";") == 0) {
            	// end of command
            	size_t command_length = i - start;
            	char **command = malloc((command_length + 1) * sizeof(char*));
            	for (size_t j = 0; j < command_length; j++) {
                	command[j] = tokens[start + j];
            	}
            	command[command_length] = NULL;
            	commands_arr[cmd_count] = command;
            	commands_len_arr[cmd_count] = command_length;
            	//printf("%s", command[0]);
            	cmd_count++;
            	// shift index to start of next command
            	start = i + 1;
        	}
    	}

        // for example :  /bin/ls ; /bin/sleep 5 ; /bin/pwd
        // split the above line as first command : /bin/ls , second command: /bin/pwd  and third command:  /bin/pwd
        
        // Call command() and pass each individual command as arguements
        for (size_t i = 0; i < cmd_count; i++) {
        	command(commands_arr[i], commands_len_arr[i]);
        	free(commands_arr[i]);
    	}
    	free(commands_arr);
    	


}

void my_quit(void) {


    /******* FILL IN THE CODE *******/
    proc_update_status();
    // Kill every process in the PCB that is either stopped or running
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (pcbTable[i].status == 4 || pcbTable[i].status == 2) {
        	printf("Killing [%d]\n", pcbTable[i].pid);
            kill(pcbTable[i].pid, SIGTERM);
        }
    }
	//puts("Goodbye\n");
	//exit(0);
    printf("\nGoodbye\n");
}
