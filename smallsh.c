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


int main(){
    // a bool for whether or not the shell should exit
    bool exit_cmd = false;

    // The built-in status int
    int status = 0;

    while(exit_cmd == false) {
        // Variables for the prompt - line is the whole readin line and can be 2048 chars long
        // Cmd is the command to be executed, input and output files, and args - which is an
        // array of all of the arguments that can be passed in
        // a bool for whether or not the command should run in the background
        char *cmd = NULL;
        char *input_file = NULL;
        char *output_file = NULL;
        char *args[512];
        bool background = false;

        // So that we can later on check if the cmd is sent without an argument
        args[0] = "\0";

        // Initialize the rest of the elements of args to NULL
        for (int i = 1; i < 512; ++i) {
            args[i] = NULL;
        }

        // Variables for the input
        char *line_cr = NULL;
        line_cr = (char *)malloc(2048*sizeof(char));

        // Print the prompt and flush the output
        printf(": ");
        fflush(stdout);

        // Read in the users input and copy it to line w/o carriage return
        fgets(line_cr, 2048, stdin);

        char *line = NULL;
        line = (char *)malloc((strlen(line_cr))*sizeof(char));
        memset(line, 0, strlen(line_cr));

        memcpy(line, line_cr, strlen(line_cr)-1);
        free(line_cr);
        int line_input = strlen(line);

        // This checks to see if the statement written is a comment
        // Also checks to see if the statement was a blank line
        // If so, skips the next 2 while loops
        bool blank_line = false;
        bool comment = false;
        bool cont = true;
        int comment_comp = strncmp(&line[0],"#",1);
        if (comment_comp == 0) {
            comment = true;
            cont = false;
        }

        if (strlen(line) == 0) {
            blank_line = true;
            cont = false;
        }

        // Line position checks the position of the char in the input string
        // Temp string is a holder for building the various parts of the input
        // m_size is for allocating memory for each arg put into the args array
        int line_pos = 0;
        int cmd_pos = 0;
        int temp_pos = 0;
        size_t m_size = 100;

        // Checks to see if an & is at the end of the line, and sets the background var
        // First creates a duplicate string to manipulate
        char *and_str = NULL;
        //and_str = malloc(strlen(line)+1);
        and_str = (char *)malloc((strlen(line)+1)*sizeof(char));
        memset(and_str, 0, strlen(line)+1);

        strcpy(and_str, line);
        and_str = line + strlen(line) - 2;
        
        // If it does end with & - then set background to true
        int and_comp_top = strncmp(and_str," &",3);
        if (and_comp_top == 0) {
            background = true;
        }

        // Booleans for checking if the cmd, inputs, outputs have been found in the input
        bool cmd_set = false;
        bool arg_set = false;
        bool in_set = false;
        bool out_set = false;

        // Loop to skip any leading spaces
        // If there is one, advance the pointer
        int lead_spc_comp = strncmp(&line[line_pos]," ",1);
        while(lead_spc_comp == 0) {
            line = line + 1;
            lead_spc_comp = strncmp(&line[line_pos]," ",1);
        }

        // Loop to identify the command
        while(cmd_set == false && line_pos < line_input && cont == true) {

            // Only have to check for a space, they signal the end of the command
                int spc_comp = strncmp(&line[line_pos]," ",1);
                if (spc_comp != 0 && (temp_pos + 1) < line_input) {
                    temp_pos ++;

                // Once the end of the command has been found, set the cmd bool to true
                // Allocate memory for cmd, copy over the string, and move the line pointer
                } else {

                    // If only a command is entered, so length of str is currently only
                    // +1 of temp_pos, increment temp_pos by 1
                    if (strlen(line) - 1 == temp_pos) {
                        temp_pos++;
                    }

                    cmd_set = true;
                    cmd = malloc(temp_pos+1);
                    memset(cmd, 0, temp_pos+1);
                    strncpy(cmd, line, temp_pos);
                    line = line + temp_pos;
                    temp_pos = 0;

                    // Finally we must put the cmd into the first arg of the array
                    args[0] = strdup(cmd);
                }
                line_pos++;
        }
       
        // In order to check if it is the end of the args, we need a temp
        // string that can check to see if the next 3 chars are " < ", " > ", and " &"
        // Temp arg and arg count are for allocating arguments to the array
        // Space cmp str is for identifying leading spaces before the rest of the arguments
        char inp_out_comp_str[4];
        char and_comp_str[3];
        char *temp_line;
        char *temp_arg = NULL;
        int arg_count = 1;
        char space_cmp_str[3];

        // Args, input, and output are all optional, the following
        // If statements identify which type the next set of statements are
        // and uses the a while loop to pull them out of line

        while(line_pos < line_input && cont == true) {

            // Within this loop, we need to first get rid of all leading spaces like before
            // We will do this by looking for double spaces - at which point we will move on
            strncpy(space_cmp_str, line, 2);
            int lead_other_spc_comp = strncmp(space_cmp_str,"  ",2);
            while(lead_other_spc_comp == 0) {
                line = line + 1;
                strncpy(space_cmp_str, line, 2);
                lead_other_spc_comp = strncmp(space_cmp_str,"  ",2);
            }

            // These assign the temp line variables
            temp_line = malloc(strlen(line)+1);
            strcpy(temp_line, line);
            temp_line = line + temp_pos;

            // Copy sub string over for checks
            strncpy(inp_out_comp_str, temp_line, 3);
            strncpy(and_comp_str, temp_line, 2);

            // Comparisons for checking the next 2-3 characters
            int inp_comp = strncmp(inp_out_comp_str," < ",3);
            int out_comp = strncmp(inp_out_comp_str," > ",3);
            int and_comp = strncmp(and_comp_str," &",2);

            if (inp_comp == 0) {

                // Move pointer up 3 slots
                line = line + 3;
                line_pos = line_pos + 3;

                bool lead_in_space = true;

                // Loop finds the location and puts it in a var
                while(in_set == false) {

                    // First we need another leading space loop to remove leading " "
                    // We only need to go through the loop once, hence setting lead_in_space to false
                    int lead_in_spc_comp = strncmp(&line[temp_pos]," ",1);
                    while(lead_in_spc_comp == 0 && lead_in_space == true) {
                        line = line + 1;
                        lead_in_spc_comp = strncmp(&line[temp_pos]," ",1);
                    }
                    lead_in_space = false;

                    // Only have to check for a space, they signal the end of the location
                    int spc_comp = strncmp(&line[temp_pos]," ",1);
                    if (spc_comp != 0) {
                        temp_pos ++;

                    // Once the end of the command has been found, set the input bool to true
                    // Allocate memory for input file, copy over the string, and move the line pointer
                    } else {
                        in_set = true;
                        input_file = malloc(temp_pos+1);
                        memset(input_file, 0, temp_pos+1);
                        strncpy(input_file, line, temp_pos);
                        line = line + temp_pos;
                        temp_pos = 0;
                        line_pos--;
                    }
                    line_pos++;
                }

            } else if (out_comp == 0) {
                
                // Move pointer up 3 slots
                line = line + 3;
                line_pos = line_pos + 3;

                bool lead_out_space = true;

                // Loop finds the location and puts it in a var
                while(out_set == false) {

                    // First we need another leading space loop to remove leading " "
                    // We only need to go through the loop once, hence setting lead_in_space to false
                    int lead_out_spc_comp = strncmp(&line[temp_pos]," ",1);
                    while(lead_out_spc_comp == 0 && lead_out_space == true) {
                        line = line + 1;
                        lead_out_spc_comp = strncmp(&line[temp_pos]," ",1);
                    }
                    lead_out_space = false;

                    // Only have to check for a space, they signal the end of the location
                    int spc_comp = strncmp(&line[temp_pos]," ",1);
                    if (spc_comp != 0) {
                        temp_pos++;
                    // Once the end of the command has been found, set the output bool to true
                    // Allocate memory for output file, copy over the string, and move the line pointer
                    } else {
                        out_set = true;
                        output_file = malloc(temp_pos+1);
                        memset(output_file, 0, temp_pos+1);
                        strncpy(output_file, line, temp_pos);
                        line = line + temp_pos;
                        temp_pos = 0;
                        line_pos--;
                    }
                    line_pos++;
                }

            // In the case that the input ends with &, this ends the loop
            } else if (and_comp == 0) {
                cont = false;
            
            // This final statement is for the arguments
            } else {

                // Move pointer up 1 slot
                line = line + 1;
                line_pos = line_pos + 1;

                // Loop finds the location and puts it in a var
                while(arg_set == false) {

                    // Only have to check for a space, they signal the end of the location
                    int spc_comp = strncmp(&line[temp_pos]," ",1);
                    if (spc_comp != 0) {
                        temp_pos ++;

                    // Once the end of the command has been found, set the arg bool to true
                    // Allocate memory for temp arg, copy over the string, put it in the array
                    // increment the arg counter and finally move the line pointer
                    } else {
                        arg_set = true;
                        temp_arg = malloc(temp_pos+1);
                        memset(temp_arg, 0, temp_pos+1);
                        strncpy(temp_arg, line, temp_pos);
                        args[arg_count] = strdup(temp_arg);
                        arg_count++;
                        line = line + temp_pos;
                        temp_pos = 0;
                        line_pos--;
                    }
                    line_pos++;
                }

            }

            // Reset the arg bool
            arg_set = false;
            line_pos++;
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

        if (cmd_set == true)
        {
            printf("%s->cmd\n",cmd);
        }
        if (in_set == true)
        {
            printf("%s->in\n",input_file);
        }
        if (out_set == true)
        {
            printf("%s->out\n",output_file);
        }

        for (int i = 0; i < arg_count; ++i)
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
            //printf(": %s\n",line);  

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
        printf("%s\n",getcwd(cwd, sizeof(cwd)));

        // This is the exit built in command - if exit is run, then call this
        // which ends the while loop
        } else if (exit_comp == 0) {
            exit_cmd = true;

        // This prints out the most recent status
        } else if (status_comp == 0) {
            // Print out the most recent value of status
            printf("%d\n",status);
        
        // If none of the commands were seen above, then the command is a linux cmd
        // We need to fork of a child and run the command
        } else {
            // pid_t spawnpid = -5;
            // // If fork is successful, the value of spawnpid will be 0 in the child, the child's pid in the parent
            // spawnpid = fork();
            // printf("I am a parent! %d\n",getpid());
            // if (fork() == 0) {
            //     printf("I am a child! %s\n",cmd);
            //     // int roger = execvp(cmd,args);
            //     int roger = execlp(cmd,"ls","-al",NULL);
            // } 

            // If fork is successful, the value of pid will be 0 and the command will be executed
            // Code adapted from class lectures
            int childStatus;
            pid_t childPid = fork();
            if(childPid == 0){

                // Child process running the exec command
                int roger = execvp(cmd,args);

            } else{

                // Parent process - waits for the child to finish
                waitpid(childPid, &childStatus, 0);
            
            }
            printf("%d\n",childStatus);

        }
    }

    
    
    return 0;

}