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


bool and_signal_ignore = false;

// Signal Handler for Sig Stop - adapted from the lecture
void int_handler(int sig_int){
    char get_pid[15];
    char str_sig[2];
    sprintf(get_pid, "%d", getpid());
    sprintf(str_sig, "%d", sig_int);

    // in the child ctr c should do what it usually do
    // in the parent we want it to ignore

    //char* message = "SIGINT Test \n";
  // We are using write rather than printf
    write(STDOUT_FILENO, get_pid, strlen(get_pid));
    write(STDOUT_FILENO, str_sig, strlen(get_pid));
}

void bg_handler(int sig_tstp){

    if (and_signal_ignore == false) {
        char* message = "\n Entering foreground-only mode (& is now ignored)\n";
        and_signal_ignore = true;

        // We are using write rather than printf
        write(STDOUT_FILENO, message, strlen(message));

    // If the bash is already in     
    } else {
        char* message = "\n Exiting foreground-only mode\n";
        and_signal_ignore = false;

        // We are using write rather than printf
        write(STDOUT_FILENO, message, strlen(message));
    }

  
}



int main(){
    struct sigaction SIGINT_action;
    SIGINT_action.sa_handler = int_handler;
    sigfillset(&SIGINT_action.sa_mask);
    SIGINT_action.sa_flags = 0;

    struct sigaction SIGSTP_struct;
    SIGSTP_struct.sa_handler = bg_handler;
    sigfillset(&SIGSTP_struct.sa_mask);
    SIGSTP_struct.sa_flags = 0;

    struct sigaction SIGSTP_ign_struct;
    SIGSTP_ign_struct.sa_handler = SIG_IGN;
    sigfillset(&SIGSTP_ign_struct.sa_mask);
    SIGSTP_ign_struct.sa_flags = 0;

    sigaction(SIGINT, &SIGINT_action, NULL);
    sigaction(SIGTSTP, &SIGSTP_struct, NULL);

    // a bool for whether or not the shell should exit
    bool exit_cmd = false;

    // The built-in status int
    int status = 0;

    while(exit_cmd == false) {
        
        //sigaction(SIGTSTP, &SIGSTP_ign_struct, NULL);
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

        // Initialize line and dollar check line
        char *doll_line = NULL;
        doll_line = (char *)malloc((strlen(line_cr))*sizeof(char));
        memset(doll_line, 0, strlen(line_cr));
        memcpy(doll_line, line_cr, strlen(line_cr)-1);

        // We need to first go through the line and see if there are any 
        // "$$" that need to be converted to the PID
        int dollar_pid_count = 0;
        int dollar_pos = 0;
        while(dollar_pos < strlen(doll_line)) {
            int convert_pid = strncmp(&doll_line[dollar_pos],"$$",2);
            if (convert_pid == 0) {
                dollar_pid_count++;
            }
            dollar_pos++;
        }

        // Multiply the number of instances of $$ with the len of the pid
        long pid_length = dollar_pid_count * strlen(get_pid);
        dollar_pos = 0;

        // Now create the line var with this extended length
        char *line = NULL;
        line = (char *)malloc((strlen(line_cr)+pid_length)*sizeof(char));
        memset(line, 0, strlen(line_cr)+pid_length);

        // If dollar_pid is < 1, that means no $$ were found, thus just copy over the
        // input from line_cr
        if (dollar_pid_count < 1) {
            memcpy(line, line_cr, strlen(line_cr)-1);

        } else {

            // Start with a blank line
            strcpy(line,"");

            // For loop goes through each iteration of "$$"
            for (int i = 0; i < dollar_pid_count; ++i) {
                while(dollar_pos < strlen(doll_line)){

                    // While loop goes through looking for $$, then concatenates
                    // the string with the
                    int convert_pid = strncmp(&doll_line[dollar_pos],"$$",2);
                    if (convert_pid == 0) {
                        strncat(line, doll_line, dollar_pos);
                        strcat(line, get_pid);
                        doll_line = doll_line + dollar_pos + 2;
                        printf("%s\n",doll_line);
                    }
                    dollar_pos++;
                }
                dollar_pos = 0; 
            }

            // Finally concatenate the rest of the string if it isn't null
            if (strlen(doll_line) != 0) {
                strcat(line, doll_line);
            }
        }

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

        // If the & was given - make that NULL as well
        // The next slot in the args array must be NULL
        if (and_set == true && args_count > 1) {
            args[args_count-1] = NULL;
        } else {
            args[args_count] = NULL;
        }

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

                sigaction(SIGTSTP, &SIGSTP_ign_struct, NULL);

                // Putting the input file into standard input
                if (in_set == true) {

                    // Open source file
                    int source_file = open(input_file, O_RDONLY);
                    if (source_file == -1) { 
                        printf("cannot open %s for input\n",input_file);
                        fflush(stdout);
                        status = 1;
                        exit(1);
                    }

                    // Redirect stdin to source file
                    int source_result = dup2(source_file, 0);
                    if (source_result == -1) { 
                        printf("cannot open %s for input\n",input_file);
                        fflush(stdout);
                        status = 1;
                        exit(2);
                    }
                }

                // Putting the output file into standard output
                if (out_set == true) {

                    // Open destination file
                    int dest_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (dest_file == -1) { 
                        printf("cannot open %s for output\n",output_file);
                        fflush(stdout);
                        status = 1;
                        exit(1);
                    }

                    // Redirect stdout to source file
                    int dest_result = dup2(dest_file, 1);
                    if (dest_result == -1) { 
                        printf("cannot open %s for output\n",output_file);
                        fflush(stdout);
                        status = 1;
                        exit(2);
                    }
                }


                // Child process running the exec command
                int cmd_status = execvp(cmd,args);

                // If cmd_status is -1, that's because a bad or non-existant command was sent
                // Terminates the process
                if (cmd_status == -1) {
                    kill(getpid(),SIGTERM);
                }
            }

            // If the & was included, this will now run in the background
            if (and_set == true && and_signal_ignore == false) {
                printf("background pid is %d\n",childPid);
                fflush(stdout);
                int bgChildStatus;
                waitpid(childPid, &bgChildStatus, WNOHANG);
                if (WIFEXITED(bgChildStatus)) {
                    int bgStatus = WEXITSTATUS(bgChildStatus);
                //Print an error message and set status to 1

                // need to redirect stdin and stdout
                }

            // If no & was included, parent will wait for completion
            } else{

                // Parent process - waits for the child to finish
                int childStatus;
                waitpid(childPid, &childStatus, 0);

                // If the command finishes successfully, status is 0
                if (WIFEXITED(childStatus)) {
                    status = WEXITSTATUS(childStatus);

                //Print an error message and set status to 1
                } else {
                    printf("%s: no such file or directory\n",cmd);
                    fflush(stdout);
                    status = 1;
                    //printf("Child %d exited abnormally due to signal %d\n", childPid, WTERMSIG(childStatus));
                }
                
            
            }

        }
    }

    
    
    return 0;

}