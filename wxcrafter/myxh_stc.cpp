// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "myxh_stc.h"
#include <map>
#include <wx/stc/stc.h>

#define NUMBER_MARGIN_ID 0
#define EDIT_TRACKER_MARGIN_ID 1
#define SYMBOLS_MARGIN_ID 2
#define SYMBOLS_MARGIN_SEP_ID 3
#define FOLD_MARGIN_ID 4

static std::map<wxString, int> s_lexers;
#define ADD_LEXER(lexID) s_lexers.insert(std::make_pair(#lexID, lexID));

MyWxStcXmlHandler::MyWxStcXmlHandler()
    : wxXmlResourceHandler()
{
    AddWindowStyles();
    if(s_lexers.empty()) {
        ADD_LEXER(wxSTC_LEX_CONTAINER);
        ADD_LEXER(wxSTC_LEX_NULL);
        ADD_LEXER(wxSTC_LEX_PYTHON);
        ADD_LEXER(wxSTC_LEX_CPP);
        ADD_LEXER(wxSTC_LEX_HTML);
        ADD_LEXER(wxSTC_LEX_XML);
        ADD_LEXER(wxSTC_LEX_PERL);
        ADD_LEXER(wxSTC_LEX_SQL);
        ADD_LEXER(wxSTC_LEX_VB);
        ADD_LEXER(wxSTC_LEX_PROPERTIES);
        ADD_LEXER(wxSTC_LEX_ERRORLIST);
        ADD_LEXER(wxSTC_LEX_MAKEFILE);
        ADD_LEXER(wxSTC_LEX_BATCH);
        ADD_LEXER(wxSTC_LEX_XCODE);
        ADD_LEXER(wxSTC_LEX_LATEX);
        ADD_LEXER(wxSTC_LEX_LUA);
        ADD_LEXER(wxSTC_LEX_DIFF);
        ADD_LEXER(wxSTC_LEX_CONF);
        ADD_LEXER(wxSTC_LEX_PASCAL);
        ADD_LEXER(wxSTC_LEX_AVE);
        ADD_LEXER(wxSTC_LEX_ADA);
        ADD_LEXER(wxSTC_LEX_LISP);
        ADD_LEXER(wxSTC_LEX_RUBY);
        ADD_LEXER(wxSTC_LEX_EIFFEL);
        ADD_LEXER(wxSTC_LEX_EIFFELKW);
        ADD_LEXER(wxSTC_LEX_TCL);
        ADD_LEXER(wxSTC_LEX_NNCRONTAB);
        ADD_LEXER(wxSTC_LEX_BULLANT);
        ADD_LEXER(wxSTC_LEX_VBSCRIPT);
        ADD_LEXER(wxSTC_LEX_BAAN);
        ADD_LEXER(wxSTC_LEX_MATLAB);
        ADD_LEXER(wxSTC_LEX_SCRIPTOL);
        ADD_LEXER(wxSTC_LEX_ASM);
        ADD_LEXER(wxSTC_LEX_CPPNOCASE);
        ADD_LEXER(wxSTC_LEX_FORTRAN);
        ADD_LEXER(wxSTC_LEX_F77);
        ADD_LEXER(wxSTC_LEX_CSS);
        ADD_LEXER(wxSTC_LEX_POV);
        ADD_LEXER(wxSTC_LEX_LOUT);
        ADD_LEXER(wxSTC_LEX_ESCRIPT);
        ADD_LEXER(wxSTC_LEX_PS);
        ADD_LEXER(wxSTC_LEX_NSIS);
        ADD_LEXER(wxSTC_LEX_MMIXAL);
        ADD_LEXER(wxSTC_LEX_CLW);
        ADD_LEXER(wxSTC_LEX_CLWNOCASE);
        ADD_LEXER(wxSTC_LEX_LOT);
        ADD_LEXER(wxSTC_LEX_YAML);
        ADD_LEXER(wxSTC_LEX_TEX);
        ADD_LEXER(wxSTC_LEX_METAPOST);
        ADD_LEXER(wxSTC_LEX_POWERBASIC);
        ADD_LEXER(wxSTC_LEX_FORTH);
        ADD_LEXER(wxSTC_LEX_ERLANG);
        ADD_LEXER(wxSTC_LEX_OCTAVE);
        ADD_LEXER(wxSTC_LEX_MSSQL);
        ADD_LEXER(wxSTC_LEX_VERILOG);
        ADD_LEXER(wxSTC_LEX_KIX);
        ADD_LEXER(wxSTC_LEX_GUI4CLI);
        ADD_LEXER(wxSTC_LEX_SPECMAN);
        ADD_LEXER(wxSTC_LEX_AUTOMATIC);
        ADD_LEXER(wxSTC_LEX_APDL);
        ADD_LEXER(wxSTC_LEX_BASH);
        ADD_LEXER(wxSTC_LEX_ASN1);
        ADD_LEXER(wxSTC_LEX_VHDL);
        ADD_LEXER(wxSTC_LEX_CAML);
        ADD_LEXER(wxSTC_LEX_BLITZBASIC);
        ADD_LEXER(wxSTC_LEX_PUREBASIC);
        ADD_LEXER(wxSTC_LEX_HASKELL);
        ADD_LEXER(wxSTC_LEX_PHPSCRIPT);
        ADD_LEXER(wxSTC_LEX_TADS3);
        ADD_LEXER(wxSTC_LEX_REBOL);
        ADD_LEXER(wxSTC_LEX_SMALLTALK);
        ADD_LEXER(wxSTC_LEX_FLAGSHIP);
        ADD_LEXER(wxSTC_LEX_CSOUND);
        ADD_LEXER(wxSTC_LEX_FREEBASIC);
        ADD_LEXER(wxSTC_LEX_INNOSETUP);
        ADD_LEXER(wxSTC_LEX_OPAL);
        ADD_LEXER(wxSTC_LEX_SPICE);
        ADD_LEXER(wxSTC_LEX_D);
        ADD_LEXER(wxSTC_LEX_CMAKE);
        ADD_LEXER(wxSTC_LEX_GAP);
        ADD_LEXER(wxSTC_LEX_PLM);
        ADD_LEXER(wxSTC_LEX_PROGRESS);
        ADD_LEXER(wxSTC_LEX_ABAQUS);
        ADD_LEXER(wxSTC_LEX_ASYMPTOTE);
        ADD_LEXER(wxSTC_LEX_R);
        ADD_LEXER(wxSTC_LEX_MAGIK);
        ADD_LEXER(wxSTC_LEX_POWERSHELL);
        ADD_LEXER(wxSTC_LEX_MYSQL);
        ADD_LEXER(wxSTC_LEX_PO);
        ADD_LEXER(wxSTC_LEX_TAL);
        ADD_LEXER(wxSTC_LEX_COBOL);
        ADD_LEXER(wxSTC_LEX_TACL);
        ADD_LEXER(wxSTC_LEX_SORCUS);
        ADD_LEXER(wxSTC_LEX_POWERPRO);
        ADD_LEXER(wxSTC_LEX_NIMROD);
        ADD_LEXER(wxSTC_LEX_SML);
        ADD_LEXER(wxSTC_LEX_MARKDOWN);
    }
}

