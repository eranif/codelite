//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : cl_editor.cpp
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

#include "ColoursAndFontsManager.h"
#include "addincludefiledlg.h"
#include "bookmark_manager.h"
#include "breakpointdlg.h"
#include "buildtabsettingsdata.h"
#include "cc_box_tip_window.h"
#include "clEditorStateLocker.h"
#include "clPrintout.h"
#include "clSTCLineKeeper.h"
#include "cl_command_event.h"
#include "cl_editor.h"
#include "cl_editor_tip_window.h"
#include "clang_code_completion.h"
#include "code_completion_manager.h"
#include "codelite_events.h"
#include "colourrequest.h"
#include "colourthread.h"
#include "context_manager.h"
#include "ctags_manager.h"
#include "debuggerconfigtool.h"
#include "debuggerpane.h"
#include "debuggersettings.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "filedroptarget.h"
#include "fileutils.h"
#include "findreplacedlg.h"
#include "findresultstab.h"
#include "frame.h"
#include "globals.h"
#include "job.h"
#include "jobqueue.h"
#include "lexer_configuration.h"
#include "localworkspace.h"
#include "macromanager.h"
#include "manager.h"
#include "menumanager.h"
#include "new_build_tab.h"
#include "new_quick_watch_dlg.h"
#include "parse_thread.h"
#include "pluginmanager.h"
#include "precompiled_header.h"
#include "quickfindbar.h"
#include "simpletable.h"
#include "stringhighlighterjob.h"
#include "stringsearcher.h"
#include "wxCodeCompletionBoxManager.h"
#include <wx/dataobj.h>
#include <wx/dcmemory.h>
#include <wx/log.h>
#include <wx/printdlg.h>
#include <wx/regex.h>
#include <wx/richtooltip.h> // wxRichToolTip
#include <wx/wupdlock.h>
//#include "clFileOrFolderDropTarget.h"

#if wxUSE_PRINTING_ARCHITECTURE
#include "wx/paper.h"
#endif // wxUSE_PRINTING_ARCHITECTURE

// fix bug in wxscintilla.h
#ifdef EVT_STC_CALLTIP_CLICK
#undef EVT_STC_CALLTIP_CLICK
#define EVT_STC_CALLTIP_CLICK(id, fn)          \
    DECLARE_EVENT_TABLE_ENTRY(                 \
        wxEVT_STC_CALLTIP_CLICK, id, wxID_ANY, \
        (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxStyledTextEventFunction, &fn), (wxObject*)NULL),
#endif

#define NUMBER_MARGIN_ID 0
#define EDIT_TRACKER_MARGIN_ID 1
#define SYMBOLS_MARGIN_ID 2
#define SYMBOLS_MARGIN_SEP_ID 3
#define FOLD_MARGIN_ID 4

#define CL_LINE_MODIFIED_STYLE 200
#define CL_LINE_SAVED_STYLE 201

// debugger line marker xpms
extern const char* arrow_right_green_xpm[];
extern const char* stop_xpm[]; // Breakpoint
extern const char* BreakptDisabled[];
extern const char* BreakptCommandList[];
extern const char* BreakptCommandListDisabled[];
extern const char* BreakptIgnore[];
extern const char* ConditionalBreakpt[];
extern const char* ConditionalBreakptDisabled[];

const wxEventType wxCMD_EVENT_REMOVE_MATCH_INDICATOR = XRCID("remove_match_indicator");
const wxEventType wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT = ::wxNewEventType();

BEGIN_EVENT_TABLE(LEditor, wxStyledTextCtrl)
EVT_STC_CHARADDED(wxID_ANY, LEditor::OnCharAdded)
EVT_STC_MARGINCLICK(wxID_ANY, LEditor::OnMarginClick)
EVT_STC_CALLTIP_CLICK(wxID_ANY, LEditor::OnCallTipClick)
EVT_STC_DWELLEND(wxID_ANY, LEditor::OnDwellEnd)
EVT_STC_START_DRAG(wxID_ANY, LEditor::OnDragStart)
EVT_STC_DO_DROP(wxID_ANY, LEditor::OnDragEnd)
//EVT_STC_PAINTED(wxID_ANY, LEditor::OnScnPainted)
EVT_STC_UPDATEUI(wxID_ANY, LEditor::OnSciUpdateUI)
EVT_STC_SAVEPOINTREACHED(wxID_ANY, LEditor::OnSavePoint)
EVT_STC_SAVEPOINTLEFT(wxID_ANY, LEditor::OnSavePoint)
EVT_STC_MODIFIED(wxID_ANY, LEditor::OnChange)
EVT_CONTEXT_MENU(LEditor::OnContextMenu)
EVT_KEY_DOWN(LEditor::OnKeyDown)
EVT_KEY_UP(LEditor::OnKeyUp)
EVT_LEFT_DOWN(LEditor::OnLeftDown)
EVT_RIGHT_DOWN(LEditor::OnRightDown)
EVT_MOTION(LEditor::OnMotion)
EVT_MOUSEWHEEL(LEditor::OnMouseWheel)
EVT_LEFT_UP(LEditor::OnLeftUp)
EVT_LEAVE_WINDOW(LEditor::OnLeaveWindow)
EVT_KILL_FOCUS(LEditor::OnFocusLost)
EVT_SET_FOCUS(LEditor::OnFocus)
EVT_STC_DOUBLECLICK(wxID_ANY, LEditor::OnLeftDClick)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_FIND_NEXT, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACE, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_REPLACEALL, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_BOOKMARKALL, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLOSE, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxEVT_FRD_CLEARBOOKMARKS, LEditor::OnFindDialog)
EVT_COMMAND(wxID_ANY, wxCMD_EVENT_REMOVE_MATCH_INDICATOR, LEditor::OnRemoveMatchInidicator)
END_EVENT_TABLE()

// Instantiate statics
FindReplaceDialog* LEditor::m_findReplaceDlg = NULL;
FindReplaceData LEditor::m_findReplaceData;
std::map<wxString, int> LEditor::ms_bookmarkShapes;
bool LEditor::m_ccShowPrivateMembers = true;
bool LEditor::m_ccShowItemsComments = true;
bool LEditor::m_ccInitialized = false;

wxPrintData* g_printData = NULL;
wxPageSetupDialogData* g_pageSetupData = NULL;
static int ID_OPEN_URL = wxNOT_FOUND;

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

class clEditorDropTarget : public wxDropTarget
{
    wxStyledTextCtrl* m_stc;

public:
    clEditorDropTarget(wxStyledTextCtrl* stc)
        : m_stc(stc)
    {
        wxDataObjectComposite* dataobj = new wxDataObjectComposite();
        dataobj->Add(new wxTextDataObject(), true);
        dataobj->Add(new wxFileDataObject());
        SetDataObject(dataobj);
    }

    /**
     * @brief do the actual drop action
     * we support both text and file names
     */
    wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult)
    {
        if(!GetData()) { return wxDragError; }
        wxDataObjectComposite* dataobjComp = static_cast<wxDataObjectComposite*>(GetDataObject());
        if(!dataobjComp) return wxDragError;

        wxDataFormat format = dataobjComp->GetReceivedFormat();
        wxDataObject* dataobj = dataobjComp->GetObject(format);
        switch(format.GetType()) {
        case wxDF_FILENAME: {
            wxFileDataObject* fileNameObj = static_cast<wxFileDataObject*>(dataobj);
            DoFilesDrop(fileNameObj->GetFilenames());
        } break;
        case wxDF_UNICODETEXT: {
            wxTextDataObject* textObj = static_cast<wxTextDataObject*>(dataobj);
            wxString text = textObj->GetText();
#ifdef __WXOSX__
            // On OSX, textObj->GetText() returns some garbeled text
            // so use the editor to get the text that we want to copy/move
            text = m_stc->GetSelectedText();
#endif
            if(!DoTextDrop(text, x, y, (defaultDragResult == wxDragMove))) { return wxDragCancel; }
        } break;
        default:
            break;
        }
        return defaultDragResult;
    }

    /**
     * @brief open list of files in the editor
     */
    bool DoTextDrop(const wxString& text, wxCoord x, wxCoord y, bool moving)
    {
        // insert the text
        int pos = m_stc->PositionFromPoint(wxPoint(x, y));
        if(pos == wxNOT_FOUND) return false;

        // Don't allow dropping tabs on the editor
        static wxRegEx re("\\{Class:Notebook,TabIndex:([0-9]+)\\}\\{.*?\\}", wxRE_ADVANCED);
        if(re.Matches(text)) return false;

        int selStart = m_stc->GetSelectionStart();
        int selEnd = m_stc->GetSelectionEnd();

        // No text dnd if the drop is on the selection
        if((pos >= selStart) && (pos <= selEnd)) return false;
        int length = (selEnd - selStart);

        m_stc->BeginUndoAction();
        if(moving) {
            // Clear the selection

            bool movingForward = (pos > selEnd);
            m_stc->InsertText(pos, text);
            if(movingForward) {
                m_stc->Replace(selStart, selEnd, "");
                pos -= length;
            } else {
                m_stc->Replace(selStart + length, selEnd + length, "");
            }
            m_stc->SetSelectionStart(pos);
            m_stc->SetSelectionEnd(pos);
            m_stc->SetCurrentPos(pos);
        } else {
            m_stc->SelectNone();
            m_stc->SetSelectionStart(pos);
            m_stc->SetSelectionEnd(pos);
            m_stc->InsertText(pos, text);
            m_stc->SetCurrentPos(pos);
        }
        m_stc->EndUndoAction();
#ifndef __WXOSX__
        m_stc->CallAfter(&wxStyledTextCtrl::SetSelection, pos, pos + length);
#endif
        return true;
    }

    /**
     * @brief open list of files in the editor
     */
    void DoFilesDrop(const wxArrayString& filenames)
    {
        // Split the list into 2: files and folders
        wxArrayString files, folders;
        for(size_t i = 0; i < filenames.size(); ++i) {
            if(wxFileName::DirExists(filenames.Item(i))) {
                folders.Add(filenames.Item(i));
            } else {
                files.Add(filenames.Item(i));
            }
        }

        for(size_t i = 0; i < files.size(); ++i) {
            clMainFrame::Get()->GetMainBook()->OpenFile(files.Item(i));
        }
    }

    bool OnDrop(wxCoord x, wxCoord y) { return true; }
    wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult defResult) { return m_stc->DoDragOver(x, y, defResult); }
};

//=====================================================================

#if defined(__WXMSW__)
static bool MSWRemoveROFileAttribute(const wxFileName& fileName)
{
    DWORD dwAttrs = GetFileAttributes(fileName.GetFullPath().c_str());
    if(dwAttrs != INVALID_FILE_ATTRIBUTES) {
        if(dwAttrs & FILE_ATTRIBUTE_READONLY) {
            if(wxMessageBox(wxString::Format(wxT("'%s' \n%s\n%s"), fileName.GetFullPath(),
                                             _("has the read-only attribute set"),
                                             _("Would you like CodeLite to try and remove it?")),
                            _("CodeLite"), wxYES_NO | wxICON_QUESTION | wxCENTER) == wxYES) {
                // try to clear the read-only flag from the file
                if(SetFileAttributes(fileName.GetFullPath().c_str(), dwAttrs & ~(FILE_ATTRIBUTE_READONLY)) == FALSE) {
                    wxMessageBox(wxString::Format(wxT("%s '%s' %s"), _("Failed to open file"),
                                                  fileName.GetFullPath().c_str(), _("for write")),
                                 _("CodeLite"), wxOK | wxCENTER | wxICON_WARNING);
                    return false;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}
#endif

//=====================================================================
LEditor::LEditor(wxWindow* parent)
    : wxStyledTextCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(1, 1), wxNO_BORDER)
    , m_popupIsOn(false)
    , m_isDragging(false)
    , m_modifyTime(0)
    , m_isVisible(true)
    , m_hyperLinkIndicatroStart(wxNOT_FOUND)
    , m_hyperLinkIndicatroEnd(wxNOT_FOUND)
    , m_hyperLinkType(wxID_NONE)
    , m_hightlightMatchedBraces(true)
    , m_autoAddMatchedCurlyBrace(false)
    , m_autoAddNormalBraces(false)
    , m_autoAdjustHScrollbarWidth(true)
    , m_reloadingFile(false)
    , m_functionTip(NULL)
    , m_calltip(NULL)
    , m_lastCharEntered(0)
    , m_lastCharEnteredPos(0)
    , m_isFocused(true)
    , m_pluginInitializedRMenu(false)
    , m_positionToEnsureVisible(wxNOT_FOUND)
    , m_findBookmarksActive(false)
    , m_mgr(PluginManager::Get())
    , m_hasCCAnnotation(false)
    , m_richTooltip(NULL)
{
    DoUpdateOptions();
    PreferencesChanged();
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CONFIG_CHANGED, &LEditor::OnEditorConfigChanged, this);
    m_commandsProcessor.SetParent(this);

    SetDropTarget(new clEditorDropTarget(this));

    // User timer to check if we need to highlight markers
    m_timerHighlightMarkers = new wxTimer(this);
    m_timerHighlightMarkers->Start(100, true);

    Connect(m_timerHighlightMarkers->GetId(), wxEVT_TIMER, wxTimerEventHandler(LEditor::OnTimer), NULL, this);

    ms_bookmarkShapes[wxT("Small Rectangle")] = wxSTC_MARK_SMALLRECT;
    ms_bookmarkShapes[wxT("Rounded Rectangle")] = wxSTC_MARK_ROUNDRECT;
    ms_bookmarkShapes[wxT("Small Arrow")] = wxSTC_MARK_ARROW;
    ms_bookmarkShapes[wxT("Circle")] = wxSTC_MARK_CIRCLE;

    SetSyntaxHighlight();
    CmdKeyClear(wxT('D'), wxSTC_KEYMOD_CTRL); // clear Ctrl+D because we use it for something else
    Connect(wxEVT_STC_DWELLSTART, wxStyledTextEventHandler(LEditor::OnDwellStart), NULL, this);

    // Initialise the breakpt-marker array
    FillBPtoMarkerArray();

    // set EOL mode for the newly created file
    int eol = GetEOLByOS();
    int alternate_eol = GetEOLByContent();
    if(alternate_eol != wxNOT_FOUND) { eol = alternate_eol; }
    SetEOLMode(eol);

    // Create the various tip windows
    m_functionTip = new clEditorTipWindow(this);
    m_disableSmartIndent = GetOptions()->GetDisableSmartIndent();

    m_deltas = new EditorDeltasHolder;
    EventNotifier::Get()->Connect(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT,
                                  wxCommandEventHandler(LEditor::OnHighlightWordChecked), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CODEFORMATTER_INDENT_STARTING,
                                  wxCommandEventHandler(LEditor::OnFileFormatStarting), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CODEFORMATTER_INDENT_COMPLETED,
                                  wxCommandEventHandler(LEditor::OnFileFormatDone), NULL, this);

    Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnChangeActiveBookmarkType), this,
         XRCID("BookmarkTypes[start]"), XRCID("BookmarkTypes[end]"));

    // Notify that this instance is being instantiated
    clCommandEvent initEvent(wxEVT_EDITOR_INITIALIZING);
    initEvent.SetEventObject(this);
    EventNotifier::Get()->AddPendingEvent(initEvent);
}

LEditor::~LEditor()
{
    wxDELETE(m_richTooltip);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &LEditor::OnEditorConfigChanged, this);

    EventNotifier::Get()->Disconnect(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT,
                                     wxCommandEventHandler(LEditor::OnHighlightWordChecked), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CODEFORMATTER_INDENT_STARTING,
                                     wxCommandEventHandler(LEditor::OnFileFormatStarting), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CODEFORMATTER_INDENT_COMPLETED,
                                     wxCommandEventHandler(LEditor::OnFileFormatDone), NULL, this);

    Unbind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnChangeActiveBookmarkType), this,
           XRCID("BookmarkTypes[start]"), XRCID("BookmarkTypes[end]"));

    // free the timer
    Disconnect(m_timerHighlightMarkers->GetId(), wxEVT_TIMER, wxTimerEventHandler(LEditor::OnTimer), NULL, this);
    m_timerHighlightMarkers->Stop();
    wxDELETE(m_timerHighlightMarkers);

    // find deltas
    wxDELETE(m_deltas);

    if(this->HasCapture()) { this->ReleaseMouse(); }
}

time_t LEditor::GetFileLastModifiedTime() const { return GetFileModificationTime(m_fileName.GetFullPath()); }

void LEditor::SetSyntaxHighlight(const wxString& lexerName)
{
    ClearDocumentStyle();
    m_context = ContextManager::Get()->NewContext(this, lexerName);

    // Apply the lexer fonts and colours before we call
    // "SetProperties". (SetProperties function needs the correct font for
    // some of its settings)
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer(lexerName);
    if(lexer) { lexer->Apply(this); }
    SetProperties();

    SetEOL();
    m_context->SetActive();
    m_context->ApplySettings();

    UpdateColours();
}

void LEditor::SetSyntaxHighlight(bool bUpdateColors)
{
    ClearDocumentStyle();
    m_context = ContextManager::Get()->NewContextByFileName(this, m_fileName);

    SetProperties();

    m_context->SetActive();
    m_context->ApplySettings();
    if(bUpdateColors) { UpdateColours(); }
}

// Fills the struct array that marries breakpoint type to marker and mask
void LEditor::FillBPtoMarkerArray()
{
    BPtoMarker bpm;
    bpm.bp_type = BP_type_break;
    bpm.marker = smt_breakpoint;
    bpm.mask = mmt_breakpoint;
    bpm.marker_disabled = smt_bp_disabled;
    bpm.mask_disabled = mmt_bp_disabled;
    m_BPstoMarkers.push_back(bpm);

    BPtoMarker bpcmdm;
    bpcmdm.bp_type = BP_type_cmdlistbreak;
    bpcmdm.marker = smt_bp_cmdlist;
    bpcmdm.mask = mmt_bp_cmdlist;
    bpcmdm.marker_disabled = smt_bp_cmdlist_disabled;
    bpcmdm.mask_disabled = mmt_bp_cmdlist_disabled;
    m_BPstoMarkers.push_back(bpcmdm);

    BPtoMarker bpcondm;
    bpcondm.bp_type = BP_type_condbreak;
    bpcondm.marker = smt_cond_bp;
    bpcondm.mask = mmt_cond_bp;
    bpcondm.marker_disabled = smt_cond_bp_disabled;
    bpcondm.mask_disabled = mmt_cond_bp_disabled;
    m_BPstoMarkers.push_back(bpcondm);

    BPtoMarker bpignm;
    bpignm.bp_type = BP_type_ignoredbreak;
    bpignm.marker = bpignm.marker_disabled = smt_bp_ignored;
    bpignm.mask = bpignm.mask_disabled = mmt_bp_ignored; // Enabled/disabled are the same
    m_BPstoMarkers.push_back(bpignm);

    bpm.bp_type = BP_type_tempbreak;
    m_BPstoMarkers.push_back(bpm); // Temp is the same as non-temp
}

// Looks for a struct for this breakpoint-type
BPtoMarker LEditor::GetMarkerForBreakpt(enum BreakpointType bp_type)
{
    std::vector<BPtoMarker>::iterator iter = m_BPstoMarkers.begin();
    for(; iter != m_BPstoMarkers.end(); ++iter) {
        if((*iter).bp_type == bp_type) { return *iter; }
    }
    wxLogMessage(wxT("Breakpoint type not in vector!?"));
    return *iter;
}

void LEditor::SetCaretAt(long pos)
{
    DoSetCaretAt(pos);
    EnsureCaretVisible();
}

