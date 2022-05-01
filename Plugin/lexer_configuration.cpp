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
#include "lexer_configuration.h"

#include "StringUtils.h"
#include "bookmark_manager.h"
#include "clSystemSettings.h"
#include "cl_config.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "macros.h"
#include "wx_xml_compatibility.h"
#include "xmlutils.h"

#include <algorithm>
#include <wx/settings.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>
namespace
{
bool StringTolBool(const wxString& s)
{
    bool res = s.CmpNoCase(wxT("Yes")) == 0 ? true : false;
    return res;
}

/**
 * @brief allocate substyles for the control for a given base style. Do nothing
 * if the substyles have already been allocated
 */
void allocate_substyles(wxStyledTextCtrl* ctrl, int base_style, size_t num)
{
    int count = ctrl->GetSubStylesLength(base_style);
    if(count == 0) {
        // allocate the maximum number of substyles that we may require
        int start_index = ctrl->AllocateSubStyles(base_style, num);
        clDEBUG() << "Allocating" << num << "substyles" << endl;
        clDEBUG() << "First substyle starting at:" << start_index << endl;
    }
}
} // namespace

LexerConf::LexerConf()
    : m_flags(kStyleInPP)
{
}

void LexerConf::FromXml(wxXmlNode* element)
{
    if(element) {
        m_flags = 0;
        m_name = element->GetPropVal(wxT("Name"), wxEmptyString);
        m_name.MakeLower();

        m_lexerId = XmlUtils::ReadLong(element, wxT("Id"), 0);
        m_themeName = XmlUtils::ReadString(element, "Theme", "Default");
        EnableFlag(kIsActive, XmlUtils::ReadBool(element, "IsActive", false));
        EnableFlag(kUseCustomTextSelectionFgColour, XmlUtils::ReadBool(element, "UseCustomTextSelFgColour", false));
        EnableFlag(kStyleInPP,
                   element->GetPropVal(wxT("StylingWithinPreProcessor"), wxT("yes")) == wxT("yes") ? true : false);

        // load key words
        wxXmlNode* node = NULL;
        for(int i = 0; i < 10; ++i) {
            wxString tagName = "KeyWords";
            tagName << i;
            node = XmlUtils::FindFirstByTagName(element, tagName);
            if(node) {
                wxString content = node->GetNodeContent();
                content.Replace("\r", "");
                content.Replace("\n", " ");
                content.Replace("\\", " ");
                m_keyWords[i] = content;
            }
        }

        // load extensions
        node = XmlUtils::FindFirstByTagName(element, wxT("Extensions"));
        if(node) {
            m_extension = node->GetNodeContent();
            // Make sure that CMake includes the CMakeLists.txt file
            if(m_lexerId == wxSTC_LEX_CMAKE) {
                if(!m_extension.Contains("CMakeLists.txt")) {
                    m_extension = "*.cmake;*.CMAKE;*CMakeLists.txt";
                }
            }
        }

        // Hack: add RawString property to the lexers if not exist
        // By default, we use the settings defined for the wxSTC_C_STRING
        // property
        bool isCxxLexer = (m_lexerId == wxSTC_LEX_CPP);
        bool hasRawString = false;

        // Keey NULL property
        StyleProperty stringProp;
        stringProp.SetId(STYLE_PROPERTY_NULL_ID);

        // load properties
        // Search for <properties>
        node = XmlUtils::FindFirstByTagName(element, wxT("Properties"));
        if(node) {
            // We found the element, read the attributes
            wxXmlNode* prop = node->GetChildren();
            while(prop) {
                if(prop->GetName() == wxT("Property")) {
                    // Read the font attributes
                    wxString Name = XmlUtils::ReadString(prop, wxT("Name"), wxT("DEFAULT"));
                    wxString bold = XmlUtils::ReadString(prop, wxT("Bold"), wxT("no"));
                    wxString italic = XmlUtils::ReadString(prop, wxT("Italic"), wxT("no"));
                    wxString eolFill = XmlUtils::ReadString(prop, wxT("EolFilled"), wxT("no"));
                    wxString underline = XmlUtils::ReadString(prop, wxT("Underline"), wxT("no"));
                    wxString strikeout = XmlUtils::ReadString(prop, wxT("Strikeout"), wxT("no"));
                    wxString face = XmlUtils::ReadString(prop, wxT("Face"), wxT("Courier"));
                    wxString colour = XmlUtils::ReadString(prop, wxT("Colour"), wxT("black"));
                    wxString bgcolour = XmlUtils::ReadString(prop, wxT("BgColour"), wxT("white"));
                    long fontSize = XmlUtils::ReadLong(prop, wxT("Size"), 10);
                    long propId = XmlUtils::ReadLong(prop, wxT("Id"), 0);
                    long alpha = XmlUtils::ReadLong(prop, wxT("Alpha"), 50);

                    // Mainly for upgrade purposes: check if already read
                    // the StringRaw propery
                    if(isCxxLexer && !hasRawString && propId == wxSTC_C_STRINGRAW) {
                        hasRawString = true;
                    }
                    StyleProperty property =
                        StyleProperty(propId, colour, bgcolour, fontSize, Name, face, StringTolBool(bold),
                                      StringTolBool(italic), StringTolBool(underline), StringTolBool(eolFill), alpha);
                    if(isCxxLexer && propId == wxSTC_C_STRING) {
                        stringProp = property;
                    }
                    m_properties.push_back(property);
                }
                prop = prop->GetNext();
            }

            // If we don't have the raw string style property,
            // copy the string property and add it
            if(isCxxLexer && !hasRawString && !stringProp.IsNull()) {
                stringProp.SetId(wxSTC_C_STRINGRAW);
                stringProp.SetName("Raw String");
                m_properties.push_back(stringProp);
            }
        }
    }
}

