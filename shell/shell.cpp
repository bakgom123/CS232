//
//  shell.cpp
//  CS232 Command Shell
//  This is a class that runs the show. It contains the built in commands (cd, pwd, and exit)
//
//  Created by: David Yoo (gy24) 
//  Date: Mar 31, 2022.
//

#include "shell.h"

void Shell::run(){
    // runs a forever loop
	while (true){
        // display the prompt
	    Prompt pr;
        cout << pr.get() << flush;
        // wait for the user to type in their command
		CommandLine c(cin);
        char* command = c.getCommand();

        // built in commands 
        // strcmp(): https://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
        int index = path.find(command);
        if (index == -1){
            // cd
            if (strcmp(command, "cd") == 0){
                // if the user only enter "cd", goes to home directory
                if (c.getArgCount() == 1){
                    dir = getenv("HOME");
                }
                // otherwise, go to the directory that the user typed in
                else {
                    dir = c.getArgVector(1);
                }
                chdir(dir); 
            }
            // exit
            else if (strcmp(command, "exit") == 0){
                exit(0); 
            }
            // pwd, gives the full path name of the current directory
            else if (strcmp(command, "pwd") == 0){
                cout << pr.getCwd() << endl;
            }  
        }
        else {
            // handle everything else by finding in the path the program the user wants to execute and using fork/exec to run it.
            // fork(): https://www.delftstack.com/howto/cpp/cpp-fork/
            string cPath = path.getDirectory(index) + '/' + command;
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                execve(cPath.c_str(), c.getArgVector(), NULL);
                perror(command);
            }
            else{
                int status;
                pid_t wait = waitpid(pid, &status, 0);
                if (wait < 0){
                    perror("waitpid");
                }
            }
        }
	}
}




