#include "gitCommitEditor.h"

#include "drawingutils.h"

GitCommitEditor::GitCommitEditor(wxWindow* parent, wxWindowID id, const wxPoint &position, const wxSize& size, long style)
    : wxStyledTextCtrl(parent, id, position, size, style)
{
    InitStyles();
}

void GitCommitEditor::InitStyles()
{
// Initialize some styles
    StyleClearAll();
    SetLexer(wxSTC_LEX_DIFF);

    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    for (int i=0; i<=wxSTC_STYLE_DEFAULT; i++) {
        StyleSetBackground(i, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        StyleSetForeground(i, *wxBLACK);
        StyleSetFont(i, font);
    }

    SetMarginType (0, wxSTC_MARGIN_TEXT  );
    SetMarginType (1, wxSTC_MARGIN_NUMBER);
    SetMarginWidth(1, 4 + 5*TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9")));
    SetMarginWidth(2, 0);
    SetMarginWidth(3, 0);
    SetMarginWidth(4, 0);
    SetTabWidth(2);

    wxColour fg, bg;

    //Default
    fg.Set(wxT("#808080"));
    bg.Set(wxT("#ffffff"));
    StyleSetForeground(0, fg);
    StyleSetBackground(0, bg);

    //Comment
    fg.Set(wxT("#808080"));
    bg.Set(wxT("white"));
    StyleSetForeground(1, fg);
    StyleSetBackground(1, bg);

    //Command
    fg.Set(wxT("#804040"));
    bg.Set(wxT("white"));
    StyleSetForeground(2, fg);
    StyleSetBackground(2, bg);

    //Header
    fg.Set(wxT("#0000A0"));
    bg.Set(wxT("white"));
    StyleSetForeground(3, fg);
    StyleSetBackground(3, bg);

    //Position
    fg.Set(wxT("#0000A0"));
    bg.Set(wxT("white"));
    StyleSetForeground(4, fg);
    StyleSetBackground(4, bg);
    StyleSetBold(4,true);

    //Line deleted
    fg.Set(wxT("#FF0000"));
    bg.Set(wxT("white"));
    StyleSetForeground(5, fg);
//  StyleSetBackground(5, bg);
    StyleSetBackground(5,  DrawingUtils::LightColour(wxT("RED"), 9.5));
    StyleSetEOLFilled(5, true);
    
    //Line added
    fg.Set(wxT("#008000"));
    bg.Set(wxT("white"));
    StyleSetForeground(6, fg);
//  StyleSetBackground(6, bg);
    StyleSetBackground(6,  DrawingUtils::LightColour(wxT("GREEN"), 9.5));
    StyleSetEOLFilled(6, true);
    
    //Brace match
    fg.Set(wxT("black"));
    bg.Set(wxT("white"));
    StyleSetForeground(34, fg);
    StyleSetBackground(34, bg);

    //Brace bad match
    fg.Set(wxT("red"));
    bg.Set(wxT("white"));
    StyleSetForeground(35, fg);
    StyleSetBackground(35, bg);

    //Indent Guide
    fg.Set(wxT("#C0C0C0"));
    bg.Set(wxT("#FFFFFF"));
    StyleSetForeground(37, fg);
    StyleSetBackground(37, bg);

    StyleSetBackground(10, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    StyleSetForeground(10, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
}