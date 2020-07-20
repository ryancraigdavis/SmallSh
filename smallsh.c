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


int main(){
    // Variables for the prompt - line is the whole readin line and can be 2048 chars long
    // Cmd is the command to be executed, input and output files, and args - which is an
    // array of all of the arguments that can be passed in and a bool for whether or not
    // the command should run in the background
    char *cmd;
    char *input_file;
    char *output_file;
    char *args[512];
    bool background = false;

    // So that we can later on check if the cmd is sent without an argument
    args[0] = "\0";


    // Variables for the input
    char *line_cr;
    char *line;
    ssize_t line_input;
    size_t input_len = 2048;
    line_cr = (char *) malloc (input_len + 1);

    // Print the prompt and flush the output
    printf(": ");
    fflush(stdout);

    // Read in the users input and copy it to line w/o carriage return
    line_input = getline(&line_cr, &input_len, stdin);
    line = malloc(line_input);
    printf("%lu\n",strlen(line_cr));
    printf("%zd\n",line_input);
    memcpy(line, line_cr, strlen(line_cr)-1);
    free(line_cr);

    // // Variables for the input
    // char line[2048];

    // // Print the prompt and flush the output
    // printf(": ");
    // fflush(stdout);

    // // Read in the users input and copy it to line w/o carriage return
    // fgets(line, 2048, stdin);
    // printf("%lu\n",strlen(line));

    // This checks to see if the statement written is a comment
    // If so, skips the next while loop
    bool comment = false;
    bool cont = true;
    int comment_comp = strncmp(&line[0],"#",1);
    if (comment_comp == 0) {
        comment = true;
    }

    // Line position checks the position of the char in the input string
    // If comment is true, then set continue var to false to skip the loop
    // Temp string is a holder for building the various parts of the input
    // m_size is for allocating memory for each arg put into the args array
    int line_pos = 0;
    if (comment == true) {
        cont = false;
    }
    int cmd_pos = 0;
    int temp_pos = 0;
    size_t m_size = 100;

    // Checks to see if an & is at the end of the line, and sets the background var
    // First creates a duplicate string to manipulate
    char *and_str;
    and_str = malloc(strlen(line)+1);
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
    while(cmd_set == false && line_pos < line_input) {

        // Only have to check for a space, they signal the end of the command
            int spc_comp = strncmp(&line[line_pos]," ",1);
            if (spc_comp != 0 && (temp_pos + 1) < line_input) {
                temp_pos ++;

            // Once the end of the command has been found, set the cmd bool to true
            // Allocate memory for cmd, copy over the string, and move the line pointer
            } else {
                cmd_set = true;
                cmd = malloc(temp_pos+1);
                strncpy(cmd, line, temp_pos);
                line = line + temp_pos;
                temp_pos = 0;
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
    char *temp_arg;
    int arg_count = 0;
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

    printf("%s->cmd\n",cmd);
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

    // // This checks to see if the "cd" command was sent
    // int cd_comp = strcmp(cmd,"cd");
    // if (cd_comp == 0) {

    //     // If no arguments were sent, change PWD to HOME
    //     // Other wise set it to arg
    //     int arg_comp = strcmp(arg[0],"\0");
    //     if (arg_comp == 0) {
    //         chdir(getenv("HOME"));
    //     } else {
    //         // code to change to relative
    //     }
    // }
    
    return 0;

}