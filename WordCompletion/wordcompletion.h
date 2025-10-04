//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : wordcompletion.h
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

#ifndef __WordCompletion__
#define __WordCompletion__

#include "UI.h"
#include "WordCompletionRequestReply.h"
#include "cl_command_event.h"
#include "macros.h"
#include "plugin.h"

class WordCompletionDictionary;
class WordCompletionPlugin;
class WordCompleter : public wxEvtHandler
{
    WordCompletionPlugin* m_plugin = nullptr;

protected:
    void OnWordComplete(clCodeCompletionEvent& event);

public:
    WordCompleter(WordCompletionPlugin* plugin);
    virtual ~WordCompleter();
};

class WordCompletionPlugin : public IPlugin
{
    WordCompletionDictionary* m_dictionary = nullptr;
    WordCompleter* m_completer = nullptr;
    friend class WordCompleter;

private:
    IEditor* GetEditor(const wxString& filepath) const;

public:
    void OnWordComplete(clCodeCompletionEvent& event);
    void OnSettings(wxCommandEvent& event);

public:
    WordCompletionPlugin(IManager* manager);
    ~WordCompletionPlugin() override = default;

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    void CreateToolBar(clToolBarGeneric* toolbar) override;
    void CreatePluginMenu(wxMenu* pluginsMenu) override;
    void UnPlug() override;
};

#endif // WordCompletion
