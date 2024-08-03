#include "GitBlamePage.h"

#include "AsyncProcess/asyncprocess.h"
#include "AsyncProcess/processreaderthread.h"
#include "ColoursAndFontsManager.h"
#include "GitDiffOutputParser.h"
#include "clResult.hpp"
#include "clSTCHelper.hpp"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "git.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "windowattrmanager.h"

#include <cmath>
#include <wx/bitmap.h>
#include <wx/dcmemory.h>
#include <wx/tokenzr.h>

#define TEXT_MARGIN_ID 0
#define SYMBOLS_MARGIN_SEP_ID_1 1
#define LINENUMBER_MARGIN_ID 2
#define SYMBOLS_MARGIN 3
#define SYMBOLS_MARGIN_SEP_ID_2 4

namespace git::blame
{
bool LineInfo::FromPorcelainFormat(wxArrayString& lines)
{
    if(lines.empty()) {
        return false;
    }

    // locate the last chunk line (it starts with "\t")
    size_t chunk_last_line = 0;
    bool found = false;
    for(; chunk_last_line < lines.size(); ++chunk_last_line) {
        if(!lines[chunk_last_line].empty() && lines[chunk_last_line][0] == '\t') {
            ++chunk_last_line;
            found = true;
            break;
        }
    }

    if(!found) {
        clWARNING() << "Could not parse blame info. Could not locate line terminator." << lines << endl;
        return false;
    }

    LOG_IF_TRACE
    {
        for(size_t i = 0; i < chunk_last_line; ++i) {
            clTRACE() << "Line:" << lines[i] << endl;
        }
    }

    if(lines[0].length() < 40) {
        clWARNING() << "Could not obtain the commit hash. Line is too short" << endl;
        clWARNING() << "Line:\n" << lines[0] << endl;
        lines.RemoveAt(0, chunk_last_line); // consume this complete record
        return false;
    }

    this->commit_hash = lines[0].Mid(0, 40);
    wxString line_number_as_string = lines[0].AfterFirst(' ').BeforeFirst(' ');
    line_number_as_string.ToCLong(&this->line_number);

    std::unordered_map<wxString, wxString*> M = { { "author", &this->author },
                                                  { "author-mail", &this->author_email },
                                                  { "author-time", &this->author_time },
                                                  { "summary", &this->summary } };

    for(size_t i = 1; i < chunk_last_line; ++i) {
        wxString& line = lines[i];
        if(line.empty()) {
            continue;
        }

        if(line[0] == '\t') {
            this->content = line.AfterFirst('\t');
        } else {
            // space delimited
            wxString key = line.BeforeFirst(' ');
            wxString value = line.AfterFirst(' ');
            if(M.count(key)) {
                M[key]->swap(value);
            } else if(key == "previous") {
                this->prev_commit_hash = value.BeforeFirst(' ');
            }
        }
    }

    lines.RemoveAt(0, chunk_last_line); // consume this complete record

    unsigned long timestamp = 0;
    author_time.ToCULong(&timestamp);
    author_time = wxDateTime((time_t)timestamp).FormatISODate();
    display_line = wxString::Format("% 10s % 10s %s ", commit_hash.Mid(0, 10),
                                    author.length() > 10 ? author.Mid(0, 10) : author, author_time);
    return true;
}
}; // namespace git::blame

/// parse `git blame --line-porcelain <file>` output and return a `LineInfo::vec_t`
git::blame::LineInfo::vec_t ParseBlameOutputInternal(wxArrayString& blameArr, size_t* max_chars)
{
    *max_chars = 0;

    git::blame::LineInfo::vec_t result;
    result.reserve(blameArr.size() / 10);

    while(true) {
        git::blame::LineInfo line_info;
        if(line_info.FromPorcelainFormat(blameArr)) {
            *max_chars = wxMax(line_info.display_line.length(), *max_chars);
            result.push_back(line_info);
        } else {
            break;
        }
    }
    return result;
}

GitBlamePage::GitBlamePage(wxWindow* parent, GitPlugin* plugin, const wxString& fullpath)
    : clThemedSTC(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)
    , m_plugin(plugin)
    , m_filename(fullpath)
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    InitialiseView();
    Bind(wxEVT_STC_MARGIN_RIGHT_CLICK, &GitBlamePage::OnMarginRightClick, this);
    Bind(wxEVT_IDLE, &GitBlamePage::OnIdle, this);
}

GitBlamePage::~GitBlamePage()
{
    clConfig conf("git.conf");
    GitEntry data;
    conf.ReadItem(&data);
    conf.WriteItem(&data);
    Unbind(wxEVT_STC_MARGIN_RIGHT_CLICK, &GitBlamePage::OnMarginRightClick, this);
    Unbind(wxEVT_IDLE, &GitBlamePage::OnIdle, this);
}

void GitBlamePage::ParseBlameOutput(const wxString& blame)
{
    clDEBUG() << "GitBlame is called for file:" << m_filename << clEndl;
    LOG_IF_TRACE { clTRACE() << "GitBlame 'blame':\n" << blame << clEndl; }
    int char_width = TextWidth(0, "W");
    wxArrayString lines = wxStringTokenize(blame, "\n");
    const size_t count = lines.GetCount();
    size_t line_number_margin_width_in_chars =
        log10(count) + 2; // How many digits must we allow room for in the number margin?

    size_t maxChars = 0;
    auto result = ParseBlameOutputInternal(lines, &maxChars);
    m_stack.insert(m_stack.begin(), result);

    SetMarginWidth(TEXT_MARGIN_ID, maxChars * char_width);
    SetMarginWidth(LINENUMBER_MARGIN_ID, char_width * line_number_margin_width_in_chars);

    // In case we're re-entering, ensure we're r/w. For a wxSTC 'readonly' also means can't append text programmatically
    SetReadOnly(false);
    ClearAll();

    for(size_t i = 0; i < result.size(); ++i) {
        // We must append each code-line before doing MarginSetText(), which seems to fail if the line doesn't yet exist
        const auto& d = result[i];
        AppendText(d.content + '\n');
        MarginSetText(i, d.display_line);
    }

    SetReadOnly(true); // Now we can safely set it to r/o
}

