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
    // array of all of the arguments that can be passed in
    char *cmd;
    char input_file[100];
    char output_file[100];
    char *args[512];


    // Variables for the input
    char *line_cr = NULL;
    ssize_t line_input;
    size_t input_len = 2048;
    char *line;
    char *line_args;

    // Print the prompt and flush the output
    printf("%s ",":");
    fflush(stdout);

    // Read in the users input and copy it to line w/o carriage return
    line_input = getline(&line_cr, &input_len, stdin);
    line = malloc(line_input+1);
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
    int arg_pos = 0;
    size_t m_size = 100;

    // Booleans for checking if the cmd, inputs, outputs have been found in the input
    bool cmd_set = false;
    bool arg_set = false;
    bool in_set = false;
    bool out_set = false;

    while(line_pos < line_input) {
        if (cmd_set == false) {
            int spc_comp = strncmp(&line[line_pos]," ",1);
            if (spc_comp != 0) {
                cmd_pos ++;
            } else {
                cmd_set = true;
                cmd = malloc(cmd_pos+1+1);
                strncpy(cmd, line, cmd_pos);
                line_args = line + cmd_pos + 1;
            }
        }
        line_pos++;
    }
    printf("%s\n",line_args);
    printf("%s",cmd);
    printf("%s","-end");







    return 0;

}