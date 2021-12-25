#include "ThemeImporterCSS.hpp"

#include "drawingutils.h"
#include "macros.h"

#include <wx/stc/stc.h>

ThemeImporterCSS::ThemeImporterCSS()
{
    SetKeywords0("color background-color background-image background-repeat background-attachment background-position "
                 "background font-family"
                 " font-style font-variant font-weight font-size font word-spacing letter-spacing text-decoration "
                 "vertical-align text-transform"
                 " text-align text-indent line-height margin-top margin-right margin-bottom margin-left margin "
                 "padding-top padding-right padding-bottom"
                 " padding-left padding border-top-width border-right-width border-bottom-width border-left-width "
                 "border-width border-top"
                 " border-right border-bottom border-left border border-color border-style width height float clear "
                 "display white-space"
                 " list-style-type list-style-image list-style-position list-style auto none normal italic oblique "
                 "small-caps bold bolder"
                 " lighter xx-small x-small small medium large x-large xx-large larger smaller transparent repeat "
                 "repeat-x repeat-y"
                 " no-repeat scroll fixed top bottom left center right justify both underline overline line-through "
                 "blink baseline"
                 " sub super text-top middle text-bottom capitalize uppercase lowercase thin medium thick dotted "
                 "dashed solid double"
                 " groove ridge inset outset block inline list-item pre no-wrap inside outside disc circle square "
                 "decimal lower-roman"
                 " upper-roman lower-alpha upper-alpha aqua black blue fuchsia gray green lime maroon navy olive "
                 "purple red silver teal"
                 "white yellow ");
    SetKeywords1("      link active visited first-child focus hover lang left right first empty enabled disabled "
                 "checked not root target       only-child last-child nth-child nth-last-child       first-of-type "
                 "last-of-type nth-of-type nth-last-of-type only-of-type       valid invalid required optional     "
                 "indeterminate default");
    SetKeywords2(
        "border-top-color border-right-color border-bottom-color border-left-color border-color border-top-style "
        "border-right-style border-bottom-style border-left-style border-style top right bottom left position z-index "
        "direction unicode-bidi min-width max-width min-height max-height overflow clip visibility content quotes "
        "counter-reset counter-increment marker-offset size marks page-break-before page-break-after page-break-inside "
        "page orphans widows font-stretch font-size-adjust unicode-range units-per-em src panose-1 stemv stemh slope "
        "cap-height x-height ascent descent widths bbox definition-src baseline centerline mathline topline "
        "text-shadow caption-side table-layout border-collapse border-spacing empty-cells speak-header cursor outline "
        "outline-width outline-style outline-color volume speak pause-before pause-after pause cue-before cue-after "
        "cue play-during azimuth elevation speech-rate voice-family pitch pitch-range stress richness "
        "speak-punctuation speak-numeral inherit run-in compact marker table inline-table table-row-group "
        "table-header-group table-footer-group table-row table-column-group table-column table-cell table-caption "
        "static relative absolute fixed ltr rtl embed bidi-override visible hidden scroll collapse open-quote "
        "close-quote no-open-quote no-close-quote decimal-leading-zero lower-greek lower-latin upper-latin hebrew "
        "armenian georgian cjk-ideographic hiragana katakana hiragana-iroha katakana-iroha landscape portrait crop "
        "cross always avoid wider narrower ultra-condensed extra-condensed condensed semi-condensed semi-expanded "
        "expanded extra-expanded ultra-expanded caption icon menu message-box small-caption status-bar separate show "
        "hide once crosshair default pointer move text wait help e-resize ne-resize nw-resize n-resize se-resize "
        "sw-resize s-resize w-resize ActiveBorder ActiveCaption AppWorkspace Background ButtonFace ButtonHighlight "
        "ButtonShadow InactiveCaptionText ButtonText CaptionText GrayText Highlight HighlightText InactiveBorder "
        "InactiveCaption InfoBackground InfoText Menu MenuText Scrollbar ThreeDDarkShadow ThreeDFace ThreeDHighlight "
        "ThreeDLightShadow ThreeDShadow Window WindowFrame WindowText silent x-soft soft medium loud x-loud spell-out "
        "mix left-side far-left center-left center-right far-right right-side behind leftwards rightwards below level "
        "above higher lower x-slow slow medium fast x-fast faster slower male female child x-low low high x-high code "
        "digits continuous");

    SetKeywords3("background-size border-radius border-top-right-radius border-bottom-right-radius "
                 "border-bottom-left-radius border-top-left-radius box-shadow columns column-width "
                 "column-count column-rule column-gap column-rule-color column-rule-style column-rule-width       "
                 "resize opacity word-wrap ");
    SetKeywords4("first-letter first-line before after selection");
    SetFileExtensions("*.css");
}

ThemeImporterCSS::~ThemeImporterCSS() {}

LexerConf::Ptr_t ThemeImporterCSS::Import(const wxFileName& theme_file)
{
    LexerConf::Ptr_t lexer = InitializeImport(theme_file, "css", wxSTC_LEX_CSS);
    CHECK_PTR_RET_NULL(lexer);

    // Set error colour
    wxColour defaultBg(m_editor.bg_colour);
    bool isDark = DrawingUtils::IsDark(defaultBg);
    wxString errorColour = "RED";
    if(isDark) {
        errorColour = "PINK";
    }
    wxString operatorColour = isDark ? "WHITE" : "BLACK";

    /// Lexical states for SCLEX_CSS
    // Convert to CodeLite's XML properties
    AddProperty(lexer, "0", "Default", m_editor);
    AddProperty(lexer, "1", "Tag", m_editor);
    AddProperty(lexer, "2", "Class", m_klass);
    AddProperty(lexer, "3", "Pseudo Class", m_klass);
    AddProperty(lexer, "4", "Unknown Pseudo Class", m_klass);
    AddProperty(lexer, "5", "Operator", m_oper);
    AddProperty(lexer, "6", "Identifier", m_variable);
    AddProperty(lexer, "7", "Unknown Identifier", m_variable);
    AddProperty(lexer, "8", "Value", m_editor);
    AddProperty(lexer, "9", "Comment", m_multiLineComment);
    AddProperty(lexer, "10", "ID", m_variable);
    AddProperty(lexer, "11", "Important", m_editor);
    AddProperty(lexer, "12", "Directive", m_klass);
    AddProperty(lexer, "13", "String", m_string);
    AddProperty(lexer, "14", "Double String", m_string);
    AddProperty(lexer, "15", "Identifier 2", m_variable);
    AddProperty(lexer, "16", "Attribute", m_editor);
    AddProperty(lexer, "17", "Identifier 3", m_variable);
    AddProperty(lexer, "18", "Pseudo Element", m_editor);
    AddProperty(lexer, "19", "Extended Identifier", m_variable);
    AddProperty(lexer, "20", "Extended Pseudo Class", m_editor);
    AddProperty(lexer, "21", "Extended Pseudo Element", m_editor);
    AddProperty(lexer, "22", "Media", m_klass);
    AddProperty(lexer, "23", "Variable", m_variable);
    FinalizeImport(lexer);
    return lexer;
}
