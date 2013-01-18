
#include "vfs.h"

char PathName[MAX_PATH_LENGTH];
int  PathNamelen;

void VFS_Init(char *pathname)
{
    PathNamelen = strlen(pathname);
    
    // Copy Pathname to Global 
    strncpy(PathName, pathname, PathNamelen);
    
    // Make sure of trailing slash
    if (PathName[PathNamelen - 1] != '/') {
        PathName[PathNamelen++] = '/';        
    }
    PathName[PathNamelen] = '\0';
    cout << PathName << endl;
}



int VFS_WriteTileFile(char *filename, TileFile tf)
{
    int fd, size = INT(tf, MAIN_HEADER_total_size) + TILE_HEADER_SIZE;
    
    if ( (fd = open(filename, O_WRONLY | O_CREAT, S_IRWXU)) == -1 ) {
        perror("Writing ");
        return NULL;
    }
    
    while (size > 0) {
        size -= write(fd, tf, size);
    }
    
    close(fd);
    
    
    return 1;
}

TileFile read_TileFile(char *filename)
{
    int fd, rd = 0, size = MAIN_HEADER_total_size;
    DYNAMIC_TILE(tf);
    
    if ( (fd = open(filename, O_RDONLY)) == -1 ) {
        perror("Reading ");
        return NULL;
    }
    
    while (size > rd) {
        rd += read(fd, tf + rd, size - rd);
    }
    
    close(fd);
    
    return tf;
}

unsigned char *read_comp_img(char *filename, unsigned int offset, int size)
{
    int fd;
    
    unsigned char *ptr = (unsigned char *) malloc(size);
    if ( !ptr )
        return NULL;
    
    if ( (fd = open(filename, O_RDONLY)) == -1 ) {
        fprintf(stderr, "Cannot open file\n");
        
        free(ptr);
        return NULL;
    }
    
    if (lseek(fd, (off_t)offset, SEEK_CUR) == -1) {
        fprintf(stderr, "Cannot seek to image location\n");
        close(fd);
        free(ptr);
        return NULL;
    }
    
    int rd = 0;
    while (size > rd) {
        rd += read(fd, ptr + rd, size - rd);
    } 
    
    
    close(fd);
    
    return ptr;
}


//int VFS_ReadTileData(char *filename)
IplImage *VFS_ReadTileImage(char *filename, TileFile header, int level)
{
    
    IplImage *dst;
    
    unsigned char *comp = read_comp_img(filename, 
                                      INT(header, IMAGE_HEADER_offset(level)) + TILE_HEADER_SIZE, 
                                      INT(header, IMAGE_HEADER_compressed_size(level)) );
    
    if (!comp) return NULL;
    
    
    dst = cvCreateImage( cvSize(INT(header, IMAGE_HEADER_width(level)), INT(header, IMAGE_HEADER_height(level))), 
                         INT(header, IMAGE_HEADER_depth(level)),
                         INT(header, IMAGE_HEADER_nChannels(level)) );
    
    
    decompress_image( dst,  INT(header, IMAGE_HEADER_image_size(level)), 
                      comp, INT(header, IMAGE_HEADER_compressed_size(level)) );
   
    free(comp);
    
    return dst;
    
}

int vfs_fexist( char *filename ) {
    struct stat buffer ;
    
    if ( stat( filename, &buffer ) == 0 ) return 1 ;
    return 0 ;
}


