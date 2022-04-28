


#include "fs.h"

// gcc FS.c -o FS `pkg-config fuse --cflags --libs`
// ./ FS - f Desktop / OS / mountpoint4

// superblock spblock;
// filetype*  root;
// filetype   file_array[50];

fxfs_context ctx;


fxfs_context*
fxfs_get_context() {
	return &ctx;
}

void 
fxfs_init_context() {
	ctx.superblock = (superblock*)malloc(sizeof(superblock));
	ctx.root = NULL;
	ctx.file_array = (filetype*)malloc(sizeof(filetype) * 50);
}


void initialize_superblock(){
	printf("initialize_superblock is begin\n");

	memset(ctx.superblock->data_bitmap, '0', 100*sizeof(char));
	memset(ctx.superblock->inode_bitmap, '0', 100*sizeof(char));
}


void tree_to_array(filetype * queue, int * front, int * rear, int * index){
	printf("tree_to_array is begin\n");
	if(*rear < *front)
		return;
	if(*index > 30)
		return;
	printf("tree_to_array  front is %d rear is %d index is %d\n", *front,*rear,*index);

	filetype curr_node = queue[*front];
	*front += 1;
	ctx.file_array[*index] = curr_node;
	printf("file_array valid is %d,index is %d \n",ctx.file_array[*index].valid,*index);
	*index += 1;

	if(*index < 6){

		if(curr_node.valid){

			printf("curr_node.valid  curr_node.num_children is %d\n ", curr_node.num_children);
			int n = 0;
			int i;
			for(i = 0; i < curr_node.num_children; i++){
				printf("2 tree_to_array  front is %d rear is %d index is %d,num_children is %d\n", *front,*rear,*index,curr_node.num_children);
				if(*rear < *front)
					*rear = *front;
				queue[*rear] = *(curr_node.children[i]);
				*rear += 1;
				printf("3 tree_to_array  front is %d rear is %d index is %d,num_children is %d\n", *front,*rear,*index,curr_node.num_children);
			}
			while(i<5){
				filetype waste_node;
				waste_node.valid = 0;
				queue[*rear] = waste_node;
				*rear += 1;
				i++;
			}
		}
		else{
			int i = 0;
			while(i<5){
				filetype waste_node;
				waste_node.valid = 0;
				queue[*rear] = waste_node;
				*rear += 1;
				i++;
			}
		}
	}

	tree_to_array(queue, front, rear, index);
	

}


int save_contents(){
	printf("SAVING\n");
	filetype * queue = malloc(sizeof(filetype)*60);
	int front = 0;
	int rear = 0;
	queue[0] = *(ctx.root);

	//printf("queue is %d\n",root -> permissions);

	int index = 0;
	tree_to_array(queue, &front, &rear, &index);

	for(int i = 0; i < 31; i++){
		printf("%d", ctx.file_array[i].valid);
		//printf("valid is %d\n",root -> valid);
	}


	FILE * fd = fopen("file_structure.bin", "wb");

	FILE * fd1 = fopen("super.bin", "wb");


	fwrite(ctx.file_array, sizeof(filetype)*31, 1, fd);
	fwrite(&ctx.superblock,sizeof(superblock),1,fd1);

	fclose(fd);
	fclose(fd1);

	printf("\n");
	printf("SAVING end\n");
}

void initialize_root_directory() {
	printf("initialize_root_directory is begin\n");
	ctx.superblock->inode_bitmap[1]=1; //marking it with 0
	ctx.root = (filetype *) malloc (sizeof(filetype));

	strcpy(ctx.root->path, "/");
	strcpy(ctx.root->name, "/");

	ctx.root->children = NULL;
	ctx.root->num_children = 0;
	ctx.root->parent = NULL;
	ctx.root->num_links = 2;
	ctx.root->valid = 1;
	strcpy(ctx.root->test, "test");
	//ctx.root -> type = malloc(10);
	strcpy(ctx.root -> type, "directory");

	ctx.root->c_time = time(NULL);
	ctx.root->a_time = time(NULL);
	ctx.root->m_time = time(NULL);
	ctx.root->b_time = time(NULL);

	ctx.root->permissions = S_IFDIR | 0777;

	ctx.root->size = 0;
	ctx.root->group_id = getgid();
	ctx.root->user_id = getuid();

	//printf("ctx.root is %d\n",ctx.root -> permissions);
	ctx.root->number = 2;
	//ctx.root -> size = 0;
	ctx.root->blocks = 0;
	
	save_contents();
	printf("initialize_root_directory is end\n");
}


