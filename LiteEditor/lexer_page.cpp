//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_page.cpp
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

#include "lexer_page.h"
#include "lexer_configuration.h"
#include "attribute_style.h"
#include <wx/font.h>
#include "editor_config.h"
#include "macros.h"
#include "free_text_dialog.h"
#include "syntaxhighlightbasedlg.h"

///////////////////////////////////////////////////////////////////////////
LexerPage::LexerPage( wxWindow* parent, LexerConfPtr lexer, int id, wxPoint pos, wxSize size, int style )
    : LexerPageBase(parent, id, pos, wxSize(1, 1), style)
    , m_lexer(lexer)
    , m_selection(0)
    , m_isModified(false)
{
    m_properties->SetSizeHints(200, -1);
    m_propertyList = m_lexer->GetLexerProperties();
    std::list<StyleProperty>::iterator it = m_propertyList.begin();
    StyleProperty selTextProperties;

    for (; it != m_propertyList.end(); it++) {
        if (it->GetId() != SEL_TEXT_ATTR_ID) {
            m_properties->Append((*it).GetName());

        } else {
            selTextProperties = *it;

        }
    }

    if(m_properties->GetCount())
        m_properties->SetSelection(0);

    wxString initialColor     = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT).GetAsString();
    wxString bgInitialColor   = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW).GetAsString();
    wxFont   initialFont      = wxNullFont;
    //bool     initialEolFilled (false);
    bool     initialStyleWithinPreProcessor (true);

    if (m_propertyList.empty() == false) {
        StyleProperty p;
        p = (*m_propertyList.begin());
        initialColor = p.GetFgColour();
        bgInitialColor = p.GetBgColour();

        int size = p.GetFontSize();
        wxString face = p.GetFaceName();
        bool bold = p.IsBold();
        initialFont = wxFont(size, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, false, face);
        //initialEolFilled = p.GetEolFilled();
    }
    initialStyleWithinPreProcessor = m_lexer->GetStyleWithinPreProcessor();


    m_fontPicker->SetSelectedFont(initialFont);
    m_colourPicker->SetColour(wxColour(initialColor));
    m_bgColourPicker->SetColour(wxColour(bgInitialColor));
    m_globalFontPicker->SetSelectedFont(initialFont);
    m_globalBgColourPicker->SetColour(wxColour(bgInitialColor));
    m_fileSpec->SetValue(m_lexer->GetFileSpec());
    m_styleWithinPreProcessor->SetValue(initialStyleWithinPreProcessor);


    // Update selected text properties
    m_colourPickerSelTextBgColour->SetColour( selTextProperties.GetBgColour() );


    if (m_propertyList.empty()) {
        m_fontPicker->Enable(false);
        m_colourPicker->Enable(false);
    }
}

void LexerPage::OnItemSelected(wxCommandEvent & event)
{
    // update colour picker & font pickers
    wxString selectionString = event.GetString();
    m_selection = event.GetSelection();

    std::list<StyleProperty>::iterator iter = m_propertyList.begin();
    for (; iter != m_propertyList.end(); iter++) {
        if (iter->GetName() == selectionString) {
            // update font & color
            StyleProperty p = (*iter);
            wxString colour = p.GetFgColour();
            wxString bgColour = p.GetBgColour();
            wxFont font = wxNullFont;

            int size = p.GetFontSize();
            wxString face = p.GetFaceName();
            bool bold = p.IsBold();

            font = wxFont(size, wxFONTFAMILY_TELETYPE, p.GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL, bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL, p.GetUnderlined(), face);
            m_fontPicker->SetSelectedFont(font);
            m_bgColourPicker->SetColour(bgColour);
            m_colourPicker->SetColour(colour);
            m_eolFilled->SetValue( p.GetEolFilled() );
        }
    }
}

void LexerPage::OnFontChanged(wxFontPickerEvent &event)
{
    m_isModified = true;
    // update f
    wxObject *obj = event.GetEventObject();
    if (obj == m_fontPicker) {
        wxFont f = event.GetFont();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();

        iter->SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
        iter->SetFaceName(f.GetFaceName());
        iter->SetFontSize(f.GetPointSize());
        iter->SetUnderlined(f.GetUnderlined());
        iter->SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);

    } else if (obj == m_globalFontPicker) {
        wxFont f = event.GetFont();
        std::list<StyleProperty>::iterator iter = m_propertyList.begin();
        for (; iter != m_propertyList.end(); iter++) {
            iter->SetBold(f.GetWeight() == wxFONTWEIGHT_BOLD);
            iter->SetFaceName(f.GetFaceName());
            iter->SetFontSize(f.GetPointSize());
            iter->SetUnderlined(f.GetUnderlined());
            iter->SetItalic(f.GetStyle() == wxFONTSTYLE_ITALIC);
        }
        //update the style f picker as well
        m_fontPicker->SetSelectedFont(f);
    }
}

