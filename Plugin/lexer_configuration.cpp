//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : lexer_configuration.cpp
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
#include <wx/utils.h>
#include <wx/settings.h>
#include "globals.h"
#include "lexer_configuration.h"
#include "xmlutils.h"
#include "macros.h"
#include "wx_xml_compatibility.h"
#include "drawingutils.h"

static bool StringTolBool(const wxString &s)
{
    bool res = s.CmpNoCase(wxT("Yes")) == 0 ? true : false;
    return res;
}

LexerConf::LexerConf()
    : m_styleWithinPreProcessor(true)
{
}

void LexerConf::FromXml(wxXmlNode *element)
{
    if( element ) {
        m_lexerId = XmlUtils::ReadLong(element, wxT("Id"), 0);

        // read the lexer name
        m_name = element->GetPropVal(wxT("Name"), wxEmptyString);

        m_styleWithinPreProcessor = element->GetPropVal(wxT("StylingWithinPreProcessor"), wxT("yes")) == wxT("yes") ? true : false;

        // load key words
        wxXmlNode *node = NULL;
        node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords0"));
        if( node ) {
            m_keyWords[0] = node->GetNodeContent();
            m_keyWords[0].Replace(wxT("\n"), wxT(" "));
            m_keyWords[0].Replace(wxT("\r"), wxT(" "));
        }

        node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords1"));
        if( node ) {
            m_keyWords[1] = node->GetNodeContent();
            m_keyWords[1].Replace(wxT("\n"), wxT(" "));
            m_keyWords[1].Replace(wxT("\r"), wxT(" "));
        }

        node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords2"));
        if( node ) {
            m_keyWords[2] = node->GetNodeContent();
            m_keyWords[2].Replace(wxT("\n"), wxT(" "));
            m_keyWords[2].Replace(wxT("\r"), wxT(" "));
        }

        node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords3"));
        if( node ) {
            m_keyWords[3] = node->GetNodeContent();
            m_keyWords[3].Replace(wxT("\n"), wxT(" "));
            m_keyWords[3].Replace(wxT("\r"), wxT(" "));
        }

        node = XmlUtils::FindFirstByTagName(element, wxT("KeyWords4"));
        if( node ) {
            m_keyWords[4] = node->GetNodeContent();
            m_keyWords[4].Replace(wxT("\n"), wxT(" "));
            m_keyWords[4].Replace(wxT("\r"), wxT(" "));
        }

        // load extensions
        node = XmlUtils::FindFirstByTagName(element, wxT("Extensions"));
        if( node ) {
            m_extension = node->GetNodeContent();
        }

        // load properties
        // Search for <properties>
        node = XmlUtils::FindFirstByTagName(element, wxT("Properties"));
        if( node ) {
            // We found the element, read the attributes
            wxXmlNode* prop = node->GetChildren();
            while( prop ) {
                if(prop->GetName() == wxT("Property")) {
                    // Read the font attributes
                    wxString Name      = XmlUtils::ReadString(prop, wxT("Name"), wxT("DEFAULT"));
                    wxString bold      = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
                    wxString italic    = XmlUtils::ReadString(prop, wxT("Italic"), wxT("no"));
                    wxString eolFill   = XmlUtils::ReadString(prop, wxT("EolFilled"), wxT("no"));
                    wxString underline = XmlUtils::ReadString(prop, wxT("Underline"), wxT("no"));
                    wxString strikeout = XmlUtils::ReadString(prop, wxT("Strikeout"), wxT("no"));
                    wxString face      = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
                    wxString colour    = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
                    wxString bgcolour  = XmlUtils::ReadString(prop, wxT("BgColour"), wxT("white"));
                    long fontSize      = XmlUtils::ReadLong  (prop, wxT("Size"), 10);
                    long propId        = XmlUtils::ReadLong  (prop, wxT("Id"), 0);
                    long alpha         = XmlUtils::ReadLong  (prop, wxT("Alpha"), 50);

                    StyleProperty property = StyleProperty(propId, colour, bgcolour, fontSize, Name, face,
                                                           StringTolBool(bold),
                                                           StringTolBool(italic),
                                                           StringTolBool(underline),
                                                           StringTolBool(eolFill),
                                                           alpha);

                    m_properties.push_back( property );
                }
                prop = prop->GetNext();
            }
        }
    }
}

LexerConf::~LexerConf()
{
}