LexerConf::~LexerConf() {}

wxXmlNode* LexerConf::ToXml() const
{
    // convert the lexer back xml node
    wxXmlNode* node = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Lexer"));
    // set the lexer name
    node->AddProperty(wxT("Name"), GetName());
    node->AddProperty("Theme", GetThemeName());
    node->AddProperty("IsActive", IsActive() ? "Yes" : "No");
    node->AddAttribute("UseCustomTextSelFgColour", IsUseCustomTextSelectionFgColour() ? "Yes" : "No");
    node->AddProperty(wxT("StylingWithinPreProcessor"), BoolToString(GetStyleWithinPreProcessor()));
    wxString strId;
    strId << GetLexerId();
    node->AddProperty(wxT("Id"), strId);

    // set the keywords node
    wxXmlNode* keyWords0 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords0"));
    XmlUtils::SetNodeContent(keyWords0, GetKeyWords(0));
    node->AddChild(keyWords0);

    wxXmlNode* keyWords1 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords1"));
    XmlUtils::SetNodeContent(keyWords1, GetKeyWords(1));
    node->AddChild(keyWords1);

    wxXmlNode* keyWords2 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords2"));
    XmlUtils::SetNodeContent(keyWords2, GetKeyWords(2));
    node->AddChild(keyWords2);

    wxXmlNode* keyWords3 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords3"));
    XmlUtils::SetNodeContent(keyWords3, GetKeyWords(3));
    node->AddChild(keyWords3);

    wxXmlNode* keyWords4 = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("KeyWords4"));
    XmlUtils::SetNodeContent(keyWords4, GetKeyWords(4));
    node->AddChild(keyWords4);

    // set the extensions node
    wxXmlNode* extesions = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Extensions"));
    XmlUtils::SetNodeContent(extesions, GetFileSpec());
    node->AddChild(extesions);

    // set the properties
    wxXmlNode* properties = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Properties"));
    for(const StyleProperty& p : m_properties) {
        wxXmlNode* property = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("Property"));
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
    node->AddChild(properties);
    return node;
}

