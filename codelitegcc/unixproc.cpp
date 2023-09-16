#ifndef _WIN32
// Unix version
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void file_write_content(const std::string& logfile, const std::string& content)
{
    // Open the file
    int fd = ::open(logfile.c_str(), O_CREAT | O_APPEND, 0660);
    ::chmod(logfile.c_str(), 0660);

    if(fd < 0)
        return;

    // Lock it
    if(::flock(fd, LOCK_EX) < 0) {
        ::close(fd);
        return;
    }

    char cwd[1024];
    memset(cwd, 0, sizeof(cwd));
    char* pcwd = ::getcwd(cwd, sizeof(cwd));
    (void)pcwd;

    std::string line = content + "\n";

    FILE* fp = fopen(logfile.c_str(), "a+b");
    if(!fp) {
        perror("fopen");
    }

    // Write the content
    ::fprintf(fp, "%s", line.c_str());
    ::fflush(fp);
    ::fclose(fp);

    // Release the lock
    ::flock(fd, LOCK_UN);

    // close the fd
    ::close(fd);
}

int run_child_process(int argc, char** argv, const std::string& commandline)
{
    (void)commandline;
    (void)argc;
    return execvp(argv[1], argv + 1);
}

#endif
