#include <errno.h>
#include <sys/stat.h>

typedef struct config
{
    char *target;
} Config;

static void pr_config(Config *config)
{
    printf("[debug] Target : %s\n", config->target);
}

int make_config(Config *config, int argc, char *argv[])
{
    if (argc < 2)
        return -1;

    struct stat s;
    int ret = stat(*(argv + 1), &s);

    if (ret < 0)
    {
        if (errno == ENOENT)
        {
            // we setup a file
            printf("We setup a file at path %s\n", *(argv + 1));
        }
    }
    config->target = *(argv + 1);

    pr_config(config);
    return 0;
}