wxFont LexerConf::GetFontForSyle(int styleId, const wxWindow* win) const
{
    const auto& prop = GetProperty(styleId);
    if(prop.IsNull()) {
        return DrawingUtils::GetFallbackFixedFont(win);
    }

    if(prop.GetFaceName().empty()) {
        // no face name, use the fallback font
        return DrawingUtils::GetFallbackFixedFont(win, prop.IsBold());
    }

    int fontSize(prop.GetFontSize());
    const wxString& face = prop.GetFaceName();

    wxFontInfo fontInfo = wxFontInfo(DrawingUtils::FixFontSize(fontSize, win))
                              .Family(wxFONTFAMILY_MODERN)
                              .Italic(prop.GetItalic())
                              .Bold(prop.IsBold())
                              .Underlined(prop.GetUnderlined())
                              .FaceName(face);
    wxFont font(fontInfo);
    return font;
}

namespace
{
wxColor GetInactiveColor(const StyleProperty& defaultStyle)
{
    wxColour fg = defaultStyle.GetFgColour();
    wxColour bg = defaultStyle.GetBgColour();
    bool is_dark = DrawingUtils::IsDark(bg);
    return fg.ChangeLightness(is_dark ? 50 : 150);
}
wxColour to_wx_colour(const wxString& colour_as_string) { return wxColour(colour_as_string); }
} // namespace

#define CL_LINE_MODIFIED_STYLE 200
#define CL_LINE_SAVED_STYLE 201

void LexerConf::Apply(wxStyledTextCtrl* ctrl, bool applyKeywords)
{
    ctrl->SetLexer(GetLexerId());
    ctrl->StyleClearAll();
#if wxCHECK_VERSION(3, 1, 0)
    ctrl->FoldDisplayTextSetStyle(wxSTC_FOLDDISPLAYTEXT_BOXED);
    ctrl->SetIdleStyling(wxSTC_IDLESTYLING_TOVISIBLE);
#endif

#ifndef __WXMSW__
    ctrl->SetStyleBits(ctrl->GetStyleBitsNeeded());
#endif

#if defined(__WXMSW__)
    bool useDirect2D = clConfig::Get().Read("Editor/UseDirect2D", true);
    ctrl->SetTechnology(useDirect2D ? wxSTC_TECHNOLOGY_DIRECTWRITE : wxSTC_TECHNOLOGY_DEFAULT);
    ctrl->SetBufferedDraw(true);
#elif defined(__WXGTK__)
    ctrl->SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITE);
