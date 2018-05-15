#include "simpleFS.h"

int main(int argc, char **argv)
{
    /* TODO:
     * Implement :)
     */
	//int seconds = time(NULL);
	//printf("%d\n",seconds);
	init_filesystem(20, "testfile",4);
	//open_filesystem("testfile",4);
	//make_directory("/a/",3);
	//make_directory("/a/b",3);
	//char data[512];
	/*int f = read_directory("/a/b/",3,data);
	printf("size is %d\n",f);
	int i = 0;
	struct directory_entry * entry = (struct directory_entry *)data;
	for (i = 0; i < 2; ++i)
	{
		//printf("main entry is %s\n",entry->d_name);
		//entry = (struct directory_entry *)data + sizeof(struct directory_entry);
	}*/
	//char data[512];
	//read_directory("/", 1, data);
    return 0;
}
