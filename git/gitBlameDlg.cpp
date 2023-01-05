#include "ColoursAndFontsManager.h"
#include "GitDiffOutputParser.h"
#include "asyncprocess.h"
#include "editor_config.h"
#include "file_logger.h"
#include "git.h"
#include "gitBlameDlg.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "processreaderthread.h"
#include "windowattrmanager.h"
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/tokenzr.h>

#define TEXT_MARGIN_ID 0
#define LINENUMBER_MARGIN_ID 1
#define DATE_WIDTH 11 // 2000-00-00
#define AUTHOR_WIDTH 15
#define HASH_WIDTH 8

size_t FindAuthorLine(wxArrayString& blameArr, size_t n, wxString* author) // Helper function
{
    for(; n < blameArr.GetCount(); ++n) {
        if(blameArr.Item(n).StartsWith("author ", author)) {
            break;
        }
    }
    return n; // which will now index either the next 'author ' field, or the end of the array
}

// The text margin needs to be 24 chars wide for the date & hash, the max author-width, plus 2 spaces between
static size_t marginwidth = DATE_WIDTH + AUTHOR_WIDTH + HASH_WIDTH + 2;

// Helper function. 'n' indexes any previous 'author ' field + 1
wxArrayString ParseBlame(wxStyledTextCtrl* stc, wxArrayString& blameArr, size_t& n)
{
    // For a format supposedly designed for easy parsing, porcelain is decidedly unhelpful
    // Each line's entry usually has 13 fields but may have 12 (or less?)
    // Most fields start with a descriptive string e.g. 'author', 'filename', but the hash and the code-line don't.
    // 'author ' should normally be present, so search for that and then guess
    wxArrayString arr;
    wxString hash, author, dtstring, date;
    n = FindAuthorLine(blameArr, n, &author);
    if(n == blameArr.GetCount()) {
        return arr; // Either something went wrong or we've reached the end
    }

    // OK, we've found the author field. The hash should be the previous field
    hash = blameArr.Item(n - 1);
    wxCHECK_MSG(hash.Len() > 39, arr,
                "What should have been the 'commit-hash' field is too short"); // Something went seriously wrong, or the
                                                                               // output is very strange. Skip.
    hash = ' ' + hash.Left(8);

    for(size_t c = n + 1; c < blameArr.GetCount(); ++c) {
        if(blameArr.Item(c).StartsWith("author-time ", &dtstring)) {
            break;
        }
        if(blameArr.Item(c).StartsWith("author ")) {
            break; // Abort as we've overrun into the next blame line!
        }
    }
    if(!dtstring.empty()) { // Don't abort the whole item just because there's no date
        long datetime;
        if(dtstring.ToLong(&datetime)) {
            wxDateTime dt((time_t)datetime);
            if(dt.IsValid()) {
                date = dt.Format("%d-%m-%Y ");
            }
        }
    }

    // To find the code-line, the safest way is to find the _next_ 'author ' and work backwards
    size_t cl = FindAuthorLine(blameArr, n + 1, NULL);
    if(cl == blameArr.GetCount()) {
        --cl; // This must be the last item, so the code-line should be at the end
    } else {
        cl -= 2; // -1 for the next item's author field, -1 for its hash,
    }

    // Now create the margin line, carefully: the central 'author' field is truncated/padded to fill the available space
    // All this would be much easier if scintilla allowed writing text to 3 'text' margins, but afaict it doesn't
    author.Truncate(AUTHOR_WIDTH);
    author.Pad(AUTHOR_WIDTH - author.Len(), ' ');

    wxString margin;
    margin << date << author << hash;

    wxASSERT(margin.Len() <= marginwidth);
    arr.Add(margin);
    arr.Add(blameArr.Item(cl)); // code-line

    return arr;
}

