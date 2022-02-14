/*** 
 * This program is to copy the contentes of one file to another file.
 * Created by: David Yoo (gy24)
 * Date: 01/23/2022
***/


#include <stdio.h>
#include <stdlib.h> // For exit()
#include <unistd.h> // For access(), https://pubs.opengroup.org/onlinepubs/009695299/functions/access.html
#include <sys/stat.h> // For stat(), https://codeforwin.org/2018/03/c-program-find-file-properties-using-stat-function.html

int main(int argc, char *argv[]) // https://www.geeksforgeeks.org/command-line-arguments-in-c-cpp/
{
    FILE *src, *dest;
    // char filename[100], c;
    char c;
    char *srcfile = argv[1];
    char *destfile = argv[2];
    struct stat stats;

  
    /* src
     * Borrowed perror() code from https://www.ibm.com/docs/en/i/7.3?topic=functions-perror-print-error-message
     * Borrowed stat() code from https://codeforwin.org/2018/03/c-program-find-file-properties-using-stat-function.html
     *                           https://www.ibm.com/docs/en/i/7.4?topic=ssw_ibm_i_74/apis/stat.htm
     * S_ISREG() from https://stackoverflow.com/questions/4553012/checking-if-a-file-is-a-directory-or-just-a-file
    */
    if (stat(srcfile, &stats) == 0 && S_ISREG(stats.st_mode)) // Check if the source file exist, and check if the file is a regular file
    {
        src = fopen(srcfile, "r");
    }
    else
    {
        perror("Error: stat() ");
        fprintf(stderr, "%s does not exist, or it is not a regular file, please check it again.", srcfile);
        exit(-1);
    }
  
   
    /* dest
    * Borrowed access() code from https://www.geeksforgeeks.org/access-command-in-linux-with-examples/
    */
    int fd = access (destfile, F_OK); // Check if the destination file exist
    if (fd != -1)
    {
        perror("Error: access() ");
        fprintf(stderr, "%s already exist, please check it again.", destfile);
        exit(-1);
    }
    
    dest = fopen(destfile, "w");
    

    // Used fgets() code from https://www.tutorialspoint.com/c-program-to-copy-the-contents-of-one-file-to-another-file
    c = fgetc(src);
    while (c != EOF)
    {
        fputc(c, dest);
        c = fgetc(src);
    }
  
    fclose(src);
    fclose(dest);

    printf("\nFile copied.\n");
  

    return 0;
}