void LexerPage::OnColourChanged(wxColourPickerEvent &event)
{
    m_isModified = true;
    //update colour
    wxObject *obj = event.GetEventObject();
    if (obj == m_colourPicker) {
        wxColour colour = event.GetColour();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();
        iter->SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if (obj == m_bgColourPicker) {

        wxColour colour = event.GetColour();
        std::list<StyleProperty>::iterator iter = GetSelectedStyle();

        iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));

    } else if (obj == m_globalBgColourPicker) {

        wxColour colour = event.GetColour();
        std::list<StyleProperty>::iterator iter = m_propertyList.begin();
        for (; iter != m_propertyList.end(); iter++) {

            // Dont change the text selection using the global font picker
            if (iter->GetName() == wxT("Text Selection"))
                continue;

            iter->SetBgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
        }

        //update the style background colour as well
        m_bgColourPicker->SetColour(colour);
    }
}

void LexerPage::SaveSettings()
{
    if (GetIsModified()) {

        //////////////////////////////////////////////
        // Update the selected text iterator
        //////////////////////////////////////////////

        std::list<StyleProperty>::iterator iter = m_propertyList.begin();
        for (; iter != m_propertyList.end(); iter++) {
            if (iter->GetId() == SEL_TEXT_ATTR_ID) {
                iter->SetBgColour(m_colourPickerSelTextBgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX));

                break;
            }
        }

        m_lexer->SetProperties( m_propertyList );
        m_lexer->SetFileSpec( m_fileSpec->GetValue() );

        m_isModified = false;
    }

}

void LexerPage::OnEditKeyWordsButton0(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(0);
}

void LexerPage::OnEditKeyWordsButton1(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(1);
}

void LexerPage::OnEditKeyWordsButton2(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(2);
}

void LexerPage::OnEditKeyWordsButton3(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(3);
}

void LexerPage::OnEditKeyWordsButton4(wxCommandEvent& event)
{
    wxUnusedVar(event);
    EditKeyWords(4);
}

void LexerPage::EditKeyWords(int set)
{
    wxString keywords = m_lexer->GetKeyWords(set);
    FreeTextDialog *dlg = new FreeTextDialog(this, keywords);
    if (dlg->ShowModal() == wxID_OK) {
        m_isModified = true;
        m_lexer->SetKeyWords(dlg->GetValue(), set);
    }
    dlg->Destroy();
}

void LexerPage::OnText(wxCommandEvent& e)
{
    m_isModified = true;
    e.Skip();
}

void LexerPage::OnEolFilled(wxCommandEvent& event)
{
    m_isModified = true;

    std::list<StyleProperty>::iterator iter = GetSelectedStyle();
    if(iter != m_propertyList.end())
        iter->SetEolFilled(event.IsChecked());
}

void LexerPage::OnStyleWithinPreprocessor(wxCommandEvent& event)
{
    m_isModified = true;
    m_lexer->SetStyleWithinPreProcessor(event.IsChecked());
}

void LexerPage::OnStyleWithingPreProcessorUI(wxUpdateUIEvent& event)
{
    std::list<StyleProperty>::iterator iter = GetSelectedStyle();
    if(iter == m_propertyList.end())
        event.Enable(false);

    else  if (iter->GetName() == wxT("Preprocessor"))
        event.Enable(true);

    else
        event.Enable(false);
}

void LexerPage::OnAlphaChanged(wxScrollEvent& event)
{
    event.Skip();
    m_isModified = true;
}

void LexerPage::OnSelTextChanged(wxColourPickerEvent& event)
{
    event.Skip();
    m_isModified = true;
}

void LexerPage::OnOutputViewColourChanged(wxColourPickerEvent& event)
{
    event.Skip();
    m_isModified = true;
}

std::list<StyleProperty>::iterator LexerPage::GetSelectedStyle()
{
    if(m_properties->GetCount() <= (size_t)m_selection || m_selection < 0) {
        return m_propertyList.end();
    }

    wxString styleName = m_properties->GetString(m_selection);
    if(styleName.IsEmpty())
        return m_propertyList.begin();

    std::list<StyleProperty>::iterator iter = m_propertyList.begin();
    for (; iter != m_propertyList.end(); iter++) {
        if(iter->GetName() == styleName) {
            return iter;
        }
    }
    return m_propertyList.begin();
}

void LexerPage::GetTextSelectionValues(wxString& colourstring, int& alpha) const
{
    wxUnusedVar(alpha);
    colourstring = m_colourPickerSelTextBgColour->GetColour().GetAsString(wxC2S_HTML_SYNTAX);

}

void LexerPage::SetTextSelectionValues(const wxString& colourstring, int alpha)
{
    m_colourPickerSelTextBgColour->SetColour(colourstring);

}