void StoreExtraArgs(wxComboBox* m_comboExtraArgs, const wxString extraArgs) // Helper function
{
    if(m_comboExtraArgs && !extraArgs.empty()) {
        int pos = m_comboExtraArgs->FindString(extraArgs);
        if(pos == 0) {
            return; // It's already the first item in the list
        }
        if(pos != wxNOT_FOUND) {
            m_comboExtraArgs->Delete(pos);
        }
        m_comboExtraArgs->Insert(extraArgs, 0);
    }
}

GitBlameDlg::GitBlameDlg(wxWindow* parent, GitPlugin* plugin)
    : GitBlameDlgBase(parent)
    , m_plugin(plugin)
    , m_sashPositionMain(0)
    , m_sashPositionV(0)
    , m_sashPositionH(0)
{
    m_editEventsHandler.Reset(new clEditEventsHandler(m_stcBlame));

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    m_showParentCommit = data.GetGitBlameShowParentCommit();
    m_showLogControls = data.GetGitBlameShowLogControls();

    m_sashPositionMain = data.GetGitBlameDlgMainSashPos();
    m_sashPositionV = data.GetGitBlameDlgVSashPos();
    m_sashPositionH = data.GetGitBlameDlgHSashPos();

    if(m_splitterMain->IsSplit()) {
        m_splitterMain->SetSashPosition(m_sashPositionMain);
        m_splitterH->SetSashPosition(m_sashPositionH);
        m_splitterV->SetSashPosition(m_sashPositionV);
        if(!m_showLogControls) {
            m_splitterMain->Unsplit();
        }
    } else if(m_showLogControls && !m_splitterMain->IsSplit()) {
        m_splitterMain->SplitHorizontally(m_splitterPageBottom, m_splitterPageBottom, m_sashPositionMain);
        m_splitterH->SetSashPosition(m_sashPositionH);
        m_splitterV->SetSashPosition(m_sashPositionV);
    }
    m_comboExtraArgs->Clear(); // Remove the placeholder string (there to make the control wider)

    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &GitBlameDlg::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &GitBlameDlg::OnProcessTerminated, this);
    Bind(wxEVT_CHAR_HOOK, &GitBlameDlg::OnCharHook, this);
    ::clSetDialogBestSizeAndPosition(this);
}

GitBlameDlg::~GitBlameDlg()
{
    m_editEventsHandler.Reset(NULL);

    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);

    if(m_showLogControls && m_splitterMain->IsSplit()) {
        data.SetGitBlameDlgMainSashPos(m_splitterMain->GetSashPosition());
        data.SetGitBlameDlgHSashPos(m_splitterH->GetSashPosition());
        data.SetGitBlameDlgVSashPos(m_splitterV->GetSashPosition());
    } else {
        data.SetGitBlameDlgMainSashPos(m_sashPositionMain);
        data.SetGitBlameDlgHSashPos(m_sashPositionH);
        data.SetGitBlameDlgVSashPos(m_sashPositionV);
    }

    data.SetGitBlameShowParentCommit(m_showParentCommit);
    data.SetGitBlameShowLogControls(m_showLogControls);

    conf.WriteItem(&data);
}

void GitBlameDlg::OnCloseDialog(wxCommandEvent& event) { DoCloseDialog(); }

void GitBlameDlg::DoCloseDialog()
{
    m_stcBlame->ClearAll();
    m_choiceHistory->Clear();
    m_comboExtraArgs->Clear();
    m_commitStore.Clear();

    ClearLogControls();

    Hide();
}

