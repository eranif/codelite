#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "sqlite3.h"
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

std::string extract_file_name( const std::string& line );
char * normalize_path(const char * src, size_t src_len);

void * Memrchr(const void *buf, int c, size_t num);

void SaveFileFlags(const std::string &db, const std::string &filename, const std::string &flags);

#ifndef _WIN32
extern int ExecuteProcessUNIX(const std::string& commandline);
#else
extern int ExecuteProcessWIN(const std::string& commandline);
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
	
	const char *pdb = getenv("CL_COMPILATION_DB");
    std::string commandline;
    for ( int i=1; i<argc; ++i ) {
        // Wrap all arguments with spaces with double quotes
        std::string arg = argv[i];
        
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
        std::string file_name = extract_file_name(commandline);
        if(file_name.empty() == false) {
            SaveFileFlags(pdb, file_name, commandline);
        }
    }

    int exitCode = 0;
#ifdef _WIN32
    exitCode = ::ExecuteProcessWIN(commandline);
#else
    exitCode = ::ExecuteProcessUNIX(commandline);
#endif

    return exitCode;
}

void SaveFileFlags(const std::string& dbname, const std::string& filename, const std::string& flags)
{
    if( filename.empty() ) {
        //fprintf(stderr, "empty filename !\n");
        return;
    }

    if( flags.empty() ) {
        //fprintf(stderr, "empty flags !\n");
        return;
    }
    
    sqlite3* db = NULL;
    int rc = sqlite3_open(dbname.c_str(), &db);
    if ( rc != SQLITE_OK) {
        return;
    }
    
    sqlite3_busy_timeout(db, 1000); // Busy handler for 1 second
    
    sqlite3_stmt* statement = NULL;
    // Prepare the statement
    if( SQLITE_OK != sqlite3_prepare_v2(db, "REPLACE INTO COMPILATION_TABLE (FILE_NAME, COMPILE_FLAGS) VALUES (?, ?)", -1, &statement, NULL) ) {
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(statement, 1, filename.c_str(), -1, NULL);
    sqlite3_bind_text(statement, 2, flags.c_str(), -1, NULL);
    if ( sqlite3_step(statement) != SQLITE_DONE ) {
        fprintf(stderr, "codelitegcc: unable to save compilation flags for file: '%s'. %s\n", filename.c_str(), sqlite3_errmsg(db));
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
}

std::string extract_file_name(const std::string& line)
{
    std::vector<std::string> extensions;
    extensions.push_back(".cpp");
    extensions.push_back(".cxx");
    extensions.push_back(".cc");
    extensions.push_back(".c");

    // locate any of the known c/cpp extensions
    size_t where = std::string::npos;

    std::string ext;
    for(size_t i=0; i<extensions.size(); i++) {
        where = line.find( extensions.at(i) );
        if( where != std::string::npos ) {
            ext = extensions.at(i);
            break;
        }
    }

    if ( ext.empty() )
        return "";

    if ( (where + ext.length() < line.length()) && line.at(where + ext.length()) != ' ' && line.at(where + ext.length()) != '"' )
        return "";

    std::string filename;
    if (line.at(where + ext.length()) == ' ') {
        // go backward until we find the first space
        size_t start = line.rfind(' ', where);
        if( start == std::string::npos )
            start = 0;
        filename = line.substr(start + 1, where + ext.length() - start);
    }

    if (line.at(where + ext.length()) == '"') {
        // go backward until we find the first space
        size_t start = line.rfind('"', where);
        if( start == std::string::npos )
            start = 0;
        filename = line.substr(start + 1, where + ext.length() - start);
    }
    
    std::replace(filename.begin(), filename.end(), '\\', '/');
    char* ret = normalize_path( filename.c_str(), filename.length() );
    filename = ret;
    free(ret);
    
#ifdef _WIN32
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif
    
    // trim whitespaces
    
    // rtrim
    filename.erase(0, filename.find_first_not_of("\t\r\v\n "));
    
    // ltrim
    filename.erase(filename.find_last_not_of("\t\r\v\n ")+1);
    
    return filename;
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

        char pwd[PATH_MAX];
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