filetype * filetype_from_path(char * path){
	printf("filetype_from_path\n");
	printf("parent path 1 is %s\n", path);
	char curr_folder[100];
	char * path_name = malloc(strlen(path) + 2);

	strcpy(path_name, path);
	
	filetype * curr_node = ctx.root;

	fflush(stdin);

	if(strcmp(path_name, "/") == 0)
		return curr_node;
	printf("not return curr_node\n");
	if(path_name[0] != '/'){
		printf("INCORRECT PATH\n");
		exit(1);
	}
	else{
		path_name++;
		printf("1 filetype_from_path %s\n",path_name);
	}

	if(path_name[strlen(path_name)-1] == '/'){
		
		path_name[strlen(path_name)-1] = '\0';
		printf("2 filetype_from_path %s\n",path_name);
	}

	char * index;
	int flag = 0;

	while(strlen(path_name) != 0){
		index = strchr(path_name, '/');
		printf("3 filetype_from_path %s,%s\n",index,path_name);
		if(index != NULL){
			strncpy(curr_folder, path_name, index - path_name);
			printf("4 filetype_from_path %d,%s\n",(index - path_name),curr_folder);

			curr_folder[index-path_name] = '\0';
			printf("5 filetype_from_path %s\n",curr_folder);
			flag = 0;
			for(int i = 0; i < curr_node -> num_children; i++){
				printf("6 filetype_from_path %s,%s,%d\n",((curr_node -> children)[i] -> name),curr_folder,curr_node -> num_children);
				if(strcmp((curr_node -> children)[i] -> name, curr_folder) == 0){
					//printf("(curr_node -> children)[i] -> name, curr_folder) == 0\n");
					curr_node = (curr_node -> children)[i];
					printf("(curr_node -> children)[i] -> name, curr_folder) == 0\n,curr_node is %s \n",curr_node->name);
					flag = 1;
					break;
				}
			}
			if(flag == 0)
				return NULL;
		}
		else{
			printf("7 filetype_from_path %s,%s\n",curr_folder,path_name);
			strcpy(curr_folder, path_name);
			printf("8 filetype_from_path %s,%s,%d\n",curr_folder,path_name,curr_node -> num_children);
			flag = 0;
			for(int i = 0; i < curr_node -> num_children; i++){
				printf("9 filetype_from_path %s,%s\n",((curr_node -> children)[i] -> name),curr_folder);
				if(strcmp((curr_node -> children)[i] -> name, curr_folder) == 0){
					curr_node = (curr_node -> children)[i];
					//printf("curr_node children i name %s,curr_folder %s\n",((curr_node -> children)[i] -> name),curr_folder);
					printf("10 filetype_from_path %s,%s\n",(curr_node -> name),curr_folder);
					return curr_node;
				}
			}
			return NULL;
		}
		path_name = index+1;
	}

}

int find_free_inode(){
	printf("find_free_inode\n");
	for (int i = 2; i < 100; i++){
		if(ctx.superblock->inode_bitmap[i] == '0'){
			ctx.superblock->inode_bitmap[i] = '1';
			printf("free i=%d\n",i);
			return i;
		}
	 	
	}
	return 0;
}

int find_free_db(){
	printf("find_free_db\n");
	for (int i = 1; i < 100; i++){
		if(ctx.superblock->inode_bitmap[i] == '0'){
			ctx.superblock->inode_bitmap[i] = '1';
			printf("free i=%d\n",i);
			return i;
		}
		
	}
	return 0;
}

void add_child(filetype * parent, filetype * child){
	printf("add_child\n");
	(parent -> num_children)++;

	parent -> children = realloc(parent -> children, (parent -> num_children)*sizeof(filetype *));

	(parent -> children)[parent -> num_children - 1] = child;
}