wxObject* MyWxStcXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(text, wxStyledTextCtrl)
    text->Create(m_parentAsWindow, GetID(), GetPosition(), GetSize(), GetStyle(), GetName());
    bool foldMargin = GetBool("fold_margin");
    bool symbolMargin = GetBool("symbol_margin");
    bool lineNumber = GetBool("linenumbers_margin");
    bool separatorMargin = GetBool("separator_margin");
    bool displayEol = GetBool("vieweol");
    int indentGuides = GetLong("indentguides", 0);
    int wrapText = GetLong("wrap");
    int eolMode = GetLong("eolmode", -1);

    wxString slexerId = GetText("lexer", false);
    wxFont font = GetFont();

    text->StyleClearAll();
    if(font.IsOk()) {
        for(int i = 0; i < wxSTC_STYLE_MAX; ++i) {
            text->StyleSetFont(i, font);
        }
    }

    text->SetViewEOL(displayEol);
    text->SetWrapMode(wrapText);

    if(eolMode != -1) { text->SetEOLMode(eolMode); }
    text->SetIndentationGuides(indentGuides);

    int lexer = wxSTC_LEX_NULL;
    if(s_lexers.count(slexerId)) { lexer = s_lexers.find(slexerId)->second; }
    text->SetLexer(lexer);
    text->SetProperty("fold", "1");
    text->SetProperty("fold.html", "1");
    text->SetProperty("fold.comment", "1");

    // Fold margin
    text->SetMarginType(FOLD_MARGIN_ID, wxSTC_MARGIN_SYMBOL);
    text->SetMarginMask(FOLD_MARGIN_ID, wxSTC_MASK_FOLDERS);
    text->SetMarginWidth(FOLD_MARGIN_ID, foldMargin ? 16 : 0);
    text->SetMarginSensitive(FOLD_MARGIN_ID, true);

    // Make it size 0
    text->SetMarginWidth(EDIT_TRACKER_MARGIN_ID, 0);

    // Symbol margin
    text->SetMarginType(SYMBOLS_MARGIN_ID, wxSTC_MARGIN_SYMBOL);
    text->SetMarginMask(SYMBOLS_MARGIN_ID, ~(wxSTC_MASK_FOLDERS));
    text->SetMarginWidth(SYMBOLS_MARGIN_ID, symbolMargin ? 16 : 0);
    text->SetMarginSensitive(SYMBOLS_MARGIN_ID, true);

    // Line numbers
    int pixelWidth = 4 + 5 * text->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));
    text->SetMarginType(NUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);
    text->SetMarginWidth(NUMBER_MARGIN_ID, lineNumber ? pixelWidth : 0);

    // separator
    text->SetMarginType(SYMBOLS_MARGIN_SEP_ID, wxSTC_MARGIN_FORE);
    text->SetMarginMask(SYMBOLS_MARGIN_SEP_ID, 0);
    text->SetMarginWidth(SYMBOLS_MARGIN_SEP_ID, separatorMargin ? 1 : 0);

    text->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_BACKGROUND);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_BACKGROUND);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROW);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
    text->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_BACKGROUND);
    SetupWindow(text);
    return text;
}

bool MyWxStcXmlHandler::CanHandle(wxXmlNode* node) { return IsOfClass(node, wxT("wxStyledTextCtrl")); }