void GitBlameDlg::SetBlame(const wxString& blame, const wxString& args)
{
    wxString filename = args;
    size_t where = args.Find(" -- ");
    if(where != wxString::npos) {
        filename = args.Mid(where + 4);
    }
    filename.Trim().Trim(false);

    clDEBUG() << "GitBlame is called for file:" << filename << clEndl;
    LOG_IF_TRACE { clDEBUG1() << "GitBlame 'blame':\n" << blame << clEndl; }

    // Set blame editor style and fonts
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(wxFileName(filename).GetFullName());
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("default");
    }
    lexer->Apply(m_stcBlame, true);

    LexerConf::Ptr_t lex = EditorConfigST::Get()->GetLexer("diff");
    if(lex) {
        lex->Apply(m_stcDiff, true);
    }

    LexerConf::Ptr_t textLex = EditorConfigST::Get()->GetLexer("text");
    textLex->Apply(m_stcCommitMessage, true);

    m_stcBlame->SetMarginType(TEXT_MARGIN_ID, wxSTC_MARGIN_RTEXT);
    // The text margin needs to be 24 chars wide for the date & hash, the max author-width, plus 2 spaces between
    wxBitmap bmp(1, 1);
    wxMemoryDC memDC(bmp);
    memDC.SetFont(m_stcBlame->StyleGetFont(0));
    int charWidth = memDC.GetTextExtent("W").x;

    m_stcBlame->SetMarginWidth(TEXT_MARGIN_ID, marginwidth * charWidth);
    m_stcBlame->SetMarginSensitive(TEXT_MARGIN_ID, true);

    wxArrayString lines = wxStringTokenize(blame, "\n");
    const size_t count = lines.GetCount();
    wxString blameCommit;
    if(args.Contains(" -- ")) {
        // This must be a subsequent SetBlame call after a dclick. We can get the commit from args
        blameCommit = args.Left(8);
    }

    size_t numlen =
        wxString::Format("%i", (int)count).length(); // How many digits must we allow room for in the number margin?
    int m_stcBlame_PixelWidth = 4 + numlen * m_stcBlame->TextWidth(wxSTC_STYLE_LINENUMBER, "9");
    m_stcBlame->SetMarginType(LINENUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);
    m_stcBlame->SetMarginWidth(LINENUMBER_MARGIN_ID, m_stcBlame_PixelWidth);

    // In case we're re-entering, ensure we're r/w. For a wxSTC 'readonly' also means can't append text programatically
    m_stcBlame->SetReadOnly(false);
    m_stcBlame->ClearAll();

    for(size_t n = 0, line = 0; n < count; ++n) {
        wxArrayString item = ParseBlame(m_stcBlame, lines, n);
        if(!item.GetCount()) {
            continue; // The entry must be corrupt, or we overran
        }

        // We must append each code-line before doing MarginSetText(), which seems to fail if the line doesn't yet exist
        m_stcBlame->AppendText(item.Item(1) + '\n');
        m_stcBlame->MarginSetText(line++, item.Item(0));
    }

    m_stcBlame->SetReadOnly(true); // Now we can safely set it to r/o

    m_commitStore.LoadChoice(m_choiceHistory);
    if(!blameCommit.empty()) {
        for(size_t n = 0; n < m_choiceHistory->GetCount(); ++n) {
            if(m_choiceHistory->GetString(n).Left(8) == blameCommit.Left(8)) {
                m_choiceHistory->SetSelection(n);
            }
        }
    }

    if(!m_commitStore.GetRevlistOutput().GetCount()) {
        wxString filepath = m_plugin->GetEditorRelativeFilepath();
        if(!filepath.empty()) {
            m_plugin->OnGitBlameRevList("--parents ", filepath); // Find each commit's parent(s)
        }
    }

    if(!blameCommit.Left(8).empty()) {
        UpdateLogControls(blameCommit.Left(8));
    }
}

void GitBlameDlg::OnRevListOutput(const wxString& output, const wxString& Arguments)
{
    wxCHECK_RET(!output.empty(), "git rev-list returned no output");

    wxArrayString revlistOutput = wxStringTokenize(output, "\n");
    wxCHECK_RET(revlistOutput.GetCount(), "git rev-list gave no valid output");
    m_commitStore.SetRevlistOutput(revlistOutput);

    if(m_commitStore.GetCurrentlyDisplayedCommit().empty()) {
        wxString head = revlistOutput.Item(0).BeforeFirst(' '); // Add HEAD
        m_commitStore.AddCommit(head.Left(8) + " (HEAD)");
        m_commitStore.LoadChoice(m_choiceHistory);

        if(m_stcCommitMessage->IsEmpty()) {
            UpdateLogControls(head.Left(8));
        }
    }
}

