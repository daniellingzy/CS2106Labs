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

static void proc_update_status();

struct PCBTable pcbTable[MAX_PROCESSES]; 
int numOfProc = 0; 

/*******************************************************************************
 * Signal handler : ex4
 ******************************************************************************/

static void signal_handler(int signo) {
		
		pid_t pid = getpid();
		
        // Use the signo to identy ctrl-Z or ctrl-C and print “[PID] stopped or print “[PID] interrupted accordingly.
        if (signo == SIGINT) {
        	// ctrl-C pressed
        	/*
        	int status_code;
        	pid = waitpid(pid, &status_code, WNOHANG);
            if (pid < 1) {
            	//Means that process is running now.
            	printf("[%d] interrupted\n", pid);
            	kill(pid, SIGINT);
            	return;
        	}
        	*/
        	
        	
        	if (system(NULL)) {
            	// Signal SIGINT 
            	kill(pid, SIGINT);
				printf("[%d] interrupted\n", pid);
        	}
        	
    	} else if (signo == SIGTSTP) {
        	// ctrl-Z pressed
         	if (system(NULL)) {
            	// Signal SIGTSTP 
            	kill(pid, SIGTSTP);
				printf("[%d] interrupted\n", pid);
        	}
    	}
        // Update the status of the process in the PCB table 
		proc_update_status();
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

static void command_info(char option) {

        /******* FILL IN THE CODE *******/
	int numOfExited = 0;
	int numOfRunning = 0;
	int numOfTerminating = 0;
	int numOfStopped = 0;
	
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
   	} else if (option == '4') {
   		//printf("Option 4: print num of stopped processes\n");
   		for (int i = 0; i < numOfProc; i++) {
            if (pcbTable[i].status == 4) {
                numOfStopped++;
            }
        }
    	printf("Total stopped process: %d\n", numOfStopped);
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
        	// check if still running
        	if (kill(pid, 0) == 0) {
        		// If the process indicated by the process id is RUNNING, wait for it (can use waitpid()).
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

static void command_fg(pid_t pid) {

        /******* FILL IN THE CODE *******/
        
        // Find the pid in the PCBTable
    for (int i = 0; i < numOfProc; i++) {
        if (pcbTable[i].pid == pid) {
        	//printf("Found pid: %d in table\n", pid);
        	if (pcbTable[i].status == 4) {
        		// if the {PID} status is stopped
        		//Print “[PID] resumed”
        		printf("[%d] resumed\n", pcbTable[i].pid);
        		// Use kill() to send SIGCONT to {PID} to get it continue and wait for it
            	kill(pid, SIGCONT);
        		int status_code;
        		if (waitpid(pid, &status_code, WUNTRACED) == -1) {
        			fprintf(stderr, "Error waitpid, wait for child\n");
        			exit(EXIT_FAILURE);
        		}
        		// After the process terminate, update status and exit code (call proc_update_status())
        		proc_update_status();
        	}
        }
		break;
    }

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

    // fork a subprocess and execute the program

    pid_t pid;
    bool wait = true;
    if (strcmp(cmd[cmd_length - 1], "&") == 0) {
        	wait = false;
			cmd[cmd_length - 1] = NULL;
	}

    if ((pid = fork()) == 0) {
        // CHILD PROCESS
		int lessIndex = -1;
		int moreIndex = -1;
		int twoMoreIndex = -1;

        // check file redirection operation is present : ex3
		for(int i = 0; cmd[i]!= NULL; i++) {
			if (strcmp(cmd[i], "<") == 0) {
				lessIndex = i;
			} else if (strcmp(cmd[i], ">") == 0) {
				moreIndex = i;
			} else if (strcmp(cmd[i], "2>") == 0) {
				twoMoreIndex = i;
			}
		}
        
        // if < or > or 2> present: 
            // use fopen/open file to open the file for reading/writing with  permission O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_SYNC and 0644
            // use dup2 to redirect the stdin, stdout and stderr to the files
            // call execv() to execute the command in the child process
		if (lessIndex != -1) {
			FILE *filePtr = fopen(cmd[lessIndex + 1], "r");
			if (filePtr != NULL) {
				int fileDes = fileno(filePtr);
				dup2(fileDes, 0);
				fclose(filePtr);
			} else {
				fprintf(stderr, "%s does not exist\n", cmd[lessIndex + 1]);
				exit(1);
			}
		}
		if (moreIndex != -1) {
			FILE *filePtr = fopen(cmd[moreIndex+1], "w");
			if (filePtr != NULL) {
				int fileDes = fileno(filePtr);
				dup2(fileDes, 1);
				fclose(filePtr);
			} else {
				fprintf(stderr, "error occurred during file creation\n");
				exit(1);
			}
		} 
		if (twoMoreIndex != -1) {
			FILE *filePtr = fopen(cmd[twoMoreIndex + 1], "w");
			if (filePtr != NULL) {
				int fileDes = fileno(filePtr);
				dup2(fileDes, 2);
				fclose(filePtr);
			} else {
				fprintf(stderr, "error occurred during file creation\n");
				exit(1);
			}
		}

		if (lessIndex == -1 && moreIndex == -1 && twoMoreIndex == -1) {
			 // call execv() to execute the command in the child process
			 execv(cmd[0], cmd);
		} else {
			int argEnd;
			for (int i = 0; cmd[i] != NULL; i++) {
            	if ((strcmp(cmd[i], "<") == 0) || ( strcmp(cmd[i], ">") == 0 ) || (strcmp(cmd[i], "2>") == 0)) {
                	argEnd = i;
                	break;
            	}
			}
			char *newCmd[argEnd + 1];
        	for (int i = 0; i < argEnd; i++) {
            	newCmd[i] = cmd[i];
        	}
        	newCmd[argEnd] = NULL;
        	execv(cmd[0], newCmd);
		}

        // else : ex1, ex2 
        if (execv(cmd[0], cmd) == -1) {
        	fprintf(stderr, "execv() failed\n");
        	exit(EXIT_FAILURE);
    	}
    	
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
		if (wait == false) {
            fprintf(stderr, "Child [%d] in background\n", pid);
            // Use waitpid() with WNOHANG when not blocking during wait and  waitpid() with WUNTRACED when parent needs to block due to wait 
			if (waitpid(pid, &status_code, WNOHANG) == -1) {
        		fprintf(stderr, "Error waitpid, continue to accept\n");
        		exit(EXIT_FAILURE);
        	} else {
				waitpid(pid, &status_code, WNOHANG);
			}
        } else {
        	// else wait for the child process to exit 
        	if (waitpid(pid, &status_code, WUNTRACED) == -1) {
        		fprintf(stderr, "Error waitpid, wait for child\n");
        		exit(EXIT_FAILURE);
        	} else {
				waitpid(pid, &status_code, WUNTRACED);
			}
        	int exit_status = WEXITSTATUS(status_code);
            	pcbTable[index].status = 1;
            	pcbTable[index].exitCode = exit_status;
        }
        
    }
}



/*******************************************************************************
 * Command Processor
 ******************************************************************************/
static void command(char **cmd, size_t cmd_length) {

        /******* FILL IN THE CODE *******/
    
    // if command is "info" call command_info()             : ex1
    if (strcmp(cmd[0], "info") == 0) {
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
    		pid_t pid = atoi(cmd[1]);
    		command_wait(pid);
    	}
    
    } else if (strcmp(cmd[0], "terminate") == 0) {
    	// if command is "terminate" call command_terminate()   : ex2
    	if (cmd_length != 2) {
    		fprintf(stderr, "Wrong command\n");
    	} else {
    		pid_t pid = atoi(cmd[1]);
    		command_terminate(pid);
    	}
    } else if (strcmp(cmd[0], "fg") == 0) {
    	// if command is "fg" call command_fg()                 : ex4
    	if (cmd_length != 2) {
    		fprintf(stderr, "Wrong command\n");
    	} else {
    		//printf("Passing pid: %d to command_fg\n", atoi(cmd[1]));
    		pid_t pid = atoi(cmd[1]);
    		command_fg(pid);
    	}
    } else {
    	// call command_exec() for all other commands           : ex1, ex2, ex3
    	command_exec(cmd, cmd_length);
    }
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
		signal(SIGTSTP, signal_handler);
        // use signal() with SIGINT to setup a signalhandler for ctrl+c  : ex4
       	signal(SIGINT, signal_handler);

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
    printf("\nGoodbye\n");
}
