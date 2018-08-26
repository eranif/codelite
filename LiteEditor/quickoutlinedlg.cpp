//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : quickoutlinedlg.cpp
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

#ifdef WX_PRECOMP

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#else
#include <wx/wx.h>
#endif // WX_PRECOMP

#include "quickoutlinedlg.h"
#include "drawingutils.h"
#include "windowattrmanager.h"
#include "cl_editor.h"
#include "cpp_symbol_tree.h"
#include "macros.h"
#include "manager.h"

extern wxImageList* CreateSymbolTreeImages();

///////////////////////////////////////////////////////////////////////////
// BEGIN_EVENT_TABLE(QuickOutlineDlg, wxDialog)
// EVT_CHAR_HOOK(QuickOutlineDlg::OnCharHook)
// EVT_TEXT(wxID_ANY, QuickOutlineDlg::OnTextEntered)
// END_EVENT_TABLE()

QuickOutlineDlg::QuickOutlineDlg(wxWindow* parent, const wxString& fileName, int id, wxString title, wxPoint pos,
                                 wxSize size, int style)
    : wxDialog(parent, id, title, pos, size, style | wxRESIZE_BORDER)
    , m_fileName(fileName)
{
    wxBoxSizer* dialogSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(dialogSizer);
    wxPanel* mainPanel = new wxPanel(this);
    dialogSizer->Add(mainPanel, 1, wxEXPAND);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    mainPanel->SetSizer(mainSizer);

#ifdef __WXGTK__
    wxColour bgCol = DrawingUtils::GetPanelBgColour();
    wxColour fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
#else
    wxColour bgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK);
    wxColour fgCol = wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT);
#endif

    mainPanel->SetBackgroundColour(bgCol);

    // build the outline view
    m_treeOutline =
        new CppSymbolTree(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER);
    m_keyboard.reset(new clTreeKeyboardInput(m_treeOutline));
    m_treeOutline->Bind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
    m_treeOutline->SetBackgroundColour(bgCol);
    m_treeOutline->SetForegroundColour(fgCol);
    m_treeOutline->SetSymbolsImages(CreateSymbolTreeImages());

    Connect(wxEVT_CMD_CPP_SYMBOL_ITEM_SELECTED, wxCommandEventHandler(QuickOutlineDlg::OnItemSelected), NULL, this);
    mainSizer->Add(m_treeOutline, 1, wxALL | wxEXPAND, 5);

    SetName("QuickOutlineDlg");
    SetMinClientSize(wxSize(500, 400));
    Layout();

    CallAfter(&QuickOutlineDlg::DoParseActiveBuffer);

    WindowAttrManager::Load(this);
    CentreOnParent();
}

QuickOutlineDlg::~QuickOutlineDlg()
{
    m_keyboard.reset(NULL);
    m_treeOutline->Unbind(wxEVT_KEY_DOWN, &QuickOutlineDlg::OnKeyDown, this);
}

void QuickOutlineDlg::OnItemSelected(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Close();
}

void QuickOutlineDlg::OnKeyDown(wxKeyEvent& e)
{
    e.Skip();
    if(e.GetKeyCode() == WXK_ESCAPE) {
        Close();
    }
}

void QuickOutlineDlg::DoParseActiveBuffer()
{
    IEditor* editor = clGetManager()->GetActiveEditor();
    if(!editor) return;
    
    wxString filename = editor->GetFileName().GetFullPath();
    TagEntryPtrVector_t tags;
    if(!TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags)) {
        // Parse and update the cache
        tags = TagsManagerST::Get()->ParseBuffer(editor->GetCtrl()->GetText(), editor->GetFileName().GetFullPath());
        TagsManagerST::Get()->GetFileCache()->Update(editor->GetFileName(), tags);
    }
    m_treeOutline->BuildTree(m_fileName, tags);
    m_treeOutline->ExpandAll();
    m_treeOutline->CallAfter(&CppSymbolTree::SetFocus);
}