wxXmlNode *LexerConf::ToXml() const
{
    //convert the lexer back xml node
    wxXmlNode *node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Lexer"));
    //set the lexer name
    node->AddProperty(wxT("Name"), GetName());
    node->AddProperty(wxT("StylingWithinPreProcessor"), BoolToString(m_styleWithinPreProcessor));
    wxString strId;
    strId << GetLexerId();
    node->AddProperty(wxT("Id"), strId);

    //set the keywords node
    wxXmlNode *keyWords0 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords0"));
    XmlUtils::SetNodeContent(keyWords0, GetKeyWords(0));
    node->AddChild(keyWords0);

    wxXmlNode *keyWords1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords1"));
    XmlUtils::SetNodeContent(keyWords1, GetKeyWords(1));
    node->AddChild(keyWords1);

    wxXmlNode *keyWords2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords2"));
    XmlUtils::SetNodeContent(keyWords2, GetKeyWords(2));
    node->AddChild(keyWords2);

    wxXmlNode *keyWords3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords3"));
    XmlUtils::SetNodeContent(keyWords3, GetKeyWords(3));
    node->AddChild(keyWords3);

    wxXmlNode *keyWords4 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords4"));
    XmlUtils::SetNodeContent(keyWords4, GetKeyWords(4));
    node->AddChild(keyWords4);

    //set the extensions node
    wxXmlNode *extesions = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Extensions"));
    XmlUtils::SetNodeContent(extesions, GetFileSpec());
    node->AddChild(extesions);

    //set the properties
    wxXmlNode *properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Properties"));
    std::list<StyleProperty>::const_iterator iter = m_properties.begin();
    for(; iter != m_properties.end(); iter ++) {
        StyleProperty p = (*iter);
        wxXmlNode *property = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Property"));

        strId.Clear();
        strId << p.GetId();
        property->AddProperty(wxT("Id"), strId);
        property->AddProperty(wxT("Name"), p.GetName());
        property->AddProperty(wxT("Bold"), BoolToString(p.IsBold()));
        property->AddProperty(wxT("Face"), p.GetFaceName());
        property->AddProperty(wxT("Colour"), p.GetFgColour());
        property->AddProperty(wxT("BgColour"), p.GetBgColour());
        property->AddProperty(wxT("Italic"), BoolToString(p.GetItalic()));
        property->AddProperty(wxT("Underline"), BoolToString(p.GetUnderlined()));
        property->AddProperty(wxT("EolFilled"), BoolToString(p.GetEolFilled()));

        strId.Clear();
        strId << p.GetAlpha();
        property->AddProperty(wxT("Alpha"), strId);

        wxString strSize;
        strSize << p.GetFontSize();
        property->AddProperty(wxT("Size"), strSize);
        properties->AddChild(property);
    }
    node->AddChild( properties );
    return node;
}

wxFont LexerConf::GetFontForSyle(int styleId) const
{
    StylePropertyList::const_iterator iter = m_properties.begin();
    for( ; iter != m_properties.end(); ++iter ) {
        if ( iter->GetId() == styleId ) {
            wxFont font = wxFont(iter->GetFontSize(),
                                 wxFONTFAMILY_TELETYPE,
                                 iter->GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                                 iter->IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                                 iter->GetUnderlined(),
                                 iter->GetFaceName());
            return font;
        }
    }
    return wxNullFont;
}

static wxColor GetInactiveColor(const wxColor& col)
{
    wxUnusedVar(col);
#ifdef __WXGTK__
    return wxColor(wxT("GREY"));
#else
    return wxColor(wxT("LIGHT GREY"));
#endif
}

