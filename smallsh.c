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
    char input_file[100];
    char output_file[100];
    char *args[512];
    bool background = false;


    // Variables for the input
    char *line_cr = NULL;
    ssize_t line_input;
    size_t input_len = 2048;
    char *line;

    // Print the prompt and flush the output
    printf("%s ",":");
    fflush(stdout);

    // Read in the users input and copy it to line w/o carriage return
    line_input = getline(&line_cr, &input_len, stdin);
    line = malloc(line_input+2);
    strncpy(line, line_cr, strlen(line_cr)-1);

    // This checks to see if the statement written is a comment
    // If so, skips the next while loop
    bool comment = false;
    int comment_comp = strncmp(&line[0],"#",1);
    if (comment_comp == 0) {
        comment = true;
    }

    // Line position checks the position of the char in the input string
    // If comment is true, then line pos set to the length of the input line
    // Temp string is a holder for building the various parts of the input
    // m_size is for allocating memory for each arg put into the args array
    int line_pos = 0;
    if (comment == true) {
        line_pos = strlen(line);
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
    bool cont = true;

    // Loop to identify the command
    while(cmd_set == false) {
        // Only have to check for a space, they signal the end of the command
            int spc_comp = strncmp(&line[line_pos]," ",1);
            if (spc_comp != 0) {
                temp_pos ++;

            // Once the end of the command has been found, set the cmd bool to true
            // Allocate memory for cmd, copy over the string, and move the line pointer
            } else {
                cmd_set = true;
                cmd = malloc(temp_pos+1+1);
                strncpy(cmd, line, temp_pos);
                line = line + temp_pos;
                temp_pos = 0;
            }
            line_pos++;
    }

    // Args, input, and output are all optional, the following
    // If statements identify which type the next set of statements are
    // and uses the a while loop to pull them out of line

    while(line_pos < line_input && cont == true) {

        // In order to check if it is the end of the args, we need a temp
        // string that can check to see if the next 3 chars are " < ", " > ", and " &"
        char inp_out_comp_str[4];
        char and_comp_str[3];
        char *temp_line;
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

        line_pos++;
        printf("%s\n",inp_out_comp_str);
    }

    printf("%s\n",line);
    printf("%s",cmd);
    printf("%s","-end");

    // while(line_pos < line_input) {
    //     if (cmd_set == false) {







    //         // Only have to check for spaces, they signal the end of the command
    //         int spc_comp = strncmp(&line[line_pos]," ",1);
    //         if (spc_comp != 0) {
    //             temp_pos ++;

    //         // Once the end of the command has been found, set the cmd bool to true
    //      // Allocate memory for cmd, copy over the string, and move the line pointer
    //         } else {
    //             cmd_set = true;
    //             cmd = malloc(temp_pos+1+1);
    //             strncpy(cmd, line, temp_pos);
    //             line = line + temp_pos + 1;
    //             temp_pos = 0;
    //         }

    //  // For args, must check for the space and the < char
    //  // Because the < could be used in a name, it must be surrounded
    //     // by spaces to signify it is the end of args and beginning of input
    //     } else if (arg_set == false) {
            
    //         // In order to check if it is the end of the args, we need a temp
    //         // string that can check to see if the next 3 chars are " < "
    //         char inp_comp_str[4];
    //         char *temp_line;
    //         temp_line = malloc(strlen(line)+1);
    //         temp_line = line + temp_pos;

    //         strncpy(inp_comp_str, temp_line,temp_pos);
    //         int inp_comp = strncmp(inp_comp_str," < ",3);
    //         int spc_comp = strncmp(&line[line_pos]," ",1);

    //         if (spc_comp != 0 && inp_comp != 0) {
    //             temp_pos ++;
    //         } else {
    //             cmd_set = true;
    //             cmd = malloc(cmd_pos+1+1);
    //             strncpy(cmd, line, cmd_pos);
    //             line_args = line + cmd_pos + 1;
    //             line = line + cmd_pos + 1;
    //         }
            
    //     }
    //     line_pos++;
    // }
    







    return 0;

}