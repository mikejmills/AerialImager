
#include "DirHandle.h"
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#ifdef __APPLE__
#include <sys/event.h>
#else
#include <sys/inotify.h>
#endif

#include "Preprocess.h"

//----------------------------------------------------------------------------------------
//#ifdef __APPLE__
//int datFileFilter(struct direct *ent)
//#else
int datFileFilter(const struct dirent *ent)
//#endif
{
    const char *str = ent->d_name + strlen(ent->d_name) - EXTEN_SIZE;
    
    if (strcmp(str, "dat") == 0) 
        return 1;
    return 0;
}


void *handle(void *arg)
{
    DirHandler *hdl = (DirHandler *)arg;
    hdl->DirEvent();
    
    return NULL;
}



//----------------------------------------------------------------------------------------

DirHandler::DirHandler(char *path)
{
    int len = strlen(path);
    
    // No path name longer then max
    if (len > (MAX_PATH_LENGTH - 2)) {
        pathname[0] = NULL;
        return;
    }
    
    //Make copy of pathname for internal use including \0
    strncpy(pathname, path, len + 1);
    
    // Ensure trailing slash
    if (pathname[len-1] != '/') {
        pathname[len] = '/';
        pathname[len+1] = '\0';
    }
    
    pthread_create(&Thread, NULL, &handle, (void *)this);
    pFilesCnt = 0;
    pFiles = NULL;
}

DirHandler::~DirHandler()
{
    //pthread_kill(Thread, 9);
    pthread_cancel(Thread);
}


char *DirHandler::Get_Path()
{
    return pathname;
}

#ifdef __APPLE__
void DirHandler::DirEvent()
{
    int f, kq, nev;
    
    struct kevent change;
    struct kevent event;
    
    kq = kqueue();
    if (kq == -1) goto Bail;
    
    f = open(pathname, O_RDONLY);
    if (f == -1) {
        close(kq);
        goto Bail;
    }
    
    EV_SET(&change, f, EVFILT_VNODE,
           EV_ADD | EV_ENABLE | EV_ONESHOT,
           //NOTE_DELETE | NOTE_EXTEND | NOTE_WRITE | NOTE_ATTRIB,
           NOTE_WRITE,// | NOTE_ATTRIB | NOTE_EXTEND,
           0, 0);
    
    for (;;) {
        nev = kevent(kq, &change, 1, &event, 1, NULL);
        if (nev == -1) {
            perror("kevent");
            break;
        }
        
        if (nev > 0) {
            if (event.fflags & NOTE_EXTEND || event.fflags & NOTE_WRITE) {
                new_Files();
            }
        }
    }
    return;
    
Bail:
    fprintf(stderr, "Error setting up directory checking\n");
    return;
    
}
#else
#define BUF_LEN 1024 * sizeof(struct inotify_event) + 16
void DirHandler::DirEvent()
{
    char buf[BUF_LEN];
    
    int  wd, inotfd;
    
    
    inotfd = inotify_init();
    if (inotfd < 0) goto Bail;
    
    wd = inotify_add_watch(inotfd, pathname, IN_CREATE);
    if (wd < 0) goto Bail;
    
    while(1) {
        //block and wait for event
        while ( read(inotfd, buf, BUF_LEN) ) { 
            new_Files();
        }
    }
    
    return;
    
Bail:
    perror("inotify setup Error");
    return;
    
}
#endif

int DirHandler::build_file_list(struct dirent ***l)
{
    return scandir(pathname, l, &datFileFilter, NULL);
}

void DirHandler::new_Files()
{
    sleep(1); // Give some copy operations time to finish before futher processing
    
    char fname[MAX_PATH_LENGTH];
    
    int nCount = build_file_list(&nFiles);
    int found = 0;
    
    // Check new list against the previous for new files
    for(int i = 0; i < nCount; i++) {
        found = 0;
        
        for (int j = 0; j < pFilesCnt; j++) {
            
            if ( strncmp(nFiles[i]->d_name, pFiles[j]->d_name, strlen(nFiles[i]->d_name)) == 0 ) {
                found = 1;
                break;
            }
        }
        
        if (!found) {
            // Build full path filename
            
            strncpy(fname, pathname, strlen(pathname));
            strncpy(fname + strlen(pathname), nFiles[i]->d_name, strlen(nFiles[i]->d_name));
            fname[strlen(pathname) + strlen(nFiles[i]->d_name)] = '\0';
            cout << fname << endl;
            // Do preprocessing of images
            Pre_Create(fname);
     
        }
    }
    
    // Free prev list if needed
    if (pFiles)
        free(pFiles);
    
    pFiles = nFiles;
    pFilesCnt = nCount;
    
    return;
}



