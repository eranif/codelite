#include "proc.hpp"
#include "tinyjson.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <vector>

typedef std::vector<std::string> StringVec_t;

std::vector<std::string> extract_file_name(const std::string& line);
char* normalize_path(const char* src, size_t src_len);
bool is_source_file(const std::string& filename, std::string& fixed_file_name);
void* Memrchr(const void* buf, int c, size_t num);

/// Convert the file pointed by `CL_COMPILATION_DB` into a valid
/// `compile_commands.json` file
void finalize_and_exit()
{
    const char* pdb = ::getenv("CL_COMPILATION_DB");
    if(!pdb) {
        std::cerr << "ERROR: environment variable `CL_COMPILATION_DB` is not set" << std::endl;
        exit(EXIT_FAILURE);
    }

    // open the file and read it line by line
    std::ifstream file(pdb);
    if(!file.is_open()) {
        std::cerr << "ERROR: could not open file: " << pdb << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::ofstream out_file{ "compile_commands.json" };
    out_file << "[\n";
    bool prepend_comma = false;
    while(!file.eof()) {
        std::getline(file, line);
        if(prepend_comma) {
            out_file << ",";
        }
        out_file << line << "\n";
        prepend_comma = true;
    }
    out_file << "]\n";
    file.close();
    std::cout << "file compile_commands.json created successfully" << std::endl;
    exit(EXIT_SUCCESS);
}

/// Print usage and exit
void print_help_and_exit()
{
    std::string help_string = R"(
    codelite-cc: a helper program that wraps your compiler
    and records the command line in a format acceptable by
    clangd (i.e. `compile_commands.json`)
    
    The compile commands are saved to the file pointed by the
    `CL_COMPILATION_DB` environment variable
    
    Example usage:
    
    # export the required env vars
    export CL_COMPILATION_DB=/tmp/intermediate_compile_commands.log
    
    # tell your Makefile that we want a different compiler
    export CC="/usr/bin/codelite-cc gcc"
    export CXX="/usr/bin/codelite-cc g++"
    
    # build as you usually do
    make -j$(nproc)
    
    # convert the output file pointed by `CL_COMPILATION_DB` into a valid
    # `compile_commands.json`
    /usr/bin/codelite-cc --finalize
    
    # You should now have a `compile_commands.json` to make `clangd` happy
)";
    std::cout << help_string << std::endl;
    exit(EXIT_SUCCESS);
}

// A thin wrapper around gcc that intercepts the compiler (e.g. gcc) command line arguments, store them in a file and
// then invoke the compiler
int main(int argc, char** argv)
{
    // We require at least one argument
    if(argc < 2) {
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1], "--help") == 0) {
        print_help_and_exit();
    }

    if(strcmp(argv[1], "--finalize")) {
        finalize_and_exit();
    }

    StringVec_t file_names;
    const char* pdb = getenv("CL_COMPILATION_DB");
    if(!pdb) {
        std::cerr << "WARNING: missing environment variable CL_COMPILATION_DB" << std::endl;
    }

    std::string commandline;
    for(int i = 1; i < argc; ++i) {
        // Wrap all arguments with spaces with double quotes
        std::string arg = argv[i];
        std::string file_name;

        if(is_source_file(arg, file_name)) {
            file_names.push_back(file_name);
        }

        // re-escape double quotes if needed
        size_t pos = arg.find('"');
        while(pos != std::string::npos) {
            arg.replace(pos, 1, "\\\""); // replace it with escapted slash
            pos = arg.find('"', pos + 2);
        }

        if(arg.find(' ') != std::string::npos) {
            std::string a = "\"";
            a += arg;
            a += '"';
            arg.swap(a);
        }
        commandline += arg + " ";
    }

    if(pdb) {
        char cwd[1024];
        memset(cwd, 0, sizeof(cwd));
        ::getcwd(cwd, sizeof(cwd));

        for(size_t i = 0; i < file_names.size(); ++i) {
            std::string logfile = pdb;

            tinyjson::element obj;
            tinyjson::element::create_object(&obj);
            obj.add_property("directory", cwd).add_property("file", file_names[i]).add_property("command", commandline);
            std::stringstream ss;
            tinyjson::to_string(obj, ss, false);
            file_write_content(logfile, ss.str());
        }
    }

    // launch the real command
    return run_child_process(argc, argv, commandline);
}

bool ends_with(const std::string& s, const std::string& e)
{
    size_t where = s.rfind(e);
    return (where == std::string::npos ? false : ((s.length() - where) == e.length()));
}

bool is_source_file(const std::string& filename, std::string& fixed_file_name)
{
    StringVec_t extensions;
    extensions.push_back(".cpp");
    extensions.push_back(".cxx");
    extensions.push_back(".cc");
    extensions.push_back(".c");

    for(size_t n = 0; n < extensions.size(); ++n) {
        if(ends_with(filename, extensions.at(n))) {
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
            fixed_file_name.erase(fixed_file_name.find_last_not_of("\t\r\v\n\" ") + 1);
            return true;
        }
    }
    fixed_file_name.clear();
    return false;
}

char* normalize_path(const char* src, size_t src_len)
{
    std::string strpath = src;
    size_t where = strpath.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    bool has_drive = (where == 0 && strpath.length() > 1 && strpath.at(1) == ':');

    char* res;
    size_t res_len;

    const char* ptr = src;
    const char* end = &src[src_len];
    const char* next;

    if(!has_drive && (src_len == 0 || src[0] != '/')) {

        // relative path

        char pwd[4096];
        size_t pwd_len;

        if(getcwd(pwd, sizeof(pwd)) == NULL) {
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

    for(ptr = src; ptr < end; ptr = next + 1) {
        size_t len;
        next = (char*)memchr(ptr, '/', end - ptr);
        if(next == NULL) {
            next = end;
        }
        len = next - ptr;
        switch(len) {
        case 2:
            if(ptr[0] == '.' && ptr[1] == '.') {
                const char* slash = (char*)Memrchr(res, '/', res_len);
                if(slash != NULL) {
                    res_len = slash - res;
                }
                continue;
            }
            break;
        case 1:
            if(ptr[0] == '.') {
                continue;
            }
            break;
        case 0:
            continue;
        }

        if(res_len == 0 && !has_drive)
            res[res_len++] = '/';
        else if(res_len)
            res[res_len++] = '/';

        memcpy(&res[res_len], ptr, len);
        res_len += len;
    }

    if(res_len == 0) {
        res[res_len++] = '/';
    }
    res[res_len] = '\0';
    return res;
}

void* Memrchr(const void* buf, int c, size_t num)
{
    char* pMem = (char*)buf;

    for(;;) {
        if(num-- == 0) {
            return NULL;
        }

        if(pMem[num] == (unsigned char)c) {
            break;
        }
    }

    return (void*)(pMem + num);
}