/// Setup some scintilla properties
void LEditor::SetProperties()
{
#ifndef __WXMSW__
    UsePopUp(false);
#else
    UsePopUp(0);
#endif
    
    SetRectangularSelectionModifier(wxSTC_KEYMOD_CTRL);
    SetAdditionalSelectionTyping(true);
    OptionsConfigPtr options = GetOptions();
    CallTipUseStyle(1);

    CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    MarkerEnableHighlight(options->IsHighlightFoldWhenActive());

    m_hightlightMatchedBraces = options->GetHighlightMatchedBraces();
    m_autoAddMatchedCurlyBrace = options->GetAutoAddMatchedCurlyBraces();
    m_autoAddNormalBraces = options->GetAutoAddMatchedNormalBraces();
    m_autoAdjustHScrollbarWidth = options->GetAutoAdjustHScrollBarWidth();
    m_disableSmartIndent = options->GetDisableSmartIndent();
    m_disableSemicolonShift = options->GetDisableSemicolonShift();
    SetMultipleSelection(true);
    SetMultiPaste(1);

    if(!m_hightlightMatchedBraces) {
        wxStyledTextCtrl::BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
        SetHighlightGuide(0);
    }

    SetVirtualSpaceOptions(options->GetOptions() & OptionsConfig::Opt_AllowCaretAfterEndOfLine ? 2 : 1);
    SetCaretStyle(options->GetOptions() & OptionsConfig::Opt_UseBlockCaret ? wxSTC_CARETSTYLE_BLOCK
                                                                           : wxSTC_CARETSTYLE_LINE);
    SetWrapMode(options->GetWordWrap() ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
    SetViewWhiteSpace(options->GetShowWhitspaces());
    SetMouseDwellTime(500);
    SetProperty(wxT("fold"), wxT("1"));
    SetProperty(wxT("fold.html"), wxT("1"));
    SetProperty(wxT("fold.comment"), wxT("1"));

    SetProperty(wxT("fold.at.else"), options->GetFoldAtElse() ? wxT("1") : wxT("0"));
    SetProperty(wxT("fold.preprocessor"), options->GetFoldPreprocessor() ? wxT("1") : wxT("0"));
    SetProperty(wxT("fold.compact"), options->GetFoldCompact() ? wxT("1") : wxT("0"));

    // Fold and comments as well
    SetProperty(wxT("fold.comment"), wxT("1"));
    SetModEventMask(wxSTC_MOD_DELETETEXT | wxSTC_MOD_INSERTTEXT | wxSTC_PERFORMED_UNDO | wxSTC_PERFORMED_REDO |
                    wxSTC_MOD_BEFOREDELETE | wxSTC_MOD_CHANGESTYLE);

    int caretSlop = 1;
    int caretZone = 20;
    int caretStrict = 0;
    int caretEven = 0;
    int caretJumps = 0;

    SetXCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

    caretSlop = 1;
    caretZone = 1;
    caretStrict = 4;
    caretEven = 8;
    caretJumps = 0;
    SetYCaretPolicy(caretStrict | caretSlop | caretEven | caretJumps, caretZone);

    SetCaretWidth(options->GetCaretWidth());
    SetCaretPeriod(options->GetCaretBlinkPeriod());
    SetMarginLeft(1);

    // Mark current line
    SetCaretLineVisible(options->GetHighlightCaretLine());
    SetCaretLineBackground(options->GetCaretLineColour());
    SetCaretLineBackAlpha(options->GetCaretLineAlpha());

    SetFoldFlags(options->GetUnderlineFoldLine() ? 16 : 0);
    SetEndAtLastLine(!options->GetScrollBeyondLastLine());

    //------------------------------------------
    // Margin settings
    //------------------------------------------

    // symbol margin
    SetMarginType(SYMBOLS_MARGIN_ID, wxSTC_MARGIN_SYMBOL);

    // Line numbers
    SetMarginType(NUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);

    // line number margin displays every thing but folding, bookmarks and breakpoint
    SetMarginMask(NUMBER_MARGIN_ID,
                  ~(mmt_folds | mmt_all_bookmarks | mmt_indicator | mmt_compiler | mmt_all_breakpoints));

    SetMarginType(EDIT_TRACKER_MARGIN_ID, 4); // Styled Text margin
    SetMarginWidth(EDIT_TRACKER_MARGIN_ID, options->GetHideChangeMarkerMargin() ? 0 : 3);
    SetMarginMask(EDIT_TRACKER_MARGIN_ID, 0);

    // Separators
    SetMarginType(SYMBOLS_MARGIN_SEP_ID, wxSTC_MARGIN_FORE);
    SetMarginMask(SYMBOLS_MARGIN_SEP_ID, 0);

    // Fold margin - allow only folder symbols to display
    SetMarginMask(FOLD_MARGIN_ID, wxSTC_MASK_FOLDERS);

    // Set margins' width
    SetMarginWidth(SYMBOLS_MARGIN_ID, options->GetDisplayBookmarkMargin() ? 16 : 0); // Symbol margin

    // Show the separator margin only if the fold margin is hidden
    // (otherwise the fold margin is the separator)
    SetMarginWidth(SYMBOLS_MARGIN_SEP_ID, 0);

    // allow everything except for the folding symbols
    SetMarginMask(SYMBOLS_MARGIN_ID, ~(wxSTC_MASK_FOLDERS));

    // Line number margin
    int pixelWidth = 4 + 5 * TextWidth(wxSTC_STYLE_LINENUMBER, wxT("9"));

    // Show number margin according to settings.
    SetMarginWidth(NUMBER_MARGIN_ID, options->GetDisplayLineNumbers() ? pixelWidth : 0);

    // Show the fold margin
    SetMarginWidth(FOLD_MARGIN_ID, options->GetDisplayFoldMargin() ? 16 : 0); // Fold margin

    // Mark fold margin & symbols margins as sensetive
    SetMarginSensitive(FOLD_MARGIN_ID, true);
    SetMarginSensitive(SYMBOLS_MARGIN_ID, true);

    // Right margin
    SetEdgeMode(options->GetEdgeMode());
    SetEdgeColumn(options->GetEdgeColumn());
    SetEdgeColour(options->GetEdgeColour());

    //---------------------------------------------------
    // Fold settings
    //---------------------------------------------------
    // Determine the folding symbols colours
    wxColour foldFgColour = wxColor(0xff, 0xff, 0xff);
    wxColour foldBgColour = wxColor(0x80, 0x80, 0x80);
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer(GetContext()->GetName());
    if(lexer && lexer->IsDark()) {
        const StyleProperty& defaultProperty = lexer->GetProperty(0);
        if(!defaultProperty.IsNull()) {
            foldFgColour = wxColour(defaultProperty.GetBgColour()).ChangeLightness(130);
            foldBgColour = wxColour(defaultProperty.GetBgColour());
        }
    } else if(lexer) {
        const StyleProperty& defaultProperty = lexer->GetProperty(0);
        if(!defaultProperty.IsNull()) {
            foldFgColour = wxColour(defaultProperty.GetBgColour()).ChangeLightness(70);
            foldBgColour = wxColour(defaultProperty.GetBgColour());
        }
    }

    // Define the folding style to be square
    if(options->GetFoldStyle() == wxT("Flatten Tree Square Headers")) {
        DefineMarker(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_BOXMINUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_BOXPLUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_BOXPLUSCONNECTED, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, foldFgColour, foldBgColour);

    } else if(options->GetFoldStyle() == wxT("Flatten Tree Circular Headers")) {
        DefineMarker(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_CIRCLEMINUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_CIRCLEPLUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNERCURVE, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_CIRCLEPLUSCONNECTED, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_CIRCLEMINUSCONNECTED, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER, foldFgColour, foldBgColour);

    } else if(options->GetFoldStyle() == wxT("Simple")) {
        DefineMarker(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_MINUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_PLUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_PLUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_MINUS, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);

    } else { // use wxT("Arrows") as the default
        DefineMarker(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROW, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROW, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, foldFgColour, foldBgColour);
        DefineMarker(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_BACKGROUND, foldFgColour, foldBgColour);
    }

    // Bookmark
    int marker = wxSTC_MARK_ARROW;
    std::map<wxString, int>::iterator iter = ms_bookmarkShapes.find(options->GetBookmarkShape());
    if(iter != ms_bookmarkShapes.end()) { marker = iter->second; }

    for(size_t bmt = smt_FIRST_BMK_TYPE; bmt <= smt_LAST_BMK_TYPE; ++bmt) {
        MarkerDefine(bmt, marker);
        MarkerSetBackground(bmt, options->GetBookmarkBgColour(bmt - smt_FIRST_BMK_TYPE));
        MarkerSetForeground(bmt, options->GetBookmarkFgColour(bmt - smt_FIRST_BMK_TYPE));
    }

    // Breakpoints
    for(size_t bmt = smt_FIRST_BP_TYPE; bmt <= smt_LAST_BP_TYPE; ++bmt) {
        MarkerSetBackground(smt_breakpoint, "RED");
        MarkerSetAlpha(smt_breakpoint, 30);
    }

    MarkerDefineBitmap(smt_breakpoint, wxBitmap(wxImage(stop_xpm)));
    MarkerDefineBitmap(smt_bp_disabled, wxBitmap(wxImage(BreakptDisabled)));
    // Give disabled breakpoints a "grey" look
    MarkerSetBackground(smt_bp_disabled, "GREY");
    MarkerSetAlpha(smt_bp_disabled, 30);

    MarkerDefineBitmap(smt_bp_cmdlist, wxBitmap(wxImage(BreakptCommandList)));
    MarkerDefineBitmap(smt_bp_cmdlist_disabled, wxBitmap(wxImage(BreakptCommandListDisabled)));
    MarkerDefineBitmap(smt_bp_ignored, wxBitmap(wxImage(BreakptIgnore)));
    MarkerDefineBitmap(smt_cond_bp, wxBitmap(wxImage(ConditionalBreakpt)));
    MarkerDefineBitmap(smt_cond_bp_disabled, wxBitmap(wxImage(ConditionalBreakptDisabled)));

    if(options->HasOption(OptionsConfig::Opt_Mark_Debugger_Line)) {
        MarkerDefine(smt_indicator, wxSTC_MARK_BACKGROUND, wxNullColour, options->GetDebuggerMarkerLine());
        MarkerSetAlpha(smt_indicator, 50);

    } else {

        wxImage img(arrow_right_green_xpm);
        wxBitmap bmp(img);
        MarkerDefineBitmap(smt_indicator, bmp);
        MarkerSetBackground(smt_indicator, wxT("LIME GREEN"));
        MarkerSetForeground(smt_indicator, wxT("BLACK"));
    }

    // warning and error markers
    MarkerDefine(smt_warning, wxSTC_MARK_SHORTARROW);
    MarkerSetForeground(smt_error, wxColor(128, 128, 0));
    MarkerSetBackground(smt_warning, wxColor(255, 215, 0));
    MarkerDefine(smt_error, wxSTC_MARK_SHORTARROW);
    MarkerSetForeground(smt_error, wxColor(128, 0, 0));
    MarkerSetBackground(smt_error, wxColor(255, 0, 0));

    CallTipSetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    CallTipSetForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));

#if defined(__WXMAC__)
    // turning off these two greatly improves performance
    // on Mac
    SetTwoPhaseDraw(true);
    SetBufferedDraw(true);
    SetLayoutCache(wxSTC_CACHE_DOCUMENT);

#elif defined(__WXGTK__)
    SetLayoutCache(wxSTC_CACHE_PAGE);

#else // MSW
    SetTwoPhaseDraw(true);
    SetBufferedDraw(true);
    SetLayoutCache(wxSTC_CACHE_PAGE);

#endif

    // indentation settings
    SetTabIndents(true);
    SetBackSpaceUnIndents(true);

    // Should we use spaces or tabs for indenting?
    // Usually we will ask the configuration, however
    // when using Makefile we _must_ use the TABS
    SetUseTabs((GetContext()->GetName().Lower() == "makefile") ? true : options->GetIndentUsesTabs());

    SetTabWidth(options->GetTabWidth());
    SetIndent(options->GetIndentWidth());
    SetIndentationGuides(options->GetShowIndentationGuidelines() ? 3 : 0);

    size_t frame_flags = clMainFrame::Get()->GetFrameGeneralInfo().GetFlags();
    SetViewEOL(frame_flags & CL_SHOW_EOL ? true : false);

    IndicatorSetUnder(1, true);
    IndicatorSetUnder(HYPERLINK_INDICATOR, true);
    IndicatorSetUnder(MATCH_INDICATOR, false);
    IndicatorSetUnder(DEBUGGER_INDICATOR, true);

    SetUserIndicatorStyleAndColour(wxSTC_INDIC_SQUIGGLE, wxT("RED"));

    wxColour col2(wxT("LIGHT BLUE"));
    wxString val2 = EditorConfigST::Get()->GetString(wxT("WordHighlightColour"));
    if(val2.IsEmpty() == false) { col2 = wxColour(val2); }

    IndicatorSetForeground(1, options->GetBookmarkBgColour(smt_find_bookmark - smt_FIRST_BMK_TYPE));
    IndicatorSetUnder(MARKER_WORD_HIGHLIGHT, true);
    IndicatorSetForeground(MARKER_WORD_HIGHLIGHT, col2);
    long alpha = EditorConfigST::Get()->GetInteger(wxT("WordHighlightAlpha"));
    if(alpha != wxNOT_FOUND) { IndicatorSetAlpha(MARKER_WORD_HIGHLIGHT, alpha); }

    IndicatorSetUnder(MARKER_FIND_BAR_WORD_HIGHLIGHT, true);
    IndicatorSetStyle(MARKER_FIND_BAR_WORD_HIGHLIGHT, wxSTC_INDIC_BOX);
    bool isDarkTheme = (lexer && lexer->IsDark());
    IndicatorSetForeground(MARKER_FIND_BAR_WORD_HIGHLIGHT, isDarkTheme ? "WHITE" : "BLACK");
    if(alpha != wxNOT_FOUND) { IndicatorSetAlpha(MARKER_FIND_BAR_WORD_HIGHLIGHT, alpha); }

    IndicatorSetUnder(MARKER_CONTEXT_WORD_HIGHLIGHT, true);
    IndicatorSetStyle(MARKER_CONTEXT_WORD_HIGHLIGHT, wxSTC_INDIC_BOX);
    IndicatorSetForeground(MARKER_CONTEXT_WORD_HIGHLIGHT, isDarkTheme ? "WHITE" : "BLACK");
    if(alpha != wxNOT_FOUND) { IndicatorSetAlpha(MARKER_CONTEXT_WORD_HIGHLIGHT, alpha); }

    IndicatorSetStyle(HYPERLINK_INDICATOR, wxSTC_INDIC_PLAIN);
    IndicatorSetStyle(MATCH_INDICATOR, wxSTC_INDIC_BOX);
    IndicatorSetForeground(MATCH_INDICATOR, wxT("GREY"));

    IndicatorSetStyle(DEBUGGER_INDICATOR, wxSTC_INDIC_BOX);
    IndicatorSetForeground(DEBUGGER_INDICATOR, wxT("GREY"));

    CmdKeyClear(wxT('L'), wxSTC_KEYMOD_CTRL); // clear Ctrl+D because we use it for something else

    // Set CamelCase caret movement
    if(options->GetCaretUseCamelCase()) {
        // selection
        CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_CTRL | wxSTC_KEYMOD_SHIFT, wxSTC_CMD_WORDPARTLEFTEXTEND);
        CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_CTRL | wxSTC_KEYMOD_SHIFT, wxSTC_CMD_WORDPARTRIGHTEXTEND);

        // movement
        CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_WORDPARTLEFT);
        CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_WORDPARTRIGHT);
    } else {
        // selection
        CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_CTRL | wxSTC_KEYMOD_SHIFT, wxSTC_CMD_WORDLEFTEXTEND);
        CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_CTRL | wxSTC_KEYMOD_SHIFT, wxSTC_CMD_WORDRIGHTEXTEND);

        // movement
        CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_WORDLEFT);
        CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_WORDRIGHT);
    }

#ifdef __WXOSX__
    CmdKeyAssign(wxSTC_KEY_DOWN, wxSTC_KEYMOD_CTRL, wxSTC_CMD_DOCUMENTEND);
    CmdKeyAssign(wxSTC_KEY_UP, wxSTC_KEYMOD_CTRL, wxSTC_CMD_DOCUMENTSTART);

    // OSX: wxSTC_KEYMOD_CTRL => CMD key
    CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_LINEEND);
    CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_CTRL, wxSTC_CMD_HOME);

    // OSX: wxSTC_KEYMOD_META => CONTROL key
    CmdKeyAssign(wxSTC_KEY_LEFT, wxSTC_KEYMOD_META, wxSTC_CMD_WORDPARTLEFT);
    CmdKeyAssign(wxSTC_KEY_RIGHT, wxSTC_KEYMOD_META, wxSTC_CMD_WORDPARTRIGHT);
#endif
}

void LEditor::OnSavePoint(wxStyledTextEvent& event)
{
    if(!GetIsVisible()) return;

    wxString title;
    if(GetModify()) {
        title << wxT("*");

    } else {

        if(GetMarginWidth(EDIT_TRACKER_MARGIN_ID)) {

            wxWindowUpdateLocker locker(this);
            int numlines = GetLineCount();
            for(int i = 0; i < numlines; i++) {
                int style = MarginGetStyle(i);
                if(style == CL_LINE_MODIFIED_STYLE) {
                    MarginSetText(i, wxT(" "));
                    MarginSetStyle(i, CL_LINE_SAVED_STYLE);
                }
            }
            Refresh();
        }
    }

    title << GetFileName().GetFullName();
    clMainFrame::Get()->GetMainBook()->SetPageTitle(this, title);
    DoUpdateTLWTitle(false);
}

void LEditor::OnCharAdded(wxStyledTextEvent& event)
{
    OptionsConfigPtr options = GetOptions();
    if(m_prevSelectionInfo.IsOk()) {
        if(event.GetKey() == '"' && options->IsWrapSelectionWithQuotes()) {
            DoWrapPrevSelectionWithChars('"', '"');
            return;
        } else if(event.GetKey() == '[' && options->IsWrapSelectionBrackets()) {
            DoWrapPrevSelectionWithChars('[', ']');
            return;
        } else if(event.GetKey() == '\'' && options->IsWrapSelectionWithQuotes()) {
            DoWrapPrevSelectionWithChars('\'', '\'');
            return;
        } else if(event.GetKey() == '(' && options->IsWrapSelectionBrackets()) {
            DoWrapPrevSelectionWithChars('(', ')');
            return;
        }
    }

    // reset the flag
    m_prevSelectionInfo.Clear();

    int pos = GetCurrentPos();
    bool canShowCompletionBox(true);
    // make sure line is visible
    int curLine = LineFromPosition(pos);
    if(!GetFoldExpanded(curLine)) { ToggleFold(curLine); }

    bool bJustAddedIndicator = false;
    int nextChar = SafeGetChar(pos), prevChar = SafeGetChar(pos - 2);

    //-------------------------------------
    // Smart quotes management
    //-------------------------------------
    if(options->GetAutoCompleteDoubleQuotes() && (event.GetKey() == '"' || event.GetKey() == '\'') &&
       event.GetKey() == GetCharAt(pos)) {
        CharRight();
        DeleteBack();
    } else if(options->GetAutoCompleteDoubleQuotes() && !wxIsalnum(nextChar) && !wxIsalnum(prevChar)) {
        // add complete quotes; but don't if the next char is alnum,
        // which is annoying if you're trying to retrofit quotes around a string!
        // Also not if the previous char is alnum: it's more likely (especially in non-code editors)
        // that someone is trying to type _don't_ than it's a burning desire to write _don''_
        if(event.GetKey() == wxT('"') && !m_context->IsCommentOrString(pos)) {
            InsertText(pos, wxT("\""));
            SetIndicatorCurrent(MATCH_INDICATOR);
            IndicatorFillRange(pos, 1);
            bJustAddedIndicator = true;

        } else if(event.GetKey() == wxT('\'') && !m_context->IsCommentOrString(pos)) {
            InsertText(pos, wxT("'"));
            SetIndicatorCurrent(MATCH_INDICATOR);
            IndicatorFillRange(pos, 1);
            bJustAddedIndicator = true;
        }
    }

    //-------------------------------------
    // Smart quotes management
    //-------------------------------------
    if(!bJustAddedIndicator && IndicatorValueAt(MATCH_INDICATOR, pos) && event.GetKey() == GetCharAt(pos)) {
        CharRight();
        DeleteBack();

    } else if(m_autoAddNormalBraces && (event.GetKey() == ')' || event.GetKey() == ']') &&
              event.GetKey() == GetCharAt(pos)) {
        // disable the auto brace adding when inside comment or string
        if(!m_context->IsCommentOrString(pos)) {
            CharRight();
            DeleteBack();
        }
    }

    wxChar matchChar(0);
    switch(event.GetKey()) {
    case ';':
        if(!m_disableSemicolonShift && !m_context->IsCommentOrString(pos)) m_context->SemicolonShift();
        break;

    case '(':
        if(m_context->IsCommentOrString(GetCurrentPos()) == false) { CodeComplete(); }
        matchChar = ')';
        break;

    case '[':
        matchChar = ']';
        break;

    case '{':
        m_context->AutoIndent(event.GetKey());
        matchChar = '}';
        break;

    case ':':
        m_context->AutoIndent(event.GetKey());
        break;

    case ')':
        // Remove one tip from the queue. If the queue new size is 0
        // the tooltip is then cancelled
        GetFunctionTip()->Remove();
        break;

    case '}':
        m_context->AutoIndent(event.GetKey());
        break;
    case '\n': {
        long matchedPos(wxNOT_FOUND);
        // incase ENTER was hit immediatly after we inserted '{' into the code...
        if(m_lastCharEntered == wxT('{') &&                         // Last char entered was {
           m_autoAddMatchedCurlyBrace &&                            // auto-add-match-brace option is enabled
           !m_disableSmartIndent &&                                 // the disable smart indent option is NOT enabled
           MatchBraceBack(wxT('}'), GetCurrentPos(), matchedPos) && // Insert it only if it match an open brace
           !m_context->IsDefaultContext() &&                        // the editor's context is NOT the default one
           matchedPos == m_lastCharEnteredPos) { // and that open brace must be the one that we have inserted

            matchChar = '}';

            BeginUndoAction();
            // Check to see if there are more chars on the line
            int curline = GetCurrentLine();

            // get the line end position, but without the EOL
            int lineEndPos = LineEnd(curline) - GetEolString().length();
            wxString restOfLine = GetTextRange(pos, lineEndPos);
            wxString restOfLineTrimmed = restOfLine;
            restOfLineTrimmed.Trim().Trim(false);
            bool shiftCode = (!restOfLineTrimmed.StartsWith(")")) && (!restOfLineTrimmed.IsEmpty());

            if(shiftCode) {
                SetSelection(pos, lineEndPos);
                ReplaceSelection("");
            }
            InsertText(pos, matchChar);
            CharRight();
            m_context->AutoIndent(wxT('}'));
            InsertText(pos, GetEolString());
            CharRight();
            SetCaretAt(pos);
            if(shiftCode) {
                // restore the content that we just removed
                InsertText(pos, restOfLine);
            }

            m_context->AutoIndent(wxT('\n'));
            EndUndoAction();

        } else {

            m_context->AutoIndent(event.GetKey());

            // incase we are typing in a folded line, make sure it is visible
            EnsureVisible(curLine + 1);
        }
    }

    break;
    default:
        break;
    }

    // Check for code completion strings
    wxChar charTyped = event.GetKey();
    // get the previous char. Note that the current position is already *after* the
    // current char, so we need to go back 2 chars
    wxChar firstChar = SafeGetChar(GetCurrentPos() - 2);

    wxString strTyped, strTyped2;
    strTyped << charTyped;
    strTyped2 << firstChar << charTyped;

    if((GetContext()->IsStringTriggerCodeComplete(strTyped) || GetContext()->IsStringTriggerCodeComplete(strTyped2)) &&
       !GetContext()->IsCommentOrString(GetCurrentPos())) {
        // this char should trigger a code completion
        CodeComplete();
    }

    if(matchChar && !m_disableSmartIndent && !m_context->IsCommentOrString(pos)) {
        if(matchChar == ')' && m_autoAddNormalBraces) {
            // Only add a close brace if the next char is whitespace
            // or if it's an already-matched ')' (which keeps things syntactically correct)
            long matchedPos(wxNOT_FOUND);
            int nextChar = SafeGetChar(pos);
            switch(nextChar) {
            case ')':
                if(!MatchBraceBack(matchChar, PositionBeforePos(pos), matchedPos)) { break; }
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                InsertText(pos, matchChar);
                SetIndicatorCurrent(MATCH_INDICATOR);
                // use grey colour rather than black, otherwise this indicator is invisible when using the
                // black theme
                IndicatorFillRange(pos, 1);
                break;
            }
        } else if(matchChar != '}' && m_autoAddNormalBraces) {
            InsertText(pos, matchChar);
            SetIndicatorCurrent(MATCH_INDICATOR);
            // use grey colour rather than black, otherwise this indicator is invisible when using the
            // black theme
            IndicatorFillRange(pos, 1);
        }
    }

    // Show the completion box if needed. canShowCompletionBox is set to false only if it was just dismissed
    // at the top of this function
    if(IsCompletionBoxShown() == false && canShowCompletionBox) {
        // display the keywords completion box only if user typed more than 2
        // chars && the caret is placed at the end of that word
        long startPos = WordStartPosition(pos, true);
        if(GetWordAtCaret().Len() >= 2 && pos - startPos >= 2) { m_context->OnUserTypedXChars(GetWordAtCaret()); }

        if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_WORD_ASSIST) {
            if(GetWordAtCaret().Len() == (size_t)TagsManagerST::Get()->GetCtagsOptions().GetMinWordLen() &&
               pos - startPos >= TagsManagerST::Get()->GetCtagsOptions().GetMinWordLen()) {
                CompleteWord();
            }
        }
    }

    if(event.GetKey() != 13) {
        // Dont store last character if it was \r
        m_lastCharEntered = event.GetKey();

        // Since we already entered the character...
        m_lastCharEnteredPos = PositionBefore(GetCurrentPos());
    }

    event.Skip();
}

void LEditor::SetEnsureCaretIsVisible(int pos, bool preserveSelection /*=true*/, bool forceDelay /*=false*/)
{
    OptionsConfigPtr opts = EditorConfigST::Get()->GetOptions();
    if(forceDelay || (opts && opts->GetWordWrap())) {
        // If the text may be word-wrapped, don't EnsureVisible immediately but from the
        // paintevent handler, so that scintilla has time to take word-wrap into account
        m_positionToEnsureVisible = pos;
        m_preserveSelection = preserveSelection;
    } else {
        DoEnsureCaretIsVisible(pos, preserveSelection);
        m_positionToEnsureVisible = wxNOT_FOUND;
    }
}

void LEditor::OnScnPainted(wxStyledTextEvent& event)
{
    event.Skip();
    if(m_positionToEnsureVisible == wxNOT_FOUND) { return; }
    // CL_DEBUG1(wxString::Format(wxT("OnScnPainted: position = %i, preserveSelection = %s"),
    //                           m_positionToEnsureVisible,
    //                           m_preserveSelection ? wxT("true") : wxT("false")));
    DoEnsureCaretIsVisible(m_positionToEnsureVisible, m_preserveSelection);
    m_positionToEnsureVisible = wxNOT_FOUND;
}

void LEditor::DoEnsureCaretIsVisible(int pos, bool preserveSelection)
{
    int start = -1, end = -1;
    if(preserveSelection) {
        start = GetSelectionStart();
        end = GetSelectionEnd();
    }

    SetCaretAt(pos);

    // and finally restore any selection if requested
    if(preserveSelection && (start != end)) { this->SetSelection(start, end); }
}

