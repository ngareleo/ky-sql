#define _GNU_SOURCE
#include "testing.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE_PADDING 10

const char *tests[] = {
    "utilities_test",
};

int build_test_file(char *path, char *dest);
int execute_test_case(char *path);

int main()
{
    char *testcase, *cwd;
    cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("cwd");
        exit(EXIT_FAILURE);
    }

    for (testcase = tests; testcase != NULL; testcase++)
    {
        int buffersize = sizeof(char) * (strlen(cwd) + strlen(testcase) + SIZE_PADDING),
            build,
            test;
        char file_path[buffersize], test_build_path[buffersize];

        sprintf(test_build_path, "%s/out/testbuilds/%s", cwd, testcase);
        sprintf(file_path, "%s/src/%s.c", cwd, testcase);

        build = build_test_file(file_path, test_build_path);

        if (build)
        {
            continue;
        }

        test = execute_test_case(test_build_path);

        if (test == 1)
            fprintf(stderr, "\nTest suite '%s' failed\n", testcase);
        else if (test == -1)
            fprintf(stderr, "\nCouldn't execute test suite '%s' \n", testcase);
    }

    free(cwd);
    return 0;
}

int execute_test_case(char *path)
{
    pid_t pid = fork();
    int status, ret, signal, core_dump;

    if (pid == -1)
    {
        perror("fork");
        return pid;
    }

    if (!pid)
    {
        ret = execl(path, NULL);
        if (ret == -1)
            return ret;
    }

    pid = wait(&status);

    if (WIFEXITED(status))
        return 0;

    if (WIFSIGNALED(status))
    {
        signal = WTERMSIG(status);
        core_dump = WCOREDUMP(status);

        if (signal == SIGABRT)
            return 1;
    }

    return -1;
}

int build_test_file(char *path, char *dest)
{
    pid_t pid;
    int status, ret;
    char destpath[strlen(path) + SIZE_PADDING];

    if ((pid = fork()) == -1)
    {
        perror("fork");
        return -1;
    }
    sprintf(destpath, "-o %s", dest);

    if (!pid)
    {
        ret = execl("/usr/bin/clang", "clang", destpath, path, NULL);

        if (ret == -1)
        {
            fprintf(stderr, "\nCouldn't compile file at %s\n", path);
            return -1;
        }
    }

    pid = wait(&status);

    if (WIFEXITED(status))
    {
        printf("\nNormal termination with exit status=%d\n", WEXITSTATUS(status));
        return 0;
    }
    else
    {
        fprintf(stderr, "\nCouldn't compile test case at %s\n", path);
        return 1;
    }
}

void test_case(char *testcase, void(func)())
{
    printf("Executing %s\n", testcase);
    func();
    printf("Suite complete");
}
