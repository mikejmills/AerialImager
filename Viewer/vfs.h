/* Doc- #################################
#######################################*/

#ifndef VFS
#define VFS

#include <fstream>
#include <fcntl.h>
#include "../include/TileObj.h"
#include <sys/stat.h>

// Initalize the VFS with a pathname
void VFS_Init(char *pathname);


// Write the TileFile to disk 
int VFS_WriteTileFile(char *filename,  TileFile tf);

// Read the Tile Files header info ( no images are read )
TileFile read_TileFile(char *filename);

// Read the Image of tile file at level
IplImage *VFS_ReadTileImage(char *filename, TileFile header, int level);

// tmp
unsigned char *read_comp_img(char *filename, unsigned char *offset, int size);

// Returns 1 if file exists already
int vfs_fexist( char *filename );

#endif

