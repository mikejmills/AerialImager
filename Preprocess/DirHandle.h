/* Doc- #################################
#######################################*/

#ifndef DIRHANDLE
#define DIRHANGLE

#include <sys/dir.h>
#include <iostream>
#include "../include/common.h"

using namespace std;


class DirHandler
    {
    public:
        DirHandler(char *path);
        ~DirHandler();
        char *Get_Path();
        void DirEvent();
        
    private:
        char   pathname[MAX_PATH_LENGTH];
        
        struct dirent **pFiles, **nFiles;
        int    pFilesCnt;
        
        pthread_t Thread;
        
        int  build_file_list(struct dirent ***l);
        void new_Files();

    };

#endif

