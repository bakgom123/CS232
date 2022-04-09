//
//  path.cpp
//  CS232 Command Shell
//
//  This is a class to represent the directories in which your shell searches for executable programs, with public methods
//  Created by: David Yoo (gy24) 
//  Date: Mar 31, 2022.
//


#include "path.h"
using namespace std;

// This constructs a Path from the PATH environment variable, by storing each directory in PATH in a vector<string>
// stringstream: https://www.geeksforgeeks.org/stringstream-c-applications/
// getenv(): https://www.tutorialspoint.com/c_standard_library/c_function_getenv.htm
//           https://www.ibm.com/docs/en/i/7.2?topic=functions-getenv-search-environment-variables
Path::Path(){
    string p = getenv("PATH");
    // use stringstream to read from the string like cin
    stringstream ss(p);
    string directory;
    // since PATH comes with colon, split them and assign them to the vector
    while(getline(ss, directory, ':')){
        directories.push_back(directory);
    }
}
	
// This returns the index of the directory containing program, or -1 if program is in no directory
//https://man7.org/linux/man-pages/man3/opendir.3.html
// struct dirent *ent; https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
// d_name; http://www.cplusplus.com/forum/beginner/11770/
int Path::find(const string& program) const{
    int index = -1;
    DIR *dir;
    struct dirent *ent;
    for(unsigned i = 0; i < directories.size(); i++){
        dir = opendir(directories[i].c_str());
		if(dir == nullptr)
			continue;
        // check all the files and directories within directory, then returns the index of the directory
        while((ent = readdir(dir)) != nullptr){
            if(ent -> d_name == program){
                index = i;
                return index;
            }
        }
    }
    closedir(dir);
    return -1;
}

// This returns the name of the directory whose index is i.
string Path::getDirectory(int i) const {
    return directories[i];
}

