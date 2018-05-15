1) The total number of inodes are 40 since ((5*BLOCK_SIZE) / sizeof(struct inode)) = 40.
   The root directory will use 1 of them so we are left with 39. So the file system can have
   up to 39 files.

2) The largest possible file size is (8*BLOCK_SIZE) = 4096 bytes.


3) We only can have 8 directory entries in a single data block. 
   BLOCK_SIZE / sizeof(struct directory_entry) = 8
   Assuming we use all 8 data blocks, a single directory can have 64 directory entries since
   BLOCK_SIZE * 8 / sizeof(struct directory_entry) = 64.

4) No we cannot. We are reading and writing to an image file. So reading and writing concurrently
   to it will cause corruption in the data.