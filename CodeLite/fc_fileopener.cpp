//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : fc_fileopener.cpp
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

#include "fc_fileopener.h"
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <wx/filename.h>
#include <wx/filefn.h>

#ifdef __WXMSW__
#define FC_PATH_SEP "\\"
#else
#define FC_PATH_SEP "/"
#endif

fcFileOpener* fcFileOpener::ms_instance = 0;

fcFileOpener::fcFileOpener()
    : _depth(0)
    , _maxDepth(20)
{
}

fcFileOpener::~fcFileOpener() {}

fcFileOpener* fcFileOpener::Get()
{
    if(ms_instance == 0) {
        ms_instance = new fcFileOpener();
    }
    return ms_instance;
}

void fcFileOpener::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void fcFileOpener::AddSearchPath(const wxString& path)
{
    wxFileName fn(path, "");
    if(!wxFileName::DirExists(fn.GetPath())) return;
    _searchPath.push_back(fn.GetPath());
}

FILE* fcFileOpener::OpenFile(const wxString& include_path, wxString& filepath)
{
    filepath.Clear();
    if(include_path.empty()) {
        return NULL;
    }

    wxString mod_path(include_path);

    static wxString trimString("\"<> \t");

    mod_path.erase(0, mod_path.find_first_not_of(trimString));
    mod_path.erase(mod_path.find_last_not_of(trimString) + 1);

    if(_scannedfiles.count(mod_path)) {
        // we already scanned this file
        filepath.Clear();
        return NULL;
    }

    FILE* fp(NULL);

    // first try to cwd
    fp = try_open(_cwd, mod_path, filepath);
    if(fp) {
        return fp;
    }

    // Now try the search directories
    for(size_t i = 0; i < _searchPath.size(); ++i) {
        fp = try_open(_searchPath.at(i), mod_path, filepath);
        if(fp) return fp;
    }

    _scannedfiles.insert(mod_path);
    filepath.Clear();
    return NULL;
}

FILE* fcFileOpener::try_open(const wxString& path, const wxString& name, wxString& filepath)
{
    wxString fullpath(path + FC_PATH_SEP + name);
    wxFileName fn(fullpath);

    fullpath = fn.GetFullPath();
    FILE* fp = wxFopen(fullpath, "rb");
    if(fp) {

        _scannedfiles.insert(name);
        wxString pathPart = fn.GetPath();

        for(size_t i = 0; i < _excludePaths.size(); ++i) {
            if(pathPart.StartsWith(_excludePaths.at(i))) {
                ::fclose(fp);
                return NULL;
            }
        }

        _matchedfiles.insert(fullpath);
        filepath = fullpath;
        return fp;
    }
    return NULL;
}

void fcFileOpener::AddExcludePath(const wxString& path)
{
    wxFileName fn(path, "");
    if(!wxFileName::DirExists(fn.GetPath())) return;
    _excludePaths.push_back(fn.GetPath());
}

void fcFileOpener::AddNamespace(const char* ns) { _namespaces.insert(ns); }

void fcFileOpener::AddIncludeStatement(const wxString& statement)
{
    if(_includeStatements.count(statement) == 0) {
        _includeStatements.insert(statement);
    }
}

BufferState fcFileOpener::PopBufferState()
{
    if(_states.empty()) {
        return NULL;
    }

    fcState curstate = _states.top();
    BufferState state = curstate.buffer;

    // update the current directory
    _cwd = wxFileName(curstate.filename).GetPath();

    _states.pop();
    decDepth();
    return state;
}

void fcFileOpener::PushBufferState(BufferState buffer, const wxString& filename)
{
    fcState curstate;
    curstate.buffer = buffer;
    curstate.filename = filename;
    _states.push(curstate);

    // update the current directory
    _cwd = wxFileName(curstate.filename).GetPath();
    incDepth();
}
