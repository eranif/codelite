#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sstream>

typedef std::vector<std::string> StringVec_t;

std::vector<std::string> extract_file_name( const std::string& line );
char * normalize_path(const char * src, size_t src_len);
bool is_source_file(const std::string& filename, std::string &fixed_file_name);
void * Memrchr(const void *buf, int c, size_t num);

#ifdef _WIN32
extern int ExecuteProcessWIN(const std::string& commandline);
#endif

#ifndef _WIN32

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/stat.h>

void WriteContent( const std::string& logfile, const std::string& filename, const std::string& flags )
{
    // Open the file
    int fd = ::open(logfile.c_str(), O_CREAT|O_APPEND, 0660);
    ::chmod(logfile.c_str(), 0660);

    if ( fd < 0 )
        return;

    // Lock it
    if(::flock(fd, LOCK_EX) < 0) {
        ::close(fd);
        return;
    }

    char cwd[1024];
    memset(cwd, 0, sizeof(cwd));
    char* pcwd = ::getcwd(cwd, sizeof(cwd));
    (void) pcwd;

    std::string line = filename + "|" + cwd + "|" + flags + "\n";

    FILE* fp = fopen(logfile.c_str(), "a+b");
    if ( !fp ) {
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

#endif
extern void WriteContent( const std::string& logfile, const std::string& filename, const std::string& flags );

// A thin wrapper around gcc
// Its soul purpose is to parse gcc's output and to store the parsed output
// in a sqlite database
int main(int argc, char **argv)
{
    // We require at least one argument
    if ( argc < 2 ) {
        return -1;
    }

    StringVec_t file_names;
    const char *pdb = getenv("CL_COMPILATION_DB");
    std::string commandline;
    for ( int i=1; i<argc; ++i ) {
        // Wrap all arguments with spaces with double quotes
        std::string arg = argv[i];
        std::string file_name;
        
        if ( is_source_file( arg, file_name ) ) {
            file_names.push_back( file_name );
        }

        // re-escape double quotes if needed
        size_t pos = arg.find('"');
        while ( pos != std::string::npos ) {
            arg.replace(pos, 1, "\\\""); // replace it with escapted slash
            pos = arg.find('"', pos + 2);
        }

        if ( arg.find(' ') != std::string::npos ) {
            std::string a = "\"";
            a += arg;
            a += '"';
            arg.swap(a);
        }
        commandline += arg + " ";
    }

    if ( pdb ) {
        for(size_t i=0; i<file_names.size(); ++i) {
#if __DEBUG
            printf("filename: %s\n", file_names.at(i).c_str());
#endif
            std::string logfile = pdb;
            logfile += ".txt";

            WriteContent(logfile, file_names.at(i), commandline);
        }
    }

#ifdef _WIN32
    int exitCode = ::ExecuteProcessWIN(commandline);
    return exitCode;
#else
    return execvp(argv[1], argv+1);
#endif
}

bool ends_with(const std::string &s, const std::string& e)
{
    size_t where = s.rfind(e);
    return ( where == std::string::npos ? false : ((s.length() - where) == e.length()) );
}

bool is_source_file(const std::string& filename, std::string &fixed_file_name)
{
    StringVec_t extensions;
    extensions.push_back(".cpp");
    extensions.push_back(".cxx");
    extensions.push_back(".cc");
    extensions.push_back(".c");

    for(size_t n=0; n<extensions.size(); ++n) {
        if ( ends_with(filename, extensions.at(n)) ) {
            fixed_file_name = filename;

#ifdef _WIN32
            std::replace(fixed_file_name.begin(), fixed_file_name.end(), '/', '\\');
#endif
            char* ret = normalize_path(fixed_file_name.c_str(), fixed_file_name.length());
            fixed_file_name = ret;
            free(ret);

            // rtrim
            fixed_file_name.erase(0, fixed_file_name.find_first_not_of("\t\r\v\n\" "));

            // ltrim
            fixed_file_name.erase(fixed_file_name.find_last_not_of("\t\r\v\n\" ")+1);
            return true;
        }
    }
    fixed_file_name.clear();
    return false;
}

char * normalize_path(const char * src, size_t src_len)
{
    std::string strpath = src;
    size_t where = strpath.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    bool has_drive = (where == 0 && strpath.length() > 1 && strpath.at(1) == ':');

    char * res;
    size_t res_len;

    const char * ptr = src;
    const char * end = &src[src_len];
    const char * next;

    if (!has_drive && (src_len == 0 || src[0] != '/')) {

        // relative path

        char pwd[4096];
        size_t pwd_len;

        if (getcwd(pwd, sizeof(pwd)) == NULL) {
            return NULL;
        }

        pwd_len = strlen(pwd);
        std::replace(pwd, pwd + pwd_len, '\\', '/');

        res = (char*)malloc(pwd_len + 1 + src_len + 1);
        memcpy(res, pwd, pwd_len);
        res_len = pwd_len;
    } else {
        res = (char*)malloc((src_len > 0 ? src_len : 1) + 1);
        res_len = 0;
    }

    for (ptr = src; ptr < end; ptr=next+1) {
        size_t len;
        next = (char*)memchr(ptr, '/', end-ptr);
        if (next == NULL) {
            next = end;
        }
        len = next-ptr;
        switch(len) {
        case 2:
            if (ptr[0] == '.' && ptr[1] == '.') {
                const char * slash = (char*)Memrchr(res, '/', res_len);
                if (slash != NULL) {
                    res_len = slash - res;
                }
                continue;
            }
            break;
        case 1:
            if (ptr[0] == '.') {
                continue;

            }
            break;
        case 0:
            continue;
        }

        if ( res_len == 0 && !has_drive )
            res[res_len++] = '/';
        else if ( res_len )
            res[res_len++] = '/';

        memcpy(&res[res_len], ptr, len);
        res_len += len;
    }

    if (res_len == 0) {
        res[res_len++] = '/';
    }
    res[res_len] = '\0';
    return res;
}

void * Memrchr(const void *buf, int c, size_t num)
{
    char *pMem = (char *) buf;

    for (;;) {
        if (num-- == 0) {
            return NULL;
        }

        if (pMem[num] == (unsigned char) c) {
            break;
        }

    }

    return (void *) (pMem + num);

}