void GitBlameDlg::OnStcblameLeftDclick(wxMouseEvent& event)
{
    int position = m_stcBlame->PositionFromPoint(wxPoint(event.GetX(), event.GetY()));
    int line = m_stcBlame->LineFromPosition(position);
    wxString commit = m_stcBlame->MarginGetText(line).Right(8), newBlame;
    wxString filepath(m_plugin->GetEditorRelativeFilepath());
    if(!commit.empty() && commit != "00000000" /*Not yet committed*/ && !filepath.empty()) {
        if(m_showParentCommit) {
            newBlame = m_commitStore.GetCommitParent(commit);
        } else {
            newBlame = commit;
        }
        if(newBlame.empty()) {
            return; // This commit is parentless (it's probably the initial commit) so trying to access commit^ will
                    // fail
        }

        wxString args(newBlame);

        wxString extraArgs = m_comboExtraArgs->GetValue();
        if(!extraArgs.empty()) {
            StoreExtraArgs(m_comboExtraArgs, extraArgs);
            args << ' ' << extraArgs << ' ';
        }
        args << " -- " << filepath;

        m_plugin->DoGitBlame(args);

        m_commitStore.AddCommit(newBlame);
    }
}

void GitBlameDlg::OnExtraArgsTextEnter(wxCommandEvent& event)
{
    GetNewCommitBlame(m_commitStore.GetCurrentlyDisplayedCommit());
}

void GitBlameDlg::OnRefreshBlame(wxCommandEvent& event)
{
    GetNewCommitBlame(m_commitStore.GetCurrentlyDisplayedCommit());
}

void GitBlameDlg::OnPreviousBlame(wxCommandEvent& event)
{
    wxCHECK_RET(m_commitStore.CanGoBack(), "Trying to get a non-existent commit's blame");
    GetNewCommitBlame(m_commitStore.GetPreviousCommit());
}

void GitBlameDlg::OnBackUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_commitStore.CanGoBack()); }

void GitBlameDlg::OnNextBlame(wxCommandEvent& event)
{
    wxCHECK_RET(m_commitStore.CanGoForward(), "Trying to get a non-existent commit's blame");
    GetNewCommitBlame(m_commitStore.GetNextCommit());
}
void GitBlameDlg::OnForwardUpdateUI(wxUpdateUIEvent& event) { event.Enable(m_commitStore.CanGoForward()); }

void GitBlameDlg::OnHistoryItemSelected(wxCommandEvent& event)
{
    wxBusyCursor bc;
    int sel = event.GetSelection();
    wxString str = event.GetString();
    wxString commit = m_commitStore.GetCommit(sel);
    wxASSERT(commit.Left(8) == str.Left(8));

    m_commitStore.SetCurrentlyDisplayedCommit(commit);
    GetNewCommitBlame(commit);
}

void GitBlameDlg::GetNewCommitBlame(const wxString& commit)
{
    wxString filepath(m_plugin->GetEditorRelativeFilepath());
    if(!commit.empty() && !filepath.empty()) {
        wxString args(commit.Left(8)); // Just the first 8 because of "abcd1234 (HEAD)"

        wxString extraArgs = m_comboExtraArgs->GetValue();
        if(!extraArgs.empty()) {
            StoreExtraArgs(m_comboExtraArgs, extraArgs);
            args << ' ' << extraArgs << ' ';
        }

        args << " -- " << filepath;
        m_plugin->DoGitBlame(args);

        ClearLogControls();
    }
}

