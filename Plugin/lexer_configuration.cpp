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

#include "FontUtils.hpp"
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

LexerConf::~LexerConf() {}

wxFont LexerConf::GetFontForStyle(int styleId, const wxWindow* win) const
{
    wxUnusedVar(win);
    const auto& prop = GetProperty(styleId);
    if(prop.IsNull()) {
        return FontUtils::GetDefaultMonospacedFont();
    }
    auto font = FontUtils::GetDefaultMonospacedFont();
    prop.FromAttributes(&font);
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
    // need to force this to false, see thess bugs:
    // https://github.com/eranif/codelite/issues/3010
    // https://github.com/eranif/codelite/issues/2992
    ctrl->SetBufferedDraw(false);
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
    wxFont defaultFont = FontUtils::GetDefaultMonospacedFont();
    bool foundDefaultStyle = false;

    StyleProperty defaultStyle;
    for(const auto& prop : GetLexerProperties()) {
        if(prop.GetId() == 0 && !prop.IsSubstyle()) {
            defaultStyle = prop;
            prop.FromAttributes(&defaultFont);
            foundDefaultStyle = true;
            if(!defaultFont.IsOk()) {
                clWARNING() << "Found default font, but the font is NOT OK !?" << endl;
                // make sure we have a font...
                defaultFont = FontUtils::GetDefaultMonospacedFont();
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
            // fold margin, we set it at the end using the default style
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
            wxFont font = defaultFont;
            sp.FromAttributes(&font);

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

            } else {

                int style_id = sp.GetId();
                if(sp.IsSubstyle() && IsSubstyleSupported()) {
                    style_id = ctrl->GetSubStylesStart(GetSubStyleBase()) + sp.GetId();
                    LOG_IF_TRACE { clDEBUG1() << "* Found Substyle" << endl; }
                    LOG_IF_TRACE { clDEBUG1() << "* Setting style:" << style_id << ". Is substyle?" << sp.IsSubstyle() << endl; }
                    LOG_IF_TRACE { clDEBUG1() << "* Parent style:" << sp.GetId() << endl; }
                }

                // always set the font
                ctrl->StyleSetFont(style_id, font);
                ctrl->StyleSetEOLFilled(style_id, sp.GetEolFilled());

                if(style_id != LINE_NUMBERS_ATTR_ID) {
                    // Inactive state is greater by 64 from its counterpart
                    wxColor inactiveColor = GetInactiveColor(sp);
                    ctrl->StyleSetForeground(style_id, to_wx_colour(sp.GetFgColour()));
                    ctrl->StyleSetForeground(style_id + 64, inactiveColor);

                    ctrl->StyleSetBackground(style_id + 64, defaultStyle.GetBgColour());
                    ctrl->StyleSetBackground(style_id, to_wx_colour(sp.GetBgColour()));

                    ctrl->StyleSetFont(style_id + 64, font);
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
    bool is_dark = DrawingUtils::IsDark(defaultStyle.GetBgColour());
    if(is_dark) {
        ctrl->IndicatorSetStyle(1, wxSTC_INDIC_BOX);
        ctrl->IndicatorSetStyle(2, wxSTC_INDIC_BOX);
    } else {
        ctrl->IndicatorSetStyle(1, wxSTC_INDIC_ROUNDBOX);
        ctrl->IndicatorSetStyle(2, wxSTC_INDIC_ROUNDBOX);
    }

    wxColour default_bg_colour = defaultStyle.GetBgColour();
    default_bg_colour = default_bg_colour.ChangeLightness(is_dark ? 105 : 95);

    ctrl->SetFoldMarginColour(true, default_bg_colour);
    ctrl->SetFoldMarginHiColour(true, default_bg_colour);

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

    wxColour caret_colour;
    if(!DrawingUtils::IsDark(bg_colour)) {
        bg_colour = *wxWHITE;
        caret_colour = *wxBLACK;
    } else {
        // dark colour, make it a bit darker
        bg_colour = bg_colour.ChangeLightness(50);
        fg_colour = wxColour(*wxWHITE).ChangeLightness(90);
        caret_colour = *wxWHITE;
    }

    for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
        ctrl->StyleSetBackground(i, bg_colour);
        ctrl->StyleSetForeground(i, fg_colour);
    }
    ctrl->SetCaretForeground(caret_colour);
    ctrl->SetSelForeground(true, clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
    ctrl->SetSelBackground(true, clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
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
        LOG_IF_TRACE { clDEBUG1() << "Substyle calculated index is:" << substyle_index << endl; }
        ctrl->SetIdentifiers(substyle_index, keywords);
    } else {
        // default
        ctrl->SetKeyWords(word_set.index, keywords);
    }
}
