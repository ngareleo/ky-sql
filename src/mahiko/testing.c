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

int build_test_file(char *abspath);
int execute_test_case(char *abspath);
void main()
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
        pid_t pid;
        char *file_path, *test_build_path;
        int ret, buffersize, build, test;

        buffersize = sizeof(char) * (strlen(cwd) + strlen(testcase) + SIZE_PADDING);
        file_path = (char *)malloc(buffersize);
        test_build_path = (char *)malloc(buffersize);

        sprintf(file_path, "%s/src/%s.c", cwd, testcase);
        sprintf(test_build_path, "%s/out/testbuilds/%s.c", cwd, testcase);

        build = build_test_file(file_path);

        if (build)
        {
            continue;
        }

        test = execute_test_case(test_build_path);

        free(file_path);
        free(test_build_path);
    }

    free(cwd);
}

int execute_test_case(char *path)
{
    pid_t pid = fork();
    char *cmd;
    int status, ret, signal, core_dump;

    if (pid == -1)
    {
        perror("fork");
        return;
    }

    sprintf(cmd, "./%s", path);

    if (!pid)
    {
        ret = execl(cmd, NULL);
        if (ret == -1)
        {
            fprintf(stderr, "\n Couldn't execute test\n");
        }
    }

    pid = wait(&status);

    if (WIFEXITED(status))
    {
        printf("Normal termination with exit status=%d\n", WEXITSTATUS(status));
        return 0;
    }

    if (WIFSIGNALED(status))
    {
        signal = WTERMSIG(status);
        core_dump = WCOREDUMP(status);

        if (signal == SIGABRT)
        {
            fprintf(stderr, "Test case failed");
            return 1;
        }
    }

    return -1;
}

int build_test_file(char *path)
{
    pid_t pid;
    int status, ret, signal, core_dump;

    if ((pid = fork()) == -1)
    {
        perror("fork");
        return -1;
    }

    if (!pid)
    {
        ret = execl("/usr/bin/clang", "clang", "-o ./out/testbuilds/main", path, NULL);
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
        fprintf(stderr, "\nCouldn't compile test case at %s\n");
        return 1;
    }
}

void test_case(char *testcase, void(func)())
{
    printf("Executing %s\n", testcase);
    func();
    printf("Suite complete");
}