void LEditor::OnSciUpdateUI(wxStyledTextEvent& event)
{
    event.Skip();
    // Get current position
    long pos = GetCurrentPos();

    // ignore << and >>
    int charAfter = SafeGetChar(PositionAfter(pos));
    int charBefore = SafeGetChar(PositionBefore(pos));
    int beforeBefore = SafeGetChar(PositionBefore(PositionBefore(pos)));
    int charCurrnt = SafeGetChar(pos);

    bool hasSelection = (GetSelectionStart() != GetSelectionEnd());
    if(GetHighlightGuide() != wxNOT_FOUND) { SetHighlightGuide(0); }

    if(m_hightlightMatchedBraces) {
        if(hasSelection) {
            wxStyledTextCtrl::BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
        } else if((charCurrnt == '<' && charAfter == '<') ||    //<<
                  (charCurrnt == '<' && charBefore == '<') ||   //<<
                  (charCurrnt == '>' && charAfter == '>') ||    //>>
                  (charCurrnt == '>' && charBefore == '>') ||   //>>
                  (beforeBefore == '<' && charBefore == '<') || //<<
                  (beforeBefore == '>' && charBefore == '>') || //>>
                  (beforeBefore == '-' && charBefore == '>') || //->
                  (charCurrnt == '>' && charBefore == '-')) {   //->
            wxStyledTextCtrl::BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
        } else {
            if((charCurrnt == '{' || charCurrnt == '[' || GetCharAt(pos) == '<' || charCurrnt == '(') &&
               !m_context->IsCommentOrString(pos)) {
                BraceMatch((long)pos);
            } else if((charBefore == '{' || charBefore == '<' || charBefore == '[' || charBefore == '(') &&
                      !m_context->IsCommentOrString(PositionBefore(pos))) {
                BraceMatch((long)PositionBefore(pos));
            } else if((charCurrnt == '}' || charCurrnt == ']' || charCurrnt == '>' || charCurrnt == ')') &&
                      !m_context->IsCommentOrString(pos)) {
                BraceMatch((long)pos);
            } else if((charBefore == '}' || charBefore == '>' || charBefore == ']' || charBefore == ')') &&
                      !m_context->IsCommentOrString(PositionBefore(pos))) {
                BraceMatch((long)PositionBefore(pos));
            } else {
                wxStyledTextCtrl::BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
            }
        }
    }

    int mainSelectionPos = GetSelectionNCaret(GetMainSelection());
    int curLine = LineFromPosition(mainSelectionPos);

    wxString message;
    if(m_statusBarFields & kShowLine) { message << "Ln " << curLine + 1; }
    if(m_statusBarFields & kShowColumn) {
        message << (!message.empty() ? ", " : "") << "Col " << GetColumn(mainSelectionPos);
    }
    if(m_statusBarFields & kShowPosition) { message << (!message.empty() ? ", " : "") << "Pos " << mainSelectionPos; }
    if(m_statusBarFields & kShowLen) { message << (!message.empty() ? ", " : "") << "Len " << GetLength(); }
    // Always update the status bar with event, calling it directly causes performance degredation
    m_mgr->GetStatusBar()->SetLinePosColumn(message);

    SetIndicatorCurrent(MATCH_INDICATOR);
    IndicatorClearRange(0, pos);

    int end = PositionFromLine(curLine + 1);
    if(end >= pos && end < GetTextLength()) { IndicatorClearRange(end, GetTextLength() - end); }

    RecalcHorizontalScrollbar();

    static int lastLine(wxNOT_FOUND);

    // get the current position
    if((curLine != lastLine) && clMainFrame::Get()->GetMainBook()->IsNavBarShown()) {
        lastLine = curLine;
        clCodeCompletionEvent evtUpdateNavBar(wxEVT_CC_UPDATE_NAVBAR);
        evtUpdateNavBar.SetEditor(this);
        evtUpdateNavBar.SetLineNumber(curLine);
        EventNotifier::Get()->AddPendingEvent(evtUpdateNavBar);
    }

    // let the context handle this as well
    m_context->OnSciUpdateUI(event);
}

void LEditor::OnMarginClick(wxStyledTextEvent& event)
{
    int nLine = LineFromPosition(event.GetPosition());
    switch(event.GetMargin()) {
    case SYMBOLS_MARGIN_ID:
        // symbols / breakpoints margin
        {
            // If Shift-LeftDown, let the user drag any breakpoint marker
            if(event.GetShift()) {
                int markers = (MarkerGet(nLine) & mmt_all_breakpoints);
                if(!markers) { break; }
                // There doesn't seem to be an elegant way to get the defined bitmap for a marker
                wxBitmap bm;
                if(markers & mmt_bp_disabled) {
                    bm = wxBitmap(wxImage(BreakptDisabled));

                } else if(markers & mmt_bp_cmdlist) {
                    bm = wxBitmap(wxImage(BreakptCommandList));

                } else if(markers & mmt_bp_cmdlist_disabled) {
                    bm = wxBitmap(wxImage(BreakptCommandListDisabled));

                } else if(markers & mmt_bp_ignored) {
                    bm = wxBitmap(wxImage(BreakptIgnore));

                } else if(markers & mmt_cond_bp) {
                    bm = wxBitmap(wxImage(ConditionalBreakpt));

                } else if(markers & mmt_cond_bp_disabled) {
                    bm = wxBitmap(wxImage(ConditionalBreakptDisabled));

                } else {
                    // Make the standard bp bitmap the default
                    bm = wxBitmap(wxImage(stop_xpm));
                }

                // The breakpoint manager organises the actual drag/drop
                BreakptMgr* bpm = ManagerST::Get()->GetBreakpointsMgr();
                bpm->DragBreakpoint(this, nLine, bm);

                Connect(wxEVT_MOTION, wxMouseEventHandler(myDragImage::OnMotion), NULL, bpm->GetDragImage());
                Connect(wxEVT_LEFT_UP, wxMouseEventHandler(myDragImage::OnEndDrag), NULL, bpm->GetDragImage());

            } else {
                ToggleBreakpoint(nLine + 1);
            }
        }
        break;
    case FOLD_MARGIN_ID:
        // fold margin
        {
            ToggleFold(nLine);

            int caret_pos = GetCurrentPos();
            if(caret_pos != wxNOT_FOUND) {
                int caret_line = LineFromPosition(caret_pos);
                if(caret_line != wxNOT_FOUND && GetLineVisible(caret_line) == false) {
                    // the caret line is hidden (i.e. stuck in a fold) so set it somewhere else
                    while(caret_line >= 0) {
                        if((GetFoldLevel(caret_line) & wxSTC_FOLDLEVELHEADERFLAG) && GetLineVisible(caret_line)) {
                            SetCaretAt(PositionFromLine(caret_line));
                            break;
                        }
                        caret_line--;
                    }
                }
            }

            // Try to make as much as possible of the originally-displayed code stay in the same screen position
            // That's no problem if the fold-head is visible: that line and above automatically stay in place
            // However if it's off screen and the user clicks in a margin to fold, no part of the function will stay on
            // screen
            // The following code scrolls the correct amount to keep the position of the lines *below* the function
            // unchanged
            // This also brings the newly-folded function into view.
            // NB It fails if the cursor was originally inside the new fold; but at least then the fold head gets shown
            int foldparent = GetFoldParent(nLine);
            int firstvisibleline = GetFirstVisibleLine();
            if(!(GetFoldLevel(nLine) & wxSTC_FOLDLEVELHEADERFLAG) // If the click was below the fold head
               && (foldparent < firstvisibleline)) {              // and the fold head is off the screen
                int linestoscroll = foldparent - GetLastChild(foldparent, -1);
                // If there are enough lines above the screen to scroll downwards, do so
                if((firstvisibleline + linestoscroll) >= 0) { // linestoscroll will always be negative
                    LineScroll(0, linestoscroll);
                }
            }
        }
        break;
    default:
        break;
    }
}

void LEditor::DefineMarker(int marker, int markerType, wxColor fore, wxColor back)
{
    MarkerDefine(marker, markerType);
    MarkerSetForeground(marker, fore);
    MarkerSetBackground(marker, back);
}

bool LEditor::SaveFile()
{
    if(this->GetModify()) {
        if(GetFileName().FileExists() == false) { return SaveFileAs(); }

        // first save the file content
        if(!SaveToFile(m_fileName)) return false;

        // if we managed to save the file, remove the 'read only' attribute
        clMainFrame::Get()->GetMainBook()->MarkEditorReadOnly(this);

        // Take a snapshot of the current deltas. We'll need this as a 'base' for any future FindInFiles call
        m_deltas->OnFileSaved();

        wxString projName = GetProjectName();
        if(projName.Trim().Trim(false).IsEmpty()) return true;

        // clear cached file, this function does nothing if the file is not cached
        TagsManagerST::Get()->ClearCachedFile(GetFileName().GetFullPath());

        //
        if(ManagerST::Get()->IsShutdownInProgress() || ManagerST::Get()->IsWorkspaceClosing()) { return true; }

        if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISABLE_AUTO_PARSING) { return true; }
        m_context->RetagFile();
    }
    return true;
}

bool LEditor::SaveFileAs(const wxString& newname, const wxString& savePath)
{
    // Prompt the user for a new file name
    const wxString ALL(wxT("All Files (*)|*"));
    wxFileDialog dlg(this, _("Save As"), savePath.IsEmpty() ? m_fileName.GetPath() : savePath,
                     newname.IsEmpty() ? m_fileName.GetFullName() : newname, ALL, wxFD_SAVE | wxFD_OVERWRITE_PROMPT,
                     wxDefaultPosition);

    if(dlg.ShowModal() == wxID_OK) {
        // get the path
        wxFileName name(dlg.GetPath());

        // Prepare the "SaveAs" event, but dont send it just yet
        clFileSystemEvent saveAsEvent(wxEVT_FILE_SAVEAS);
        saveAsEvent.SetPath(m_fileName.Exists() ? m_fileName.GetFullPath() : wxString(""));
        saveAsEvent.SetNewpath(name.GetFullPath());

        if(!SaveToFile(name)) {
            wxMessageBox(_("Failed to save file"), _("Error"), wxOK | wxICON_ERROR);
            return false;
        }
        m_fileName = name;

        // update the tab title (again) since we really want to trigger an update to the file tooltip
        clMainFrame::Get()->GetMainBook()->SetPageTitle(this, m_fileName.GetFullName());
        DoUpdateTLWTitle(false);

        // update syntax highlight
        SetSyntaxHighlight();

        clMainFrame::Get()->GetMainBook()->MarkEditorReadOnly(this);

        // Fire the "File renamed" event
        EventNotifier::Get()->AddPendingEvent(saveAsEvent);
        return true;
    }
    return false;
}

// an internal function that does the actual file writing to disk
bool LEditor::SaveToFile(const wxFileName& fileName)
{
    {
        // Notify about file being saved
        clCommandEvent beforeSaveEvent(wxEVT_BEFORE_EDITOR_SAVE);
        beforeSaveEvent.SetFileName(fileName.GetFullPath());
        EventNotifier::Get()->ProcessEvent(beforeSaveEvent);

        if(!beforeSaveEvent.IsAllowed()) {
            // A plugin vetoed the file save
            return false;
        }
    }

    // Do all the writing on the temporary file
    wxFileName intermediateFile(fileName);
    intermediateFile.SetFullName("~" + fileName.GetFullName() + "." + ::wxGetUserId());

    {
        // Ensure that a temporary file with this name does not exist
        FileUtils::Deleter deleter(intermediateFile);
    }

    // Ensure that the temporary file that we will be creating
    // is removed when leaving the function
    FileUtils::Deleter deleter(intermediateFile);

    // save the file using the user's defined encoding
    // unless we got a BOM set
    wxCSConv fontEncConv(GetOptions()->GetFileFontEncoding());
    bool useBuiltIn = (GetOptions()->GetFileFontEncoding() == wxFONTENCODING_UTF8);

    // trim lines / append LF if needed
    TrimText(GetOptions()->GetTrimLine(), GetOptions()->GetAppendLF());

    // BUG#2982452
    // try to manually convert the text to make sure that the conversion does not fail
    wxString theText = GetText();

    // If the intermediate file exists, it means that we got problems deleting it (usually permissions)
    // Notify the user and continue
    if(intermediateFile.Exists()) {
        // We failed to delete the intermediate file
        ::wxMessageBox(
            wxString::Format(_("Unable to create intermediate file\n'%s'\nfor writing. File already exists!"),
                             intermediateFile.GetFullPath()),
            "CodeLite", wxOK | wxCENTER | wxICON_ERROR, EventNotifier::Get()->TopFrame());
        return false;
    }

    wxFFile file(intermediateFile.GetFullPath().GetData(), "wb");
    if(!file.IsOpened()) {
        // Nothing to be done
        wxMessageBox(wxString::Format(_("Failed to open file\n'%s'\nfor write"), fileName.GetFullPath()), "CodeLite",
                     wxOK | wxCENTER | wxICON_ERROR);
        return false;
    }

    // Convert the text
    const wxWX2MBbuf buf = theText.mb_str(useBuiltIn ? (const wxMBConv&)wxConvUTF8 : (const wxMBConv&)fontEncConv);
    if(!buf.data()) {
        wxMessageBox(wxString::Format(wxT("%s\n%s '%s'"), _("Save file failed!"),
                                      _("Could not convert the file to the requested encoding"),
                                      wxFontMapper::GetEncodingName(GetOptions()->GetFileFontEncoding())),
                     "CodeLite", wxOK | wxICON_WARNING);
        return false;
    }

    if((buf.length() == 0) && !theText.IsEmpty()) {
        // something went wrong in the conversion process
        wxString errmsg;
        errmsg << _(
            "File text conversion failed!\nCheck your file font encoding from\nSettings | Preferences | Misc | Locale");
        wxMessageBox(errmsg, "CodeLite", wxOK | wxICON_ERROR | wxCENTER, wxTheApp->GetTopWindow());
        return false;
    }

    if(!m_fileBom.IsEmpty()) {
        // restore the BOM
        file.Write(m_fileBom.GetData(), m_fileBom.Len());
    }
    file.Write(buf.data(), strlen(buf.data()));
    file.Close();

    // keep the original file permissions
    wxFileName symlinkedFile = fileName;
    if(wxIsFileSymlink(fileName)) { symlinkedFile = wxReadLink(fileName); }

    mode_t origPermissions = 0;
    if(!FileUtils::GetFilePermissions(symlinkedFile, origPermissions)) {
        clWARNING() << "Failed to read file permissions." << fileName << clEndl;
    }

    // If this file is not writable, prompt the user before we do something stupid
    if(symlinkedFile.FileExists() && !symlinkedFile.IsFileWritable()) {
        // Prompt the user
        if(::wxMessageBox(wxString() << _("The file\n") << fileName.GetFullPath()
                                     << _("\nis a read only file, continue?"),
                          "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING,
                          EventNotifier::Get()->TopFrame()) != wxYES) {
            return false;
        }
    }

// The write was done to a temporary file, override it
#ifdef __WXMSW__
    if(!::wxRenameFile(intermediateFile.GetFullPath(), symlinkedFile.GetFullPath(), true)) {
        bool bSaveSucceeded = false;
        // Check if the file has the ReadOnly attribute and attempt to remove it
        if(MSWRemoveROFileAttribute(symlinkedFile)) {
            if(!::wxRenameFile(intermediateFile.GetFullPath(), symlinkedFile.GetFullPath(), true)) {
                wxMessageBox(wxString::Format(_("Failed to override read-only file")), "CodeLite",
                             wxOK | wxICON_WARNING);
                return false;
            } else {
                bSaveSucceeded = true;
            }
        }

        if(!bSaveSucceeded) {
            // Try clearing the clang cache and try again
            ClangCodeCompletion::Instance()->ClearCache();
            if(!::wxRenameFile(intermediateFile.GetFullPath(), symlinkedFile.GetFullPath(), true)) {
                wxMessageBox(wxString::Format(_("Failed to override read-only file")), "CodeLite",
                             wxOK | wxICON_WARNING);
                return false;
            }
        }
    }
#else
    if(!::wxRenameFile(intermediateFile.GetFullPath(), symlinkedFile.GetFullPath(), true)) {
        // Try clearing the clang cache and try again
        wxMessageBox(wxString::Format(_("Failed to override read-only file")), "CodeLite", wxOK | wxICON_WARNING);
        return false;
    }
#endif

    // Restore the orig file permissions
    if(origPermissions) { FileUtils::SetFilePermissions(symlinkedFile, origPermissions); }

    // update the modification time of the file
    m_modifyTime = GetFileModificationTime(symlinkedFile.GetFullPath());
    SetSavePoint();

    // update the tab title (remove the star from the file name)
    clMainFrame::Get()->GetMainBook()->SetPageTitle(this, fileName.GetFullName());

    if(fileName.GetExt() != m_fileName.GetExt()) {
        // new context is required
        SetSyntaxHighlight();
    }

    // Fire a wxEVT_FILE_SAVED event
    EventNotifier::Get()->PostFileSavedEvent(fileName.GetFullPath());
    return true;
}

// this function is called before the debugger startup
void LEditor::UpdateBreakpoints()
{
    ManagerST::Get()->GetBreakpointsMgr()->DeleteAllBreakpointsByFileName(GetFileName().GetFullPath());

    // iterate over the array and update the breakpoint manager with updated line numbers for each breakpoint
    std::map<int, std::vector<BreakpointInfo> >::iterator iter = m_breakpointsInfo.begin();
    for(; iter != m_breakpointsInfo.end(); iter++) {
        int handle = iter->first;
        int line = MarkerLineFromHandle(handle);
        if(line >= 0) {
            for(size_t i = 0; i < iter->second.size(); i++) {
                iter->second.at(i).lineno = line + 1;
                iter->second.at(i).origin = BO_Editor;
            }
        }

        ManagerST::Get()->GetBreakpointsMgr()->SetBreakpoints(iter->second);
        // update the Breakpoints pane too
        clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
    }
}

wxString LEditor::GetWordAtCaret()
{
    // Get the partial word that we have
    long pos = GetCurrentPos();
    long start = WordStartPosition(pos, true);
    long end = WordEndPosition(pos, true);
    return GetTextRange(start, end);
}

//---------------------------------------------------------------------------
// Most of the functionality for this functionality
// is done in the Language & TagsManager objects, however,
// as you can see below, much work still needs to be done in the application
// layer (outside of the library) to provide the input arguments for
// the CodeParser library
//---------------------------------------------------------------------------
void LEditor::CompleteWord(bool onlyRefresh)
{
    if(EventNotifier::Get()->IsEventsDiabled()) return;
    if(AutoCompActive()) return; // Don't clobber the boxes

    // Let the plugins a chance to override the default behavior
    clCodeCompletionEvent evt(wxEVT_CC_CODE_COMPLETE);
    evt.SetPosition(GetCurrentPosition());
    evt.SetEditor(this);
    evt.SetInsideCommentOrString(m_context->IsCommentOrString(PositionBefore(GetCurrentPos())));

    evt.SetEventObject(this);

    if(EventNotifier::Get()->ProcessEvent(evt)) {
        // the plugin handled the code-complete request
        return;

    } else {
        CodeCompletionManager::Get().SetWordCompletionRefreshNeeded(onlyRefresh);
        // let the built-in context do the job
        m_context->CompleteWord();
    }
}

//------------------------------------------------------------------
// AutoCompletion, by far the nicest feature of a modern IDE
// This function attempts to resolve the string to the left of
// the '.', '->' operator and to display a popup menu with
// list of possible matches
//------------------------------------------------------------------
void LEditor::CodeComplete(bool refreshingList)
{
    if(EventNotifier::Get()->IsEventsDiabled()) return;
    if(AutoCompActive()) return; // Don't clobber the boxes..

    if(!refreshingList) {
        clCodeCompletionEvent evt(wxEVT_CC_CODE_COMPLETE);
        evt.SetPosition(GetCurrentPosition());
        evt.SetInsideCommentOrString(m_context->IsCommentOrString(PositionBefore(GetCurrentPos())));
        evt.SetEventObject(this);
        evt.SetEditor(this);

        if(EventNotifier::Get()->ProcessEvent(evt))
            // the plugin handled the code-complete request
            return;

        else {
            // let the built-in context do the job
            m_context->CodeComplete();
        }

    } else {
        CompleteWord();
    }
}

//----------------------------------------------------------------
// Demonstrate how to achieve symbol browsing using the CodeLite
// library, in addition we implements here a memory for allowing
// user to go back and forward
//----------------------------------------------------------------
void LEditor::GotoDefinition()
{
    // Let the plugins process this first
    wxString word = GetWordAtCaret();
    clCodeCompletionEvent event(wxEVT_CC_FIND_SYMBOL, GetId());
    event.SetEventObject(this);
    event.SetEditor(this);
    event.SetWord(word);
    event.SetPosition(GetCurrentPosition());
    event.SetInsideCommentOrString(m_context->IsCommentOrString(PositionBefore(GetCurrentPos())));
    if(EventNotifier::Get()->ProcessEvent(event)) return;

    m_context->GotoDefinition();
}

void LEditor::OnDwellStart(wxStyledTextEvent& event)
{
    // First see if we're hovering over a breakpoint or build marker
    // Assume anywhere to the left of the fold margin qualifies
    int margin = 0;
    wxPoint pt(ScreenToClient(wxGetMousePosition()));
    wxRect clientRect = GetClientRect();

    // If the mouse is no longer over the editor, cancel the tooltip
    if(!clientRect.Contains(pt)) { return; }

    // Always cancel the previous tooltip...
    DoCancelCodeCompletionBox();

    for(int n = 0; n < FOLD_MARGIN_ID; ++n) {
        margin += GetMarginWidth(n);
    }

    if(IsContextMenuOn() || IsDragging() || !GetSTCFocus()) {
        // Don't cover the context menu or a potential drop-point with a calltip!
        // And, especially, try to avoid scintilla's party-piece: placing a permanent calltip on top of some innocent
        // app!

    } else if(event.GetX() > 0 // It seems that we can get spurious events with x == 0
              && event.GetX() < margin) {

        // We can't use event.GetPosition() here, as in the margin it returns -1
        int position = PositionFromPoint(wxPoint(event.GetX(), event.GetY()));
        int line = LineFromPosition(position);
        wxString tooltip, title;
        wxString fname = GetFileName().GetFullPath();

        if(MarkerGet(line) & mmt_all_breakpoints) {
            ManagerST::Get()->GetBreakpointsMgr()->GetTooltip(fname, line + 1, tooltip, title);
        }

        else if(MarkerGet(line) & mmt_all_bookmarks) {
            GetBookmarkTooltip(line, tooltip, title);
        }

        // Compiler marker takes precedence over any other tooltip on that margin
        if((MarkerGet(line) & mmt_compiler) && m_compilerMessagesMap.count(line)) {
            // Get the compiler tooltip
            tooltip = m_compilerMessagesMap.find(line)->second;
            if(MarkerGet(line) & (1 << smt_warning)) {
                title = "<color=\"yellow\">WARNING</color>";
            } else {
                title = "<color=\"pink\">ERROR</color>";
            }
        }

        if(!title.IsEmpty() && !tooltip.IsEmpty()) { DoShowCalltip(-1, title, tooltip); }

    } else if(ManagerST::Get()->DbgCanInteract() && clientRect.Contains(pt)) {
        m_context->OnDbgDwellStart(event);

    } else if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISP_TYPE_INFO) {

        // Allow the plugins to override the default built-in behavior of displaying
        // the type info tooltip
        clCodeCompletionEvent evtTypeinfo(wxEVT_CC_TYPEINFO_TIP, GetId());
        evtTypeinfo.SetEventObject(this);
        evtTypeinfo.SetEditor(this);
        evtTypeinfo.SetPosition(event.GetPosition());
        evtTypeinfo.SetInsideCommentOrString(m_context->IsCommentOrString(event.GetPosition()));

        if(EventNotifier::Get()->ProcessEvent(evtTypeinfo)) {
            // Did the user provide a tooltip?
            if(!evtTypeinfo.GetTooltip().IsEmpty()) { DoShowCalltip(wxNOT_FOUND, "", evtTypeinfo.GetTooltip()); }
        } else {
            m_context->OnDwellStart(event);
        }
    }
}

void LEditor::OnDwellEnd(wxStyledTextEvent& event)
{
    DoCancelCalltip();
    m_context->OnDwellEnd(event);
    m_context->OnDbgDwellEnd(event);
}

void LEditor::OnCallTipClick(wxStyledTextEvent& event) { m_context->OnCallTipClick(event); }

void LEditor::OnMenuCommand(wxCommandEvent& event)
{
    MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());

    if(handler) { handler->ProcessCommandEvent(this, event); }
}

void LEditor::OnUpdateUI(wxUpdateUIEvent& event)
{
    MenuEventHandlerPtr handler = MenuManager::Get()->GetHandler(event.GetId());

    if(handler) { handler->ProcessUpdateUIEvent(this, event); }
}

//-----------------------------------------------------------------------
// Misc functions
//-----------------------------------------------------------------------

wxString LEditor::PreviousWord(int pos, int& foundPos)
{
    // Get the partial word that we have
    wxChar ch = 0;
    long curpos = PositionBefore(pos);
    if(curpos == 0) {
        foundPos = wxNOT_FOUND;
        return wxT("");
    }

    while(true) {
        ch = GetCharAt(curpos);
        if(ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
            long tmpPos = curpos;
            curpos = PositionBefore(curpos);
            if(curpos == 0 && tmpPos == curpos) break;
        } else {
            long start = WordStartPosition(curpos, true);
            long end = WordEndPosition(curpos, true);
            return GetTextRange(start, end);
        }
    }
    foundPos = wxNOT_FOUND;
    return wxT("");
}

wxChar LEditor::PreviousChar(const int& pos, int& foundPos, bool wantWhitespace)
{
    wxChar ch = 0;
    long curpos = PositionBefore(pos);
    if(curpos == 0) {
        foundPos = curpos;
        return ch;
    }

    while(true) {
        ch = GetCharAt(curpos);
        if(ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
            // if the caller is intrested in whitepsaces,
            // simply return it
            if(wantWhitespace) {
                foundPos = curpos;
                return ch;
            }

            long tmpPos = curpos;
            curpos = PositionBefore(curpos);
            if(curpos == 0 && tmpPos == curpos) break;
        } else {
            foundPos = curpos;
            return ch;
        }
    }
    foundPos = -1;
    return ch;
}

