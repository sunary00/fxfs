#ifndef FXFS_FS_H
#define FXFS_FS_H

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define block_size 1024

typedef struct superblock
{
	char datablocks[block_size * 100]; // total number of data blocks
	char data_bitmap[105];			   // array of data block numbers that are available
	char inode_bitmap[105];			   // array of inode numbers that are available
} superblock;

typedef struct inode
{
	int datablocks[16]; // data block number that the inode points to
	int number;
	int blocks; //==number of blocks the particular inode points to
	// int link;                    //==number of links
	int size; //==size of file/directory
} inode;

typedef struct filetype
{
	int valid;
	char test[10];
	char path[100];
	char name[100];				// name
	inode *inum;				// inode number
	struct filetype **children; //??
	int num_children;
	int num_links;
	struct filetype *parent; //?
	char type[20];			 //==file extension
	mode_t permissions;		 // Permissions
	uid_t user_id;			 // userid
	gid_t group_id;			 // groupid
	time_t a_time;			 // Access time
	time_t m_time;			 // Modified time
	time_t c_time;			 // Status change time
	time_t b_time;			 // Creation time
	off_t size;				 // Size of the node

	int datablocks[16];
	int number;
	int blocks;

} filetype;


typedef struct fxfs_context {
	superblock* superblock;
	filetype*   root;
	filetype*   file_array;
} fxfs_context;

// superblock spblock;
// filetype * root;
// filetype file_array[50];

void initialize_superblock();
void tree_to_array(filetype *, int *, int *, int *);
int save_contents();

void initialize_root_directory();

filetype *filetype_from_path(char *);
int find_free_inode();
int find_free_db();

void add_child(filetype *, filetype *);

// FUSE operations
int mymkdir(const char *, mode_t);
int myreaddir(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
int mygetattr(const char *, struct stat *);

int myrmdir(const char *);
int myrm(const char *);

int mycreate(const char *, mode_t, struct fuse_file_info *);
int myopen(const char *, struct fuse_file_info *);
int myread(const char *, char *, size_t, off_t, struct fuse_file_info *);
int myaccess(const char *, int);
int myrename(const char *, const char *);
int mytruncate(const char *, off_t);

int mywrite(const char *, const char *, size_t, off_t, struct fuse_file_info *);


fxfs_context* fxfs_get_context();
void          fxfs_init_context();

#endif