#endif

    if(IsSubstyleSupported()) {
        allocate_substyles(ctrl, GetSubStyleBase(), sizeof(m_wordSets) / sizeof(m_wordSets[0]));
    }

    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    bool tooltip(false);

    ctrl->SetProperty(wxT("styling.within.preprocessor"), this->GetStyleWithinPreProcessor() ? "1" : "0");

    // turn off PP tracking/updating by default
    if(GetName() == "c++") {
        // ctrl->SetProperty(wxT("lexer.cpp.track.preprocessor"), "1");
        // ctrl->SetProperty(wxT("lexer.cpp.update.preprocessor"), "1");
    }

    if(GetName() == "scss") {
        // Enable SCSS property (will tell the lexer to search for variables)
        ctrl->SetProperty("lexer.css.scss.language", "1");
    }

    // Find the default style
    wxFont defaultFont;
    bool foundDefaultStyle = false;
    int nDefaultFontSize = DrawingUtils::GetFallbackFixedFontSize(ctrl);

    StyleProperty defaultStyle;
    for(const auto& prop : GetLexerProperties()) {
        if(prop.GetId() == 0 && !prop.IsSubstyle()) {
            defaultStyle = prop;

            wxString fontFace =
                prop.GetFaceName().IsEmpty() ? DrawingUtils::GetFallbackFixedFontFace() : prop.GetFaceName();
            if(!prop.GetFaceName().IsEmpty()) {
                nDefaultFontSize = prop.GetFontSize();
            }

            defaultFont = wxFont(wxFontInfo(nDefaultFontSize).Family(wxFONTFAMILY_MODERN).FaceName(fontFace));
            if(prop.IsBold()) {
                defaultFont.SetWeight(wxFONTWEIGHT_BOLD);
            }
            if(prop.GetUnderlined()) {
                defaultFont.SetStyle(wxFONTSTYLE_ITALIC);
            }
            foundDefaultStyle = true;
            if(!defaultFont.IsOk()) {
                clWARNING() << "Found default font, but the font is NOT OK !?" << endl;
                // make sure we have a font...
                defaultFont = DrawingUtils::GetDefaultFixedFont();
            }
            break;
        }
    }

    // Reset all colours to use the default style colour
    bool defaultFontOK = defaultFont.IsOk();
    if(foundDefaultStyle) {
        for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
            ctrl->StyleSetBackground(i, defaultStyle.GetBgColour());
            ctrl->StyleSetForeground(i, defaultStyle.GetFgColour());
            if(defaultFontOK) {
                ctrl->StyleSetFont(i, defaultFont);
            }
        }
    } else {
        clSYSTEM() << "Could not found default font!" << endl;
    }

    for(const auto& sp : GetLexerProperties()) {
        int size = nDefaultFontSize;
        wxString face = sp.GetFaceName();
        bool bold = sp.IsBold();
        bool italic = sp.GetItalic();
        bool underline = sp.GetUnderlined();

        // handle special cases
        switch(sp.GetId()) {
        case WHITE_SPACE_ATTR_ID: {
            // whitespace colour. We dont allow changing the background colour, only the foreground colour
            wxColour whitespaceColour = to_wx_colour(sp.GetFgColour());
            if(whitespaceColour.IsOk()) {
                ctrl->SetWhitespaceForeground(true, whitespaceColour);
            }
            break;
        }
        case FOLD_MARGIN_ATTR_ID:
            // fold margin foreground colour
            ctrl->SetFoldMarginColour(true, to_wx_colour(sp.GetBgColour()));
            ctrl->SetFoldMarginHiColour(true, to_wx_colour(sp.GetFgColour()));
            break;
        case SEL_TEXT_ATTR_ID: {
            // selection colour
            wxColour sel_bg_colour = sp.GetBgColour();
            wxColour sel_fg_colour = sp.GetFgColour();
            ctrl->SetSelForeground(true, sel_fg_colour);
            ctrl->SetSelBackground(true, sel_bg_colour);
            break;
        }
        case CARET_ATTR_ID: {
            // caret colour
            wxColour caretColour = to_wx_colour(sp.GetFgColour());
            if(!caretColour.IsOk()) {
                caretColour = *wxBLACK;
            }
            ctrl->SetCaretForeground(caretColour);
            break;
        }
        default: {
            wxString faceName = face;
            int fontSize(size);
            if(face.IsEmpty()) {
                // defaults
                fontSize = nDefaultFontSize;
                faceName = DrawingUtils::GetFallbackFixedFontFace();
            }
            fontSize = DrawingUtils::FixFontSize(size, ctrl);
            wxFontInfo fontInfo = wxFontInfo(fontSize)
                                      .Family(wxFONTFAMILY_MODERN)
                                      .Italic(italic)
                                      .Bold(bold)
                                      .Underlined(underline)
                                      .FaceName(faceName);
            wxFont font(fontInfo);

            if(sp.GetId() == 0) { // default
                ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
                ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, sp.GetFgColour());

                // Set the inactive state colours
                // Inactive state is greater by 64 from its counterpart
                wxColor inactiveColor = GetInactiveColor(defaultStyle);
                ctrl->StyleSetForeground(wxSTC_STYLE_DEFAULT + 64, inactiveColor);
                ctrl->StyleSetFont(wxSTC_STYLE_DEFAULT + 64, font);
                ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT + 64, sp.GetBgColour());
                ctrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, sp.GetBgColour());
                ctrl->StyleSetFont(wxSTC_STYLE_LINENUMBER + 64, font);

            } else if(sp.GetId() == wxSTC_STYLE_CALLTIP) {
                tooltip = true;
                if(sp.GetFaceName().IsEmpty()) {
                    font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
                    fontSize = font.GetPointSize();
                }
            } else {

                int style_id = sp.GetId();
                if(sp.IsSubstyle() && IsSubstyleSupported()) {
                    style_id = ctrl->GetSubStylesStart(GetSubStyleBase()) + sp.GetId();
                    clDEBUG1() << "* Found Substyle" << endl;
                    clDEBUG1() << "* Setting style:" << style_id << ". Is substyle?" << sp.IsSubstyle() << endl;
                    clDEBUG1() << "* Parent style:" << sp.GetId() << endl;
                }

                // always set the font
                ctrl->StyleSetFont(style_id, font);
                ctrl->StyleSetEOLFilled(style_id, sp.GetEolFilled());

                if(style_id != LINE_NUMBERS_ATTR_ID) {
                    ctrl->StyleSetForeground(style_id, to_wx_colour(sp.GetFgColour()));
                    // Inactive state is greater by 64 from its counterpart
                    wxColor inactiveColor = GetInactiveColor(sp);
                    ctrl->StyleSetForeground(style_id + 64, inactiveColor);
                    ctrl->StyleSetFont(style_id + 64, font);
                    // ctrl->StyleSetSize(style_id + 64, size);
                    ctrl->StyleSetBackground(style_id + 64, defaultStyle.GetBgColour());

                    ctrl->StyleSetBackground(style_id, to_wx_colour(sp.GetBgColour()));
                }
            }
        } break;
        } // switch
    }

    // set line number colours
    wxColour bg_colour = ctrl->StyleGetBackground(0);
    wxColour fg_colour = bg_colour;
    if(IsDark()) {
        fg_colour = bg_colour.ChangeLightness(140);
    } else {
        fg_colour = fg_colour.ChangeLightness(50);
    }
    ctrl->StyleSetBackground(LINE_NUMBERS_ATTR_ID, bg_colour);
    ctrl->StyleSetForeground(LINE_NUMBERS_ATTR_ID, fg_colour);

    // set the calltip font
    if(!tooltip) {
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        ctrl->StyleSetFont(wxSTC_STYLE_CALLTIP, font);
    }

    if(applyKeywords) {
        ctrl->SetKeyWords(0, GetKeyWords(0));
        ctrl->SetKeyWords(1, GetKeyWords(1));
        ctrl->SetKeyWords(2, GetKeyWords(2));
        ctrl->SetKeyWords(3, GetKeyWords(3));
        ctrl->SetKeyWords(4, GetKeyWords(4));
    }

    // by default indicators are set to be opaque rounded box
    if(DrawingUtils::IsDark(defaultStyle.GetBgColour())) {
        ctrl->IndicatorSetStyle(1, wxSTC_INDIC_BOX);
        ctrl->IndicatorSetStyle(2, wxSTC_INDIC_BOX);
    } else {
        ctrl->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
        ctrl->IndicatorSetStyle(2, wxSTC_INDIC_ROUNDBOX);
    }

    // Annotations markers
    // Warning style
    wxColour textColour = IsDark() ? *wxWHITE : *wxBLACK;
    ctrl->StyleSetBackground(ANNOTATION_STYLE_WARNING, defaultStyle.GetBgColour());
    ctrl->StyleSetForeground(ANNOTATION_STYLE_WARNING, textColour);

    // Error style
    ctrl->StyleSetBackground(ANNOTATION_STYLE_ERROR, defaultStyle.GetBgColour());
    ctrl->StyleSetForeground(ANNOTATION_STYLE_ERROR, textColour);

    // Code completion errors
    ctrl->StyleSetBackground(ANNOTATION_STYLE_CC_ERROR, defaultStyle.GetBgColour());
    ctrl->StyleSetForeground(ANNOTATION_STYLE_CC_ERROR, textColour);

    // annotation style 'boxed'
    ctrl->AnnotationSetVisible(wxSTC_ANNOTATION_BOXED);

    // Define the styles for the editing margin
    ctrl->StyleSetBackground(CL_LINE_SAVED_STYLE, wxColour(wxT("FOREST GREEN")));
    ctrl->StyleSetBackground(CL_LINE_MODIFIED_STYLE, wxColour(wxT("ORANGE")));

    // Indentation
    ctrl->SetUseTabs(options->GetIndentUsesTabs());
    ctrl->SetTabWidth(options->GetTabWidth());
    ctrl->SetIndent(options->GetIndentWidth());

    // Overide TAB vs Space settings incase the file is a makefile
    // It is not an option for Makefile to use SPACES
    if(GetName().Lower() == "makefile") {
        ctrl->SetUseTabs(true);
    }
    ctrl->SetLayoutCache(wxSTC_CACHE_PAGE);

    // caret width and blink
    int caretWidth = clConfig::Get().Read("editor/caret_width", 2);
    caretWidth = ::clGetSize(caretWidth, ctrl);
    ctrl->SetCaretWidth(caretWidth);
    ctrl->SetCaretPeriod(options->GetCaretBlinkPeriod());
}

