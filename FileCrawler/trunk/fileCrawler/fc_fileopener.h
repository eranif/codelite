#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>
#include <stdio.h>
#include <list>

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#    define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#    define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#    define WXDLLIMPEXP_CL
#endif

#endif

class WXDLLIMPEXP_CL fcFileOpener
{
public:
    typedef std::list<std::string>   List_t;
    typedef std::set<std::string>    Set_t;
    typedef std::vector<std::string> Vector_t;

protected:
    static fcFileOpener* ms_instance       ;

    Vector_t _searchPath       ;
    Vector_t _excludePaths     ;
    Set_t    _matchedfiles     ;
    Set_t    _scannedfiles     ;
    int      _depth            ;
    int      _maxDepth         ;
    Set_t    _namespaces       ;
    Set_t    _namespaceAliases ;
    List_t   _includeStatements;

private:
    bool IsPathExist(const std::string &path);
    bool IsExcludePathExist(const std::string &path);
    static void normalize_path( std::string &path );
    FILE *try_open(const std::string &path, const std::string &name);

public:
    static fcFileOpener* Instance();
    static void Release();

    static std::string extract_path(const std::string &filePath);

    void AddSearchPath(const std::string &path);
    void AddExcludePath(const std::string &path);
    FILE *OpenFile(const std::string &include_path);

    void ClearResults()    {
        _matchedfiles.clear();
        _scannedfiles.clear();
        _namespaces.clear();
        _namespaceAliases.clear();
        _includeStatements.clear();
        _depth = 0;
    }

    void ClearSearchPath() {
        _searchPath.clear();
        _excludePaths.clear();
    }

    void incDepth() {
        _depth++;
    }

    void decDepth() {
        _depth--;
        if ( _depth < 0) {
            _depth = 0;
        }
    }

    int  getDepth() {
        return _depth;
    }

    void setMaxDepth(const int& _maxDepth) {
        this->_maxDepth = _maxDepth;
    }

    const int& getMaxDepth() const {
        return _maxDepth;
    }

    // getters
    const Set_t& GetResults() const {
        return _matchedfiles;
    }

    ////////////////////////////////////////////////////
    // Using namespace support
    const Set_t& GetNamespaces() const {
        return _namespaces;
    }

    void ClearNamespace() {
        _namespaces.clear();
    }
    void AddNamespace(const char* ns);

    ////////////////////////////////////////////////////
    // Namespace aliasing
    const Set_t& GetNamespaceAliases() const {
        return _namespaceAliases;
    }
    void ClearNamespaceAliases() {
        _namespaceAliases.clear();
    }
    void AddNamespaceAlias(const char* alias) {
        _namespaceAliases.insert(alias);
    }
    void AddIncludeStatement(const std::string& statement);
    const List_t& GetIncludeStatements() const {
        return _includeStatements;
    }
private:
    fcFileOpener();
    virtual ~fcFileOpener();


};
#endif // __fcfileopener__