void GitBlameDlg::ClearLogControls()
{
    m_stcCommitMessage->SetEditable(true);
    m_stcDiff->SetEditable(true);

    m_stcCommitMessage->ClearAll();
    m_fileListBox->Clear();
    m_diffMap.clear();
    m_stcDiff->ClearAll();

    m_stcCommitMessage->SetEditable(false);
    m_stcDiff->SetEditable(false);
}
void GitBlameDlg::OnSettings(wxCommandEvent& event)
{
    GitBlameSettingsDlg dlg(this, m_showParentCommit, m_showLogControls);
    if(dlg.ShowModal() == wxID_OK) {
        m_showParentCommit = dlg.GetCheckParentCommit()->IsChecked();
        m_showLogControls = dlg.GetCheckShowLogControls()->IsChecked();

        clConfig conf("git.conf");
        GitEntry data;
        conf.ReadItem(&data);

        data.SetGitBlameShowParentCommit(m_showParentCommit);
        data.SetGitBlameShowLogControls(m_showLogControls);
        conf.WriteItem(&data);

        if(m_splitterMain->IsSplit() && !m_showLogControls) {
            m_sashPositionMain = m_splitterMain->GetSashPosition();
            m_sashPositionH = m_splitterH->GetSashPosition();
            m_sashPositionV = m_splitterV->GetSashPosition();
            m_splitterMain->Unsplit();
        } else if(m_showLogControls && !m_splitterMain->IsSplit()) {
            m_splitterMain->SplitHorizontally(m_splitterPageTop, m_splitterPageBottom, m_sashPositionMain);
            m_splitterH->SetSashPosition(m_sashPositionH);
            m_splitterV->SetSashPosition(m_sashPositionV);
        }
    }
}

GitBlameSettingsDlg::GitBlameSettingsDlg(wxWindow* parent, bool showParentCommit, bool showLogControls)
    : GitBlameSettingsDlgBase(parent)
{
    m_checkParentCommit->SetValue(showParentCommit);
    m_checkShowLogControls->SetValue(showLogControls);
}

void GitBlameDlg::OnChangeFile(wxCommandEvent& event)
{
    int sel = m_fileListBox->GetSelection();
    wxString file = m_fileListBox->GetString(sel);
    m_stcDiff->SetReadOnly(false);
    m_stcDiff->SetText(m_diffMap[file]);
    m_stcDiff->SetReadOnly(true);
}

void GitBlameDlg::OnProcessTerminated(clProcessEvent& event)
{
    wxUnusedVar(event);
    wxDELETE(m_process);

    m_stcCommitMessage->SetEditable(true);
    m_stcDiff->SetEditable(true);
    m_stcCommitMessage->ClearAll();
    m_fileListBox->Clear();
    m_diffMap.clear();
    m_stcDiff->ClearAll();
    m_commandOutput.Replace(wxT("\r"), wxT(""));

    wxArrayString commitMessage;
    GitDiffOutputParser diff_parser;
    diff_parser.GetDiffMap(m_commandOutput, m_diffMap, &commitMessage);

    for(wxStringMap_t::iterator it = m_diffMap.begin(); it != m_diffMap.end(); ++it) {
        m_fileListBox->Append((*it).first);
    }

    if(m_diffMap.size() != 0) {
        wxString file = m_plugin->GetEditorRelativeFilepath();
        m_stcDiff->SetText(m_diffMap[file]);
        m_fileListBox->SetStringSelection(file);
    }

    for(size_t i = 0; i < commitMessage.GetCount(); ++i) {
        m_stcCommitMessage->AppendText(commitMessage.Item(i));
    }

    m_stcDiff->SetEditable(false);
    m_commandOutput.Clear();
    m_stcCommitMessage->SetEditable(false);
}

void GitBlameDlg::OnProcessOutput(clProcessEvent& event) { m_commandOutput.Append(event.GetOutput()); }

void GitBlameDlg::UpdateLogControls(const wxString& commit)
{
    if(!commit.empty()) {
        wxString command_args;
        command_args << "--no-pager show " << commit;
        m_process = m_plugin->AsyncRunGit(this, command_args, IProcessCreateDefault | IProcessWrapInShell,
                                          m_plugin->GetRepositoryPath());
    }
}

void GitBlameDlg::Show()
{
    ::clSetDialogBestSizeAndPosition(this);
    wxDialog::Show();
}

void GitBlameDlg::OnCharHook(wxKeyEvent& event)
{
    event.Skip();
    if(event.GetKeyCode() == WXK_ESCAPE) {
        DoCloseDialog();
    }
}