const StyleProperty& LexerConf::GetProperty(int propertyId) const
{
    for(const auto& sp : m_properties) {
        if(sp.GetId() == propertyId) {
            return sp;
        }
    }

    static StyleProperty NullProperty;
    NullProperty.SetId(STYLE_PROPERTY_NULL_ID);
    return NullProperty;
}

StyleProperty& LexerConf::GetProperty(int propertyId)
{
    for(auto& sp : m_properties) {
        if(sp.GetId() == propertyId) {
            return sp;
        }
    }

    static StyleProperty NullProperty;
    NullProperty.SetId(STYLE_PROPERTY_NULL_ID);
    return NullProperty;
}

bool LexerConf::IsDark() const
{
    const StyleProperty& prop = GetProperty(0);
    if(prop.IsNull()) {
        return false;
    }
    return DrawingUtils::IsDark(prop.GetBgColour());
}

void LexerConf::SetDefaultFgColour(const wxColour& colour)
{
    StyleProperty& style = GetProperty(0);
    if(!style.IsNull()) {
        style.SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
    }
}

void LexerConf::SetLineNumbersFgColour(const wxColour& colour)
{
    StyleProperty& style = GetProperty(LINE_NUMBERS_ATTR_ID);
    if(!style.IsNull()) {
        style.SetFgColour(colour.GetAsString(wxC2S_HTML_SYNTAX));
    }
}

