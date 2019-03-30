//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : tags_options_dlg.h
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

#ifndef __tags_options_dlg__
#define __tags_options_dlg__

#include "tags_options_base_dlg.h"
#include "serialized_object.h"
#include "wx/filename.h"
#include "tags_options_data.h"
#include "clEditorEditEventsHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// Class CodeCompletionSettingsDialog
///////////////////////////////////////////////////////////////////////////////
class wxStyledTextCtrl;
class CodeCompletionSettingsDialog : public TagsOptionsBaseDlg
{
    TagsOptionsData m_data;
    std::vector<clEditEventsHandler::Ptr_t> m_handlers;

    void SetFlag(CodeCompletionOpts flag, bool set);
    void SetColouringFlag(CodeCompletionColourOpts flag, bool set);
    void CopyData();

protected:
    virtual void OnButtonCancel(wxCommandEvent& event);
    virtual void OnButtonOk(wxCommandEvent& event);
    virtual void OnColouringPropertyValueChanged(wxPropertyGridEvent& event);
    virtual void OnSuggestCtags(wxCommandEvent& event);
    wxArrayString GetCTagsSearchPaths() const;
    void DoSuggest(wxStyledTextCtrl* textCtrl);
    void DoSetEditEventsHandler(wxWindow* win);

protected:
    virtual void OnAddExcludePath(wxCommandEvent& event);
    virtual void OnAddSearchPath(wxCommandEvent& event);
    virtual void OnAutoShowWordAssitUI(wxUpdateUIEvent& event);
    virtual void OnFileSelectedUI(wxUpdateUIEvent& event);
    virtual void OnParse(wxCommandEvent& event);

public:
    void Parse();

public:
    CodeCompletionSettingsDialog(wxWindow* parent, const TagsOptionsData& data);
    virtual ~CodeCompletionSettingsDialog();
    TagsOptionsData& GetData() { return m_data; }
};

#endif //__tags_options_dlg__
