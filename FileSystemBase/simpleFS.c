
#include "simpleFS.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <limits.h>
 /**
 * blar blar
 */
// Initialize a filesystem of the size specified in number of data blocks
void init_filesystem(unsigned int size, char *real_path, unsigned int n)
{
     //fp = fopen(real_path);
     if (size < 0) {
         exit(0);
         return;
     }
     if (size > 512) {
        exit(0);
        return;
     }
     struct directory_entry same;
     struct directory_entry parent;
     int x = 0;
     for (x = 0; x < 57; ++x) {
        same.d_name[x] = 0;
        parent.d_name[x] = 0;
     }
     same.d_inode = 2;
     same.d_file_type = 2;
     same.d_name_len = 1;
     strcpy(same.d_name, ".");
     parent.d_inode = 2;
     parent.d_file_type = 2;
     parent.d_name_len = 2;
     strcpy(parent.d_name, "..");
     int inodetablesize = 5 * 512;
     int inodestructsize = sizeof(struct inode);
     int inodeNo = inodetablesize / inodestructsize;
     sb.s_inodes_count = inodeNo;
     sb.s_blocks_count = size;
     sb.s_free_inodes_count = inodeNo - 3;
     sb.s_free_blocks_count = size - 1;
     sb.s_first_data_block = 8;
     sb.s_first_ino = 2;
     sb.s_magic = 0x554e4958;
     struct inode inodetable[inodeNo];
 	 int i = 0;
     int j = 0;
 	 for (i = 0; i < inodeNo; ++i) {
 	 	inodetable[i].i_mode = 0;
 	 	inodetable[i].i_uid = 0;
 	 	inodetable[i].i_gid = 0;
        inodetable[i].i_links_count = 0;
        inodetable[i].i_size = 0;
        inodetable[i].i_time = 0;
        inodetable[i].i_ctime = 0;
        inodetable[i].i_mtime = 0;
        inodetable[i].i_dtime = 0;
        inodetable[i].i_blocks = 0;
        for (j = 0; j < 8; ++j) {
            inodetable[i].i_block[j] = 0;
        }
 	 }
     int userpermission = S_IRWXU | S_IRWXG | S_IRWXO;
     inodetable[2].i_mode = S_IFDIR | userpermission;
     inodetable[2].i_uid = getuid();
     inodetable[2].i_gid = getgid();
     inodetable[2].i_links_count = 1;
     inodetable[2].i_size = 128;
     inodetable[2].i_time = time(NULL);
     inodetable[2].i_ctime = time(NULL);
     inodetable[2].i_mtime = time(NULL);
     inodetable[2].i_dtime = 0;
     inodetable[2].i_blocks = 1;
     inodetable[2].i_block[0] = 0;
     for (i = 0; i < BLOCK_SIZE; ++i) {
          block_bm[i] = 0;
          inode_bm[i] = 0;
     }
     for (i = 0; i < BLOCK_SIZE / 8; i++) {
        for (j = 0; j < CHAR_BIT; j++) {
            if ((i == 0 && j == 0) || ((i * CHAR_BIT) + (j)) >= size) {
                block_bm[i] |= 1 << j;
            } else {
                block_bm[i] &= ~(1 << j);
            }
        }
     }
     inode_bm[0] = 0x07;
     char databuffer;
     databuffer = 0;
     fp = fopen (real_path, "w+");
     if (fp == NULL) {
              printf("cannot open\n");
              exit(0);
              return;
     } 
     fblockwrite(&sb, sizeof(struct superblock), 1 , fp);
     fblockwrite(block_bm, sizeof(block_bm), 1, fp);
     fblockwrite(inode_bm, sizeof(inode_bm), 1, fp);
     fwrite(inodetable, sizeof(struct inode), inodeNo, fp);
     fwrite(&same, sizeof(struct directory_entry), 1, fp);
     fwrite(&parent, sizeof(struct directory_entry), 1, fp);
     for (i = 0; i < 384; ++i) {
        fwrite(&databuffer, 1, 1, fp);
     }
     for (i = 0; i < (size-1); ++i) {
        for (j = 0; j < 512; ++j) {
            fwrite(&databuffer, 1, 1, fp);
        }
     }
     //fseek( fp, 0, SEEK_SET );
     //printf("inodeNo = %d\n",S_IFDIR);
}