JSONItem LexerConf::ToJSON(bool forExport) const
{
    JSONItem json = JSONItem::createObject(GetName());
    json.addProperty("Name", GetName());
    json.addProperty("Theme", GetThemeName());
    json.addProperty("Flags", m_flags);
    json.addProperty("Id", GetLexerId());
    json.addProperty("KeyWords0", GetKeyWords(0));
    json.addProperty("KeyWords1", GetKeyWords(1));
    json.addProperty("KeyWords2", GetKeyWords(2));
    json.addProperty("KeyWords3", GetKeyWords(3));
    json.addProperty("KeyWords4", GetKeyWords(4));
    json.addProperty("Extensions", GetFileSpec());
    json.addProperty("SubstyleBase", GetSubStyleBase());

    JSONItem word_set = json.createArray("WordSet");
    word_set.arrayAppend(m_wordSets[WS_CLASS].to_json());     // 0
    word_set.arrayAppend(m_wordSets[WS_FUNCTIONS].to_json()); // 1
    word_set.arrayAppend(m_wordSets[WS_VARIABLES].to_json()); // 2
    word_set.arrayAppend(m_wordSets[WS_OTHERS].to_json());    // 3

    json.addProperty("WordSet", word_set);
    JSONItem properties = JSONItem::createArray("Properties");
    json.append(properties);

    for(const auto& sp : GetLexerProperties()) {
        properties.arrayAppend(sp.ToJSON(forExport));
    }
    return json;
}

