Bash Shell written in C

The shell runs command line instructions and return the results similar to other shells.

SmallSh works like the bash shell prompting for a command line and running commands.
The shell allows for the redirection of standard input and standard output and it supports both foreground and background processes (controllable by the command line and by receiving signals).

Supports three built in commands: exit, cd, and status - as well as running all other system commands by forking a child process. 

It also support comments, which are lines beginning with the # character.