wxChar LEditor::NextChar(const int& pos, int& foundPos)
{
    wxChar ch = 0;
    long nextpos = pos;
    while(true) {

        if(nextpos >= GetLength()) break;

        ch = GetCharAt(nextpos);
        if(ch == wxT('\t') || ch == wxT(' ') || ch == wxT('\r') || ch == wxT('\v') || ch == wxT('\n')) {
            nextpos = PositionAfter(nextpos);
            continue;
        } else {
            foundPos = nextpos;
            return ch;
        }
    }
    foundPos = -1;
    return ch;
}

int LEditor::FindString(const wxString& str, int flags, const bool down, long pos)
{
    // initialize direction
    if(down) {
        SetTargetStart(pos);
        SetTargetEnd(GetLength());
    } else {
        SetTargetStart(pos);
        SetTargetEnd(0);
    }
    SetSearchFlags(flags);

    // search string
    int _pos = SearchInTarget(str);
    if(_pos >= 0)
        return _pos;
    else
        return -1;
}

bool LEditor::MatchBraceBack(const wxChar& chCloseBrace, const long& pos, long& matchedPos)
{
    if(pos <= 0) return false;

    wxChar chOpenBrace;

    switch(chCloseBrace) {
    case '}':
        chOpenBrace = '{';
        break;
    case ')':
        chOpenBrace = '(';
        break;
    case ']':
        chOpenBrace = '[';
        break;
    case '>':
        chOpenBrace = '<';
        break;
    default:
        return false;
    }

    long nPrevPos = pos;
    wxChar ch;
    int depth = 1;

    // We go backward
    while(true) {
        if(nPrevPos == 0) break;
        nPrevPos = PositionBefore(nPrevPos);

        // Make sure we are not in a comment
        if(m_context->IsCommentOrString(nPrevPos)) continue;

        ch = GetCharAt(nPrevPos);
        if(ch == chOpenBrace) {
            // Dec the depth level
            depth--;
            if(depth == 0) {
                matchedPos = nPrevPos;
                return true;
            }
        } else if(ch == chCloseBrace) {
            // Inc depth level
            depth++;
        }
    }
    return false;
}

void LEditor::RecalcHorizontalScrollbar()
{
    if(m_autoAdjustHScrollbarWidth) { ::clRecalculateSTCHScrollBar(this); }
}

//--------------------------------------------------------
// Brace match
//--------------------------------------------------------

bool LEditor::IsCloseBrace(int position)
{
    return GetCharAt(position) == '}' || GetCharAt(position) == ']' || GetCharAt(position) == ')';
}

bool LEditor::IsOpenBrace(int position)
{
    return GetCharAt(position) == '{' || GetCharAt(position) == '[' || GetCharAt(position) == '(';
}

void LEditor::MatchBraceAndSelect(bool selRegion)
{
    // Get current position
    long pos = GetCurrentPos();

    if(IsOpenBrace(pos) && !m_context->IsCommentOrString(pos)) {
        BraceMatch(selRegion);
        return;
    }

    if(IsOpenBrace(PositionBefore(pos)) && !m_context->IsCommentOrString(PositionBefore(pos))) {
        SetCurrentPos(PositionBefore(pos));
        BraceMatch(selRegion);
        return;
    }

    if(IsCloseBrace(pos) && !m_context->IsCommentOrString(pos)) {
        BraceMatch(selRegion);
        return;
    }

    if(IsCloseBrace(PositionBefore(pos)) && !m_context->IsCommentOrString(PositionBefore(pos))) {
        SetCurrentPos(PositionBefore(pos));
        BraceMatch(selRegion);
        return;
    }
}

void LEditor::BraceMatch(long pos)
{
    // Check if we have a match
    int indentCol = 0;
    long endPos = wxStyledTextCtrl::BraceMatch(pos);
    if(endPos != wxSTC_INVALID_POSITION) {
        wxStyledTextCtrl::BraceHighlight(pos, endPos);
#ifdef __WXMSW__
        Refresh();
#endif
        if(GetIndentationGuides() != 0 && GetIndent() > 0) {
            // Highlight indent guide if exist
            indentCol =
                std::min(GetLineIndentation(LineFromPosition(pos)), GetLineIndentation(LineFromPosition(endPos)));
            indentCol /= GetIndent();
            indentCol *= GetIndent(); // round down to nearest indentation guide column
            SetHighlightGuide(GetLineIndentation(LineFromPosition(pos)));
        }
    } else {
        wxStyledTextCtrl::BraceBadLight(pos);
    }
    SetHighlightGuide(indentCol);
}

void LEditor::BraceMatch(const bool& bSelRegion)
{
    // Check if we have a match
    long endPos = wxStyledTextCtrl::BraceMatch(GetCurrentPos());
    if(endPos != wxSTC_INVALID_POSITION) {
        // Highlight indent guide if exist
        long startPos = GetCurrentPos();
        if(bSelRegion) {
            // Select the range
            if(endPos > startPos) {
                SetSelectionEnd(PositionAfter(endPos));
                SetSelectionStart(startPos);
            } else {
                SetSelectionEnd(PositionAfter(startPos));
                SetSelectionStart(endPos);
            }
        } else {
            SetSelectionEnd(endPos);
            SetSelectionStart(endPos);
            SetCurrentPos(endPos);
        }
        EnsureCaretVisible();
    }
}

void LEditor::SetActive()
{
    // ensure that the top level window parent of this editor is 'Raised'
    bool raise(true);
#ifdef __WXGTK__
    // On Wayland and gtk+3.22, raise not only fails, it hangs the subsequent DnD call. See
    // http://trac.wxwidgets.org/ticket/17853
    raise = !clMainFrame::Get()->GetIsWaylandSession();
#endif
    DoUpdateTLWTitle(raise);

    // if the find and replace dialog is opened, set ourself
    // as the event owners
    if(m_findReplaceDlg) { m_findReplaceDlg->SetEventOwner(GetEventHandler()); }

    SetFocus();
    SetSTCFocus(true);

    m_context->SetActive();

    wxStyledTextEvent dummy;
    OnSciUpdateUI(dummy);
}

// Popup a Find/Replace dialog
/**
 * \brief
 * \param isReplaceDlg
 */
void LEditor::DoFindAndReplace(bool isReplaceDlg)
{
    if(m_findReplaceDlg == NULL) {
        // Create the dialog
        m_findReplaceDlg = new FindReplaceDialog(clMainFrame::Get(), m_findReplaceData);
        m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
    }

    if(m_findReplaceDlg->IsShown()) {
        // make sure that dialog has focus and that this instace
        // of LEditor is the owner for the events
        m_findReplaceDlg->SetEventOwner(this->GetEventHandler());
        m_findReplaceDlg->SetFocus();
        return;
    }

    // the search always starts from the current line
    // if there is a selection, set it
    if(GetSelectedText().IsEmpty() == false) {
        // if this string does not exist in the array add it
        wxString Selection(GetSelectedText());
        if(isReplaceDlg) {
            if(!Selection.Contains(wxT("\n"))) {
                // Don't try to use a multiline selection as the 'find' token. It looks ugly and
                // it won't be what the user wants (it'll be the 'Replace in Selection' selection)
                m_findReplaceDlg->GetData().SetFindString(GetSelectedText());
            } else {
                m_findReplaceDlg->GetData().SetFlags(m_findReplaceDlg->GetData().GetFlags() | wxFRD_SELECTIONONLY);
            }
        } else {
            // always set the find string in 'Find' dialog
            m_findReplaceDlg->GetData().SetFindString(GetSelectedText());
        }
    }

    if(isReplaceDlg) { // Zeroise
        m_findReplaceDlg->ResetReplacedCount();
        m_findReplaceDlg->SetReplacementsMessage(frd_dontshowzeros);
    }

    m_findReplaceDlg->Show(isReplaceDlg ? REPLACE_DLG : FIND_DLG);
}

void LEditor::OnFindDialog(wxCommandEvent& event)
{
    wxEventType type = event.GetEventType();
    bool dirDown = !(m_findReplaceDlg->GetData().GetFlags() & wxFRD_SEARCHUP ? true : false);

    if(type == wxEVT_FRD_FIND_NEXT) {
        FindNext(m_findReplaceDlg->GetData());
    } else if(type == wxEVT_FRD_REPLACE) {
        // Perform a "Replace" operation
        if(!Replace()) {
            int saved_pos = GetCurrentPos();

            // place the caret at the new position
            if(dirDown) {
                SetCaretAt(0);
            } else {
                SetCaretAt(GetLength());
            }

            // replace again
            if(!Replace()) {
                // restore the caret
                SetCaretAt(saved_pos);

                // popup a message
                wxMessageBox(_("Can not find the string '") + m_findReplaceDlg->GetData().GetFindString() + wxT("'"),
                             _("CodeLite"), wxICON_WARNING | wxOK);
            }
        }
    } else if(type == wxEVT_FRD_REPLACEALL) {
        ReplaceAll();

    } else if(type == wxEVT_FRD_BOOKMARKALL) {
        SetFindBookmarksActive(true);
        MarkAllFinds();

    } else if(type == wxEVT_FRD_CLEARBOOKMARKS) {
        DelAllMarkers(smt_find_bookmark);
        SetFindBookmarksActive(false);
        clMainFrame::Get()->SelectBestEnvSet();
    }
}

void LEditor::FindNext(const FindReplaceData& data)
{
    bool dirDown = !(data.GetFlags() & wxFRD_SEARCHUP ? true : false);
    if(!FindAndSelect(data)) {
        int saved_pos = GetCurrentPos();
        if(dirDown) {
            DoSetCaretAt(0);
        } else {
            DoSetCaretAt(GetLength());
        }

        if(!FindAndSelect(data)) {
            // restore the caret
            DoSetCaretAt(saved_pos);
            // Kill the "...continued from start" statusbar message
            m_mgr->GetStatusBar()->SetMessage("");
            ::wxMessageBox(_("Can not find the string '") + data.GetFindString() + wxT("'"), _("CodeLite"),
                           wxOK | wxICON_WARNING);
        }
    } else {
        // The string *was* found, without needing to restart from the top
        // So cancel any previous statusbar restart message
        m_mgr->GetStatusBar()->SetMessage("");
    }
}

bool LEditor::Replace() { return Replace(m_findReplaceDlg->GetData()); }

bool LEditor::FindAndSelect() { return FindAndSelect(m_findReplaceDlg->GetData()); }

bool LEditor::FindAndSelect(const FindReplaceData& data)
{
    wxString findWhat = data.GetFindString();
    size_t flags = SearchFlags(data);
    int offset = GetCurrentPos();

    int dummy, dummy_len(0), dummy_c, dummy_len_c(0);
    if(GetSelectedText().IsEmpty() == false) {
        if(flags & wxSD_SEARCH_BACKWARD) {
            // searching up
            if(StringFindReplacer::Search(GetSelectedText().wc_str(), GetSelectedText().Len(), findWhat.wc_str(), flags,
                                          dummy, dummy_len, dummy_c, dummy_len_c) &&
               dummy_len_c == (int)GetSelectedText().Len()) {
                // place the caret at the start of the selection so the search will skip this selected text
                int sel_start = GetSelectionStart();
                int sel_end = GetSelectionEnd();
                sel_end > sel_start ? offset = sel_start : offset = sel_end;
            }
        } else {
            // searching down
            if(StringFindReplacer::Search(GetSelectedText().wc_str(), 0, findWhat.wc_str(), flags, dummy, dummy_len,
                                          dummy_c, dummy_len_c) &&
               dummy_len_c == (int)GetSelectedText().Len()) {
                // place the caret at the end of the selection so the search will skip this selected text
                int sel_start = GetSelectionStart();
                int sel_end = GetSelectionEnd();
                sel_end > sel_start ? offset = sel_end : offset = sel_start;
            }
        }
    }

    int pos(0);
    int match_len(0);

    if(StringFindReplacer::Search(GetText().wc_str(), offset, findWhat.wc_str(), flags, pos, match_len)) {

        SetEnsureCaretIsVisible(pos);

        if(flags & wxSD_SEARCH_BACKWARD) {
            SetSelection(pos + match_len, pos);
        } else {
            SetSelection(pos, pos + match_len);
        }

        return true;
    }
    return false;
}

bool LEditor::FindAndSelect(const wxString& _pattern, const wxString& name)
{
    return DoFindAndSelect(_pattern, name, 0, NavMgr::Get());
}

void LEditor::FindAndSelectV(const wxString& _pattern, const wxString& name, int pos /*=0*/,
                             NavMgr* WXUNUSED(unused)) // Similar but returns void, so can be async
{
    // Use CallAfter() here. With wxGTK-3.1 (perhaps due to its scintilla update) if the file wasn't already loaded,
    // EnsureVisible() is called too early and fails
    wxArrayString strings; // CallAfter can only cope with 2 parameters, so combine the wxStrings
    ClearSelections();
    strings.Add(_pattern);
    strings.Add(name);
    CallAfter(&LEditor::DoFindAndSelectV, strings, pos);
}

void LEditor::DoFindAndSelectV(const wxArrayString& strings, int pos) // Called with CallAfter()
{
    wxCHECK_RET(strings.Count() == 2, "Unexpected number of wxStrings supplied");
    wxString _pattern(strings.Item(0));
    wxString name(strings.Item(1));
    DoFindAndSelect(_pattern, name, pos, NavMgr::Get());
}

bool LEditor::Replace(const FindReplaceData& data)
{
    // the string to be replaced should be selected
    if(GetSelectedText().IsEmpty() == false) {
        int pos(0);
        int match_len(0);
        size_t flags = SearchFlags(data);
        if(StringFindReplacer::Search(GetSelectedText().wc_str(), 0, data.GetFindString().wc_str(), flags, pos,
                                      match_len)) {
            ReplaceSelection(data.GetReplaceString());
            m_findReplaceDlg->IncReplacedCount();
            m_findReplaceDlg->SetReplacementsMessage();
        }
    }

    //  and find another match in the document
    return FindAndSelect();
}

size_t LEditor::SearchFlags(const FindReplaceData& data)
{
    size_t flags = 0;
    size_t wxflags = data.GetFlags();
    wxflags& wxFRD_MATCHWHOLEWORD ? flags |= wxSD_MATCHWHOLEWORD : flags = flags;
    wxflags& wxFRD_MATCHCASE ? flags |= wxSD_MATCHCASE : flags = flags;
    wxflags& wxFRD_REGULAREXPRESSION ? flags |= wxSD_REGULAREXPRESSION : flags = flags;
    wxflags& wxFRD_SEARCHUP ? flags |= wxSD_SEARCH_BACKWARD : flags = flags;
    return flags;
}

//----------------------------------------------
// Folds
//----------------------------------------------
void LEditor::ToggleCurrentFold()
{
    int line = GetCurrentLine();
    if(line >= 0) {
        ToggleFold(line);

        if(GetLineVisible(line) == false) {
            // the caret line is hidden, make sure the caret is visible
            while(line >= 0) {
                if((GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) && GetLineVisible(line)) {
                    SetCaretAt(PositionFromLine(line));
                    break;
                }
                line--;
            }
        }
    }
}

void LEditor::DoRecursivelyExpandFolds(bool expand, int startline, int endline)
{
    for(int line = startline; line < endline; ++line) {
        if(GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) {
            int BottomOfFold = GetLastChild(line, -1);

            if(expand) {
                // Expand this fold
                SetFoldExpanded(line, true);
                ShowLines(line + 1, BottomOfFold);
                // Recursively do any contained child folds
                DoRecursivelyExpandFolds(expand, line + 1, BottomOfFold);
            } else {
                DoRecursivelyExpandFolds(expand, line + 1, BottomOfFold);
                // Hide this fold
                SetFoldExpanded(line, false);
                HideLines(line + 1, BottomOfFold);
            }

            line = BottomOfFold; // Now skip over the fold we've just dealt with, ready for any later siblings
        }
    }
}

void LEditor::ToggleAllFoldsInSelection()
{
    int selStart = GetSelectionStart();
    int selEnd = GetSelectionEnd();
    if(selStart == selEnd) {
        return; // No selection. UpdateUI prevents this from the menu, but not from an accelerator
    }

    int startline = LineFromPos(selStart);
    int endline = LineFromPos(selEnd);
    if(startline == endline) {
        ToggleFold(startline); // For a single-line selection just toggle
        return;
    }
    if(startline > endline) { wxSwap(startline, endline); }

    // First see if there are any folded lines in the selection. If there are, we'll be in 'unfold' mode
    bool expanding(false);
    for(int line = startline; line < endline;
        ++line) { // not <=. If only the last line of the sel is folded it's unlikely that the user meant it
        if(!GetLineVisible(line)) {
            expanding = true;
            break;
        }
    }

    for(int line = startline; line < endline; ++line) {
        if(!(GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG)) { continue; }
        int BottomOfFold = GetLastChild(line, -1);
        if(BottomOfFold > (endline + 1)) { // GetLastChild() seems to be 1-based, not zero-based. Without the +1, a } at
                                           // endline will be considered outside the selection
            continue;                      // This fold continues past the end of the selection
        }
        DoRecursivelyExpandFolds(expanding, line, BottomOfFold);
        line = BottomOfFold;
    }

    if(!expanding) {
        // The caret will (surely) be inside the selection, and unless it was on the first line or an unfolded one,
        // it'll now be hidden
        // If so place it at the top, which will be visible. Unfortunately SetCaretAt() destroys the selection,
        // and I can't find a way to preserve/reinstate it while still setting the caret. DoEnsureCaretIsVisible() also
        // fails :(
        int caretline = LineFromPos(GetCurrentPos());
        if(!GetLineVisible(caretline)) { SetCaretAt(selStart); }
    }
}

// If the cursor is on/in/below an open fold, collapse all. Otherwise expand all
void LEditor::FoldAll()
{
    // >(0,-1);  SciTE did this here, but it doesn't seem to accomplish anything

    // First find the current fold-point, and ask it whether or not it's folded
    int lineSeek = GetCurrentLine();
    while(true) {
        if(GetFoldLevel(lineSeek) & wxSTC_FOLDLEVELHEADERFLAG) break;
        int parentline = GetFoldParent(lineSeek); // See if we're inside a fold area
        if(parentline >= 0) {
            lineSeek = parentline;
            break;
        } else
            lineSeek--; // Must have been between folds
        if(lineSeek < 0) return;
    }
    bool expanded = GetFoldExpanded(lineSeek);

    int maxLine = GetLineCount();

    // Some files, especially headers with #ifndef FOO_H, will collapse into one big fold
    // So, if we're collapsing, skip any all-encompassing top level fold
    bool SkipTopFold = false;
    if(expanded) {
        int topline = 0;
        while(!(GetFoldLevel(topline) & wxSTC_FOLDLEVELHEADERFLAG)) {
            // This line wasn't a fold-point, so inc until we find one
            if(++topline >= maxLine) return;
        }
        int BottomOfFold = GetLastChild(topline, -1);
        if(BottomOfFold >= maxLine || BottomOfFold == -1) return;
        // We've found the bottom of the topmost fold-point. See if there's another fold below it
        ++BottomOfFold;
        while(!(GetFoldLevel(BottomOfFold) & wxSTC_FOLDLEVELHEADERFLAG)) {
            if(++BottomOfFold >= maxLine) {
                // If we're here, the top fold must encompass the whole file, so set the flag
                SkipTopFold = true;
                break;
            }
        }
    }

    // Now go through the whole document, toggling folds that match the original one's level if we're collapsing
    // or all collapsed folds if we're expanding (so that internal folds get expanded too).
    // The (level & wxSTC_FOLDLEVELHEADERFLAG) means "If this level is a Fold start"
    // (level & wxSTC_FOLDLEVELNUMBERMASK) returns a value for the 'indent' of the fold.
    // This starts at wxSTC_FOLDLEVELBASE==1024. A sub fold-point == 1025, a subsub 1026...
    for(int line = 0; line < maxLine; line++) {
        int level = GetFoldLevel(line);
        // If we're skipping an all-encompassing fold, we use wxSTC_FOLDLEVELBASE+1
        if((level & wxSTC_FOLDLEVELHEADERFLAG) &&
           (expanded ? ((level & wxSTC_FOLDLEVELNUMBERMASK) == (wxSTC_FOLDLEVELBASE + SkipTopFold))
                     : ((level & wxSTC_FOLDLEVELNUMBERMASK) >= wxSTC_FOLDLEVELBASE))) {
            if(GetFoldExpanded(line) == expanded) ToggleFold(line);
        }
    }

    // make sure the caret is visible. If it was hidden, place it at the first visible line
    int curpos = GetCurrentPos();
    if(curpos != wxNOT_FOUND) {
        int curline = LineFromPosition(curpos);
        if(curline != wxNOT_FOUND && GetLineVisible(curline) == false) {
            // the caret line is hidden, make sure the caret is visible
            while(curline >= 0) {
                if((GetFoldLevel(curline) & wxSTC_FOLDLEVELHEADERFLAG) && GetLineVisible(curline)) {
                    SetCaretAt(PositionFromLine(curline));
                    break;
                }
                curline--;
            }
        }
    }
}

// Toggle all the highest-level folds in the selection i.e. if the selection contains folds of level 3, 4 and 5, toggle
// all the level 3 ones
void LEditor::ToggleTopmostFoldsInSelection()
{
    int selStart = GetSelectionStart();
    int selEnd = GetSelectionEnd();
    if(selStart == selEnd) {
        return; // No selection. UpdateUI prevents this from the menu, but not from an accelerator
    }

    int startline = LineFromPos(selStart);
    int endline = LineFromPos(selEnd);
    if(startline == endline) {
        ToggleFold(startline); // For a single-line selection just toggle
        return;
    }
    if(startline > endline) { wxSwap(startline, endline); }

    // Go thru the selection to find the topmost contained fold level. Also ask the first one of this level if it's
    // folded
    int toplevel(wxSTC_FOLDLEVELNUMBERMASK);
    bool expanded(true);
    for(int line = startline; line < endline;
        ++line) { // not <=. If only the last line of the sel is folded it's unlikely that the user meant it
        if(!GetLineVisible(line)) { break; }
        if(GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) {
            int level = GetFoldLevel(line) & wxSTC_FOLDLEVELNUMBERMASK;
            if(level < toplevel) {
                toplevel = level;
                expanded = GetFoldExpanded(line);
            }
        }
    }
    if(toplevel == wxSTC_FOLDLEVELNUMBERMASK) { // No fold found
        return;
    }

    for(int line = startline; line < endline; ++line) {
        if(GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG) {
            if((GetFoldLevel(line) & wxSTC_FOLDLEVELNUMBERMASK) == toplevel && GetFoldExpanded(line) == expanded) {
                ToggleFold(line);
            }
        }
    }

    // make sure the caret is visible. If it was hidden, place it at the first visible line
    int curpos = GetCurrentPos();
    if(expanded && curpos != wxNOT_FOUND) {
        int curline = LineFromPosition(curpos);
        if(curline != wxNOT_FOUND && GetLineVisible(curline) == false) {
            // the caret line is hidden, make sure the caret is visible
            while(curline >= 0) {
                if((GetFoldLevel(curline) & wxSTC_FOLDLEVELHEADERFLAG) && GetLineVisible(curline)) {
                    SetCaretAt(PositionFromLine(curline));
                    break;
                }
                curline--;
            }
        }
    }
}

void LEditor::StoreCollapsedFoldsToArray(clEditorStateLocker::VecInt_t& folds) const
{
    clEditorStateLocker::SerializeFolds(const_cast<wxStyledTextCtrl*>(static_cast<const wxStyledTextCtrl*>(this)),
                                        folds);
}

void LEditor::LoadCollapsedFoldsFromArray(const clEditorStateLocker::VecInt_t& folds)
{
    clEditorStateLocker::ApplyFolds(GetCtrl(), folds);
}

//----------------------------------------------
// Bookmarks
//----------------------------------------------
void LEditor::AddMarker()
{
    int nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    MarkerAdd(nLine, GetActiveBookmarkType());

    // Notify about marker changes
    NotifyMarkerChanged(nLine);
}

void LEditor::DelMarker()
{
    int nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    for(int i = smt_FIRST_BMK_TYPE; i < smt_LAST_BMK_TYPE; ++i) {
        MarkerDelete(nLine, i);
        // Notify about marker changes
        NotifyMarkerChanged(nLine);
    }
}

