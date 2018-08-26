//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : php_parser_thread.h
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

#ifndef PHPPARSERTHREAD_H
#define PHPPARSERTHREAD_H

#include <worker_thread.h>


class PHPParserThreadRequest : public ThreadRequest
{
public:
    enum ePHPParserThreadRequestType {
        kParseWorkspaceFilesFull,
        kParseWorkspaceFilesQuick,
        kParseSingleFile,
    };
    
    ePHPParserThreadRequestType requestType;
    wxArrayString files;
    wxString workspaceFile;
    wxString file;
    wxArrayString frameworksPaths;
    
public:
    PHPParserThreadRequest(ePHPParserThreadRequestType type)
        : requestType(type)
    {
    }
    virtual ~PHPParserThreadRequest() {}
};

class PHPParserThread : public WorkerThread
{
    static PHPParserThread* ms_instance;
    static bool ms_goingDown;
    
public:
    static PHPParserThread* Instance();
    static void Release();

private:
    PHPParserThread();
    virtual ~PHPParserThread();
    void ParseFiles(PHPParserThreadRequest* request);
    void ParseFile(PHPParserThreadRequest* request);

public:
    virtual void ProcessRequest(ThreadRequest* request);
    static void Clear();
};

#endif // PHPPARSERTHREAD_H
