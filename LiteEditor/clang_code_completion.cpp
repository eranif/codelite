//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : clang_code_completion.cpp
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

#if HAS_LIBCLANG

#include "clang_code_completion.h"
#include "clang_compilation_db_thread.h"
#include "compilation_database.h"
#include "compiler_command_line_parser.h"
#include "ctags_manager.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "includepathlocator.h"
#include "jobqueue.h"
#include "manager.h"
#include "parse_thread.h"
#include "pluginmanager.h"
#include "processreaderthread.h"
#include "procutils.h"
#include "project.h"
#include "shell_command.h"
#include "tags_options_data.h"
#include "workspace.h"
#include <memory>
#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>

#define CHECK_CLANG_ENABLED_RET() \
    if(!(TagsManagerST::Get()->GetCtagsOptions().GetClangOptions() & CC_CLANG_ENABLED)) return;

ClangCodeCompletion* ClangCodeCompletion::ms_instance = 0;

ClangCodeCompletion::ClangCodeCompletion()
    : m_allEditorsAreClosing(false)
{
    EventNotifier::Get()->Connect(wxEVT_ACTIVE_EDITOR_CHANGED, wxCommandEventHandler(ClangCodeCompletion::OnFileLoaded),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED, clCommandEventHandler(ClangCodeCompletion::OnFileSaved), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSING,
                                  wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_ALL_EDITORS_CLOSED,
                                  wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_STARTING, clBuildEventHandler(ClangCodeCompletion::OnBuildStarting), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_BUILD_ENDED, clBuildEventHandler(ClangCodeCompletion::OnBuildEnded), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(ClangCodeCompletion::OnWorkspaceClosed),
                                  NULL, this);
}

ClangCodeCompletion::~ClangCodeCompletion()
{
    EventNotifier::Get()->Disconnect(wxEVT_ACTIVE_EDITOR_CHANGED,
                                     wxCommandEventHandler(ClangCodeCompletion::OnFileLoaded), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED, clCommandEventHandler(ClangCodeCompletion::OnFileSaved), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSING,
                                     wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosing), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_ALL_EDITORS_CLOSED,
                                     wxCommandEventHandler(ClangCodeCompletion::OnAllEditorsClosed), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_STARTING, clBuildEventHandler(ClangCodeCompletion::OnBuildStarting),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_BUILD_ENDED, clBuildEventHandler(ClangCodeCompletion::OnBuildEnded), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,
                                     wxCommandEventHandler(ClangCodeCompletion::OnWorkspaceClosed), NULL, this);
}

ClangCodeCompletion* ClangCodeCompletion::Instance()
{
    if(ms_instance == 0) { ms_instance = new ClangCodeCompletion(); }
    return ms_instance;
}

void ClangCodeCompletion::Release()
{
    if(ms_instance) { delete ms_instance; }
    ms_instance = 0;
}

void ClangCodeCompletion::ClearCache() { m_clang.ClearCache(); }