#define CL_LINE_MODIFIED_STYLE      200
#define CL_LINE_SAVED_STYLE         201
void LexerConf::Apply(wxStyledTextCtrl* ctrl)
{
    ctrl->StyleClearAll();
    ctrl->SetStyleBits(ctrl->GetStyleBitsNeeded());

    // Define the styles for the editing margin
    ctrl->StyleSetBackground(CL_LINE_MODIFIED_STYLE, wxColour(wxT("FOREST GREEN")));
    ctrl->StyleSetBackground(CL_LINE_SAVED_STYLE,    wxColour(wxT("ORANGE")));
    
    // by default indicators are set to be opaque rounded box
    ctrl->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
    ctrl->IndicatorSetStyle(2, wxSTC_INDIC_ROUNDBOX);

    //ctrl->IndicatorSetAlpha(1, 80);
    //ctrl->IndicatorSetAlpha(2, 80);

    bool tooltip(false);

    std::list<StyleProperty> styles;
    styles = GetLexerProperties();
    ctrl->SetProperty(wxT("styling.within.preprocessor"), this->GetStyleWithinPreProcessor() ? wxT("1") : wxT("0"));

    // Find the default style
    wxFont defaultFont;
    bool foundDefaultStyle = false;
    std::list<StyleProperty>::iterator iter = styles.begin();
    for (; iter != styles.end(); iter++) {
        if(iter->GetId() == 0) {
            defaultFont = wxFont(iter->GetFontSize(),
                                 wxFONTFAMILY_TELETYPE,
                                 iter->GetItalic() ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                                 iter->IsBold() ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                                 iter->GetUnderlined(),
                                 iter->GetFaceName());
            foundDefaultStyle = true;
            break;
        }
    }

    if (foundDefaultStyle) {
        for(int i=0; i<256; i++) {
            ctrl->StyleSetFont(i, defaultFont);
        }
    }

    iter = styles.begin();
    for (; iter != styles.end(); iter++) {

        StyleProperty sp        = (*iter);
        int           size      = sp.GetFontSize();
        wxString      face      = sp.GetFaceName();
        bool          bold      = sp.IsBold();
        bool          italic    = sp.GetItalic();
        bool          underline = sp.GetUnderlined();
        //int           alpha     = sp.GetAlpha();

        // handle special cases
        if ( sp.GetId() == FOLD_MARGIN_ATTR_ID ) {

            // fold margin foreground colour
            ctrl->SetFoldMarginColour(true, sp.GetBgColour());
            ctrl->SetFoldMarginHiColour(true, sp.GetFgColour());

        } else if ( sp.GetId() == SEL_TEXT_ATTR_ID ) {

            // selection colour
            if(wxColour(sp.GetBgColour()).IsOk()) {
                ctrl->SetSelBackground(true, sp.GetBgColour());
            }

        } else if ( sp.GetId() == CARET_ATTR_ID ) {

            // caret colour
            wxColour caretColour = sp.GetFgColour();
            if ( !caretColour.IsOk() ) {
                caretColour = *wxBLACK;
            }
            ctrl->SetCaretForeground( caretColour );

        } else {
            int fontSize( size );

            wxFont font = wxFont(size, wxFONTFAMILY_TELETYPE, italic ? wxITALIC : wxNORMAL , bold ? wxBOLD : wxNORMAL, underline, face);
            if (sp.GetId() == 0) { //default
                ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
                ctrl->StyleSetSize(wxSTC_STYLE_DEFAULT, size);
                ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, (*iter).GetFgColour());

                // Inactive state is greater by 64 from its counterpart
                wxColor inactiveColor = GetInactiveColor((*iter).GetFgColour());
                ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT + 64, inactiveColor);
                ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT + 64,       font);
                ctrl->StyleSetSize(wxSTC_STYLE_DEFAULT + 64,       size);

                ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, (*iter).GetBgColour());
                ctrl->StyleSetSize(wxSTC_STYLE_LINENUMBER, size);
                ctrl->SetFoldMarginColour(true, (*iter).GetBgColour());
                ctrl->SetFoldMarginHiColour(true, (*iter).GetBgColour());

                // test the background colour of the editor, if it is considered "dark"
                // set the indicator to be hollow rectanlgle
                StyleProperty sp = (*iter);
                if ( DrawingUtils::IsDark(sp.GetBgColour()) ) {
                    ctrl->IndicatorSetStyle(1, wxSTC_INDIC_BOX);
                    ctrl->IndicatorSetStyle(2, wxSTC_INDIC_BOX);
                }
            } else if(sp.GetId() == wxSTC_STYLE_CALLTIP) {
                tooltip = true;
                if(sp.GetFaceName().IsEmpty()) {
                    font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                    fontSize = font.GetPointSize();
                }
            }

            ctrl->StyleSetFont(sp.GetId(), font);
            ctrl->StyleSetSize(sp.GetId(), fontSize);
            ctrl->StyleSetEOLFilled(sp.GetId(), iter->GetEolFilled());

            if(iter->GetId() == LINE_NUMBERS_ATTR_ID) {
                // Set the line number colours only if requested
                // otherwise, use default colours provided by scintilla
                if(sp.GetBgColour().IsEmpty() == false)
                    ctrl->StyleSetBackground(sp.GetId(), sp.GetBgColour());

                if(sp.GetFgColour().IsEmpty() == false)
                    ctrl->StyleSetForeground(sp.GetId(), sp.GetFgColour());
                else
                    ctrl->StyleSetForeground(sp.GetId(), wxT("BLACK"));

            } else {
                ctrl->StyleSetForeground(sp.GetId(), sp.GetFgColour());

                // Inactive state is greater by 64 from its counterpart
                wxColor inactiveColor = GetInactiveColor(iter->GetFgColour());
                ctrl->StyleSetForeground(sp.GetId() + 64, inactiveColor);
                ctrl->StyleSetFont(sp.GetId() + 64,       font);
                ctrl->StyleSetSize(sp.GetId() + 64,       size);

                ctrl->StyleSetBackground(sp.GetId(), sp.GetBgColour());
            }
        }
    }

    // set the calltip font
    if( !tooltip ) {
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        ctrl->StyleSetFont(wxSTC_STYLE_CALLTIP, font);
    }
}