void LEditor::ToggleMarker()
{
    // Add/Remove marker
    if(!LineIsMarked(mmt_standard_bookmarks)) {
        AddMarker();
    } else {
        DelMarker();
    }
}

bool LEditor::LineIsMarked(enum marker_mask_type mask)
{
    int nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    int nBits = MarkerGet(nLine);
    // 'mask' is a bitmap representing a bookmark, or a type of breakpt, or...
    return (nBits & mask ? true : false);
}

void LEditor::StoreMarkersToArray(wxArrayString& bookmarks)
{
    clEditorStateLocker::SerializeBookmarks(GetCtrl(), bookmarks);
}

void LEditor::LoadMarkersFromArray(const wxArrayString& bookmarks)
{
    clEditorStateLocker::ApplyBookmarks(GetCtrl(), bookmarks);
}

void LEditor::DelAllMarkers(int which_type)
{
    // Delete all relevant markers from the view
    // If 0, delete just the currently active type, -1 delete them all.
    // Otherwise just the specified type, which will usually be the 'find' bookmark
    if(which_type > 0) {
        MarkerDeleteAll(which_type);
    } else if(which_type == 0) {
        MarkerDeleteAll(GetActiveBookmarkType());
    } else {
        for(size_t bmt = smt_FIRST_BMK_TYPE; bmt <= smt_LAST_BMK_TYPE; ++bmt) {
            MarkerDeleteAll(bmt);
        }
    }

    // delete other markers as well
    SetIndicatorCurrent(1);
    IndicatorClearRange(0, GetLength());

    SetIndicatorCurrent(MARKER_WORD_HIGHLIGHT);
    IndicatorClearRange(0, GetLength());

    SetIndicatorCurrent(HYPERLINK_INDICATOR);
    IndicatorClearRange(0, GetLength());

    SetIndicatorCurrent(DEBUGGER_INDICATOR);
    IndicatorClearRange(0, GetLength());

    SetIndicatorCurrent(MARKER_FIND_BAR_WORD_HIGHLIGHT);
    IndicatorClearRange(0, GetLength());

    // Notify about marker changes
    NotifyMarkerChanged();
}

bool LEditor::HasCompilerMarkers()
{
    // try to locate *any* compiler marker
    int mask = mmt_compiler;
    int nFoundLine = MarkerNext(0, mask);
    return nFoundLine >= 0;
}

size_t LEditor::GetFindMarkers(std::vector<std::pair<int, wxString> >& bookmarksVector)
{
    int nPos = 0;
    int nFoundLine = LineFromPosition(nPos);
    while(nFoundLine < GetLineCount()) {
        nFoundLine = MarkerNext(nFoundLine, GetActiveBookmarkMask());
        if(nFoundLine == wxNOT_FOUND) { break; }
        wxString snippet = GetLine(nFoundLine);
        snippet.Trim().Trim(false);
        if(!snippet.IsEmpty()) {
            snippet = snippet.Mid(0, snippet.size() > 40 ? 40 : snippet.size());
            if(snippet.size() == 40) { snippet << "..."; }
        }
        bookmarksVector.push_back({ nFoundLine + 1, snippet });
        ++nFoundLine;
    }
    return bookmarksVector.size();
}

void LEditor::FindNextMarker()
{
    int nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    int nFoundLine = MarkerNext(nLine + 1, GetActiveBookmarkMask());
    if(nFoundLine >= 0) {
        // mark this place before jumping to next marker
        CenterLine(nFoundLine);
    } else {
        // We reached the last marker, try again from top
        nLine = LineFromPosition(0);
        nFoundLine = MarkerNext(nLine, GetActiveBookmarkMask());
        if(nFoundLine >= 0) { CenterLine(nFoundLine); }
    }
    if(nFoundLine >= 0) {
        EnsureVisible(nFoundLine);
        EnsureCaretVisible();
    }
}

void LEditor::FindPrevMarker()
{
    int nPos = GetCurrentPos();
    int nLine = LineFromPosition(nPos);
    int mask = GetActiveBookmarkMask();
    int nFoundLine = MarkerPrevious(nLine - 1, mask);
    if(nFoundLine >= 0) {
        CenterLine(nFoundLine);
    } else {
        // We reached first marker, try again from button
        int nFileSize = GetLength();
        nLine = LineFromPosition(nFileSize);
        nFoundLine = MarkerPrevious(nLine, mask);
        if(nFoundLine >= 0) { CenterLine(nFoundLine); }
    }
    if(nFoundLine >= 0) {
        EnsureVisible(nFoundLine);
        EnsureCaretVisible();
    }
}

bool LEditor::ReplaceAll()
{
    int offset(0);

    wxString findWhat = m_findReplaceDlg->GetData().GetFindString();
    wxString replaceWith = m_findReplaceDlg->GetData().GetReplaceString();
    size_t flags = SearchFlags(m_findReplaceDlg->GetData());

    int pos(0);
    int match_len(0);
    int posInChars(0);
    int match_lenInChars(0);

    wxString txt;
    if(m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY) {
        txt = GetSelectedText();
    } else {
        txt = GetText();
    }

    bool replaceInSelectionOnly = m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY;

    BeginUndoAction();
    m_findReplaceDlg->ResetReplacedCount();

    long savedPos = GetCurrentPos();
    while(StringFindReplacer::Search(txt.wc_str(), offset, findWhat.wc_str(), flags, pos, match_len, posInChars,
                                     match_lenInChars)) {
        // Manipulate the buffer
        txt.Remove(posInChars, match_lenInChars);
        txt.insert(posInChars, replaceWith);

        // When not in 'selection only' update the editor buffer as well
        if(!replaceInSelectionOnly) {
            SetSelectionStart(pos);
            SetSelectionEnd(pos + match_len);
            ReplaceSelection(replaceWith);
        }

        m_findReplaceDlg->IncReplacedCount();
        offset = pos + clUTF8Length(replaceWith.wc_str(), replaceWith.length()); // match_len;
    }

    if(replaceInSelectionOnly) {

        // Prepare the next selection
        int selStart = GetSelectionStart();
        int selEnd = selStart + txt.Len();

        // replace the selection
        ReplaceSelection(txt);

        // Keep the selection
        SetSelectionStart(selStart);
        SetSelectionEnd(selEnd);

        // place the caret at the end of the selection
        EnsureCaretVisible();

    } else {
        // The editor buffer was already updated
        // Restore the caret
        SetCaretAt(savedPos);
    }

    EndUndoAction();
    m_findReplaceDlg->SetReplacementsMessage();
    return m_findReplaceDlg->GetReplacedCount() > 0;
}

bool LEditor::MarkAllFinds()
{
    wxString findWhat = m_findReplaceDlg->GetData().GetFindString();

    if(findWhat.IsEmpty()) { return false; }

    // Save the caret position
    long savedPos = GetCurrentPos();
    size_t flags = SearchFlags(m_findReplaceDlg->GetData());

    int pos(0);
    int match_len(0);

    // remove reverse search
    flags &= ~wxSD_SEARCH_BACKWARD;
    int offset(0);

    wxString txt;
    int fixed_offset(0);
    if(m_findReplaceDlg->GetData().GetFlags() & wxFRD_SELECTIONONLY) {
        txt = GetSelectedText();
        fixed_offset = GetSelectionStart();
    } else {
        txt = GetText();
    }

    DelAllMarkers(smt_find_bookmark);

    // set the active indicator to be 1
    SetIndicatorCurrent(1);

    while(StringFindReplacer::Search(txt.wc_str(), offset, findWhat.wc_str(), flags, pos, match_len)) {
        MarkerAdd(LineFromPosition(fixed_offset + pos), smt_find_bookmark);

        // add indicator as well
        IndicatorFillRange(fixed_offset + pos, match_len);
        offset = pos + match_len;
    }

    // Restore the caret
    SetCurrentPos(savedPos);
    EnsureCaretVisible();
    clMainFrame::Get()->SelectBestEnvSet(); // Updates the statusbar display
    NotifyMarkerChanged();
    return true;
}

int LEditor::GetActiveBookmarkType() const
{
    if(IsFindBookmarksActive()) {
        return smt_find_bookmark;

    } else {
        return BookmarkManager::Get().GetActiveBookmarkType();
    }
}

enum marker_mask_type LEditor::GetActiveBookmarkMask() const
{
    wxASSERT(1 << smt_find_bookmark == mmt_find_bookmark);
    if(IsFindBookmarksActive()) {
        return mmt_find_bookmark;
    } else {
        return (marker_mask_type)(1 << BookmarkManager::Get().GetActiveBookmarkType());
    }
}

wxString LEditor::GetBookmarkLabel(sci_marker_types type)
{
    wxCHECK_MSG(type >= smt_FIRST_BMK_TYPE && type <= smt_LAST_BMK_TYPE, "", "Invalid marker type");
    wxString label = BookmarkManager::Get().GetMarkerLabel(type);
    if(label.empty()) { label = wxString::Format("Type %i", type - smt_FIRST_BMK_TYPE + 1); }

    return label;
}

void LEditor::OnChangeActiveBookmarkType(wxCommandEvent& event)
{
    int requested = event.GetId() - XRCID("BookmarkTypes[start]");
    BookmarkManager::Get().SetActiveBookmarkType(requested + smt_FIRST_BMK_TYPE - 1);
    if((requested + smt_FIRST_BMK_TYPE - 1) != smt_find_bookmark) { SetFindBookmarksActive(false); }

    clMainFrame::Get()->SelectBestEnvSet(); // Updates the statusbar display
}

void LEditor::GetBookmarkTooltip(int lineno, wxString& tip, wxString& title)
{
    title << "<b>Bookmarks</b>";
    // If we've arrived here we know there's a bookmark on the line; however we don't know which type(s)
    // If multiple, list each, with the visible one first
    int linebits = MarkerGet(lineno);
    if(linebits & GetActiveBookmarkMask()) {
        wxString label = GetBookmarkLabel((sci_marker_types)GetActiveBookmarkType());
        wxString suffix = label.Lower().Contains("bookmark") ? "" : " bookmark";
        tip << label << suffix;
    }

    for(int bmt = smt_FIRST_BMK_TYPE; bmt <= smt_LAST_BMK_TYPE; ++bmt) {
        if(bmt != GetActiveBookmarkType()) {
            if(linebits & (1 << bmt)) {
                if(!tip.empty()) { tip << "\n"; }
                wxString label = GetBookmarkLabel((sci_marker_types)bmt);
                wxString suffix = label.Lower().Contains("bookmark") ? "" : " bookmark";
                tip << label << suffix;
            }
        }
    }
}

void LEditor::OpenFile()
{
    wxWindowUpdateLocker locker(this);
    SetReloadingFile(true);

    DoCancelCalltip();
    GetFunctionTip()->Deactivate();

    if(m_fileName.GetFullPath().IsEmpty() == true || !m_fileName.FileExists()) {
        SetEOLMode(GetEOLByOS());
        SetReloadingFile(false);
        return;
    }

    // State locker (on dtor it restores: bookmarks, current line, breakpoints and folds)
    clEditorStateLocker stateLocker(GetCtrl());

    int lineNumber = GetCurrentLine();
    m_mgr->GetStatusBar()->SetMessage(_("Loading file..."));

    wxString text;

    // Read the file we currently support:
    // BOM, Auto-Detect encoding & User defined encoding
    m_fileBom.Clear();
    ReadFileWithConversion(m_fileName.GetFullPath(), text, GetOptions()->GetFileFontEncoding(), &m_fileBom);

    SetText(text);

    m_modifyTime = GetFileLastModifiedTime();

    SetSavePoint();
    EmptyUndoBuffer();
    GetCommandsProcessor().Reset();

    // Update the editor properties
    DoUpdateOptions();
    SetProperties();
    UpdateColours();
    SetEOL();

    int doclen = GetLength();
    int lastLine = LineFromPosition(doclen);
    lineNumber > lastLine ? lineNumber = lastLine : lineNumber;

    SetEnsureCaretIsVisible(PositionFromLine(lineNumber));

    // mark read only files
    clMainFrame::Get()->GetMainBook()->MarkEditorReadOnly(this);
    SetReloadingFile(false);

    // Notify that a file has been loaded into the editor
    clCommandEvent fileLoadedEvent(wxEVT_FILE_LOADED);
    fileLoadedEvent.SetFileName(GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(fileLoadedEvent);

    SetProperty(wxT("lexer.cpp.track.preprocessor"), wxT("0"));
    SetProperty(wxT("lexer.cpp.update.preprocessor"), wxT("0"));
    m_mgr->GetStatusBar()->SetMessage(_("Ready"));
}

void LEditor::SetEditorText(const wxString& text)
{
    wxWindowUpdateLocker locker(this);
    SetText(text);

    // remove breakpoints belongs to this file
    DelAllBreakpointMarkers();
}

void LEditor::Create(const wxString& project, const wxFileName& fileName)
{
    // set the file name
    SetFileName(fileName);
    // set the project name
    SetProject(project);
    // let the editor choose the syntax highlight to use according to file extension
    // and set the editor properties to default
    SetSyntaxHighlight(false); // Dont call 'UpdateColors' it is called in 'OpenFile'
    // reload the file from disk
    OpenFile();
}

void LEditor::InsertTextWithIndentation(const wxString& text, int lineno)
{
    wxString textTag = FormatTextKeepIndent(text, PositionFromLine(lineno));
    InsertText(PositionFromLine(lineno), textTag);
}

wxString LEditor::FormatTextKeepIndent(const wxString& text, int pos, size_t flags)
{
    // keep the page idnetation level
    wxString textToInsert(text);
    wxString indentBlock;

    int indentSize = 0;
    int indent = 0;

    if(flags & Format_Text_Indent_Prev_Line) {
        indentSize = GetIndent();
        int foldLevel = (GetFoldLevel(LineFromPosition(pos)) & wxSTC_FOLDLEVELNUMBERMASK) - wxSTC_FOLDLEVELBASE;
        indent = foldLevel * indentSize;

    } else {
        indentSize = GetIndent();
        indent = GetLineIndentation(LineFromPosition(pos));
    }

    if(GetUseTabs()) {
        if(indentSize) indent = indent / indentSize;

        for(int i = 0; i < indent; i++) {
            indentBlock << wxT("\t");
        }
    } else {
        for(int i = 0; i < indent; i++) {
            indentBlock << wxT(" ");
        }
    }

    wxString eol = GetEolString();
    textToInsert.Replace(wxT("\r"), wxT("\n"));
    wxStringTokenizerMode tokenizerMode = (flags & Format_Text_Save_Empty_Lines) ? wxTOKEN_RET_EMPTY : wxTOKEN_STRTOK;
    wxArrayString lines = wxStringTokenize(textToInsert, wxT("\n"), tokenizerMode);

    textToInsert.Clear();
    for(size_t i = 0; i < lines.GetCount(); i++) {
        textToInsert << indentBlock;
        textToInsert << lines.Item(i) << eol;
    }
    return textToInsert;
}

void LEditor::OnContextMenu(wxContextMenuEvent& event)
{
    wxString selectText = GetSelectedText();
    wxPoint pt = event.GetPosition();
    if(pt != wxDefaultPosition) { // Analyze position only for mouse-originated events
        wxPoint clientPt = ScreenToClient(pt);

        // If the right-click is in the margin, provide a different context menu: bookmarks/breakpts
        int margin = 0;
        for(int n = 0; n < FOLD_MARGIN_ID;
            ++n) { // Assume a click anywhere to the left of the fold margin is for markers
            margin += GetMarginWidth(n);
        }
        if(clientPt.x < margin) {
            GotoPos(PositionFromPoint(clientPt));
            DoBreakptContextMenu(clientPt);
            return;
        }

        int closePos = PositionFromPointClose(clientPt.x, clientPt.y);
        if(closePos != wxNOT_FOUND) {
            if(!selectText.IsEmpty()) {
                // If the selection text is placed under the cursor,
                // keep it selected, else, unselect the text
                // and place the caret to be under cursor
                int selStart = GetSelectionStart();
                int selEnd = GetSelectionEnd();
                if(closePos < selStart || closePos > selEnd) {
                    // cursor is not over the selected text, unselect and re-position caret
                    SetCaretAt(closePos);
                }
            } else {
                // no selection, just place the caret
                SetCaretAt(closePos);
            }
        }
    }
    // Let the plugins handle this event first
    wxCommandEvent contextMenuEvent(wxEVT_CMD_EDITOR_CONTEXT_MENU, GetId());
    contextMenuEvent.SetEventObject(this);
    if(EventNotifier::Get()->ProcessEvent(contextMenuEvent)) return;

    wxMenu* menu = m_context->GetMenu();
    if(!menu) return;

    // Let the context add it dynamic content
    m_context->AddMenuDynamicContent(menu);

    // add the debugger (if currently running) to add its dynamic content
    IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
    if(debugger && debugger->IsRunning()) { AddDebuggerContextMenu(menu); }

    // turn the popupIsOn value to avoid annoying
    // calltips from firing while our menu is popped
    m_popupIsOn = true;

    // Notify about menu is about to be shown
    clContextMenuEvent menuEvent(wxEVT_CONTEXT_MENU_EDITOR);
    menuEvent.SetEditor(this);
    menuEvent.SetMenu(menu);
    EventNotifier::Get()->ProcessEvent(menuEvent);

    // let the plugins hook their content
    PluginManager::Get()->HookPopupMenu(menu, MenuTypeEditor);

    // +++++------------------------------------------------------
    // if the selection is URL, offer to open it in the browser
    // +++++------------------------------------------------------
    wxString selectedText = GetSelectedText();
    if(!selectedText.IsEmpty() && !selectedText.Contains("\n")) {
        static wxRegEx reUrl("https?://.*?", wxRE_ADVANCED);
        if(reUrl.IsValid() && reUrl.Matches(selectText)) {
            // Offer to open the URL
            if(ID_OPEN_URL == wxNOT_FOUND) { ID_OPEN_URL = ::wxNewId(); }
            wxString text;
            text << "Go to " << reUrl.GetMatch(selectText);
            menu->PrependSeparator();
            menu->Prepend(ID_OPEN_URL, text);
            menu->Bind(wxEVT_MENU, &LEditor::OpenURL, this, ID_OPEN_URL);
        }
    }
    // +++++--------------------------
    // Popup the menu
    // +++++--------------------------
    PopupMenu(menu);
    wxDELETE(menu);

    m_popupIsOn = false;
    event.Skip();
}

void LEditor::OnKeyDown(wxKeyEvent& event)
{
    // always cancel the tip
    DoCancelCodeCompletionBox();

    m_prevSelectionInfo.Clear();
    if(HasSelection()) {
        for(int i = 0; i < GetSelections(); ++i) {
            int selStart = GetSelectionNStart(i);
            int selEnd = GetSelectionNEnd(i);
            m_prevSelectionInfo.AddSelection(selStart, selEnd);
        }
        m_prevSelectionInfo.Sort();
    }

    bool escapeUsed = false; // If the quickfind bar is open we'll use an ESC to close it; but only if we've not already
                             // used it for something else

    // Hide tooltip dialog if its ON
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    bool dbgTipIsShown = ManagerST::Get()->GetDebuggerTip()->IsShown();
    bool keyIsControl = event.GetModifiers() == wxMOD_CONTROL;

    if(keyIsControl) {
        // Debugger tooltip is shown when clicking 'Control/CMD'
        // while the mouse is over a word
        wxPoint pt = ScreenToClient(wxGetMousePosition());
        int pos = PositionFromPointClose(pt.x, pt.y);
        if(pos != wxNOT_FOUND) {
            int wordStart = WordStartPos(pos, true);
            int wordEnd = WordEndPos(pos, true);
            wxString wordAtMouse = GetTextRange(wordStart, wordEnd);
            if(!wordAtMouse.IsEmpty()) {
                // wxLogMessage("Event wxEVT_DBG_EXPR_TOOLTIP is fired for string: %s", wordAtMouse);
                clDebugEvent tipEvent(wxEVT_DBG_EXPR_TOOLTIP);
                tipEvent.SetString(wordAtMouse);
                if(EventNotifier::Get()->ProcessEvent(tipEvent)) { return; }
            }
        }
    }

    if(dbgTipIsShown && !keyIsControl) {

        // If any key is pressed, but the CONTROL key hide the
        // debugger tip
        ManagerST::Get()->GetDebuggerTip()->HideDialog();
        escapeUsed = true;

    } else if(dbgr && dbgr->IsRunning() && ManagerST::Get()->DbgCanInteract() && keyIsControl) {

        DebuggerInformation info;
        DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);

        if(info.showTooltipsOnlyWithControlKeyIsDown) {
            // CONTROL Key + Debugger is running and interactive
            // and no debugger tip is shown -> emulate "Dwell" event
            wxStyledTextEvent sciEvent;
            wxPoint pt(ScreenToClient(wxGetMousePosition()));
            sciEvent.SetPosition(PositionFromPointClose(pt.x, pt.y));

            m_context->OnDbgDwellStart(sciEvent);
        }
    }

    // let the context process it as well
    if(event.GetKeyCode() == WXK_ESCAPE) {

        if(GetFunctionTip()->IsActive()) {
            GetFunctionTip()->Deactivate();
            escapeUsed = true;
        }

        // If we've not already used ESC, there's a reasonable chance that the user wants to close the QuickFind bar
        if(!escapeUsed) {
            clMainFrame::Get()->GetMainBook()->ShowQuickBar(
                false); // There's no easy way to tell if it's actually showing, so just do a Close

            // In addition, if we have multiple selections, de-select them
            if(GetSelections()) {
                clEditorStateLocker editor(this);
                ClearSelections();
            }
        }
    }

    m_context->OnKeyDown(event);
}

void LEditor::OnLeftUp(wxMouseEvent& event)
{
    m_isDragging = false; // We can't still be in D'n'D, so stop disabling callticks
    long value = EditorConfigST::Get()->GetInteger(wxT("QuickCodeNavigationUsesMouseMiddleButton"), 0);
    if(!value) { DoQuickJump(event, false); }
    PostCmdEvent(wxEVT_EDITOR_CLICKED);
    event.Skip();
}

void LEditor::OnLeaveWindow(wxMouseEvent& event)
{
    m_hyperLinkIndicatroStart = wxNOT_FOUND;
    m_hyperLinkIndicatroEnd = wxNOT_FOUND;
    m_hyperLinkType = wxID_NONE;

    SetIndicatorCurrent(HYPERLINK_INDICATOR);
    IndicatorClearRange(0, GetLength());
    event.Skip();
}

void LEditor::OnFocusLost(wxFocusEvent& event)
{
    m_isFocused = false;
    event.Skip();
    if(HasCapture()) {
        CL_DEBUG("Releasing the mouse...");
        ReleaseMouse();
    }
}

void LEditor::OnRightDown(wxMouseEvent& event)
{
    int mod = GetCodeNavModifier();
    if(event.GetModifiers() == mod && mod != wxMOD_NONE) {
        ClearSelections();
        long pos = PositionFromPointClose(event.GetX(), event.GetY());
        if(pos != wxNOT_FOUND) { DoSetCaretAt(pos); }

        clCodeCompletionEvent event(wxEVT_CC_SHOW_QUICK_NAV_MENU);
        event.SetEditor(this);
        event.SetPosition(pos);
        event.SetInsideCommentOrString(m_context->IsCommentOrString(pos));
        EventNotifier::Get()->AddPendingEvent(event);

    } else {
        event.Skip();
    }
}

void LEditor::OnMotion(wxMouseEvent& event)
{
    int mod = GetCodeNavModifier();
    if(event.GetModifiers() == mod && mod != wxMOD_NONE) {

        m_hyperLinkIndicatroStart = wxNOT_FOUND;
        m_hyperLinkIndicatroEnd = wxNOT_FOUND;
        m_hyperLinkType = wxID_NONE;

        SetIndicatorCurrent(HYPERLINK_INDICATOR);
        IndicatorClearRange(0, GetLength());
        DoMarkHyperlink(event, true);
    } else {
        event.Skip();
    }
}

