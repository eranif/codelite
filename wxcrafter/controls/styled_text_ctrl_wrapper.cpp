#include "styled_text_ctrl_wrapper.h"
#include "allocator_mgr.h"
#include "bool_property.h"
#include "category_property.h"
#include "choice_property.h"
#include "font_property.h"
#include "multi_strings_property.h"
#include "wxgui_helpers.h"
#include <wx/stc/stc.h>

StyledTextCtrlWrapper::LexersMap_t StyledTextCtrlWrapper::m_lexers;

#define ADD_LEXER(lexID) m_lexers.insert(std::make_pair(#lexID, lexID));

StyledTextCtrlWrapper::StyledTextCtrlWrapper()
    : wxcWidget(ID_WXSTC)
{
    // Register the known lexers
    if(m_lexers.empty()) {
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

    DelProperty(PROP_FONT);
    DelProperty(PROP_BG);
    DelProperty(PROP_FG);

    RegisterEvent("wxEVT_STC_CHANGE", "wxStyledTextEvent", "");
    RegisterEvent(
        "wxEVT_STC_STYLENEEDED", "wxStyledTextEvent",
        _("If you used SCI_SETLEXER(SCLEX_CONTAINER) to make the container act as the lexer\nyou will receive this "
          "notification when Scintilla is about to display or print text that requires styling"));
    RegisterEvent("wxEVT_STC_CHARADDED", "wxStyledTextEvent", _("A char was added"));
    RegisterEvent("wxEVT_STC_SAVEPOINTREACHED", "wxStyledTextEvent",
                  _("Sent to the container when the save point is entered\nallowing the container to display a "
                    "\"document saved\" indicator and change its menus"));
    RegisterEvent("wxEVT_STC_SAVEPOINTLEFT", "wxStyledTextEvent",
                  _("Sent to the container when the save point is left\nallowing the container to display a \"document "
                    "dirty\" indicator and change its menus"));
    RegisterEvent("wxEVT_STC_ROMODIFYATTEMPT", "wxStyledTextEvent",
                  _("An attempt to modify a read-only document was made"));
    RegisterEvent("wxEVT_STC_KEY", "wxStyledTextEvent", _("Key was pressed"));
    RegisterEvent("wxEVT_STC_DOUBLECLICK", "wxStyledTextEvent", _("Double-click"));
    RegisterEvent("wxEVT_STC_UPDATEUI", "wxStyledTextEvent", _("Update UI"));
    RegisterEvent("wxEVT_STC_MODIFIED", "wxStyledTextEvent", _("Document modified"));
    RegisterEvent("wxEVT_STC_MACRORECORD", "wxStyledTextEvent", _("Macro record started/stopped"));
    RegisterEvent("wxEVT_STC_MARGINCLICK", "wxStyledTextEvent", _("One of margins was left clicked with the mouse"));
    RegisterEvent(
        "wxEVT_STC_NEEDSHOWN", "wxStyledTextEvent",
        _("Scintilla has determined that a range of lines that is currently invisible should be made visible"));
    RegisterEvent("wxEVT_STC_PAINTED", "wxStyledTextEvent",
                  _("Painting has just been done.\nUseful when you want to update some other widgets based on a change "
                    "in Scintilla\nbut want to have the paint occur first to appear more responsive"));
    RegisterEvent("wxEVT_STC_USERLISTSELECTION", "wxStyledTextEvent",
                  _("The user has selected an item in a user list."));
    RegisterEvent("wxEVT_STC_URIDROPPED", "wxStyledTextEvent",
                  _("Only on the GTK+ version. Indicates that the user has dragged a URI such as a file name or Web "
                    "address onto Scintilla"));
    RegisterEvent("wxEVT_STC_DWELLSTART", "wxStyledTextEvent",
                  _("Generated when the user keeps the mouse in one position for the dwell period"));
    RegisterEvent("wxEVT_STC_DWELLEND", "wxStyledTextEvent",
                  _("Generated after a wxEVT_STC_DWELLSTART and the mouse is moved or other activity such as key press "
                    "indicates the dwell is over"));
    RegisterEvent("wxEVT_STC_START_DRAG", "wxStyledTextEvent", _("Drag and Drop start event"));
    RegisterEvent("wxEVT_STC_DRAG_OVER", "wxStyledTextEvent", _("Drag and Drop over event"));
    RegisterEvent("wxEVT_STC_DO_DROP", "wxStyledTextEvent", _("Drag and Drop drop event"));
    RegisterEvent("wxEVT_STC_ZOOM", "wxStyledTextEvent",
                  _("This event is generated when the user zooms the display using the keyboard"));
    RegisterEvent("wxEVT_STC_HOTSPOT_CLICK", "wxStyledTextEvent", _("A hotspot was clicked"));
    RegisterEvent("wxEVT_STC_HOTSPOT_DCLICK", "wxStyledTextEvent", _("A hotspot was double clicked"));
    RegisterEvent("wxEVT_STC_CALLTIP_CLICK", "wxStyledTextEvent", _("The calltip window was clicked"));
    RegisterEvent("wxEVT_STC_AUTOCOMP_SELECTION", "wxStyledTextEvent", _("Auto-completion list selection was made"));
    RegisterEvent("wxEVT_STC_INDICATOR_CLICK", "wxStyledTextEvent",
                  _("User clicked the mouse on a text that has an indicator"));
    RegisterEvent("wxEVT_STC_INDICATOR_RELEASE", "wxStyledTextEvent",
                  _("User released the mouse on a text that has an indicator"));
    RegisterEvent("wxEVT_STC_AUTOCOMP_CANCELLED", "wxStyledTextEvent", _("Auto completion list was cancelled"));
    RegisterEvent("wxEVT_STC_AUTOCOMP_CHAR_DELETED", "wxStyledTextEvent",
                  _("The user deleted a character while auto-completion list was active"));

    SetPropertyString(_("Common Settings"), "wxStyledTextCtrl");
    AddProperty(new BoolProperty(PROP_STC_MARGIN_FOLD, true, ""));
    AddProperty(new BoolProperty(PROP_STC_MARGIN_LINE_NUMBERS, true, ""));
    AddProperty(new BoolProperty(PROP_STC_MARGIN_SEPARATOR, false, ""));
    AddProperty(new BoolProperty(PROP_STC_MARGIN_SYMBOL, false, ""));

    m_wrapOptions.Add("None"); // 0
    m_wrapOptions.Add("Word"); // 1
    m_wrapOptions.Add("Char"); // 2

    AddProperty(new ChoiceProperty(PROP_STC_WRAP, m_wrapOptions, 0, _("Wrap text")));

    m_indentGuides.Add("None");
    m_indentGuides.Add("Real");
    m_indentGuides.Add("Look Forward");
    m_indentGuides.Add("Look Both");

    AddProperty(new ChoiceProperty(PROP_STC_INDENT_GUIDES, m_indentGuides, 0,
                                   _("Display indentation guides (vertical lines)")));

    m_eolMode.Add("CRLF");    // 0
    m_eolMode.Add("CR");      // 1
    m_eolMode.Add("LF");      // 2
    m_eolMode.Add("Default"); // 3

    AddProperty(new ChoiceProperty(PROP_STC_EOL_MODE, m_eolMode, 3, _("EOL Mode")));
    AddProperty(new BoolProperty(PROP_STC_VIEW_EOL, false, _("Display the line endings characters")));

    wxArrayString lexersArr = GetLexers();
    int sel = lexersArr.Index("wxSTC_LEX_NULL");
    AddProperty(
        new ChoiceProperty(PROP_STC_LEXER, lexersArr, sel,
                           _("Set the wxStyledTextCtrl lexer.\nThe lexer affects the syntax highlight of control")));
    AddProperty(new FontProperty(PROP_FONT, "", _("Set the basic styles font")));

    AddProperty(new CategoryProperty(_("Keywords")));
    AddProperty(new MultiStringsProperty(PROP_KEYWORDS_SET_1, _("Keywords set 1"), " "));
    AddProperty(new MultiStringsProperty(PROP_KEYWORDS_SET_2, _("Keywords set 2"), " "));
    AddProperty(new MultiStringsProperty(PROP_KEYWORDS_SET_3, _("Keywords set 3"), " "));
    AddProperty(new MultiStringsProperty(PROP_KEYWORDS_SET_4, _("Keywords set 4"), " "));
    AddProperty(new MultiStringsProperty(PROP_KEYWORDS_SET_5, _("Keywords set 5"), " "));

    m_namePattern = "m_stc";
    SetName(GenerateName());
}

StyledTextCtrlWrapper::~StyledTextCtrlWrapper() {}

wxcWidget* StyledTextCtrlWrapper::Clone() const { return new StyledTextCtrlWrapper(); }

wxString StyledTextCtrlWrapper::CppCtorCode() const
{
    wxString cppCode;
    cppCode << CPPStandardWxCtor("0");

    bool foldMargin = PropertyBool(PROP_STC_MARGIN_FOLD) == "true";
    bool symbolMargin = PropertyBool(PROP_STC_MARGIN_SYMBOL) == "true";
    bool lineNumber = PropertyBool(PROP_STC_MARGIN_LINE_NUMBERS) == "true";
    bool separatorMargin = PropertyBool(PROP_STC_MARGIN_SEPARATOR) == "true";

    // Fold margin
    cppCode << "// Configure the fold margin\n";
    cppCode << GetName() << "->SetMarginType     (4, wxSTC_MARGIN_SYMBOL);\n";
    cppCode << GetName() << "->SetMarginMask     (4, wxSTC_MASK_FOLDERS);\n";
    cppCode << GetName() << "->SetMarginSensitive(4, true);\n";
    cppCode << GetName() << "->SetMarginWidth    (4, " << (foldMargin ? 16 : 0) << ");\n\n";

    if(foldMargin) {
        cppCode << GetName() << "->SetProperty(" << wxCrafter::WXT("fold") << "," << wxCrafter::WXT("1") << ");\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_ARROWDOWN);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_ARROW);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_BACKGROUND);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_BACKGROUND);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_ARROW);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);\n";
        cppCode << GetName() << "->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_BACKGROUND);\n";
    }

    // Make it size 0

    cppCode << "// Configure the tracker margin\n";
    cppCode << GetName() << "->SetMarginWidth(1, 0);\n\n";

    // Symbol margin
    cppCode << "// Configure the symbol margin\n";
    cppCode << GetName() << "->SetMarginType (2, wxSTC_MARGIN_SYMBOL);\n";
    cppCode << GetName() << "->SetMarginMask (2, ~(wxSTC_MASK_FOLDERS));\n";
    cppCode << GetName() << "->SetMarginWidth(2, " << (symbolMargin ? 16 : 0) << ");\n";
    cppCode << GetName() << "->SetMarginSensitive(2, true);\n\n";

    // Line numbers
    cppCode << "// Configure the line numbers margin\n";
    wxString pixelWidth = GetName() + "_PixelWidth";
    if(lineNumber) {
        cppCode << "int " << pixelWidth << " = 4 + 5 *" << GetName()
                << "->TextWidth(wxSTC_STYLE_LINENUMBER, wxT(\"9\"));\n";
    }

    cppCode << GetName() << "->SetMarginType(0, wxSTC_MARGIN_NUMBER);\n";
    cppCode << GetName() << "->SetMarginWidth(0," << (lineNumber ? pixelWidth : "0") << ");\n\n";

    // separator
    cppCode << "// Configure the line symbol margin\n";
    cppCode << GetName() << "->SetMarginType(3, wxSTC_MARGIN_FORE);\n";
    cppCode << GetName() << "->SetMarginMask(3, 0);\n";
    cppCode << GetName() << "->SetMarginWidth(3," << (separatorMargin ? 1 : 0) << ");\n";

    // Lexer
    cppCode << "// Select the lexer\n";
    cppCode << GetName() << "->SetLexer(" << PropertyString(PROP_STC_LEXER) << ");\n";

    cppCode << "// Set default font / styles\n";
    cppCode << GetName() << "->StyleClearAll();\n";
    wxString font_var = GetName() + "Font";
    wxString fontCode = wxCrafter::FontToCpp(PropertyString(PROP_FONT), font_var);
    if(fontCode != "wxNullFont") {
        cppCode << "for(int i=0; i<wxSTC_STYLE_MAX; ++i) {\n";
        cppCode << "    " << GetName() << "->StyleSetFont(i, " << font_var << ");\n";
        cppCode << "}\n";
    }

    // Wrap
    int wrapMode = m_wrapOptions.Index(PropertyString(PROP_STC_WRAP));
    if(wrapMode != -1) { cppCode << GetName() << "->SetWrapMode(" << wrapMode << ");\n"; }

    int indentGuides = m_indentGuides.Index(PropertyString(PROP_STC_INDENT_GUIDES));
    if(indentGuides != -1) { cppCode << GetName() << "->SetIndentationGuides(" << indentGuides << ");\n"; }

    if(PropertyBool(PROP_STC_VIEW_EOL) == "true") { cppCode << GetName() << "->SetViewEOL(true);\n"; }

    int eolMode = m_eolMode.Index(PropertyString(PROP_STC_EOL_MODE));
    if(eolMode != 3) { // 3 is invalid
        cppCode << GetName() << "->SetEOLMode(" << eolMode << ");\n";
    }

    cppCode << GetName() << "->SetKeyWords(0, " << wxCrafter::WXT(PropertyString(PROP_KEYWORDS_SET_1)) << ");\n";
    cppCode << GetName() << "->SetKeyWords(1, " << wxCrafter::WXT(PropertyString(PROP_KEYWORDS_SET_2)) << ");\n";
    cppCode << GetName() << "->SetKeyWords(2, " << wxCrafter::WXT(PropertyString(PROP_KEYWORDS_SET_3)) << ");\n";
    cppCode << GetName() << "->SetKeyWords(3, " << wxCrafter::WXT(PropertyString(PROP_KEYWORDS_SET_4)) << ");\n";
    cppCode << GetName() << "->SetKeyWords(4, " << wxCrafter::WXT(PropertyString(PROP_KEYWORDS_SET_5)) << ");\n";
    return cppCode;
}

void StyledTextCtrlWrapper::GetIncludeFile(wxArrayString& headers) const { headers.Add("#include <wx/stc/stc.h>"); }

wxString StyledTextCtrlWrapper::GetWxClassName() const { return "wxStyledTextCtrl"; }

void StyledTextCtrlWrapper::ToXRC(wxString& text, XRC_TYPE type) const
{
    if(type == XRC_LIVE) {

        // There is no support for STC in XRC
        text << XRCUnknown();

    } else {
        text << XRCPrefix() << XRCSize() << XRCStyle() << XRCCommonAttributes();

        if(PropertyBool(PROP_STC_MARGIN_FOLD) == "true") { text << "<fold_margin>1</fold_margin>"; }

        if(PropertyBool(PROP_STC_MARGIN_SYMBOL) == "true") { text << "<symbol_margin>1</symbol_margin>"; }
        if(PropertyBool(PROP_STC_MARGIN_LINE_NUMBERS) == "true") {
            text << "<linenumbers_margin>1</linenumbers_margin>";
        }
        if(PropertyBool(PROP_STC_MARGIN_SEPARATOR) == "true") { text << "<separator_margin>1</separator_margin>"; }

        int wrapMode = m_wrapOptions.Index(PropertyString(PROP_STC_WRAP));
        text << "<wrap>" << wrapMode << "</wrap>";

        text << "<indentguides>" << m_indentGuides.Index(PropertyString(PROP_STC_INDENT_GUIDES)) << "</indentguides>";

        if(PropertyBool(PROP_STC_VIEW_EOL) == "true") { text << "<vieweol>1</vieweol>"; }

        int eolMode = m_eolMode.Index(PropertyString(PROP_STC_EOL_MODE));
        if(eolMode != 3) { text << "<eolmode>" << eolMode << "</eolmode>"; }

        text << "<lexer>" << PropertyString(PROP_STC_LEXER) << "</lexer>";
        text << XRCSuffix();
    }
}

wxArrayString StyledTextCtrlWrapper::GetLexers() const
{
    wxArrayString arr;
    LexersMap_t::const_iterator iter = m_lexers.begin();
    for(; iter != m_lexers.end(); ++iter) {
        arr.Add(iter->first);
    }
    return arr;
}
