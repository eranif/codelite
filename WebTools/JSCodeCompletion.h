//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : JSCodeCompletion.h
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

#ifndef JSCODECOMPLETION_H
#define JSCODECOMPLETION_H

#include "ieditor.h"
#include <wx/event.h>
#include "smart_ptr.h"
#include "clTernServer.h"
#include "wxCodeCompletionBoxEntry.hpp"
#include "cl_calltip.h"
#include "clJSCTags.h"
#include "ServiceProvider.h"

class WebTools;
class JSCodeCompletion : public ServiceProvider
{
    clTernServer m_ternServer;
    int m_ccPos;
    wxString m_workingDirectory;
    WebTools* m_plugin = nullptr;

public:
    typedef SmartPtr<JSCodeCompletion> Ptr_t;
    void OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename);
    void OnFunctionTipReady(clCallTipPtr calltip, const wxString& filename);
    void OnDefinitionFound(const clTernDefinition& loc);
    void OnGotoDefinition(wxCommandEvent& event);
    void OnInfoBarClicked(clCommandEvent& event);

    void OnFindSymbol(clCodeCompletionEvent& event);
    void OnCodeComplete(clCodeCompletionEvent& event);
    void OnCodeCompleteFunctionCalltip(clCodeCompletionEvent& event);
   
    /**
     * @brief start code completion based on the word completion plugin
     */
    void TriggerWordCompletion();

protected:
    bool SanityCheck();
    void DoPromptForInstallNodeJS();
    void DoPromptForInstallTern();

public:
    JSCodeCompletion(const wxString& workingDirectory, WebTools* plugin);
    virtual ~JSCodeCompletion();

    /**
     * @brief Is Java Script code completion enabled?
     */
    bool IsEnabled() const;

    /**
     * @brief code complete the current expression
     */
    void CodeComplete(IEditor* editor);

    /**
     * @brief find definition of selected expression or the word under the caret
     */
    void FindDefinition(IEditor* editor);

    /**
     * @brief notify tern to clear the cache
     * @param editor
     */
    void ResetTern(bool force);

    /**
     * @brief reparse the file
     */
    void ReparseFile(IEditor* editor);

    /**
     * @brief add java script context menu entries
     */
    void AddContextMenu(wxMenu* menu, IEditor* editor);

    /**
     * @brief restart the tern server
     */
    void Reload();

    void ClearFatalError() { m_ternServer.ClearFatalErrorFlag(); }
};

#endif // JSCODECOMPLETION_H
