//
//  shell.h
//  CS232 Command Shell
//
//  Created by: David Yoo (gy24) 
//  Date: Mar 31, 2022.
//

#ifndef __CS232_SHELL_H__
#define __CS232_SHELL_H__

#include "commandline.h"
#include "path.h"
#include "prompt.h"
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

class Shell
{
public:
	Shell(){};
	void run();
private:
	Path path;
	Prompt pr;
	char* homeDir;	
    const char* dir;
    const char* cPath;
};

#endif