size_t fblockwrite(const void * ptr, size_t size, size_t count, FILE * stream) 
{
    char databuffer;
    databuffer = 0;
	size_t remaining = 512 - size;
	fwrite(ptr, size, count, stream);
    int i = 0;
    for (i = 0; i < remaining; ++i) {
        fwrite(&databuffer, 1, 1, fp);
    }
	return 0;
}
void open_filesystem(char *real_path, unsigned int n)
{
     if (fp != NULL) {
        fclose(fp);
     }
     fp = fopen(real_path, "r+");
     fseek( fp, 0, SEEK_SET );
     //struct superblock testblock;
     //unsigned char testblockbm[512];
     fread(&sb, sizeof(struct superblock), 1, fp);
     fseek(fp, 484, SEEK_CUR);
     fread(block_bm, sizeof(block_bm), 1, fp);
     fread(inode_bm, sizeof(inode_bm), 1, fp);

     if (sb.s_magic == 0x554e4958) {
        //printf("passed\n");
     } else {
        exit(0);
        return;
     }

     //printf("test block s_inodes_count is %d\n",block_bm[2]);

}
void make_directory(char *path, unsigned int n)
{

     int depth = 0;
     char ** paths = parsepath(path, n, &depth);
     fseek( fp, (512*3), SEEK_SET );
     struct inode inodetable[40];
     fread(inodetable, sizeof(struct inode), 40, fp);
     struct inode curr = inodetable[2];
     //printf("%d\n",curr.i_links_count );
     int curr_inode_num = 2;
     int i = 0;
     uint32_t inode_num = 2;
     //printf("depth is %d\n", depth);
     for (i = 0; i < depth-1; ++i) {
        inode_num = contains(&curr, paths[i]);
        //printf("paths is %s\n",paths[i]);
        //printf("inode number is %d\n", inode_num);
        if (inode_num == -1) {
            printf("parent directory does not exist\n");
            exit(0);
            return;
        } else {
            curr = inodetable[inode_num];
            curr_inode_num = inode_num;
        }
     }
     //printf("contains is %d\n",contains(&curr,paths[i]));
     if (contains(&curr, paths[i]) != -1) {
        printf("directory already exists\n");
        exit(0);
        return;
     }
     int new_inode_num = firstfreebit(inode_bm);
     //printf("new inode num is %d\n",new_inode_num);
     int userpermission = S_IRWXU | S_IRWXG | S_IRWXO;
     struct inode * new_inode = &inodetable[new_inode_num];
     new_inode->i_mode = S_IFDIR | userpermission;
     new_inode->i_uid = getuid();
     new_inode->i_gid = getgid();
     new_inode->i_links_count = 1;
     new_inode->i_size = 128;
     new_inode->i_time = time(NULL);
     new_inode->i_ctime = time(NULL);
     new_inode->i_mtime = time(NULL);
     new_inode->i_blocks = 1;
     int firstfreeblock = firstfreebit(block_bm);
     //printf("first free block %d\n",firstfreeblock);
     new_inode->i_block[0] = firstfreeblock;
     //fseek(fp,(8+new_inode->i_block[0]) * BLOCK_SIZE,SEEK_SET);
     struct directory_entry curr_dir;
     //fread(&curr_dir,sizeof(char),BLOCK_SIZE,fp);
     curr_dir.d_inode = new_inode_num;
     curr_dir.d_file_type = 2;
     memset(curr_dir.d_name, 0, 57);
     strcpy(curr_dir.d_name, ".");
     curr_dir.d_name_len = 1;
     struct directory_entry prnt_dir;
     prnt_dir.d_inode = curr_inode_num;
     prnt_dir.d_file_type = 2;
     memset(prnt_dir.d_name, 0, 57);
     strcpy(prnt_dir.d_name, "..");
     prnt_dir.d_name_len = 2;
     struct directory_entry new_dir;
     new_dir.d_inode = new_inode_num;
     new_dir.d_file_type = 2;
     memset(new_dir.d_name, 0, 57);
     strcpy(new_dir.d_name, paths[depth-1]);
     //printf("path[depth - 1] is %s\n",paths[depth-1]);
     new_dir.d_name_len = strlen(new_dir.d_name);
     //printf("%d\n",new_dir.d_name_len);
     int index = inodetable[inode_num].i_block[0];
     //printf("index is %d\n",index);
     fseek(fp, ((8 + index) * BLOCK_SIZE), SEEK_SET);
     struct directory_entry direntries[8];
     fread(direntries, sizeof(struct directory_entry), 8, fp);
     fseek(fp, -512, SEEK_CUR);
     int dirNo = inodetable[inode_num].i_size / 64;
     //printf("dirNo is %d\n",dirNo);
     direntries[dirNo] = new_dir;
     inodetable[inode_num].i_size = inodetable[inode_num].i_size + 64;
     inodetable[inode_num].i_mtime = time(NULL);
     //printf("name is %s\n",new_dir.d_name);
     //printf("direntries is %s\n",direntries[3].d_name);
     fwrite(direntries, sizeof(struct directory_entry), 8, fp);
     fseek(fp, ((8 + firstfreeblock) * BLOCK_SIZE), SEEK_SET);
     fwrite(&curr_dir, sizeof(struct directory_entry), 1, fp);
     fwrite(&prnt_dir, sizeof(struct directory_entry), 1, fp);
     char databuffer = 0;
     for (i = 0; i < 384; ++i) {
        fwrite(&databuffer, 1, 1, fp);
     }
     //fwrite(&new_dir,sizeof(struct directory_entry),1,fp);
     sb.s_free_inodes_count -= 1;
     sb.s_free_blocks_count -= 1;
     fseek( fp, 0, SEEK_SET );
     fblockwrite(&sb, sizeof(struct superblock), 1, fp);
     fblockwrite(block_bm, sizeof(block_bm), 1, fp);
     fblockwrite(inode_bm, sizeof(inode_bm), 1, fp);
     fwrite(inodetable, sizeof(struct inode), 40, fp);

     //printf("%s\n",paths[0]);
     //printf("depth is %d\n",depth);
     //printf("%s\n",);
     //printf("%s\n",paths[0]);
}
unsigned int read_directory(char *path, unsigned int n, char *data)
{
     int depth = 0;
     char ** paths = parsepath(path, n, &depth);
     printf("path[0] is %s\n", paths[0]);
     fseek( fp, (512*3), SEEK_SET );
     struct inode inodetable[40];
     fread(inodetable, sizeof(struct inode), 40, fp);
     struct inode curr = inodetable[2];
     int i = 0;
     uint32_t inode_num = 2;
     for (i = 0; i < depth; ++i) {
        inode_num = contains(&curr, paths[i]);
        if (inode_num == -1) {
            printf("directory does not exist\n");
            exit(0);
            return 0;
        } else {
            curr = inodetable[inode_num];
        }
     }
     inodetable[inode_num].i_mtime = time(NULL);
     int num_of_dirs = curr.i_size / sizeof(struct directory_entry);
     fseek(fp, (8 + curr.i_block[0]) * BLOCK_SIZE, SEEK_SET);
     char directory_block[BLOCK_SIZE];
     fread(directory_block, sizeof(char), BLOCK_SIZE, fp);
     char * ptr = directory_block;
     //printf("number of dirs is %d\n",num_of_dirs);
     int returnvalue = num_of_dirs * 64;
     //printf("size is %d\n",returnvalue);
     for (i = 0; i < num_of_dirs; i++) {
        //struct directory_entry * entry = (struct directory_entry *) (ptr + i * sizeof(struct directory_entry));
        //printf("It has %s\n", entry->d_name);
    }
     memcpy(data, ptr, returnvalue);
     return returnvalue;
}
void rm_directory(char *path, unsigned int n)
{

}
void create_file(char *path, unsigned int n, unsigned int size, char *data)
{

}
void rm_file(char *path, unsigned int n)
{

}
unsigned int read_file(char *path, unsigned int n, char *data)
{

     return 0;
}
void make_link(char *path, unsigned int n, char *target)
{

}