const git::blame::LineInfo::vec_t& GitBlamePage::current_info() const
{
    static git::blame::LineInfo::vec_t EmptyResult;
    if(m_stack.empty()) {
        return EmptyResult;
    }
    return m_stack.front();
}

void GitBlamePage::InitialiseView()
{
    // Set blame editor style and fonts
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexerForFile(m_filename);
    if(!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("default");
    }
    lexer->Apply(this, true);

    SetMarginType(TEXT_MARGIN_ID, wxSTC_MARGIN_TEXT);
    SetMarginType(LINENUMBER_MARGIN_ID, wxSTC_MARGIN_NUMBER);
    SetMarginType(SYMBOLS_MARGIN_SEP_ID_1, wxSTC_MARGIN_COLOUR);
    SetMarginMask(SYMBOLS_MARGIN_SEP_ID_2, 0);
    SetMarginSensitive(TEXT_MARGIN_ID, true);

    wxColour bgColour = StyleGetBackground(0);
    bgColour = DrawingUtils::IsDark(bgColour) ? bgColour.ChangeLightness(120) : bgColour.ChangeLightness(60);
    SetMarginMask(SYMBOLS_MARGIN_SEP_ID_1, 0);
    SetMarginWidth(SYMBOLS_MARGIN_SEP_ID_1, FromDIP(1));
    SetMarginBackground(SYMBOLS_MARGIN_SEP_ID_1, bgColour);

    SetMarginType(SYMBOLS_MARGIN_SEP_ID_2, wxSTC_MARGIN_COLOUR);
    SetMarginWidth(SYMBOLS_MARGIN_SEP_ID_2, FromDIP(1));
    SetMarginBackground(SYMBOLS_MARGIN_SEP_ID_2, bgColour);

    SetMarginType(SYMBOLS_MARGIN, wxSTC_MARGIN_SYMBOL);
    SetMarginMask(SYMBOLS_MARGIN, 0);
    SetMarginWidth(SYMBOLS_MARGIN, FromDIP(16));

    SetCaretLineVisible(true);
#if wxCHECK_VERSION(3, 3, 0)
    SetCaretLineFrame(1);
#endif
    SetCaretLineBackground(bgColour);

    SetMarginCursor(SYMBOLS_MARGIN_SEP_ID_1, 8);
    SetMarginCursor(SYMBOLS_MARGIN_SEP_ID_2, 8);
    SetMarginCursor(SYMBOLS_MARGIN, 8);
    SetMarginCursor(LINENUMBER_MARGIN_ID, 8);
    SetMarginCursor(TEXT_MARGIN_ID, 8);
    UsePopUp(0);
}

void GitBlamePage::OnMarginRightClick(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    event.Skip(false);

    wxPoint mousePtInScreenCoord = ::wxGetMousePosition();
    wxPoint clientPt = ScreenToClient(mousePtInScreenCoord);
    size_t line_number = LineFromPosition(PositionFromPoint(clientPt));

    wxMenu menu;
    menu.Append(XRCID("copy-commit-id"), _("Copy commit hash"));
    menu.Append(XRCID("copy-commit-date"), _("Copy commit date"));
    menu.AppendSeparator();
    menu.Append(XRCID("copy-author-email"), _("Copy author email"));
    menu.Append(XRCID("copy-author-name"), _("Copy author name"));
    menu.Bind(
        wxEVT_MENU,
        [this, line_number](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(line_number >= current_info().size()) {
                return;
            }
            ::CopyToClipboard(current_info()[line_number].commit_hash);
            clGetManager()->SetStatusMessage(_("Commit hash copied to clipboard"), 3);
        },
        XRCID("copy-commit-id"));
    menu.Bind(
        wxEVT_MENU,
        [this, line_number](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(line_number >= current_info().size()) {
                return;
            }
            ::CopyToClipboard(current_info()[line_number].author_time);
            clGetManager()->SetStatusMessage(_("Commit date copied to clipboard"), 3);
        },
        XRCID("copy-commit-date"));
    menu.Bind(
        wxEVT_MENU,
        [this, line_number](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(line_number >= current_info().size()) {
                return;
            }
            ::CopyToClipboard(current_info()[line_number].author_email);
            clGetManager()->SetStatusMessage(_("Author email copied to clipboard"), 3);
        },
        XRCID("copy-author-email"));
    menu.Bind(
        wxEVT_MENU,
        [this, line_number](wxCommandEvent& event) {
            wxUnusedVar(event);
            if(line_number >= current_info().size()) {
                return;
            }
            ::CopyToClipboard(current_info()[line_number].author);
            clGetManager()->SetStatusMessage(_("Author name copied to clipboard"), 3);
        },
        XRCID("copy-author-name"));
    PopupMenu(&menu);
}

void GitBlamePage::OnIdle(wxIdleEvent& event)
{
    event.Skip();
    int first_visible_line = GetFirstVisibleLine();
    if(first_visible_line != m_first_visible_line) {
        m_first_visible_line = first_visible_line;
        clSTCHelper::UpdateScrollbarWidth(this);
    }
}