int mymkdir(const char *path, mode_t mode) {
	printf("MKDIR\n");

	int index = find_free_inode();

	filetype * new_folder = malloc(sizeof(filetype));

	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);
	printf("1 pathname done %s\n",pathname);

	char * rindex = strrchr(pathname, '/');
	printf("2 rindex done %s \n",rindex);

	//new_folder -> name = malloc(strlen(pathname)+2);
	strcpy(new_folder -> name, rindex+1);
	//new_folder -> path = malloc(strlen(pathname)+2);
	strcpy(new_folder -> path, pathname);



	*rindex = '\0';
	printf("3 rindex done %s pathname is %s\n",rindex,pathname);

	if(strlen(pathname) == 0)
	strcpy(pathname, "/");

	new_folder -> children = NULL;
	new_folder -> num_children = 0;
	printf("new_folder -> parent = filetype_from_path(pathname) %s\n",pathname);
	new_folder -> parent = filetype_from_path(pathname);
	printf("new_folder -> parent = filetype_from_path(pathname) %s\n",new_folder -> parent->name);
	new_folder -> num_links = 2;
	new_folder -> valid = 1;
	strcpy(new_folder -> test, "test");

	if(new_folder -> parent == NULL)
		return -ENOENT;

	//printf(";;;;%p;;;;\n", new_folder);

	add_child(new_folder->parent, new_folder);

	//new_folder -> type = malloc(10);
	strcpy(new_folder -> type, "directory");

	new_folder->c_time = time(NULL);
	new_folder->a_time = time(NULL);
	new_folder->m_time = time(NULL);
	new_folder->b_time = time(NULL);

	new_folder -> permissions = S_IFDIR | 0777;

	new_folder -> size = 0;
	new_folder->group_id = getgid();
	new_folder->user_id = getuid();


	new_folder -> number = index;
	printf("new_folder -> number %d \n",new_folder -> number);
	new_folder -> blocks = 0;


	save_contents();
	printf("MKDIR end\n");
	return 0;

}


int myreaddir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi ){ //ls children
	printf("READDIR\n");

	filler(buffer, ".", NULL, 0, FUSE_FILL_DIR_PLUS);
	filler(buffer, "..", NULL, 0, FUSE_FILL_DIR_PLUS);

	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);
	printf("1 READDIR %s\n", pathname);

	filetype * dir_node = filetype_from_path(pathname);
	printf("2 READDIR %s\n", dir_node->name);

	if(dir_node == NULL){
		printf("dir_node == NULL\n");
		return -ENOENT;
	}
	else{
		dir_node->a_time=time(NULL);
		for(int i = 0; i < dir_node->num_children; i++){
			printf("aa\n");
			filler(buffer,dir_node->children[i]->name, NULL, 0, FUSE_FILL_DIR_PLUS);
		}
	}
	printf("READDIR end\n");
	return 0;
}

int mygetattr(const char *path, struct stat *statit) { //cd 
	printf("mygetattr\n");
	char *pathname;
	pathname=(char *)malloc(strlen(path) + 2);

	strcpy(pathname, path);

	printf("GETATTR %s\n", pathname);
	
	filetype * file_node = filetype_from_path(pathname);
	
	if(file_node == NULL){

		printf("file_node == NULL\n");

		return -ENOENT;}
	
	printf("filetype_from_path done %s\n",file_node->name);

	statit->st_uid = file_node -> user_id; // The owner of the file/directory is the user who mounted the filesystem
	statit->st_gid = file_node -> group_id; // The group of the file/directory is the same as the group of the user who mounted the filesystem
	statit->st_atime = file_node -> a_time; // The last "a"ccess of the file/directory is right now
	statit->st_mtime = file_node -> m_time; // The last "m"odification of the file/directory is right now
	statit->st_ctime = file_node -> c_time;
	//statit->st_mode = S_IFDIR | 0777;
	printf("file_node1 is not none\n");
	printf("attr is %d, %d, %d \n", file_node -> user_id, file_node -> group_id, file_node -> a_time);
	statit->st_mode = file_node -> permissions;
    printf("file_node2 is not none\n");
	printf("permissions done %d\n",file_node -> permissions);
	statit->st_nlink = file_node -> num_links + file_node -> num_children;
	statit->st_size = file_node -> size;
	statit->st_blocks = file_node -> blocks;

	printf("attr is %d, %d, %d, %d, %d, %d, %d, %d, %d\n", statit->st_uid, statit->st_gid, statit->st_atime, statit->st_mtime, statit->st_ctime,
			file_node -> permissions, statit->st_nlink, statit->st_size, statit->st_blocks);
	printf("mygetattr end\n");
	return 0;
}