void LEditor::OnLeftDown(wxMouseEvent& event)
{
    HighlightWord(false);
    wxDELETE(m_richTooltip);

    // Clear context word highlight
    SetIndicatorCurrent(MARKER_CONTEXT_WORD_HIGHLIGHT);
    IndicatorClearRange(0, GetLength());

    // hide completion box
    DoCancelCalltip();
    GetFunctionTip()->Deactivate();

    if(ManagerST::Get()->GetDebuggerTip()->IsShown()) { ManagerST::Get()->GetDebuggerTip()->HideDialog(); }

    int mod = GetCodeNavModifier();
    if(m_hyperLinkType != wxID_NONE && event.GetModifiers() == mod && mod != wxMOD_NONE) {
        ClearSelections();
        SetCaretAt(PositionFromPointClose(event.GetX(), event.GetY()));
    }
    SetActive();

    // Clear any messages from the status bar
    clGetManager()->GetStatusBar()->SetMessage("");
    event.Skip();
}

void LEditor::OnPopupMenuUpdateUI(wxUpdateUIEvent& event)
{
    // pass it to the context
    m_context->ProcessEvent(event);
}

BrowseRecord LEditor::CreateBrowseRecord()
{
    // Remember this position before skipping to the next one
    BrowseRecord record;
    record.lineno = LineFromPosition(GetCurrentPos()) + 1; // scintilla counts from zero, while tagentry from 1
    record.filename = GetFileName().GetFullPath();
    record.project = GetProject();

    // if the file is part of the workspace set the project name
    // else, open it with empty project
    record.position = GetCurrentPos();
    return record;
}

void LEditor::DoBreakptContextMenu(wxPoint pt)
{
    // turn the popupIsOn value to avoid annoying
    // calltips from firing while our menu is popped
    m_popupIsOn = true;

    wxMenu menu;

    // First, add/del bookmark
    menu.Append(XRCID("toggle_bookmark"),
                LineIsMarked(mmt_standard_bookmarks) ? wxString(_("Remove Bookmark")) : wxString(_("Add Bookmark")));
    menu.Append(XRCID("removeall_bookmarks"), _("Remove All Bookmarks"));

    BookmarkManager::Get().CreateBookmarksSubmenu(&menu);
    menu.AppendSeparator();

    menu.Append(XRCID("add_breakpoint"), wxString(_("Add Breakpoint")));
    menu.Append(XRCID("insert_temp_breakpoint"), wxString(_("Add a Temporary Breakpoint")));
    menu.Append(XRCID("insert_disabled_breakpoint"), wxString(_("Add a Disabled Breakpoint")));
    menu.Append(XRCID("insert_cond_breakpoint"), wxString(_("Add a Conditional Breakpoint..")));

    BreakpointInfo& bp =
        ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoint(GetFileName().GetFullPath(), GetCurrentLine() + 1);

    // What we show depends on whether there's already a bp here (or several)
    if(!bp.IsNull()) {

        // Disable all the "Add*" entries
        menu.Enable(XRCID("add_breakpoint"), false);
        menu.Enable(XRCID("insert_temp_breakpoint"), false);
        menu.Enable(XRCID("insert_disabled_breakpoint"), false);
        menu.Enable(XRCID("insert_cond_breakpoint"), false);
        menu.AppendSeparator();

        menu.Append(XRCID("delete_breakpoint"), wxString(_("Remove Breakpoint")));
        menu.Append(XRCID("ignore_breakpoint"), wxString(_("Ignore Breakpoint")));
        // On MSWin it often crashes the debugger to try to load-then-disable a bp
        // so don't show the menu item unless the debugger is running *** Hmm, that was written about 4 years ago.
        // Let's
        // try it again...
        menu.Append(XRCID("toggle_breakpoint_enabled_status"),
                    bp.is_enabled ? wxString(_("Disable Breakpoint")) : wxString(_("Enable Breakpoint")));
        menu.Append(XRCID("edit_breakpoint"), wxString(_("Edit Breakpoint")));
    }

    if(ManagerST::Get()->DbgCanInteract()) {
        menu.AppendSeparator();
        menu.Append(XRCID("dbg_run_to_cursor"), _("Run to here"));
    }

    clContextMenuEvent event(wxEVT_CONTEXT_MENU_EDITOR_MARGIN);
    event.SetMenu(&menu);
    if(EventNotifier::Get()->ProcessEvent(event)) return;

    PopupMenu(&menu, pt.x, pt.y);
    m_popupIsOn = false;
}

void LEditor::AddOtherBreakpointType(wxCommandEvent& event)
{
    bool is_temp = (event.GetId() == XRCID("insert_temp_breakpoint"));
    bool is_disabled = (event.GetId() == XRCID("insert_disabled_breakpoint"));

    wxString conditions;
    if(event.GetId() == XRCID("insert_cond_breakpoint")) {
        conditions = wxGetTextFromUser(_("Enter the condition statement"), _("Create Conditional Breakpoint"));
        if(conditions.IsEmpty()) { return; }
    }

    AddBreakpoint(-1, conditions, is_temp, is_disabled);
}

void LEditor::OnIgnoreBreakpoint()
{
    if(ManagerST::Get()->GetBreakpointsMgr()->IgnoreByLineno(GetFileName().GetFullPath(), GetCurrentLine() + 1)) {
        clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
    }
}

void LEditor::OnEditBreakpoint()
{
    ManagerST::Get()->GetBreakpointsMgr()->EditBreakpointByLineno(GetFileName().GetFullPath(), GetCurrentLine() + 1);
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::AddBreakpoint(int lineno /*= -1*/, const wxString& conditions /*=wxT("")*/, const bool is_temp /*=false*/,
                            const bool is_disabled /*=false*/)
{
    if(lineno == -1) { lineno = GetCurrentLine() + 1; }

    ManagerST::Get()->GetBreakpointsMgr()->SetExpectingControl(true);
    if(!ManagerST::Get()->GetBreakpointsMgr()->AddBreakpointByLineno(GetFileName().GetFullPath(), lineno, conditions,
                                                                     is_temp, is_disabled)) {
        wxMessageBox(_("Failed to insert breakpoint"));

    } else {

        clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
        wxString message(_("Breakpoint successfully added")), prefix;
        if(is_temp) {
            prefix = _("Temporary ");
        } else if(is_disabled) {
            prefix = _("Disabled ");
        } else if(!conditions.IsEmpty()) {
            prefix = _("Conditional ");
        }
        m_mgr->GetStatusBar()->SetMessage(prefix + message);
    }
}

void LEditor::DelBreakpoint(int lineno /*= -1*/)
{
    if(lineno == -1) { lineno = GetCurrentLine() + 1; }
    wxString message;
    // enable the 'expectingControl' to 'true'
    // this is used by Manager class to detect whether the control
    // was triggered by user action
    ManagerST::Get()->GetBreakpointsMgr()->SetExpectingControl(true);

    int result = ManagerST::Get()->GetBreakpointsMgr()->DelBreakpointByLineno(GetFileName().GetFullPath(), lineno);
    switch(result) {
    case true:
        clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
        m_mgr->GetStatusBar()->SetMessage(_("Breakpoint successfully deleted"));
        return;
    case wxID_CANCEL:
        return;
    case false:
        message = _("No breakpoint found on this line");
        break;
    default:
        message = _("Breakpoint deletion failed");
    }

    wxMessageBox(message, _("Breakpoint not deleted"), wxICON_ERROR | wxOK);
}

void LEditor::ToggleBreakpoint(int lineno)
{
    // Coming from OnMarginClick() means that lineno comes from the mouse position, not necessarily the current line
    if(lineno == -1) { lineno = GetCurrentLine() + 1; }

    // Does any of the plugins want to handle this?
    clDebugEvent dbgEvent(wxEVT_DBG_UI_TOGGLE_BREAKPOINT);
    dbgEvent.SetInt(lineno);
    dbgEvent.SetFileName(GetFileName().GetFullPath());
    if(EventNotifier::Get()->ProcessEvent(dbgEvent)) { return; }

    const BreakpointInfo& bp =
        ManagerST::Get()->GetBreakpointsMgr()->GetBreakpoint(GetFileName().GetFullPath(), lineno);

    if(bp.IsNull()) {
        // This will (always?) be from a margin mouse-click, so assume it's a standard breakpt that's wanted
        AddBreakpoint(lineno);

    } else {
        DelBreakpoint(lineno);
    }
}

void LEditor::SetWarningMarker(int lineno, const wxString& annotationText)
{
    if(lineno >= 0) {

        // Keep the text message
        if(m_compilerMessagesMap.count(lineno)) { m_compilerMessagesMap.erase(lineno); }
        m_compilerMessagesMap.insert(std::make_pair(lineno, annotationText));

        BuildTabSettingsData options;
        EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

        if(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks) {
            MarkerAdd(lineno, smt_warning);
            NotifyMarkerChanged(lineno);
        }

        if(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Annotate) {
            // define the warning marker
            AnnotationSetText(lineno, annotationText);
            AnnotationSetStyle(lineno, ANNOTATION_STYLE_WARNING);
        }
    }
}

void LEditor::SetErrorMarker(int lineno, const wxString& annotationText)
{
    if(lineno >= 0) {

        // Keep the text message
        if(m_compilerMessagesMap.count(lineno)) { m_compilerMessagesMap.erase(lineno); }
        m_compilerMessagesMap.insert(std::make_pair(lineno, annotationText));

        BuildTabSettingsData options;
        EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &options);

        if(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Bookmarks) {
            MarkerAdd(lineno, smt_error);
            NotifyMarkerChanged(lineno);
        }

        if(options.GetErrorWarningStyle() & BuildTabSettingsData::EWS_Annotate) {
            AnnotationSetText(lineno, annotationText);
            AnnotationSetStyle(lineno, ANNOTATION_STYLE_ERROR);
        }
    }
}

void LEditor::DelAllCompilerMarkers()
{
    MarkerDeleteAll(smt_warning);
    MarkerDeleteAll(smt_error);
    AnnotationClearAll();
    m_compilerMessagesMap.clear();

    // Notify about marker changes
    NotifyMarkerChanged();
}

// Maybe one day we'll display multiple bps differently
void LEditor::SetBreakpointMarker(int lineno, BreakpointType bptype, bool is_disabled,
                                  const std::vector<BreakpointInfo>& bps)
{
    BPtoMarker bpm = GetMarkerForBreakpt(bptype);
    sci_marker_types markertype = is_disabled ? bpm.marker_disabled : bpm.marker;
    int markerHandle = MarkerAdd(lineno - 1, markertype);
    NotifyMarkerChanged(lineno - 1);
    // keep the breakpoint info vector for this marker
    m_breakpointsInfo.insert(std::make_pair(markerHandle, bps));
}

void LEditor::DelAllBreakpointMarkers()
{
    // remove the stored information
    m_breakpointsInfo.clear();

    for(int bp_type = BP_FIRST_ITEM; bp_type <= BP_LAST_MARKED_ITEM; ++bp_type) {
        BPtoMarker bpm = GetMarkerForBreakpt((BreakpointType)bp_type);
        MarkerDeleteAll(bpm.marker);
        MarkerDeleteAll(bpm.marker_disabled);
    }
    // Notify about marker changes
    NotifyMarkerChanged();
}

void LEditor::HighlightLine(int lineno)
{
    if(GetLineCount() <= 0) { return; }

    int sci_line = lineno - 1;
    if(GetLineCount() < sci_line - 1) { sci_line = GetLineCount() - 1; }
    MarkerAdd(sci_line, smt_indicator);
    NotifyMarkerChanged(sci_line);
}

void LEditor::UnHighlightAll()
{
    MarkerDeleteAll(smt_indicator); // Notify about marker changes
    NotifyMarkerChanged();
}

void LEditor::AddDebuggerContextMenu(wxMenu* menu)
{
    if(!ManagerST::Get()->DbgCanInteract()) { return; }

    wxString word = GetSelectedText();
    if(word.IsEmpty()) {
        word = GetWordAtCaret();
        if(word.IsEmpty()) { return; }
    }

    if(word.Contains("\n")) {
        // Don't create massive context menu
        return;
    }

    // Truncate the word
    if(word.length() > 20) {
        word = word.Mid(0, 20);
        word << "...";
    }

    m_customCmds.clear();
    wxString menuItemText;

    wxMenuItem* item;
    item = new wxMenuItem(menu, wxID_SEPARATOR);
    menu->Prepend(item);
    m_dynItems.push_back(item);

    //---------------------------------------------
    // Add custom commands
    //---------------------------------------------
    menu->Prepend(XRCID("debugger_watches"), _("More Watches"), DoCreateDebuggerWatchMenu(word));

    menuItemText.Clear();
    menuItemText << _("Add Watch") << wxT(" '") << word << wxT("'");
    item = new wxMenuItem(menu, wxNewId(), menuItemText);
    menu->Prepend(item);
    menu->Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgAddWatch), NULL,
                  this);
    m_dynItems.push_back(item);

    menuItemText.Clear();
    menu->Prepend(XRCID("dbg_run_to_cursor"), _("Run to Caret Line"), _("Run to Caret Line"));
    menu->Prepend(XRCID("dbg_jump_cursor"), _("Jump to Caret Line"), _("Jump to Caret Line"));
    m_dynItems.push_back(item);
}

void LEditor::RemoveDebuggerContextMenu(wxMenu* menu)
{
    std::vector<wxMenuItem*>::iterator iter = m_dynItems.begin();

    // disconnect all event handlers
    for(; iter != m_dynItems.end(); iter++) {
        Disconnect((*iter)->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgCustomWatch),
                   NULL, this);
        menu->Remove(*iter);
    }

    wxMenuItem* item = menu->FindItem(XRCID("debugger_watches"));
    while(item) {
        if(item) { menu->Destroy(item); }
        item = menu->FindItem(XRCID("debugger_watches"));
    }

    m_dynItems.clear();
    m_customCmds.clear();
}

void LEditor::OnDbgAddWatch(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxString word = GetSelectedText();
    if(word.IsEmpty()) {
        word = GetWordAtCaret();
        if(word.IsEmpty()) { return; }
    }
    clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->AddExpression(word);
    clMainFrame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::WATCHES);
    clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();
}

void LEditor::OnDbgCustomWatch(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString word = GetSelectedText();
    if(word.IsEmpty()) {
        word = GetWordAtCaret();
        if(word.IsEmpty()) { return; }
    }

    // find the custom command to run
    std::map<int, wxString>::iterator iter = m_customCmds.find(event.GetId());
    if(iter != m_customCmds.end()) {

        // Replace $(Variable) with the actual string
        wxString command = iter->second;
        command = MacroManager::Instance()->Replace(command, wxT("variable"), word, true);

        clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->AddExpression(command);
        clMainFrame::Get()->GetDebuggerPane()->SelectTab(DebuggerPane::WATCHES);
        clMainFrame::Get()->GetDebuggerPane()->GetWatchesTable()->RefreshValues();
    }
}

void LEditor::UpdateColours()
{
    SetKeywordClasses("");
    SetKeywordLocals("");

    if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_VARS ||
       TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_COLOUR_MACRO_BLOCKS) {
        m_context->OnFileSaved();

    } else {
        if(m_context->GetName() == wxT("C++")) {
            SetKeyWords(1, wxEmptyString); // Classes
            SetKeyWords(2, wxEmptyString);
            SetKeyWords(3, wxEmptyString); // Locals
            SetKeyWords(4, GetPreProcessorsWords());
        }
    }
    Colourise(0, wxSTC_INVALID_POSITION);
}

int LEditor::SafeGetChar(int pos)
{
    if(pos < 0 || pos >= GetLength()) { return 0; }
    return GetCharAt(pos);
}

void LEditor::OnDragStart(wxStyledTextEvent& e)
{
    m_isDragging = true; // Otherwise it sometimes obscures the desired drop zone!
    e.Skip();
}

void LEditor::OnDragEnd(wxStyledTextEvent& e)
{
    // For future reference, this will only be called when D'n'D ends successfully with a drop.
    // Unfortunately scintilla doesn't seem to provide any notification when ESC is pressed, or the drop-zone is
    // invalid
    m_isDragging = false; // Turn on calltips again

    e.Skip();
}

void LEditor::ShowCompletionBox(const std::vector<TagEntryPtr>& tags, const wxString& word)
{
    if(tags.empty()) { return; }

    // When using this method, use an automated refresh completion box
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(this, tags, wxCodeCompletionBox::kRefreshOnKeyType,
                                                        wxNOT_FOUND);
}

int LEditor::GetCurrLineHeight()
{
    int point = GetCurrentPos();
    wxPoint pt = PointFromPosition(point);

    // calculate the line height
    int curline = LineFromPosition(point);
    int ll;
    int hh(0);
    if(curline > 0) {
        ll = curline - 1;
        int pp = PositionFromLine(ll);
        wxPoint p = PointFromPosition(pp);
        hh = pt.y - p.y;
    } else {
        ll = curline + 1;
        int pp = PositionFromLine(ll);
        wxPoint p = PointFromPosition(pp);
        hh = p.y - pt.y;
    }

    if(hh == 0) {
        hh = 12; // default height on most OSs
    }

    return hh;
}

void LEditor::DoHighlightWord()
{
    // Read the primary selected text
    int mainSelectionStart = GetSelectionNStart(GetMainSelection());
    int mainSelectionEnd = GetSelectionNEnd(GetMainSelection());
    wxString word = GetTextRange(mainSelectionStart, mainSelectionEnd);

    wxString selectedTextTrimmed = word;
    selectedTextTrimmed.Trim().Trim(false);
    if(selectedTextTrimmed.IsEmpty()) { return; }

    // Search only the visible areas
    StringHighlighterJob j;
    int firstVisibleLine = GetFirstVisibleLine();
    int lastDocLine = LineFromPosition(GetLength());
    int offset = PositionFromLine(firstVisibleLine);

    if(GetAllLinesVisible()) {
        // The simple case: there aren't any folds
        int lastLine = firstVisibleLine + LinesOnScreen();
        if(lastLine > lastDocLine) { lastLine = lastDocLine; }
        int lastPos = PositionFromLine(lastLine) + LineLength(lastLine);
        wxString text = GetTextRange(offset, lastPos);
        j.Set(text, word, offset);
        j.Process();
    } else {

        // There are folds, so we have to process each visible section separately
        firstVisibleLine = DocLineFromVisible(firstVisibleLine); // This copes with folds above the displayed lines
        int lineCount(0);
        int nextLineToProcess(firstVisibleLine);
        int screenLines(LinesOnScreen());
        while(lineCount < screenLines && nextLineToProcess <= lastDocLine) {
            int offset(-1);
            int line = nextLineToProcess;

            // Skip over any invisible lines
            while(!GetLineVisible(line) && line < lastDocLine) {
                ++line;
            }

            // EOF?
            if(line >= lastDocLine) break;

            while(GetLineVisible(line) && line <= lastDocLine) {
                if(offset == -1) {
                    offset = PositionFromLine(line); // Get offset value the first time through
                }
                ++line;
                ++lineCount;
                if(lineCount >= screenLines) { break; }
            }
            if(line > lastDocLine) { line = lastDocLine; }
            nextLineToProcess = line;

            int lastPos = PositionFromLine(nextLineToProcess) + LineLength(nextLineToProcess);
            wxString text = GetTextRange(offset, lastPos);
            j.Set(text, word, offset);
            j.Process();
        }
    }

    // Keep the first offset
    m_highlightedWordInfo.Clear();
    m_highlightedWordInfo.SetFirstOffset(offset);
    m_highlightedWordInfo.SetWord(word);
    HighlightWord((StringHighlightOutput*)&j.GetOutput());
}

void LEditor::HighlightWord(bool highlight)
{
    if(highlight) {
        DoHighlightWord();

    } else {
        SetIndicatorCurrent(MARKER_WORD_HIGHLIGHT);
        IndicatorClearRange(0, GetLength());
        m_highlightedWordInfo.Clear();
    }
}

void LEditor::OnLeftDClick(wxStyledTextEvent& event)
{
    long highlight_word = EditorConfigST::Get()->GetInteger(wxT("highlight_word"), 0);
    if(GetSelectedText().IsEmpty() == false && highlight_word) { DoHighlightWord(); }
    event.Skip();
}

bool LEditor::IsCompletionBoxShown() { return wxCodeCompletionBoxManager::Get().IsShown(); }

int LEditor::GetCurrentLine()
{
    // return the current line number
    int pos = GetCurrentPos();
    return LineFromPosition(pos);
}

void LEditor::DoSetCaretAt(long pos)
{
    SetCurrentPos(pos);
    SetSelectionStart(pos);
    SetSelectionEnd(pos);
    int line = LineFromPosition(pos);
    if(line >= 0) {
        // This is needed to unfold the line if it were folded
        // The various other 'EnsureVisible' things don't do this
        EnsureVisible(line);
    }
}

int LEditor::GetEOLByContent()
{
    if(GetLength() == 0) { return wxNOT_FOUND; }

    // locate the first EOL
    wxString txt = GetText();
    size_t pos1 = static_cast<size_t>(txt.Find(wxT("\n")));
    size_t pos2 = static_cast<size_t>(txt.Find(wxT("\r\n")));
    size_t pos3 = static_cast<size_t>(txt.Find(wxT("\r")));

    size_t max_size_t = static_cast<size_t>(-1);
    // the buffer is not empty but it does not contain any EOL as well
    if(pos1 == max_size_t && pos2 == max_size_t && pos3 == max_size_t) { return wxNOT_FOUND; }

    size_t first_eol_pos(0);
    pos2 < pos1 ? first_eol_pos = pos2 : first_eol_pos = pos1;
    pos3 < first_eol_pos ? first_eol_pos = pos3 : first_eol_pos = first_eol_pos;

    // get the EOL at first_eol_pos
    wxChar ch = SafeGetChar(first_eol_pos);
    if(ch == wxT('\n')) { return wxSTC_EOL_LF; }

    if(ch == wxT('\r')) {
        wxChar secondCh = SafeGetChar(first_eol_pos + 1);
        if(secondCh == wxT('\n')) {
            return wxSTC_EOL_CRLF;
        } else {
            return wxSTC_EOL_CR;
        }
    }
    return wxNOT_FOUND;
}

int LEditor::GetEOLByOS()
{
    OptionsConfigPtr options = GetOptions();
    if(options->GetEolMode() == wxT("Unix (LF)")) {
        return wxSTC_EOL_LF;
    } else if(options->GetEolMode() == wxT("Mac (CR)")) {
        return wxSTC_EOL_CR;
    } else if(options->GetEolMode() == wxT("Windows (CRLF)")) {
        return wxSTC_EOL_CRLF;
    } else {
// set the EOL by the hosting OS
#if defined(__WXMAC__)
        return wxSTC_EOL_LF;
#elif defined(__WXGTK__)
        return wxSTC_EOL_LF;
#else
        return wxSTC_EOL_CRLF;
#endif
    }
}

void LEditor::ShowFunctionTipFromCurrentPos()
{
    clDEBUG1() << "Calling ShowFunctionTipFromCurrentPos..." << clEndl;
    if(TagsManagerST::Get()->GetCtagsOptions().GetFlags() & CC_DISP_FUNC_CALLTIP) {

        if(EventNotifier::Get()->IsEventsDiabled()) return;

        int pos = DoGetOpenBracePos();
        clDEBUG1() << "Brace open position is:" << pos << clEndl;
        clDEBUG1() << "Firing wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP event..." << clEndl;
        // see if any of the plugins want to handle it
        clCodeCompletionEvent evt(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, GetId());
        evt.SetEventObject(this);
        evt.SetEditor(this);
        evt.SetPosition(pos);
        evt.SetInsideCommentOrString(m_context->IsCommentOrString(pos));
        if(EventNotifier::Get()->ProcessEvent(evt)) return;

        if(pos != wxNOT_FOUND) {
            clDEBUG1() << "Using default behavior for wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP event" << clEndl;
            m_context->CodeComplete(pos);
        }
    }
}

wxString LEditor::GetSelection() { return wxStyledTextCtrl::GetSelectedText(); }

int LEditor::GetSelectionStart() { return wxStyledTextCtrl::GetSelectionStart(); }

int LEditor::GetSelectionEnd() { return wxStyledTextCtrl::GetSelectionEnd(); }

void LEditor::ReplaceSelection(const wxString& text) { wxStyledTextCtrl::ReplaceSelection(text); }

void LEditor::ClearUserIndicators()
{
    SetIndicatorCurrent(USER_INDICATOR);
    IndicatorClearRange(0, GetLength());
}

int LEditor::GetUserIndicatorEnd(int pos) { return wxStyledTextCtrl::IndicatorEnd(USER_INDICATOR, pos); }

int LEditor::GetUserIndicatorStart(int pos) { return wxStyledTextCtrl::IndicatorStart(USER_INDICATOR, pos); }

void LEditor::SelectText(int startPos, int len)
{
    SetSelectionStart(startPos);
    SetSelectionEnd(startPos + len);
}