void ClangCodeCompletion::CodeComplete(IEditor* editor)
{
    if(m_clang.IsBusy()) return;

    m_clang.SetContext(CTX_CodeCompletion);
    m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::DoCleanUp()
{
    CL_DEBUG(wxT("Aborting PCH caching..."));
    m_clang.Abort();
}

void ClangCodeCompletion::CancelCodeComplete()
{
    CHECK_CLANG_ENABLED_RET();
    DoCleanUp();
}

void ClangCodeCompletion::Calltip(IEditor* editor)
{
    if(m_clang.IsBusy()) return;

    m_clang.SetContext(CTX_Calltip);
    m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::OnFileLoaded(wxCommandEvent& e)
{
    e.Skip();
    CHECK_CLANG_ENABLED_RET();

    if(TagsManagerST::Get()->GetCtagsOptions().GetClangCachePolicy() == TagsOptionsData::CLANG_CACHE_ON_FILE_LOAD) {
        CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() START"));
        if(m_clang.IsBusy() || m_allEditorsAreClosing) {
            CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() ENDED"));
            return;
        }
        if(::clGetManager()->GetActiveEditor()) {
            IEditor* editor = ::clGetManager()->GetActiveEditor();
            // sanity
            if(editor->GetProjectName().IsEmpty() || editor->GetFileName().GetFullName().IsEmpty()) return;
            if(!TagsManagerST::Get()->IsValidCtagsFile(editor->GetFileName())) return;

            m_clang.SetContext(CTX_CachePCH);
            m_clang.CodeCompletion(editor);
        }
        CL_DEBUG(wxT("ClangCodeCompletion::OnFileLoaded() ENDED"));
    }
}

void ClangCodeCompletion::OnAllEditorsClosed(wxCommandEvent& e)
{
    e.Skip();
    m_allEditorsAreClosing = false;
}

void ClangCodeCompletion::OnAllEditorsClosing(wxCommandEvent& e)
{
    e.Skip();
    m_allEditorsAreClosing = true;
}

bool ClangCodeCompletion::IsCacheEmpty() { return m_clang.IsCacheEmpty(); }

void ClangCodeCompletion::WordComplete(IEditor* editor)
{
    if(m_clang.IsBusy()) return;
    m_clang.SetContext(CTX_WordCompletion);
    m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::OnFileSaved(clCommandEvent& e)
{
    e.Skip();
    CHECK_CLANG_ENABLED_RET();

    if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISABLE_AUTO_PARSING) {
        clDEBUG1() << "ClangCodeCompletion::OnFileSaved: Auto-parsing of saved files is disabled";
        return;
    }

    // Incase a file has been saved, we need to reparse its translation unit
    wxFileName fn(e.GetString());
    if(!TagsManagerST::Get()->IsValidCtagsFile(fn)) return;

    m_clang.ReparseFile(fn.GetFullPath());
}

void ClangCodeCompletion::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    CHECK_CLANG_ENABLED_RET();

    // Clear environment variables previously set by this class
    ::wxUnsetEnv("CL_COMPILATION_DB");
    ::wxUnsetEnv("CXX");
    ::wxUnsetEnv("CC");

    // Clear the TU cache
    ClearCache();
}

void ClangCodeCompletion::OnBuildStarting(clBuildEvent& e)
{
    e.Skip();
    CHECK_CLANG_ENABLED_RET();

    // Determine the compilation database
    CompilationDatabase cdb;
    cdb.Open();
    cdb.Close();

    // Set the compilation database environment variable
    ::wxSetEnv(wxT("CL_COMPILATION_DB"), cdb.GetFileName().GetFullPath());

    // If this is NOT a custom project, set the CXX and CC environment
    wxString project = e.GetProjectName();
    wxString config = e.GetConfigurationName();

    BuildConfigPtr bldConf = clCxxWorkspaceST::Get()->GetProjBuildConf(project, config);
    if(bldConf && !bldConf->IsCustomBuild()) {
        wxString cxx = bldConf->GetCompiler()->GetTool(wxT("CXX"));
        wxString cc = bldConf->GetCompiler()->GetTool(wxT("CC"));

        // CMake does not handle backslahses pretty well...
        cxx.Replace("\\", "/");
        cc.Replace("\\", "/");

        cxx.Prepend(wxT("codelite-cc "));
        cc.Prepend(wxT("codelite-cc "));

        ::wxSetEnv("CXX", cxx);
        ::wxSetEnv("CC", cc);
    }
}

void ClangCodeCompletion::GotoDeclaration(IEditor* editor)
{
    if(m_clang.IsBusy()) return;

    m_clang.SetContext(CTX_GotoDecl);
    m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::GotoImplementation(IEditor* editor)
{
    if(m_clang.IsBusy()) return;

    m_clang.SetContext(CTX_GotoImpl);
    m_clang.CodeCompletion(editor);
}

void ClangCodeCompletion::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    ClearCache();
}

#endif // HAS_LIBCLANG
