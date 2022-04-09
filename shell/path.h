//
//  path.h
//  CS232 Command Shell
//
//  Created by: David Yoo (gy24) 
//  Date: Mar 31, 2022.
//

#ifndef __CS232_Command_Shell__path__
#define __CS232_Command_Shell__path__

#include <string>
#include <vector>
#include <sstream>
#include <dirent.h>
#include <unistd.h>
using namespace std;

class Path{
    public:
        Path();
        int find(const string& program) const;
        string getDirectory(int i) const;

    private:
        vector<string> directories;


};

#endif /* defined(__CS232_Command_Shell__path__) */