char ** parsepath(char * path, int  size, int * depth) 
{
    int count = 0;
    int i = 0;
    for (i = 0; i < size; ++i) {
        if (path[i] == '/') {
            count = count + 1;
        }
    }
    //char paths[count][57];
    char ** paths = (char **) malloc(count*sizeof(char *));
    char * data = strdup(path);
    char * dirname = strtok(data, "/");
    i = 0;
    while (dirname != NULL) {
        //printf("parsepath %s\n", dirname);
        paths[i] = dirname;
        dirname = strtok(NULL, "/");
        count = count + 1;
        i = i + 1;
    }
    *depth = i;
    return paths;
}

uint32_t contains(struct inode * dir, char * dirName) 
{
    int i;
    int num_of_entries = dir->i_size / sizeof(struct directory_entry);
    //printf("number of entries %d\n", num_of_entries);
    //struct directory_entry * entries = (struct directory_entry *) (dir->i_block[0]*BLOCK_SIZE + (512*8));
    fseek(fp, (8 + dir->i_block[0]) * BLOCK_SIZE, SEEK_SET);
    char directory_block[BLOCK_SIZE];
    fread(directory_block, sizeof(char), BLOCK_SIZE, fp);
    char * ptr = directory_block;
    for (i = 0; i < num_of_entries; i++) {
        struct directory_entry * entry = (struct directory_entry *) (ptr + i * sizeof(struct directory_entry));
        //printf("Entry is %s\n", entry->d_name);
        if (!strcmp(entry->d_name, dirName)) { //if the same, return inode number.
            return entry->d_inode;
        }
    }
    return -1;
}
 
int firstfreebit(unsigned char * bitmap)  
{
    int i = 0;
    int j = 0;
    for (i = 0; i < BLOCK_SIZE / CHAR_BIT; ++i) {  //using 512 bits. so tells u number of bytes to iterate thru
        for (j = 0; j < CHAR_BIT; ++j) { //loop thru every bit in a byte
            if ((bitmap[i] & (1 << j)) == 0) { //looka for first 0th bit
                bitmap[i] |= (1 << j); //set to 1
                return i * CHAR_BIT + j; //return the position
            }
        }
    }
    return -1;
}