int myrmdir(const char * path){// rm -r
	printf("myrmdiring\n");
	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);

	char * rindex = strrchr(pathname, '/');

	char * folder_delete = malloc(strlen(rindex+1)+2);

	strcpy(folder_delete, rindex+1);

	*rindex = '\0';

	if(strlen(pathname) == 0)
		strcpy(pathname, "/");

	filetype * parent = filetype_from_path(pathname);

	if(parent == NULL)
		return -ENOENT;

	if(parent -> num_children == 0)
		return -ENOENT;

	filetype * curr_child = (parent -> children)[0];
	int index = 0;
	while(index < (parent -> num_children)){
		if(strcmp(curr_child -> name, folder_delete) == 0){
			break;
		}
		index++;
		curr_child = (parent -> children)[index];
	}

	if(index < (parent -> num_children)){
		if(((parent -> children)[index] -> num_children) != 0)
			return -ENOTEMPTY;
		for(int i = index+1; i < (parent -> num_children); i++){
			(parent -> children)[i-1] = (parent -> children)[i];
		}
		(parent -> num_children) -= 1;
	}

	else{
		return -ENOENT;
	}

	save_contents();
	printf("myrmdiring end\n");
	return 0;

}

int myrm(const char * path){//rm
	printf("rming\n");

	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);

	char * rindex = strrchr(pathname, '/');

	char * folder_delete = malloc(strlen(rindex+1)+2);

	strcpy(folder_delete, rindex+1);

	*rindex = '\0';

	if(strlen(pathname) == 0)
		strcpy(pathname, "/");

	filetype * parent = filetype_from_path(pathname);

	if(parent == NULL)
		return -ENOENT;

	if(parent -> num_children == 0)
		return -ENOENT;

	filetype * curr_child = (parent -> children)[0];
	int index = 0;
	while(index < (parent -> num_children)){
		if(strcmp(curr_child -> name, folder_delete) == 0){
			break;
		}
		index++;
		curr_child = (parent -> children)[index];
	}

	if(index < (parent -> num_children)){
		if(((parent -> children)[index] -> num_children) != 0)
			return -ENOTEMPTY;
		for(int i = index+1; i < (parent -> num_children); i++){
			(parent -> children)[i-1] = (parent -> children)[i];
		}
		(parent -> num_children) -= 1;
	}

	else{
		return -ENOENT;
	}

	save_contents();
	printf("rm end\n");
	return 0;

}


int mycreate(const char * path, mode_t mode, struct fuse_file_info *fi) {

	printf("CREATEFILE\n");

	int index = find_free_inode();

	filetype * new_file = malloc(sizeof(filetype));

	char * pathname = malloc(strlen(path)+2);
	strcpy(pathname, path);

	char * rindex = strrchr(pathname, '/');

	strcpy(new_file -> name, rindex+1);
	strcpy(new_file -> path, pathname);

	*rindex = '\0';

	if(strlen(pathname) == 0)
		strcpy(pathname, "/");

	new_file -> children = NULL;
	new_file -> num_children = 0;
	new_file -> parent = filetype_from_path(pathname);
	new_file -> num_links = 0;
	new_file -> valid = 1;

	if(new_file -> parent == NULL)
	return -ENOENT;

	add_child(new_file->parent, new_file);

	//new_file -> type = malloc(10);
	strcpy(new_file -> type, "file");

	new_file->c_time = time(NULL);
	new_file->a_time = time(NULL);
	new_file->m_time = time(NULL);
	new_file->b_time = time(NULL);

	new_file -> permissions = S_IFREG | 0777;

	new_file -> size = 0;
	new_file->group_id = getgid();
	new_file->user_id = getuid();


	new_file -> number = index;

	for(int i = 0; i < 16; i++){
		(new_file -> datablocks)[i] = find_free_db();
	}

	//new_file -> size = 0;
	new_file -> blocks = 0;

	save_contents();
	printf("CREATEFILE end\n");
	return 0;
}


