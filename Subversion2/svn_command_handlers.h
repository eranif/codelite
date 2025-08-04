//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2009 by Eran Ifrah
// file name            : svn_command_handlers.h
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

#ifndef SVNCOMMITHANDLER_H
#define SVNCOMMITHANDLER_H

#include "svn_default_command_handler.h" // Base class
#include "project.h"

//----------------------------------------------------
// Commit Handler
//----------------------------------------------------

class SvnCommitHandler : public SvnDefaultCommandHandler
{
public:
    SvnCommitHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
    {
    }
    ~SvnCommitHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Update Handler
//----------------------------------------------------

class SvnUpdateHandler : public SvnDefaultCommandHandler
{

public:
    SvnUpdateHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
    {
    }
    ~SvnUpdateHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Diff Handler
//----------------------------------------------------

class SvnDiffHandler : public SvnDefaultCommandHandler
{

public:
    SvnDiffHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
    {
    }
    ~SvnDiffHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Patch Handler
//----------------------------------------------------

class SvnPatchHandler : public SvnDefaultCommandHandler
{
    bool     delFileWhenDone;
    wxString patchFile;
public:
    SvnPatchHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner, bool d, const wxString& pf)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
        , delFileWhenDone(d)
        , patchFile(pf)
    {
    }
    ~SvnPatchHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Dry-Run Patch Handler
//----------------------------------------------------

class SvnPatchDryRunHandler : public SvnDefaultCommandHandler
{
    bool     delFileWhenDone;
    wxString patchFile;

public:
    SvnPatchDryRunHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner, bool d, const wxString& pf)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
        , delFileWhenDone(d)
        , patchFile(pf)
    {
    }
    ~SvnPatchDryRunHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
//Svn version handler
//----------------------------------------------------

class SvnVersionHandler : public SvnDefaultCommandHandler
{

public:
    SvnVersionHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
    {
    }
    ~SvnVersionHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
//Svn Log handler
//----------------------------------------------------

class SvnLogHandler : public SvnDefaultCommandHandler
{
    bool     m_compact;
    wxString m_url;

protected:
    wxString Compact(const wxString &message);
public:
    SvnLogHandler(Subversion2 *plugin, const wxString &url, bool compact, int commandId, wxEvtHandler *owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
        , m_compact(compact)
        , m_url(url)
    {}

    ~SvnLogHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Svn Checkout handler
//----------------------------------------------------

class SvnCheckoutHandler : public SvnDefaultCommandHandler
{
public:
    SvnCheckoutHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
    {
    }
    ~SvnCheckoutHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Svn blame handler
//----------------------------------------------------

class SvnBlameHandler : public SvnCommandHandler
{
    wxString m_filename;
public:
    SvnBlameHandler(Subversion2* plugin, int commandId, wxEvtHandler* owner, const wxString& filename)
        : SvnCommandHandler(plugin, commandId, owner)
        , m_filename(filename)
    {
    }
    ~SvnBlameHandler() override = default;

public:
    void Process(const wxString& output) override;
};

//----------------------------------------------------
// Svn Repo List handler
//----------------------------------------------------

class SvnRepoListHandler : public SvnDefaultCommandHandler
{
    ProjectPtr m_proj;
    wxString m_workDir;
    bool m_excludeBin;
    wxString m_excludeExtensions;

public:
    SvnRepoListHandler(Subversion2* plugin,
                       ProjectPtr proj,
                       const wxString& workDir,
                       bool excludeBin,
                       const wxString& excludeExtensions,
                       int commandId,
                       wxEvtHandler* owner)
        : SvnDefaultCommandHandler(plugin, commandId, owner)
        , m_proj(proj)
        , m_workDir(workDir)
        , m_excludeBin(excludeBin)
        , m_excludeExtensions(excludeExtensions)
    {
    }
    ~SvnRepoListHandler() override = default;

public:
    void Process(const wxString& output) override;
};

#endif // SVNCOMMITHANDLER_H
