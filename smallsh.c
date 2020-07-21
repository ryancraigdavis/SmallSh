// daviryan@oregonstate.edu
// SmallSh - Bash Shell Project

#define  _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <dirent.h>
#include <limits.h>
#include <signal.h>

// Signal Handler for Sig Stop - adapted from the lecture
void int_handle(int sig_int){
    char get_pid[15];
    sprintf(get_pid, "%d", getpid());
    char* message = "Caught SIGINT, sleeping for 10 seconds\n";
  // We are using write rather than printf
    write(STDOUT_FILENO, get_pid, 15);
}


int main(){
    struct sigaction SIGINT_action;
    SIGINT_action.sa_handler = int_handle;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;

    // a bool for whether or not the shell should exit
    bool exit_cmd = false;

    // The built-in status int
    int status = 0;

    while(exit_cmd == false) {
        sigaction(SIGINT, &SIGINT_action, NULL);
        // Variables for the prompt - line is the whole readin line and can be 2048 chars long
        // Cmd is the command to be executed, input and output files, and args - which is an
        // array of all of the arguments that can be passed in
        // a bool for whether or not the command should run in the background
        char *cmd = NULL;
        char *input_file = NULL;
        char *output_file = NULL;
        char *args[512];
        char *line_args[512];

        // Counters for the loops that delimit the input and insert args
        int main_count = 0;
        int args_count = 0;
        int in_int = 0;
        int out_int = 0;
        int and_int = 0;

        // Booleans that tell whether an arg exists
        bool cmd_set = false;
        bool no_args = false;
        bool in_set = false;
        bool out_set = false;
        bool and_set = false;
        bool comment = false;
        bool blank_line = false;

        // Variables for the input
        char *line_cr = NULL;
        line_cr = (char *)malloc(2048*sizeof(char));
        
        printf(": ");
        fflush(stdout);

        // Read in the users input and copy it to line w/o carriage return
        fgets(line_cr, 2048, stdin);
        
        // Get the PID
        char get_pid[15];
        sprintf(get_pid, "%d", getpid());

        // Initialize
        char *line = NULL;
        line = (char *)malloc((strlen(line_cr))*sizeof(char));
        memset(line, 0, strlen(line_cr));
        memcpy(line, line_cr, strlen(line_cr)-1);

        // Checking to see if a comment was sent
        int comment_comp = strncmp(&line[0],"#",1);
        if (comment_comp == 0) {
            comment = true;
        }

        // Checking to see if the input is a blank line
        if (strlen(line) == 0) {
            blank_line = true;
        }

        // This loop goes through each argument separated by spaces
        // and puts it into the line args array making notes of the position
        // for assigning the args later
        char* token = strtok(line, " ");
        while(token != NULL && comment == false && blank_line == false) {
            line_args[main_count] = strdup(token);

            // These are the symbol comparisons
            // Input
            int in_cmp = strcmp(token,"<");
            if(in_cmp == 0){
                in_int = main_count;
                in_set = true;
                if (in_int  == 1) {
                    no_args = true;
                }
            }

            // Output
            int out_cmp = strcmp(token,">");
            if(out_cmp == 0){
                out_int = main_count;
                out_set = true;
                if (out_int  == 1) {
                    no_args = true;
                }
            }

            // & for background processes
            int and_cmp = strcmp(token,"&");
            if(and_cmp == 0){
                and_int = main_count;
                and_set = true;
                if (and_int == 1) {
                    no_args = true;
                }
            }

            // Increment main count and move to next delimiter
            main_count++;
            token = strtok(NULL, " ");
        }

        // Final check to see if there are args after the command
        if (main_count == 1) {
            no_args = true;
        }

        // Now go through and insert the line args into their various variables
        for (int i = 0; i < main_count; ++i) {
            if (i == 0) {

                // First line_arg is the command, copy it to the array
                cmd = (char *)malloc((strlen(line_args[i])+1)*sizeof(char));
                memset(cmd, 0, strlen(line_args[i])+1);
                strncpy(cmd, line_args[i], strlen(line_args[i]));
                cmd_set = true;
                
                // Copy it to the first arg spot as well and increase arg count
                args[i] = (char *)malloc((strlen(line_args[i])+1)*sizeof(char));
                memset(args[i], 0, strlen(line_args[i])+1);
                args[i] = strdup(line_args[i]);
                args_count++;

            // If i is the input int then the next line arg is the input file
            // Also no_args is set to true as there are no more arguments       
            } else if (i == in_int) {
                input_file = (char *)malloc((strlen(line_args[i+1])+1)*sizeof(char));
                memset(input_file, 0, strlen(line_args[i+1])+1);
                strncpy(input_file, line_args[i+1], strlen(line_args[i+1]));
                no_args = true;

            // If i is the output int then the next line arg is the output file
            // Also no_args is set to true as there are no more arguments   
            } else if (i == out_int) {
                output_file = (char *)malloc((strlen(line_args[i+1])+1)*sizeof(char));
                memset(output_file, 0, strlen(line_args[i+1])+1);
                strncpy(output_file, line_args[i+1], strlen(line_args[i+1]));
                no_args = true;

            // If no_args is false, then there are still arguments to copy over to the array
            } else if (no_args == false) {
                args[i] = (char *)malloc((strlen(line_args[i])+1)*sizeof(char));
                memset(args[i], 0, strlen(line_args[i])+1);
                args[i] = strdup(line_args[i]);
                args_count++;
            }

        }

        // The next slot in the args array must be NULL
        args[args_count] = NULL;
 

        // These statements initialize the line vars if they weren't already
        if (cmd == NULL) {
            cmd = malloc(2);
            memset(cmd, 0, 2);
        }
        if (input_file == NULL) {
            input_file = malloc(2);
            memset(input_file, 0, 2);
        }
        if (output_file == NULL) {
            output_file = malloc(2);
            memset(output_file, 0, 2);
        }

        if (cmd_set == true) {
            printf("%s->cmd\n",cmd);
        }
        if (in_set == true) {
            printf("%s->in\n",input_file);
        }
        if (out_set == true) {
            printf("%s->out\n",output_file);
        }

        for (int i = 0; i < args_count; ++i)
        {
            printf("%s->arg\n",args[i]);
        }

        // This checks to see what command was sent
        // First looking at comments and built-in commands
        int cd_comp = strcmp(cmd,"cd");
        int status_comp = strcmp(cmd,"status");
        int exit_comp = strcmp(cmd,"exit");
        
        // First check to see if it is a comment or blank line - then print it out
        if (comment == true || blank_line == true) {

            // In this case do nothing 

        // Next check to see if it was the "cd" command
        } else if (cd_comp == 0) {

            // Used this answer to understand getcwd
            // https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
            char cwd[PATH_MAX];

            // If no arguments were sent, change PWD to HOME
            // Other wise set it to arg
            // int arg_comp = strcmp(args[1],"\0");
            // if (arg_comp == 0) {
            if (args[1] == NULL) {
                chdir(getenv("HOME"));
            } else {

                // If the arg starts with a period, then the relative command is formatted
                // and we simply must pass it through
                int cd_rel_comp = strncmp(args[1],".",1);
                int abs_rel_comp = strncmp(args[1],"/",1);
                if (cd_rel_comp == 0) {
                    chdir(args[1]);

                // If it doesn't start with a period, we need to check to see if it is
                // an absolute path, which should start with a "/"
                } else if (abs_rel_comp == 0) {
                    chdir(args[1]);

                // Otherwise, it is a relative path but we need to put in the "./"
                // Then we can change the directory
                } else {

                    // New var relative_dir - we will concatenate args[0] to "./"
                    // Then free the memory
                    char *relative_dir = NULL;
                    relative_dir = (char *)malloc((strlen(args[1])+3)*sizeof(char));
                    memset(relative_dir, 0, strlen(args[1])+3);
                    memcpy(relative_dir,"./",2);
                    strcat(relative_dir,args[1]);
                    chdir(relative_dir);
                    free(relative_dir);
                }
            }

        // This is the exit built in command - if exit is run, then call this
        // which ends the while loop
        } else if (exit_comp == 0) {
            exit_cmd = true;

        // This prints out the most recent status
        } else if (status_comp == 0) {
            // Print out the most recent value of status
            printf("Exit value %d\n",status);
        
        // If none of the commands were seen above, then the command is a linux cmd
        // We need to fork a child and run the command
        } else {

            // If fork is successful, the value of pid will be 0 and the command will be executed
            pid_t childPid = fork();
            if(childPid == 0){
                // Putting the input file into standard input
                if (in_set == true) {
                    int input_file_int = open(input_file,O_RDONLY);
                    printf("%s\n","huh");
                    int in_check = dup2(input_file_int, 1);

                    //If the dup2 failed, set status to 1
                    if (in_check == -1) {
                        printf("%s\n","didnt work");
                        status = 1;
                    }
                }

                // Putting the output file into standard output
                if (out_set == true) {
                    int output_file_int = open(output_file,O_WRONLY);
                    printf("%s\n","huh");
                    int out_check = dup2(output_file_int, 1);
                    
                    // If the dup2 failed, set status to 1
                    if (out_check == -1) {
                        printf("%s\n","didnt work");
                        status = 1;
                        printf("%d\n",status);
                    }
                }


                // Child process running the exec command
                int cmd_status = execvp(cmd,args);
                

                // If cmd_status is -1, that's because a bad or non-existant command was sent
                // Terminates the process
                if (cmd_status == -1) {
                    kill(getpid(),SIGTERM);
                }

            } else{

                // Parent process - waits for the child to finish
                int childStatus;
                printf("%d child pid\n",childPid);
                waitpid(childPid, &childStatus, 0);

                // If the command finishes successfully, status is 0
                if (WIFEXITED(childStatus)) {
                    status = WEXITSTATUS(childStatus);

                //Print an error message and set status to 1
                } else {
                    printf("%s: no such file or directory\n",cmd);
                    status = 1;
                }
                
            
            }

        }
    }

    
    
    return 0;

}