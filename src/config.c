#include <errno.h>
#include <sys/stat.h>
#include "headers/log.h"

typedef struct config
{
    char *target;
} Config;

static void pr_argv(int argc, char **argv)
{
    int count;
    if (argv == NULL)
        return;
    debug("pr_agv { Argc: %d }", argc);
    for (count = 0; argv[count] != NULL; count++)
        debug("pr_agv { Loc: %d, Val: '%s' }", count + 1, argv[count]);
}

static void pr_config(Config *config)
{
    debug("pr_config { Target file: '%s' }", config->target);
}

int make_config(Config *config, int argc, char *argv[])
{
    struct stat s;
    int ret;

    pr_argv(argc, argv);

    if (argc < 2)
        return -1;

    ret = stat(*(argv + 1), &s);

    if (ret < 0)
    {
        if (errno == ENOENT)
        {
            // we setup a file
            info("We setup a file at path %s", *(argv + 1));
        }
    }

    config->target = *(argv + 1);
    pr_config(config);

    return 0;
}
