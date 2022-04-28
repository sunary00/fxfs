#include "fs.h"
#define FUSE_USE_VERSION 30

static struct fuse_operations operations = {
    .mkdir   = mymkdir,
    .getattr = mygetattr,
    .readdir = myreaddir,
    .rmdir   = myrmdir,
    .open    = myopen,
    .read    = myread,
    .write   = mywrite,
    .create  = mycreate,
    .rename  = myrename,
    .unlink  = myrm,
};

int main(int argc, char *argv[])
{
    fxfs_init_context();
    fxfs_context* ctx = fxfs_get_context();

    printf("main begin\n");
    FILE *fd = fopen("file_structure.bin", "rb");
    if (fd)
    {
        printf("LOADING\n");
        fread(&(ctx->file_array), sizeof(filetype) * 31, 1, fd);

        int child_startindex = 1;
        ctx->file_array[0].parent = NULL;

        for (int i = 0; i < 6; i++)
        {
            printf("i= %d\n", i);
            ctx->file_array[i].num_children = 0;
            ctx->file_array[i].children = NULL;
            for (int j = child_startindex; j < child_startindex + 5; j++)
            {
                printf("j= %d,ctx->file_array[j].valid=%d,name=%s\n", j, ctx->file_array[j].valid, ctx->file_array[j].name);
                if (ctx->file_array[j].valid)
                {
                    printf("ctx->file_array[j].valid ,j=%d\n", j);
                    add_child(&ctx->file_array[i], &ctx->file_array[j]);
                }
            }
            child_startindex += 5;
        }

        ctx->root = &ctx->file_array[0];

        FILE *fd1 = fopen("super.bin", "rb");
        fread(&(ctx->superblock), sizeof(superblock), 1, fd1);
    }
    else
    {
        initialize_superblock();
        initialize_root_directory();
    }
    printf("main end\n");
    return fuse_main(argc, argv, &operations, NULL); // demoan qi hou tai chengxu
}
