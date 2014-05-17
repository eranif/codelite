#include "fc_fileopener.h"
#include <cstdio>
#include <cctype>
#include <algorithm>

fcFileOpener* fcFileOpener::ms_instance = 0;

fcFileOpener::fcFileOpener()
    : _depth(0)
    , _maxDepth(20)
{
}

fcFileOpener::~fcFileOpener()
{
}

fcFileOpener* fcFileOpener::Instance()
{
    if (ms_instance == 0) {
        ms_instance = new fcFileOpener();
    }
    return ms_instance;
}

void fcFileOpener::Release()
{
    if (ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void fcFileOpener::AddSearchPath(const std::string& path)
{
    std::string p ( path );
    normalize_path( p );
    if ( IsPathExist( p ) ) {
        return;
    }

    _searchPath.push_back( p );
}

bool fcFileOpener::IsPathExist(const std::string& path)
{
    for (size_t i=0; i<_searchPath.size(); i++) {
        if (_searchPath.at(i) == path) {
            return true;
        }
    }
    return false;
}

bool fcFileOpener::IsExcludePathExist(const std::string& path)
{
    for (size_t i=0; i<_excludePaths.size(); i++) {
        if (_excludePaths.at(i) == path) {
            return true;
        }
    }
    return false;
}

FILE* fcFileOpener::OpenFile(const std::string& include_path)
{
    if ( include_path.empty() ) {
        return NULL;
    }

    std::string mod_path ( include_path );
    static std::string trimString("\"<> \t");

    mod_path.erase(0, mod_path.find_first_not_of(trimString));
    mod_path.erase(mod_path.find_last_not_of    (trimString)+1);

    if ( _scannedfiles.find(mod_path) != _scannedfiles.end() ) {
        // we already scanned this file
        return NULL;
    }

    // try to open the file as is
    FILE *fp (NULL);

    // try to prepend the search paths
    for (size_t i=0; i<_searchPath.size(); i++) {
        fp = try_open(_searchPath.at(i), mod_path);
        if ( fp ) return fp;
    }
    _scannedfiles.insert( mod_path );
    return NULL;
}

std::string fcFileOpener::extract_path(const std::string &filePath)
{
    std::string path(filePath);
    normalize_path( path );

    size_t where = path.rfind('/');
    if ( where == std::string::npos ) {
        return "";
    }

    std::string p = path.substr(0, where);
    return p;
}

FILE* fcFileOpener::try_open(const std::string &path, const std::string &name)
{
    std::string fullpath ( path + "/" + name );
    normalize_path( fullpath );

    FILE *fp = fopen(fullpath.c_str(), "r" );
    if ( fp ) {
        _scannedfiles.insert( name );
        std::string p = extract_path(fullpath);

        for(size_t i=0; i<_excludePaths.size(); i++) {
            size_t where = p.find(_excludePaths.at(i));
            if(where != std::string::npos && where == 0) {
                // the matched file is locatd inside an excluded directory
                fclose ( fp );
                return NULL;
            }
        }

        _matchedfiles.insert( fullpath );
        return fp;
    }
    return NULL;
}

void fcFileOpener::AddExcludePath(const std::string& path)
{
    std::string normalizedPath ( path );
    normalize_path( normalizedPath );

    if ( IsExcludePathExist(normalizedPath) ) {
        return;
    }
    _excludePaths.push_back(normalizedPath);
}

void fcFileOpener::normalize_path(std::string& path)
{
    for (size_t i=0; i<path.length(); i++) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }

#ifdef _WIN32
    // Under Windows, make the path lowercase
    std::transform(path.begin(), path.end(), path.begin(), tolower);
#endif
}

void fcFileOpener::AddNamespace(const char* ns)
{
    _namespaces.insert(ns);
}

void fcFileOpener::AddIncludeStatement(const std::string& statement)
{
    if( std::find(_includeStatements.begin(), _includeStatements.end(), statement) == _includeStatements.end()) {
        _includeStatements.push_back(statement);
    }
}
