//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : fc_fileopener.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef __fcfileopener__
#define __fcfileopener__

#include <vector>
#include <string>
#include <set>
#include <stack>
#include <stdio.h>
#include <list>
#include "codelite_exports.h"
#include <wx/string.h>
#include "wxStringHash.h"

typedef struct yy_buffer_state* BufferState;
struct fcState
{
    BufferState buffer;
    wxString filename;
};

class WXDLLIMPEXP_CL fcFileOpener
{
public:
    typedef std::unordered_set<wxString> Set_t;
    typedef std::vector<wxString> Vector_t;
    typedef std::stack<fcState> Stack_t;

protected:
    static fcFileOpener* ms_instance;

    fcFileOpener::Vector_t _searchPath;
    fcFileOpener::Vector_t _excludePaths;
    fcFileOpener::Set_t _matchedfiles;
    fcFileOpener::Set_t _scannedfiles;
    int _depth;
    int _maxDepth;
    fcFileOpener::Set_t _namespaces;
    fcFileOpener::Set_t _namespaceAliases;
    fcFileOpener::Set_t _includeStatements;
    fcFileOpener::Stack_t _states;
    wxString _cwd;

public:
    static fcFileOpener* Get();
    static void Release();

    FILE* try_open(const wxString& path, const wxString& name, wxString& filepath);

    // Flex buffer states
    BufferState PopBufferState();
    void PushBufferState(BufferState buffer, const wxString& filename);
    bool IsStateStackEmpty() const { return _states.empty(); }

    void SetCwd(const wxString& _cwd) { this->_cwd = _cwd; }
    void AddSearchPath(const wxString& path);
    void AddExcludePath(const wxString& path);

    /**
     * @brief open a file based on the include paths and return a pointer to it.
     * @param include_path the string as appears inside the #include statement
     * @param filepath [output] the file that was opened
     */
    FILE* OpenFile(const wxString& include_path, wxString& filepath);

    void ClearResults()
    {
        _matchedfiles.clear();
        _scannedfiles.clear();
        _namespaces.clear();
        _namespaceAliases.clear();
        _includeStatements.clear();
        _depth = 0;
        _cwd.Clear();
        while(!_states.empty()) {
            _states.pop();
        }
    }

    void ClearSearchPath()
    {
        _searchPath.clear();
        _excludePaths.clear();
    }

    void incDepth() { _depth++; }

    void decDepth()
    {
        _depth--;
        if(_depth < 0) {
            _depth = 0;
        }
    }

    int getDepth() { return _depth; }

    void setMaxDepth(const int& _maxDepth) { this->_maxDepth = _maxDepth; }

    const int& getMaxDepth() const { return _maxDepth; }

    // getters
    const fcFileOpener::Set_t& GetResults() const { return _matchedfiles; }

    ////////////////////////////////////////////////////
    // Using namespace support
    const fcFileOpener::Set_t& GetNamespaces() const { return _namespaces; }

    void ClearNamespace() { _namespaces.clear(); }
    void AddNamespace(const char* ns);

    ////////////////////////////////////////////////////
    // Namespace aliasing
    const fcFileOpener::Set_t& GetNamespaceAliases() const { return _namespaceAliases; }
    void ClearNamespaceAliases() { _namespaceAliases.clear(); }
    void AddNamespaceAlias(const char* alias) { _namespaceAliases.insert(alias); }
    void AddIncludeStatement(const wxString& statement);
    const fcFileOpener::Set_t& GetIncludeStatements() const { return _includeStatements; }
    fcFileOpener::Set_t& GetIncludeStatements() { return _includeStatements; }

private:
    fcFileOpener();
    virtual ~fcFileOpener();
};
#endif // __fcfileopener__