void LEditor::SetUserIndicator(int startPos, int len)
{
    SetIndicatorCurrent(USER_INDICATOR);
    IndicatorFillRange(startPos, len);
}

void LEditor::SetUserIndicatorStyleAndColour(int style, const wxColour& colour)
{
    IndicatorSetForeground(USER_INDICATOR, colour);
    IndicatorSetStyle(USER_INDICATOR, style);
    IndicatorSetUnder(USER_INDICATOR, true);
}

int LEditor::GetLexerId() { return GetLexer(); }

int LEditor::GetStyleAtPos(int pos) { return GetStyleAt(pos); }

int LEditor::WordStartPos(int pos, bool onlyWordCharacters)
{
    return wxStyledTextCtrl::WordStartPosition(pos, onlyWordCharacters);
}

int LEditor::WordEndPos(int pos, bool onlyWordCharacters)
{
    return wxStyledTextCtrl::WordEndPosition(pos, onlyWordCharacters);
}

void LEditor::DoMarkHyperlink(wxMouseEvent& event, bool isMiddle)
{
    if(event.m_controlDown || isMiddle) {
        SetIndicatorCurrent(HYPERLINK_INDICATOR);
        long pos = PositionFromPointClose(event.GetX(), event.GetY());

        wxColour bgCol = StyleGetBackground(0);
        if(DrawingUtils::IsDark(bgCol)) {
            IndicatorSetForeground(HYPERLINK_INDICATOR, *wxWHITE);
        } else {
            IndicatorSetForeground(HYPERLINK_INDICATOR, *wxBLUE);
        }

        if(pos != wxSTC_INVALID_POSITION) {
            m_hyperLinkType = m_context->GetHyperlinkRange(pos, m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd);
            if(m_hyperLinkType != wxID_NONE) {
                IndicatorFillRange(m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd - m_hyperLinkIndicatroStart);
            } else {
                m_hyperLinkIndicatroStart = wxNOT_FOUND;
                m_hyperLinkIndicatroEnd = wxNOT_FOUND;
            }
        }
    }
}

void LEditor::DoQuickJump(wxMouseEvent& event, bool isMiddle)
{
    if(m_hyperLinkIndicatroStart != wxNOT_FOUND && m_hyperLinkIndicatroEnd != wxNOT_FOUND) {
        // indicator is highlighted
        long pos = PositionFromPointClose(event.GetX(), event.GetY());
        if(m_hyperLinkIndicatroStart <= pos && pos <= m_hyperLinkIndicatroEnd) {
            bool altLink = (isMiddle && event.m_controlDown) || (!isMiddle && event.m_altDown);

            // Let the plugins handle it first
            clCodeCompletionEvent event(wxEVT_CC_JUMP_HYPER_LINK);
            event.SetString(GetTextRange(m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd));
            event.SetInt(m_hyperLinkIndicatroStart);
            if(EventNotifier::Get()->ProcessEvent(event)) { return; }

            // Run the default action
            m_context->GoHyperlink(m_hyperLinkIndicatroStart, m_hyperLinkIndicatroEnd, m_hyperLinkType, altLink);
        }
    }

    // clear the hyper link indicators
    m_hyperLinkIndicatroStart = wxNOT_FOUND;
    m_hyperLinkIndicatroEnd = wxNOT_FOUND;

    SetIndicatorCurrent(HYPERLINK_INDICATOR);
    IndicatorClearRange(0, GetLength());
    event.Skip();
}

void LEditor::TrimText(bool trim, bool appendLf)
{
    bool dontTrimCaretLine = GetOptions()->GetDontTrimCaretLine();
    bool trimOnlyModifiedLInes = GetOptions()->GetTrimOnlyModifiedLines();

    if(!trim && !appendLf) { return; }

    // wrap the entire operation in a single undo action
    BeginUndoAction();

    if(trim) {
        int maxLines = GetLineCount();
        int currLine = GetCurrentLine();
        for(int line = 0; line < maxLines; line++) {

            // only trim lines modified by the user in this session
            if(trimOnlyModifiedLInes && (MarginGetStyle(line) != CL_LINE_MODIFIED_STYLE)) continue;

            // We can trim in the following cases:
            // 1) line is is NOT the caret line OR
            // 2) line is the caret line, however dontTrimCaretLine is FALSE
            bool canTrim = ((line != currLine) || (line == currLine && !dontTrimCaretLine));

            if(canTrim) {
                int lineStart = PositionFromLine(line);
                int lineEnd = GetLineEndPosition(line);
                int i = lineEnd - 1;
                wxChar ch = (wxChar)(GetCharAt(i));
                while((i >= lineStart) && ((ch == _T(' ')) || (ch == _T('\t')))) {
                    i--;
                    ch = (wxChar)(GetCharAt(i));
                }
                if(i < (lineEnd - 1)) {
                    SetTargetStart(i + 1);
                    SetTargetEnd(lineEnd);
                    ReplaceTarget(_T(""));
                }
            }
        }
    }

    if(appendLf) {
        // The following code was adapted from the SciTE sourcecode
        int maxLines = GetLineCount();
        int enddoc = PositionFromLine(maxLines);
        if(maxLines <= 1 || enddoc > PositionFromLine(maxLines - 1)) InsertText(enddoc, GetEolString());
    }

    EndUndoAction();
}

wxString LEditor::GetEolString()
{
    wxString eol;
    switch(this->GetEOLMode()) {
    case wxSTC_EOL_CR:
        eol = wxT("\r");
        break;
    case wxSTC_EOL_CRLF:
        eol = wxT("\r\n");
        break;
    case wxSTC_EOL_LF:
        eol = wxT("\n");
        break;
    }
    return eol;
}

void LEditor::DoShowCalltip(int pos, const wxString& title, const wxString& tip)
{
    DoCancelCalltip();
    wxPoint pt;
    wxString tooltip;
    tooltip << title;
    tooltip.Trim().Trim(false);
    if(!tooltip.IsEmpty()) { tooltip << "\n<hr>"; }
    tooltip << tip;
    m_calltip = new CCBoxTipWindow(this, tooltip);
    if(pos == wxNOT_FOUND) {
        pt = ::wxGetMousePosition();
    } else {
        pt = PointFromPosition(pos);
    }

    // Place the tip on top of the mouse position, not under it
    pt.y -= m_calltip->GetSize().GetHeight();
    m_calltip->CallAfter(&CCBoxTipWindow::PositionAt, pt, this);
}

void LEditor::DoCancelCalltip()
{
    CallTipCancel();
    DoCancelCodeCompletionBox();
}

int LEditor::DoGetOpenBracePos()
{
    // determine the closest open brace from the current caret position
    int depth(0);
    int char_tested(0); // we add another performance tuning here: dont test more than 256 characters backward
    bool exit_loop(false);

    int pos = PositionBefore(GetCurrentPos());
    while((pos > 0) && (char_tested < 256)) {
        wxChar ch = SafeGetChar(pos);
        if(m_context->IsCommentOrString(pos)) {
            pos = PositionBefore(pos);
            continue;
        }

        char_tested++;

        switch(ch) {
        case wxT('{'):
            depth++;
            pos = PositionBefore(pos);
            break;
        case wxT('}'):
            depth--;
            pos = PositionBefore(pos);
            break;
        case wxT(';'):
            exit_loop = true;
            break;
        case wxT('('):
            depth++;
            if(depth == 1) {
                pos = PositionAfter(pos);
                exit_loop = true;
            } else {
                pos = PositionBefore(pos);
            }
            break;
        case wxT(')'):
            depth--;
        // fall through
        default:
            pos = PositionBefore(pos);
            break;
        }

        if(exit_loop) break;
    }

    if(char_tested == 256) {
        return wxNOT_FOUND;
    } else if(depth == 1 && pos >= 0) {
        return pos;
    }
    return wxNOT_FOUND;
}

void LEditor::SetEOL()
{
    // set the EOL mode
    int eol = GetEOLByOS();
    int alternate_eol = GetEOLByContent();
    if(alternate_eol != wxNOT_FOUND) { eol = alternate_eol; }
    SetEOLMode(eol);
}

void LEditor::OnChange(wxStyledTextEvent& event)
{
    event.Skip();
    bool isCoalesceStart = event.GetModificationType() & wxSTC_STARTACTION;
    bool isInsert = event.GetModificationType() & wxSTC_MOD_INSERTTEXT;
    bool isDelete = event.GetModificationType() & wxSTC_MOD_DELETETEXT;
    bool isUndo = event.GetModificationType() & wxSTC_PERFORMED_UNDO;
    bool isRedo = event.GetModificationType() & wxSTC_PERFORMED_REDO;

    // Remove any code completion annotations if we have some...
    if(m_hasCCAnnotation) {
        CallAfter(&LEditor::AnnotationClearAll);
        m_hasCCAnnotation = false;
    }

    // Notify about this editor being changed
    clCommandEvent eventMod(wxEVT_EDITOR_MODIFIED);
    eventMod.SetFileName(GetFileName().GetFullPath());
    EventNotifier::Get()->AddPendingEvent(eventMod);

    if((m_autoAddNormalBraces && !m_disableSmartIndent) || GetOptions()->GetAutoCompleteDoubleQuotes()) {
        if((event.GetModificationType() & wxSTC_MOD_BEFOREDELETE) &&
           (event.GetModificationType() & wxSTC_PERFORMED_USER)) {
            wxString deletedText = GetTextRange(event.GetPosition(), event.GetPosition() + event.GetLength());
            if(deletedText.IsEmpty() == false && deletedText.Length() == 1) {
                if(deletedText.GetChar(0) == wxT('[') || deletedText.GetChar(0) == wxT('(')) {
                    int where = wxStyledTextCtrl::BraceMatch(event.GetPosition());
                    if(where != wxNOT_FOUND) {
                        wxCommandEvent e(wxCMD_EVENT_REMOVE_MATCH_INDICATOR);
                        // the removal will take place after the actual deletion of the
                        // character, so we set it to be position before
                        e.SetInt(PositionBefore(where));
                        AddPendingEvent(e);
                    }
                } else if(deletedText.GetChar(0) == '\'' || deletedText.GetChar(0) == '"') {

                    wxChar searchChar = deletedText.GetChar(0);
                    // search for the matching close quote
                    int from = event.GetPosition() + 1;
                    int until = GetLineEndPosition(GetCurrentLine());

                    for(int i = from; i < until; ++i) {
                        if(SafeGetChar(i) == searchChar) {
                            wxCommandEvent e(wxCMD_EVENT_REMOVE_MATCH_INDICATOR);
                            // the removal will take place after the actual deletion of the
                            // character, so we set it to be position before
                            e.SetInt(PositionBefore(i));
                            AddPendingEvent(e);
                        }
                    }
                }
            }
        }
    }

    if(isCoalesceStart && GetCommandsProcessor().HasOpenCommand()) {
        // The user has changed mode e.g. from inserting to deleting, so the current command must be closed
        GetCommandsProcessor().CommandProcessorBase::ProcessOpenCommand(); // Use the base-class method, as this time we
                                                                           // don't need to tell scintilla too
    }

    if(isInsert || isDelete) {

        if(!GetReloadingFile() && !isUndo && !isRedo) {
            CLCommand::Ptr_t currentOpen = GetCommandsProcessor().GetOpenCommand();
            if(!currentOpen) {
                GetCommandsProcessor().StartNewTextCommand(isInsert ? CLC_insert : CLC_delete);
            }
            // We need to cope with a selection being deleted by typing; this results in 0x2012 followed immediately
            // by
            // 0x11 i.e. with no intervening wxSTC_STARTACTION
            else if(isInsert && currentOpen->GetCommandType() != CLC_insert) {
                GetCommandsProcessor().ProcessOpenCommand();
                GetCommandsProcessor().StartNewTextCommand(CLC_insert);

            } else if(isDelete && currentOpen->GetCommandType() != CLC_delete) {
                GetCommandsProcessor().ProcessOpenCommand();
                GetCommandsProcessor().StartNewTextCommand(CLC_delete);
            }

            wxCHECK_RET(GetCommandsProcessor().HasOpenCommand(), "Trying to add to a non-existent or closed command");
            wxCHECK_RET(GetCommandsProcessor().CanAppend(isInsert ? CLC_insert : CLC_delete),
                        "Trying to add to the wrong type of command");
            GetCommandsProcessor().AppendToTextCommand(event.GetText(), event.GetPosition());
        }

        // Cache details of the number of lines added/removed
        // This is used to 'update' any affected FindInFiles result. See bug 3153847
        if(event.GetModificationType() & wxSTC_PERFORMED_UNDO) {
            m_deltas->Pop();
        } else {
            m_deltas->Push(event.GetPosition(),
                           event.GetLength() * (event.GetModificationType() & wxSTC_MOD_DELETETEXT ? -1 : 1));
        }

        int numlines(event.GetLinesAdded());

        if(numlines) {
            if(GetReloadingFile() == false) {
                // a line was added to or removed from the document, so synchronize the breakpoints on this editor
                // and the breakpoint manager
                UpdateBreakpoints();
            } else {
                // The file has been reloaded, so the cached line-changes are no longer relevant
                m_deltas->Clear();
            }
        }

        // ignore this event incase we are in the middle of file reloading
        if(GetReloadingFile() == false && GetMarginWidth(EDIT_TRACKER_MARGIN_ID) /* margin is visible */) {
            int curline(LineFromPosition(event.GetPosition()));

            if(numlines == 0) {
                // probably only the current line was modified
                MarginSetText(curline, wxT(" "));
                MarginSetStyle(curline, CL_LINE_MODIFIED_STYLE);

            } else {

                for(int i = 0; i <= numlines; i++) {
                    MarginSetText(curline + i, wxT(" "));
                    MarginSetStyle(curline + i, CL_LINE_MODIFIED_STYLE);
                }
            }
        }
    }
}

void LEditor::OnRemoveMatchInidicator(wxCommandEvent& e)
{
    // get the current indicator end range
    if(IndicatorValueAt(MATCH_INDICATOR, e.GetInt()) == 1) {
        int curpos = GetCurrentPos();
        SetSelection(e.GetInt(), e.GetInt() + 1);
        ReplaceSelection(wxEmptyString);
        SetCaretAt(curpos);
    }
}

bool LEditor::FindAndSelect(const wxString& pattern, const wxString& what, int pos, NavMgr* navmgr)
{
    return DoFindAndSelect(pattern, what, pos, navmgr);
}

bool LEditor::DoFindAndSelect(const wxString& _pattern, const wxString& what, int start_pos, NavMgr* navmgr)
{
    BrowseRecord jumpfrom = CreateBrowseRecord();

    bool realPattern(false);
    wxString pattern(_pattern);
    pattern.StartsWith(wxT("/^"), &pattern);
    if(_pattern.Length() != pattern.Length()) { realPattern = true; }

    if(pattern.EndsWith(wxT("$/"))) {
        pattern = pattern.Left(pattern.Len() - 2);
        realPattern = true;
    } else if(pattern.EndsWith(wxT("/"))) {
        pattern = pattern.Left(pattern.Len() - 1);
        realPattern = true;
    }

    size_t flags = wxSD_MATCHCASE | wxSD_MATCHWHOLEWORD;

    pattern.Trim();
    if(pattern.IsEmpty()) return false;

    FindReplaceData data;
    data.SetFindString(pattern);
    data.SetFlags(flags);

    // keep current position
    long curr_pos = GetCurrentPos();
    int match_len(0), pos(0);

    // set the caret at the document start
    if(start_pos < 0 || start_pos > GetLength()) { start_pos = 0; }

    // set the starting point
    SetCurrentPos(0);
    SetSelectionStart(0);
    SetSelectionEnd(0);

    int offset(start_pos);
    bool again(false);
    bool res(false);

    do {
        again = false;
        flags = wxSD_MATCHCASE | wxSD_MATCHWHOLEWORD;

        if(StringFindReplacer::Search(GetText().wc_str(), offset, pattern.wc_str(), flags, pos, match_len)) {

            int line = LineFromPosition(pos);
            wxString dbg_line = GetLine(line).Trim().Trim(false);

            wxString tmp_pattern(pattern);
            tmp_pattern.Trim().Trim(false);
            if(dbg_line.Len() != tmp_pattern.Len() && tmp_pattern != what) {
                offset = pos + match_len;
                again = true;
            } else {

                // select only the name at the given text range
                wxString display_name = what.BeforeFirst(wxT('('));

                int match_len1(0), pos1(0);
                flags |= wxSD_SEARCH_BACKWARD;
                flags |= wxSD_MATCHWHOLEWORD;

                if(realPattern) {
                    // the inner search is done on the pattern without the part of the
                    // signature
                    pattern = pattern.BeforeFirst(wxT('('));
                }

                if(StringFindReplacer::Search(pattern.wc_str(), clUTF8Length(pattern.wc_str(), pattern.Len()),
                                              display_name.wc_str(), flags, pos1, match_len1)) {

                    // select only the word
                    // Check that pos1 is *not* 0 otherwise will get into an infinite loop
                    if(pos1 && GetContext()->IsCommentOrString(pos + pos1)) {
                        // try again
                        offset = pos + pos1;
                        again = true;
                    } else {
                        SetSelection(pos + pos1, pos + pos1 + match_len1);
                        res = true;
                    }
                } else {

                    // as a fallback, mark the whole line
                    ClearSelections();
                    SetCurrentPos(pos);
                    SetSelectionStart(pos);
                    SetSelectionEnd(pos + match_len);
                    res = true;
                }

                if(res && (line >= 0) && !again) {
                    SetEnsureCaretIsVisible(pos);
                    SetLineVisible(LineFromPosition(pos));
                    CenterLinePreserveSelection(LineFromPosition(pos));
                }
            }

        } else {
            // match failed, restore the caret
            SetCurrentPos(curr_pos);
            SetSelectionStart(curr_pos);
            SetSelectionEnd(curr_pos);
        }
    } while(again);

    if(res && navmgr) { navmgr->AddJump(jumpfrom, CreateBrowseRecord()); }
    this->ScrollToColumn(0);
    return res;
}

wxMenu* LEditor::DoCreateDebuggerWatchMenu(const wxString& word)
{
    DebuggerSettingsPreDefMap data;
    DebuggerConfigTool::Get()->ReadObject(wxT("DebuggerCommands"), &data);

    DebuggerPreDefinedTypes preDefTypes = data.GetActiveSet();
    DebuggerCmdDataVec cmds = preDefTypes.GetCmds();

    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);
    wxString menuItemText;

    for(size_t i = 0; i < cmds.size(); i++) {
        DebuggerCmdData cmd = cmds.at(i);
        menuItemText.Clear();
        menuItemText << _("Watch") << wxT(" '") << word << wxT("' ") << _("as") << wxT(" '") << cmd.GetName()
                     << wxT("'");
        item = new wxMenuItem(menu, wxNewId(), menuItemText);
        menu->Prepend(item);
        Connect(item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(LEditor::OnDbgCustomWatch), NULL,
                this);
        m_customCmds[item->GetId()] = cmd.GetCommand();
    }

    return menu;
}

void LEditor::DoUpdateOptions()
{
    // Start by getting the global settings
    m_options = EditorConfigST::Get()->GetOptions();

    // Now let any local preferences overwrite the global equivalent
    if(ManagerST::Get()->IsWorkspaceOpen()) { LocalWorkspaceST::Get()->GetOptions(m_options, GetProject()); }

    clEditorConfigEvent event(wxEVT_EDITOR_CONFIG_LOADING);
    event.SetFileName(GetFileName().GetFullPath());
    if(EventNotifier::Get()->ProcessEvent(event)) { m_options->UpdateFromEditorConfig(event.GetEditorConfig()); }
}

bool LEditor::ReplaceAllExactMatch(const wxString& what, const wxString& replaceWith)
{
    int offset(0);
    wxString findWhat = what;
    size_t flags = wxSD_MATCHWHOLEWORD | wxSD_MATCHCASE;

    int pos(0);
    int match_len(0);
    int posInChars(0);
    int match_lenInChars(0);
    int matchCount(0);
    wxString txt = GetText();

    while(StringFindReplacer::Search(txt.wc_str(), offset, findWhat.wc_str(), flags, pos, match_len, posInChars,
                                     match_lenInChars)) {
        txt.Remove(posInChars, match_lenInChars);
        txt.insert(posInChars, replaceWith);
        matchCount++;
        offset = pos + clUTF8Length(replaceWith.wc_str(), replaceWith.length()); // match_len;
    }

    // replace the buffer
    BeginUndoAction();
    long savedPos = GetCurrentPos();

    SetText(txt);
    // Restore the caret
    SetCaretAt(savedPos);

    EndUndoAction();
    return (matchCount > 0);
}

void LEditor::SetLexerName(const wxString& lexerName) { SetSyntaxHighlight(lexerName); }

void LEditor::HighlightWord(StringHighlightOutput* highlightOutput)
{
    // the search highlighter thread has completed the calculations, fetch the results and mark them in the editor
    const std::vector<std::pair<int, int> >& matches = highlightOutput->matches;
    SetIndicatorCurrent(MARKER_WORD_HIGHLIGHT);

    // clear the old markers
    IndicatorClearRange(0, GetLength());
    if(!highlightOutput->matches.empty()) {
        m_highlightedWordInfo.SetHasMarkers(true);
        int selStart = GetSelectionStart();
        for(size_t i = 0; i < matches.size(); i++) {
            const std::pair<int, int>& p = matches.at(i);

            // Dont highlight the current selection
            if(p.first != selStart) { IndicatorFillRange(p.first, p.second); }
        }
    } else {
        m_highlightedWordInfo.Clear();
    }
}

void LEditor::ChangeCase(bool toLower)
{
    bool hasSelection = (GetSelectedText().IsEmpty() == false);

    if(hasSelection) {

        // Simply change the case of the selection
        toLower ? LowerCase() : UpperCase();

    } else {

        if(GetCurrentPos() >= GetLength()) return;

        // Select the char
        SelectText(GetCurrentPos(), 1);
        toLower ? LowerCase() : UpperCase();
        CharRight();
    }
}

int LEditor::LineFromPos(int pos) { return wxStyledTextCtrl::LineFromPosition(pos); }

int LEditor::PosFromLine(int line) { return wxStyledTextCtrl::PositionFromLine(line); }

int LEditor::LineEnd(int line)
{
    int pos = wxStyledTextCtrl::PositionFromLine(line);
    return pos + wxStyledTextCtrl::LineLength(line);
}

wxString LEditor::GetTextRange(int startPos, int endPos) { return wxStyledTextCtrl::GetTextRange(startPos, endPos); }

void LEditor::DelayedSetActive() { CallAfter(&LEditor::SetActive); }

void LEditor::OnFocus(wxFocusEvent& event)
{
    m_isFocused = true;
    event.Skip();
}

bool LEditor::IsFocused() const
{
#ifdef __WXGTK__
    // Under GTK, when popup menu is ON, we will receive a "FocusKill" event
    // which means that we lost the focus. So the IsFocused() method is using
    // either the m_isFocused flag or the m_popupIsOn flag
    return m_isFocused || m_popupIsOn;
#else
    return m_isFocused;
#endif
}

void LEditor::ShowCalltip(clCallTipPtr tip)
{
    GetFunctionTip()->AddCallTip(tip);
    GetFunctionTip()->Highlight(m_context->DoGetCalltipParamterIndex());

    // In an ideal world, we would like our tooltip to be placed
    // on top of the caret.
    wxPoint pt = PointFromPosition(GetCurrentPosition());
    GetFunctionTip()->Activate(pt, GetCurrLineHeight(), StyleGetBackground(wxSTC_C_DEFAULT));
}

int LEditor::PositionAfterPos(int pos) { return wxStyledTextCtrl::PositionAfter(pos); }

int LEditor::GetCharAtPos(int pos) { return wxStyledTextCtrl::GetCharAt(pos); }

int LEditor::PositionBeforePos(int pos) { return wxStyledTextCtrl::PositionBefore(pos); }

void LEditor::GetChanges(std::vector<int>& changes) { m_deltas->GetChanges(changes); }

void LEditor::OnFindInFiles() { m_deltas->Clear(); }

void LEditor::OnHighlightWordChecked(wxCommandEvent& e)
{
    e.Skip();
// Mainly needed under Mac to toggle the
// buffered drawing on and off
#ifdef __WXMAC__
    SetBufferedDraw(e.GetInt() == 1 ? true : false);
    // wxLogMessage("Settings buffered drawing to: %d", e.GetInt());
    if(e.GetInt()) { Refresh(); }
#endif
}