void LexerConf::FromJSON(const JSONItem& json)
{
    auto word_set = json["WordSet"];
    if(word_set.arraySize() == 4) {
        m_wordSets[WS_CLASS].from_json(word_set[0]);
        m_wordSets[WS_FUNCTIONS].from_json(word_set[1]);
        m_wordSets[WS_VARIABLES].from_json(word_set[2]);
        m_wordSets[WS_OTHERS].from_json(word_set[3]);
    }

    m_name = json.namedObject("Name").toString();
    m_lexerId = json.namedObject("Id").toInt();
    m_themeName = json.namedObject("Theme").toString();
    if(json.hasNamedObject("Flags")) {
        m_flags = json.namedObject("Flags").toSize_t();
    } else {
        SetIsActive(json.namedObject("IsActive").toBool());
        SetUseCustomTextSelectionFgColour(json.namedObject("UseCustomTextSelFgColour").toBool());
        SetStyleWithinPreProcessor(json.namedObject("StylingWithinPreProcessor").toBool());
    }

    SetKeyWords(json.namedObject("KeyWords0").toString(), 0);
    SetKeyWords(json.namedObject("KeyWords1").toString(), 1);
    SetKeyWords(json.namedObject("KeyWords2").toString(), 2);
    SetKeyWords(json.namedObject("KeyWords3").toString(), 3);
    SetKeyWords(json.namedObject("KeyWords4").toString(), 4);
    SetFileSpec(json.namedObject("Extensions").toString());
    SetSubstyleBase(json.namedObject("SubstyleBase").toInt(wxNOT_FOUND));

    m_properties.clear();
    JSONItem properties = json.namedObject("Properties");
    int arrSize = properties.arraySize();
    m_properties.reserve(arrSize);

    for(int i = 0; i < arrSize; ++i) {
        // Construct a style property
        StyleProperty p;
        p.FromJSON(properties.arrayItem(i));
        m_properties.emplace_back(std::move(p));
    }
}

void LexerConf::SetKeyWords(const wxString& keywords, int set)
{
    wxString content = keywords;
    content.Replace("\r", "");
    content.Replace("\n", " ");
    content.Replace("\\", " ");
    m_keyWords[set] = content;
}

void LexerConf::ApplySystemColours(wxStyledTextCtrl* ctrl)
{
    Apply(ctrl);
    wxColour fg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour bg_colour = clSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX);
    if(!DrawingUtils::IsDark(bg_colour)) {
        bg_colour = *wxWHITE;
    } else {
        // dark colour, make it a bit darker
        bg_colour = bg_colour.ChangeLightness(50);
        fg_colour = wxColour(*wxWHITE).ChangeLightness(90);
    }

    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        ctrl->StyleSetBackground(i, bg_colour);
        ctrl->StyleSetForeground(i, fg_colour);
    }
}

void LexerConf::ApplyWordSet(wxStyledTextCtrl* ctrl, eWordSetIndex index, const wxString& keywords)
{
    CHECK_PTR_RET(ctrl);
    const WordSetIndex& word_set = m_wordSets[index];
    CHECK_COND_RET(word_set.is_ok());

    if(word_set.is_substyle) {
        allocate_substyles(ctrl, GetSubStyleBase(), sizeof(m_wordSets) / sizeof(m_wordSets[0]));

        // index is 0 based, substyles are 1 based
        int substyle_index = ctrl->GetSubStylesStart(GetSubStyleBase()) + word_set.index;
        clDEBUG1() << "Substyle calculated index is:" << substyle_index << endl;
        ctrl->SetIdentifiers(substyle_index, keywords);
    } else {
        // default
        ctrl->SetKeyWords(word_set.index, keywords);
    }
}