int myopen(const char *path, struct fuse_file_info *fi) { //vi
	printf("OPEN\n");

	char * pathname = malloc(sizeof(path)+1);
	strcpy(pathname, path);

	filetype * file = filetype_from_path(pathname);
	printf("OPEN end\n");
	return 0;
}

int myread(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi) { //vi  cat

	printf("READ\n");

	char * pathname = malloc(sizeof(path)+1);
	strcpy(pathname, path);

	filetype * file = filetype_from_path(pathname);
	if(file == NULL)
		return -ENOENT;

	else{
		char * str = malloc(sizeof(char)*1024*(file -> blocks));

		printf(":%d:\n", file->size);
		strcpy(str, "");
		int i;
		for(i = 0; i < (file -> blocks) - 1; i++){
			strncat(str, &(ctx.superblock->datablocks[block_size*(file -> datablocks[i])]), 1024);
			printf("--> %s", str);
		}
		strncat(str, &(ctx.superblock->datablocks[block_size*(file -> datablocks[i])]), (file -> size)%1024);
		printf("--> %s", str);
		//strncpy(str, &spblock.datablocks[block_size*(file -> datablocks[0])], file->size);
		strcpy(buf, str);
	}
	printf("READ end\n");
	return file->size;
}

int myaccess(const char * path, int mask){ //quanxianjiancha
	printf("myaccess not yet\n");
	return 0;
}


int myrename(const char* from, const char* to) { //mv 
	printf("RENAME: %s\n", from);
	printf("RENAME: %s\n", to);

	char * pathname = malloc(strlen(from)+2);
	strcpy(pathname, from);

	char * rindex1 = strrchr(pathname, '/');

	filetype * file = filetype_from_path(pathname);

	*rindex1 = '\0';

	char * pathname2 = malloc(strlen(to)+2);
	strcpy(pathname2, to);

	char * rindex2 = strrchr(pathname2, '/');


	if(file == NULL)
		return -ENOENT;

	//file -> name = realloc(file -> name, strlen(rindex2+1)+2);
	strcpy(file -> name, rindex2+1);
	//file -> path = realloc(file -> path, strlen(to)+2);
	strcpy(file -> path, to);



	printf(":%s:\n", file->name);
	printf(":%s:\n", file->path);

	save_contents();
	printf("RENAME end\n");

	return 0;
}

int mytruncate(const char *path, off_t size) {//wen jian jieduan  > /xx
	printf("mytruncate not yet\n");
	return 0;
}

int mywrite(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {//echo 'aaa'>/xx

	printf("WRITING\n");

	char * pathname = malloc(sizeof(path)+1);
	strcpy(pathname, path);

	filetype * file = filetype_from_path(pathname);
	if(file == NULL)
		return -ENOENT;

	int indexno = (file->blocks)-1;

	if(file -> size == 0){
		strcpy(&(ctx.superblock->datablocks[block_size*((file -> datablocks)[0])]), buf);
		file -> size = strlen(buf);
		(file -> blocks)++;
	}
	else{
		int currblk = (file->blocks)-1;
		int len1 = 1024 - (file -> size % 1024);
		if(len1 >= strlen(buf)){
			strcat(&(ctx.superblock->datablocks[block_size*((file -> datablocks)[currblk])]), buf);
			file->size += strlen(buf);
			printf("---> %s\n", &(ctx.superblock->datablocks[block_size*((file -> datablocks)[currblk])]));
		}
		else{
			char * cpystr = malloc(1024*sizeof(char));
			strncpy(cpystr, buf, len1-1);
			strcat(&(ctx.superblock->datablocks[block_size*((file -> datablocks)[currblk])]), cpystr);
			strcpy(cpystr, buf);
			strcpy(&(ctx.superblock->datablocks[block_size*((file -> datablocks)[currblk+1])]), (cpystr+len1-1));
			file -> size += strlen(buf);
			printf("---> %s\n", &(ctx.superblock->datablocks[block_size*((file -> datablocks)[currblk])]));
			(file -> blocks)++;
		}

	}
	save_contents();
	printf("WRITING end\n");
	return strlen(buf);
}
