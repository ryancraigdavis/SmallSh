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
	char cmd[100];
	char input_file[100];
	char output_file[100];
	char ground[1];
	char *args[512];

	// Variables for the input
	char *line = NULL;
	ssize_t line_input;
    size_t input_len = 2048;

    // Print the prompt and flush the output
	printf("%s ",":");
	fflush(stdout);

	// Read in the users input
    line_input = getline(&line, &input_len, stdin);

    // This checks to see if the statement written is a comment
    // If so, skips the next while loop

    bool comment = false;
    if (line[0] == "#") {
    	comment = true;
    }

    // Line position checks the position of the char in the input string
    // Temp string is a holder for building the various parts of the input
    // m_size is for allocating memory for each arg put into the args array
    if (comment == true) {
    	int line_pos = line.length();
    } else {
    	int line_pos = 0;
    }
    char temp_str[100];
    size_t m_size = 100;

    // Booleans for checking if the cmd, inputs, outputs have been found in the input
    bool cmd_set = false;
    bool in_set = false;
    bool out_set = false;

    do {
    	if (cmd_set == false) {
    		if (line[line_pos] != ' ') {
    			strcat(temp_str, line[line_pos]);
    		}
    	}
    	line_pos++;
    } while(line_pos < line_input);









	return 0;

}