void LEditor::PasteLineAbove()
{
    // save the current column / line
    int curpos = GetCurrentPos();
    int col = GetColumn(curpos);
    int line = GetCurrentLine();

    int pasteLine = line;
    if(pasteLine > 0) { ++line; }

    int pastePos = PositionFromLine(pasteLine);
    SetCaretAt(pastePos);
    Paste();

    // restore caret position
    int newpos = FindColumn(line, col);
    SetCaretAt(newpos);
}

void LEditor::OnKeyUp(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_CONTROL || event.GetKeyCode() == WXK_SHIFT || event.GetKeyCode() == WXK_ALT) {

        // Clear hyperlink markers
        SetIndicatorCurrent(HYPERLINK_INDICATOR);
        IndicatorClearRange(0, GetLength());
        m_hyperLinkType = wxID_NONE;

        // Clear debugger marker
        SetIndicatorCurrent(DEBUGGER_INDICATOR);
        IndicatorClearRange(0, GetLength());
    }
}

size_t LEditor::GetCodeNavModifier()
{
    size_t mod = wxMOD_NONE;
    if(GetOptions()->GetOptions() & OptionsConfig::Opt_NavKey_Alt) mod |= wxMOD_ALT;
    if(GetOptions()->GetOptions() & OptionsConfig::Opt_NavKey_Control) mod |= wxMOD_CONTROL;
    if(GetOptions()->GetOptions() & OptionsConfig::Opt_NavKey_Shift) mod |= wxMOD_ALT;
    return mod;
}

void LEditor::OnFileFormatDone(wxCommandEvent& e)
{
    if(e.GetString() != GetFileName().GetFullPath()) {
        // not this file
        e.Skip();
        return;
    }

    // Restore the markers
    DoRestoreMarkers();
}

void LEditor::OnFileFormatStarting(wxCommandEvent& e)
{
    if(e.GetString() != GetFileName().GetFullPath()) {
        // not this file
        e.Skip();
        return;
    }
    DoSaveMarkers();
}

void LEditor::DoRestoreMarkers()
{
    MarkerDeleteAll(mmt_all_bookmarks);
    for(size_t i = smt_FIRST_BMK_TYPE; i < m_savedMarkers.size(); ++i) {
        MarkerAdd(m_savedMarkers.at(i).first, m_savedMarkers.at(i).second);
    }
    m_savedMarkers.clear();
    NotifyMarkerChanged();
}

void LEditor::DoSaveMarkers()
{
    m_savedMarkers.clear();
    int nLine = LineFromPosition(0);

    int nFoundLine = MarkerNext(nLine, mmt_all_bookmarks);
    while(nFoundLine >= 0) {
        for(size_t type = smt_FIRST_BMK_TYPE; type < smt_LAST_BMK_TYPE; ++type) {
            int mask = (1 << type);
            if(MarkerGet(nLine) & mask) { m_savedMarkers.push_back(std::make_pair(nFoundLine, type)); }
        }
        nFoundLine = MarkerNext(nFoundLine + 1, mmt_all_bookmarks);
    }
}

void LEditor::ToggleBreakpointEnablement()
{
    int lineno = GetCurrentLine() + 1;

    BreakptMgr* bm = ManagerST::Get()->GetBreakpointsMgr();
    BreakpointInfo bp = bm->GetBreakpoint(GetFileName().GetFullPath(), lineno);
    if(bp.IsNull()) return;

    if(!bm->DelBreakpointByLineno(bp.file, bp.lineno)) return;

    bp.is_enabled = !bp.is_enabled;
    bp.debugger_id = wxNOT_FOUND;
    bp.internal_id = bm->GetNextID();
    ManagerST::Get()->GetBreakpointsMgr()->AddBreakpoint(bp);
    clMainFrame::Get()->GetDebuggerPane()->GetBreakpointView()->Initialize();
}

void LEditor::DoUpdateTLWTitle(bool raise)
{
    // ensure that the top level window parent of this editor is 'Raised'
    wxWindow* tlw = ::wxGetTopLevelParent(this);
    if(tlw && raise) { tlw->Raise(); }

    if(!IsDetached()) {
        clMainFrame::Get()->SetFrameTitle(this);

    } else {
        wxString title;
        title << GetFileName().GetFullPath();
        if(GetModify()) { title.Prepend("*"); }
        tlw->SetLabel(title);
    }
}

bool LEditor::IsDetached() const
{
    const wxWindow* tlw = ::wxGetTopLevelParent(const_cast<LEditor*>(this));
    return (tlw && (clMainFrame::Get() != tlw));
}

void LEditor::GetWordAtMousePointer(wxString& word, wxRect& wordRect)
{
    word.clear();
    wordRect = wxRect();

    long start = wxNOT_FOUND;
    long end = wxNOT_FOUND;
    if(GetSelectedText().IsEmpty()) {
        wxPoint mousePtInScreenCoord = ::wxGetMousePosition();
        wxPoint clientPt = ScreenToClient(mousePtInScreenCoord);
        int pos = PositionFromPoint(clientPt);
        if(pos != wxNOT_FOUND) {
            start = WordStartPosition(pos, true);
            end = WordEndPosition(pos, true);
        }
    } else {
        start = GetSelectionStart();
        end = GetSelectionEnd();
    }

    wxFont font = StyleGetFont(0);
    wxBitmap bmp(1, 1);
    wxMemoryDC memdc(bmp);
    memdc.SetFont(font);
    wxSize sz = memdc.GetTextExtent(GetTextRange(start, end));
    wxPoint ptStart = PointFromPosition(start);
    wxRect rr(ptStart, sz);

    word = GetTextRange(start, end);
    wordRect = rr;
}

void LEditor::ShowRichTooltip(const wxString& tip, const wxString& title, int pos)
{
    if(m_richTooltip) return;
    wxUnusedVar(pos);
    wxString word;
    wxRect rect;
    GetWordAtMousePointer(word, rect);
    m_richTooltip = new wxRichToolTip(title, tip);
    m_richTooltip->ShowFor(this, &rect);
}

wxString LEditor::GetFirstSelection()
{
    int nNumSelections = GetSelections();
    if(nNumSelections > 1) {
        for(int i = 0; i < nNumSelections; ++i) {
            int startPos = GetSelectionNStart(i);
            int endPos = GetSelectionNEnd(i);
            if(endPos > startPos) { return wxStyledTextCtrl::GetTextRange(startPos, endPos); }
        }
        // default
        return wxEmptyString;

    } else {
        return wxStyledTextCtrl::GetSelectedText();
    }
}

void LEditor::SetLineVisible(int lineno)
{
    int offsetFromTop = 10;
    if(lineno != wxNOT_FOUND) {
        // try this: set the first visible line to be -10 lines from
        // the requested lineNo
        lineno -= offsetFromTop;
        if(lineno < 0) { lineno = 0; }
        SetFirstVisibleLine(VisibleFromDocLine(lineno));
        // If the line is hidden - expand it
        EnsureVisible(lineno);
    }
}

void LEditor::DoWrapPrevSelectionWithChars(wxChar first, wxChar last)
{
    // Undo the previous action
    BeginUndoAction();

    // Restore the previous selection
    Undo();
    ClearSelections();

    int charsAdded(0);
    std::vector<std::pair<int, int> > selections;
    for(size_t i = 0; i < m_prevSelectionInfo.GetCount(); ++i) {
        int startPos, endPos;
        m_prevSelectionInfo.At(i, startPos, endPos);

        // insert the wrappers characters
        // Each time we add character into the document, we move the insertion
        // point by 1 (this is why charsAdded is used)
        startPos += charsAdded;
        InsertText(startPos, first);
        ++charsAdded;

        endPos += charsAdded;
        InsertText(endPos, last);
        ++charsAdded;

        selections.push_back(std::make_pair(startPos + 1, endPos));
    }

    // And select it
    for(size_t i = 0; i < selections.size(); ++i) {
        const std::pair<int, int>& range = selections.at(i);
        if(i == 0) {
            SetSelection(range.first, range.second);
        } else {
            AddSelection(range.first, range.second);
        }
    }
    EndUndoAction();
}

void LEditor::OnTimer(wxTimerEvent& event)
{
    event.Skip();
    m_timerHighlightMarkers->Start(100, true);
    if(!HasFocus()) return;

    if(!HasSelection()) {
        HighlightWord(false);

    } else {
        if(EditorConfigST::Get()->GetInteger("highlight_word") == 1) {
            int pos = GetCurrentPos();
            int wordStartPos = WordStartPos(pos, true);
            int wordEndPos = WordEndPos(pos, true);
            wxString word = GetTextRange(wordStartPos, wordEndPos);

            // Read the primary selected text
            int mainSelectionStart = GetSelectionNStart(GetMainSelection());
            int mainSelectionEnd = GetSelectionNEnd(GetMainSelection());

            wxString selectedText = GetTextRange(mainSelectionStart, mainSelectionEnd);
            if(!m_highlightedWordInfo.IsValid(this)) {

                // Check to see if we have marker already on
                // we got a selection
                bool textMatches = (selectedText == word);
                if(textMatches) {
                    // No markers set yet
                    DoHighlightWord();

                } else if(!textMatches) {
                    // clear markers if the text does not match
                    HighlightWord(false);
                }
            } else {
                // we got the markers on, check that they still matches the highlighted word
                if(selectedText != m_highlightedWordInfo.GetWord()) {
                    HighlightWord(false);
                } else {
                    // clDEBUG1() << "Markers are valid - nothing more to be done" << clEndl;
                }
            }
        }
    }
    GetContext()->ProcessIdleActions();
}

void LEditor::SplitSelection()
{
    CHECK_COND_RET(HasSelection() && GetSelections() == 1);

    int selLineStart = LineFromPosition(GetSelectionStart());
    int selLineEnd = LineFromPosition(GetSelectionEnd());

    if(selLineEnd != selLineStart) {
        if(selLineStart > selLineEnd) {
            // swap
            std::swap(selLineEnd, selLineStart);
        }

        ClearSelections();
        for(int i = selLineStart; i <= selLineEnd; ++i) {
            int caretPos;
            if(i != GetLineCount() - 1) {
                // Normally use PositionBefore as LineEnd includes the EOL as well
                caretPos = PositionBefore(LineEnd(i));
            } else {
                caretPos = LineEnd(i); // but it seems not for the last line of the doc
            }
            if(i == selLineStart) {
                // first selection
                SetSelection(caretPos, caretPos);
            } else {
                AddSelection(caretPos, caretPos);
            }
        }
    }
}

void LEditor::CenterLinePreserveSelection(int line)
{
    int linesOnScreen = LinesOnScreen();
    // To place our line in the middle, the first visible line should be
    // the: line - (linesOnScreen / 2)
    int firstVisibleLine = line - (linesOnScreen / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    EnsureVisible(firstVisibleLine);
    SetFirstVisibleLine(firstVisibleLine);
}

void LEditor::CenterLine(int line, int col)
{
    int linesOnScreen = LinesOnScreen();
    // To place our line in the middle, the first visible line should be
    // the: line - (linesOnScreen / 2)
    int firstVisibleLine = line - (linesOnScreen / 2);
    if(firstVisibleLine < 0) { firstVisibleLine = 0; }
    EnsureVisible(firstVisibleLine);
    SetFirstVisibleLine(firstVisibleLine);

    int pos = PositionFromLine(line);
    if(col != wxNOT_FOUND) { pos += col; }
    SetCaretAt(pos);
}

void LEditor::OnEditorConfigChanged(wxCommandEvent& event)
{
    event.Skip();
    DoUpdateOptions();
    SetProperties();
}

void LEditor::ConvertIndentToSpaces()
{
    clSTCLineKeeper lk(GetCtrl());
    bool useTabs = GetUseTabs();
    SetUseTabs(false);
    BeginUndoAction();
    int lineCount = GetLineCount();
    for(int i = 0; i < lineCount; ++i) {
        int indentStart = PositionFromLine(i);
        int indentEnd = GetLineIndentPosition(i);
        int lineIndentSize = GetLineIndentation(i);

        if(indentEnd > indentStart) {
            // this line have indentation
            // delete it
            DeleteRange(indentStart, indentEnd - indentStart);
            SetLineIndentation(i, lineIndentSize);
        }
    }
    EndUndoAction();
    SetUseTabs(useTabs);
}

void LEditor::ConvertIndentToTabs()
{
    clSTCLineKeeper lk(GetCtrl());
    bool useTabs = GetUseTabs();
    SetUseTabs(true);
    BeginUndoAction();
    int lineCount = GetLineCount();
    for(int i = 0; i < lineCount; ++i) {
        int indentStart = PositionFromLine(i);
        int indentEnd = GetLineIndentPosition(i);
        int lineIndentSize = GetLineIndentation(i);

        if(indentEnd > indentStart) {
            // this line have indentation
            // delete it
            DeleteRange(indentStart, indentEnd - indentStart);
            SetLineIndentation(i, lineIndentSize);
        }
    }
    EndUndoAction();
    SetUseTabs(useTabs);
}

void LEditor::DoCancelCodeCompletionBox()
{
    if(m_calltip) {
        m_calltip->Hide();
        m_calltip->Destroy();
        m_calltip = NULL;
    }
}

void LEditor::SetCodeCompletionAnnotation(const wxString& text, int lineno)
{
    AnnotationClearAll();
    m_hasCCAnnotation = true;
    AnnotationSetText(lineno, text);
    AnnotationSetStyle(lineno, ANNOTATION_STYLE_CC_ERROR);
}

int LEditor::GetFirstSingleLineCommentPos(int from, int commentStyle)
{
    int lineNu = LineFromPos(from);
    int lastPos = from + LineLength(lineNu);
    for(int i = from; i < lastPos; ++i) {
        if(GetStyleAt(i) == commentStyle) { return i; }
    }
    return wxNOT_FOUND;
}

int LEditor::GetNumberFirstSpacesInLine(int line)
{
    int start = PositionFromLine(line);
    int lastPos = start + LineLength(line);
    for(int i = start; i < lastPos; ++i) {
        if(!isspace(GetCharAt(i))) { return i - start; }
    }
    return wxNOT_FOUND;
}

void LEditor::ToggleLineComment(const wxString& commentSymbol, int commentStyle)
{
    int start = GetSelectionStart();
    int end = GetSelectionEnd();

    if(start > end) { wxSwap(start, end); }

    int lineStart = LineFromPosition(start);
    int lineEnd = LineFromPosition(end);

    // Check if the "end" position is at the start of a line, in that case, don't
    // include it. Only do this in case of a selection.
    int endLineStartPos = PositionFromLine(lineEnd);
    if(lineStart < lineEnd && endLineStartPos == end) { --lineEnd; }

    bool indentedComments = GetOptions()->GetIndentedComments();

    bool doingComment;
    int indent = 0;
    if(indentedComments) {
        // Check if there is a comment in the line 'lineStart'
        int startCommentPos = GetFirstSingleLineCommentPos(PositionFromLine(lineStart), commentStyle);
        doingComment = (startCommentPos == wxNOT_FOUND);
        if(doingComment) {
            // Find the minimum indent (in whitespace characters) among all the selected lines
            // The comments will be indented with the found number of characters
            indent = 100000;
            bool indentFound = false;
            for(int i = lineStart; i <= lineEnd; i++) {
                int indentThisLine = GetNumberFirstSpacesInLine(i);
                if((indentThisLine != wxNOT_FOUND) && (indentThisLine < indent)) {
                    indent = indentThisLine;
                    indentFound = true;
                }
            }
            if(!indentFound) {
                // Set the indent to zero in case of selection of empty lines
                indent = 0;
            }
        }
    } else {
        doingComment = (GetStyleAt(start) != commentStyle);
    }

    BeginUndoAction();
    for(; lineStart <= lineEnd; ++lineStart) {
        start = PositionFromLine(lineStart);
        if(doingComment) {
            if(indentedComments) {
                if(indent < LineLength(lineStart)) {
                    // Shift the position of the comment by the 'indent' number of characters
                    InsertText(start + indent, commentSymbol);
                }
            } else {
                InsertText(start, commentSymbol);
            }

        } else {
            int firstCommentPos = GetFirstSingleLineCommentPos(start, commentStyle);
            if(firstCommentPos != wxNOT_FOUND) {
                if(GetStyleAt(firstCommentPos) == commentStyle) {
                    SetAnchor(firstCommentPos);
                    SetCurrentPos(PositionAfter(PositionAfter(firstCommentPos)));
                    DeleteBackNotLine();
                }
            }
        }
    }
    EndUndoAction();

    SetCaretAt(PositionFromLine(lineEnd + 1));
    ChooseCaretX();
}

void LEditor::CommentBlockSelection(const wxString& commentBlockStart, const wxString& commentBlockEnd)
{
    int start = GetSelectionStart();
    int end = GetSelectionEnd();
    if(LineFromPosition(PositionBefore(end)) != LineFromPosition(end)) { end = PositionBefore(end); }
    if(start == end) return;

    SetCurrentPos(end);

    BeginUndoAction();
    InsertText(end, commentBlockEnd);
    InsertText(start, commentBlockStart);
    EndUndoAction();

    CharRight();
    CharRight();
    ChooseCaretX();
}

void LEditor::QuickAddNext()
{
    if(!HasSelection()) {
        int start = WordStartPos(GetCurrentPos(), true);
        int end = WordEndPos(GetCurrentPos(), true);
        SetSelection(start, end);
        return;
    }

    int count = GetSelections();
    int start = GetSelectionNStart(count - 1);
    int end = GetSelectionNEnd(count - 1);
    if(GetSelections() == 1) {
        ClearSelections();
        SetSelection(start, end);
        SetMainSelection(0);
    }

    wxString findWhat = GetTextRange(start, end);
    int where = this->FindText(end, GetLength(), findWhat, wxSTC_FIND_MATCHCASE);
    if(where != wxNOT_FOUND) {
        AddSelection(where, where + findWhat.length());
        CenterLineIfNeeded(LineFromPos(where));
    }

    wxString message;
    message << _("Found and selected ") << GetSelections() << _(" matches");
    clGetManager()->GetStatusBar()->SetMessage(message);
}

void LEditor::QuickFindAll()
{
    if(GetSelections() != 1) return;

    int start = GetSelectionStart();
    int end = GetSelectionEnd();
    wxString findWhat = GetTextRange(start, end);
    if(findWhat.IsEmpty()) return;

    ClearSelections();

    int matches(0);
    int firstMatch(wxNOT_FOUND);
    // clWordCharslocker wcl(this);
    int where = this->FindText(0, GetLength(), findWhat, wxSTC_FIND_MATCHCASE);
    while(where != wxNOT_FOUND) {
        if(matches == 0) {
            firstMatch = where;
            SetSelection(where, where + findWhat.length());
            SetMainSelection(0);
            CenterLineIfNeeded(LineFromPos(where));

        } else {
            AddSelection(where, where + findWhat.length());
        }
        ++matches;
        where = this->FindText(where + findWhat.length(), GetLength(), findWhat, wxSTC_FIND_MATCHCASE);
    }
    wxString message;
    message << _("Found and selected ") << GetSelections() << _(" matches");
    clGetManager()->GetStatusBar()->SetMessage(message);
    if(firstMatch != wxNOT_FOUND) { SetMainSelection(0); }
}

void LEditor::CenterLineIfNeeded(int line, bool force)
{
    // Center this line
    int linesOnScreen = LinesOnScreen();
    if(force || ((line < GetFirstVisibleLine()) || (line > (GetFirstVisibleLine() + LinesOnScreen())))) {
        // To place our line in the middle, the first visible line should be
        // the: line - (linesOnScreen / 2)
        int firstVisibleLine = line - (linesOnScreen / 2);
        if(firstVisibleLine < 0) { firstVisibleLine = 0; }
        EnsureVisible(firstVisibleLine);
        SetFirstVisibleLine(firstVisibleLine);
    }
}

void LEditor::Print()
{
#if wxUSE_PRINTING_ARCHITECTURE
    if(g_printData == NULL) {
        g_printData = new wxPrintData();
        wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);
        g_printData->SetPaperId(paper->GetId());
        g_printData->SetPaperSize(paper->GetSize());
        g_printData->SetOrientation(wxPORTRAIT);
        g_pageSetupData = new wxPageSetupDialogData();
        (*g_pageSetupData) = *g_printData;
        PageSetup();
    }

    // Black on White print mode
    SetPrintColourMode(wxSTC_PRINT_BLACKONWHITE);

    // No magnifications
    SetPrintMagnification(0);

    wxPrintDialogData printDialogData(*g_printData);
    wxPrinter printer(&printDialogData);
    clPrintout printout(this, GetFileName().GetFullPath());

    if(!printer.Print(this, &printout, true /*prompt*/)) {
        if(wxPrinter::GetLastError() == wxPRINTER_ERROR) {
            wxLogError(wxT("There was a problem printing. Perhaps your current printer is not set correctly?"));
        } else {
            wxLogMessage(wxT("You canceled printing"));
        }
    } else {
        (*g_printData) = printer.GetPrintDialogData().GetPrintData();
    }
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void LEditor::PageSetup()
{
#if wxUSE_PRINTING_ARCHITECTURE
    if(g_printData == NULL) {
        g_printData = new wxPrintData();
        wxPrintPaperType* paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);
        g_printData->SetPaperId(paper->GetId());
        g_printData->SetPaperSize(paper->GetSize());
        g_printData->SetOrientation(wxPORTRAIT);
        g_pageSetupData = new wxPageSetupDialogData();
        (*g_pageSetupData) = *g_printData;
    }
    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();
    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void LEditor::OnMouseWheel(wxMouseEvent& event)
{
    event.Skip();
    if(::wxGetKeyState(WXK_CONTROL) && !GetOptions()->IsMouseZoomEnabled()) {
        event.Skip(false);
        return;
    } else if(IsCompletionBoxShown()) {
        event.Skip(false);
        wxCodeCompletionBoxManager::Get().GetCCWindow()->DoMouseScroll(event);
    }
}

void LEditor::ClearCCAnnotations()
{
    if(IsHasCCAnnotation()) { AnnotationClearAll(); }
}

void LEditor::ApplyEditorConfig() { SetProperties(); }

void LEditor::OpenURL(wxCommandEvent& event)
{
    wxString url = GetSelectedText();
    ::wxLaunchDefaultBrowser(url);
}

void LEditor::ReloadFromDisk(bool keepUndoHistory)
{
    wxWindowUpdateLocker locker(this);
    SetReloadingFile(true);

    DoCancelCalltip();
    GetFunctionTip()->Deactivate();

    if(m_fileName.GetFullPath().IsEmpty() == true || !m_fileName.FileExists()) {
        SetEOLMode(GetEOLByOS());
        SetReloadingFile(false);
        return;
    }

    clEditorStateLocker stateLocker(GetCtrl());

    wxString text;

    // Read the file we currently support:
    // BOM, Auto-Detect encoding & User defined encoding
    m_fileBom.Clear();
    ReadFileWithConversion(m_fileName.GetFullPath(), text, GetOptions()->GetFileFontEncoding(), &m_fileBom);

    SetText(text);
    m_modifyTime = GetFileLastModifiedTime();
    SetSavePoint();

    if(!keepUndoHistory) {
        EmptyUndoBuffer();
        GetCommandsProcessor().Reset();
    }

    SetReloadingFile(false);
}

void LEditor::PreferencesChanged()
{
    m_statusBarFields = 0;
    if(clConfig::Get().Read(kConfigStatusbarShowLine, true)) { m_statusBarFields |= kShowLine; }
    if(clConfig::Get().Read(kConfigStatusbarShowColumn, true)) { m_statusBarFields |= kShowColumn; }
    if(clConfig::Get().Read(kConfigStatusbarShowPosition, false)) { m_statusBarFields |= kShowPosition; }
    if(clConfig::Get().Read(kConfigStatusbarShowLength, false)) { m_statusBarFields |= kShowLen; }
}

void LEditor::NotifyMarkerChanged(int lineNumber)
{
    // Notify about marker changes
    clCommandEvent eventMarker(wxEVT_MARKER_CHANGED);
    eventMarker.SetFileName(GetFileName().GetFullPath());
    if(lineNumber != wxNOT_FOUND) { eventMarker.SetLineNumber(lineNumber); }
    EventNotifier::Get()->AddPendingEvent(eventMarker);
}

// ----------------------------------
// SelectionInfo
// ----------------------------------
struct SelectorSorter {
    bool operator()(const std::pair<int, int>& a, const std::pair<int, int>& b) { return a.first < b.first; }
};

void LEditor::SelectionInfo::Sort() { std::sort(this->selections.begin(), this->selections.end(), SelectorSorter()); }
