#include "BuildTab.hpp"
#include "ColoursAndFontsManager.h"
#include "StringUtils.h"
#include "build_settings_config.h"
#include "clAsciiEscapCodeHandler.hpp"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "frame.h"
#include "globals.h"
#include "mainbook.h"
#include "shell_command.h"
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/tokenzr.h>

namespace
{
class MyAsciiRenderer : public clControlWithItemsRowRenderer
{
    clAsciiEscapeCodeHandler handler;
    wxFont m_font;

public:
    MyAsciiRenderer() {}

    void SetFont(const wxFont& f) { this->m_font = f; }
    void Render(wxWindow* window, wxDC& dc, const clColours& colours, int row_index, clRowEntry* entry) override
    {
        wxUnusedVar(window);
        wxUnusedVar(row_index);

        // draw the ascii line
        handler.Reset();
        handler.Parse(entry->GetLabel(0));

        if(entry->IsSelected()) {
            dc.SetPen(colours.GetSelItemBgColour());
            dc.SetBrush(colours.GetSelItemBgColour());
            dc.DrawRectangle(entry->GetItemRect());
        }

        clRenderDefaultStyle ds;
        ds.bg_colour = colours.GetBgColour();
        ds.fg_colour = colours.GetItemTextColour();
        ds.font = m_font;
        handler.Render(dc, ds, 0, entry->GetItemRect());
    }
};

// Helper function to post an event to the frame
void SetActive(clEditor* editor)
{
    wxCHECK_RET(editor, wxT("Null editor parameter"));
    wxCommandEvent event(wxEVT_ACTIVATE_EDITOR);
    event.SetEventObject(editor);
    wxPostEvent(clMainFrame::Get(), event);
}

} // namespace

BuildTab::BuildTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    GetSizer()->Fit(this);

    m_view = new clDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_MULTIPLE | wxDV_NO_HEADER);
    GetSizer()->Add(m_view, 1, wxEXPAND);
    m_renderer = new MyAsciiRenderer();
    m_view->SetCustomRenderer(m_renderer);
    m_view->AppendIconTextColumn(_("Message"), wxDATAVIEW_CELL_INERT, -2, wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE);

    // Event handling
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_STARTED, &BuildTab::OnBuildStarted, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ADDLINE, &BuildTab::OnBuildAddLine, this);
    EventNotifier::Get()->Bind(wxEVT_BUILD_PROCESS_ENDED, &BuildTab::OnBuildEnded, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &BuildTab::OnSysColourChanged, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, [this](clWorkspaceEvent& e) {
        e.Skip();
        Cleanup();
    });

    m_view->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &BuildTab::OnLineActivated, this);
    m_view->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &BuildTab::OnContextMenu, this);
    ApplyStyle();
}

BuildTab::~BuildTab() {}

void BuildTab::OnBuildStarted(clBuildEvent& e)
{
    e.Skip();
    m_buildInProgress = true;
    m_buffer.clear();
    if(e.IsCleanLog()) {
        ClearView();
    }

    // read the build tab settings
    EditorConfigST::Get()->ReadObject(wxT("build_tab_settings"), &m_buildTabSettings);

    // clean the last used compiler
    m_activeCompiler.Reset(nullptr);
    m_error_count = m_warn_count = 0;

    // get the toolchain from the event and attempt to load the compiler
    const wxString& toolchain = e.GetToolchain();
    if(!toolchain.empty() && BuildSettingsConfigST::Get()->IsCompilerExist(toolchain)) {
        m_activeCompiler = BuildSettingsConfigST::Get()->GetCompiler(toolchain);
        clDEBUG() << "Active compiler is set to:" << m_activeCompiler->GetName() << endl;
    }
}

void BuildTab::OnBuildAddLine(clBuildEvent& e)
{
    e.Skip();
    m_buffer << e.GetString();
    ProcessBuffer();
}

void BuildTab::OnBuildEnded(clBuildEvent& e)
{
    e.Skip();
    ProcessBuffer(true);

    // at this point, m_buffer is empty
    wxString text;
    text << "\n";
    eAsciiColours c;
    if(m_error_count) {
        text = _("==== build completed with errors ");
        c = eAsciiColours::RED;
    } else if(m_warn_count) {
        text = _("=== build ended with warnings ");
        c = eAsciiColours::YELLOW;
    } else {
        text = _("=== build completed successfully ");
        c = eAsciiColours::GREEN;
    }
    text << "(" << m_error_count << _(" errors, ") << m_warn_count << _(" warnings) ===");
    text = WrapLineInColour(text, c);
    m_buffer.swap(text);
    ProcessBuffer(true);
}

void BuildTab::ProcessBuffer(bool last_line)
{
    // process completed lines
    m_view->Begin();
    auto lines = ::wxStringTokenize(m_buffer, "\n", wxTOKEN_RET_DELIMS);
    wxString remainder;
    while(!lines.empty()) {
        auto& line = lines.back();
        if(!last_line && !line.EndsWith("\n")) {
            // not a complete line
            remainder.swap(line);
            break;
        }
        line.Trim();

        // easy path: check for common makefile messages
        if(line.Lower().Contains("entering directory") || line.Lower().Contains("leaving directory")) {
            line = WrapLineInColour(line, eAsciiColours::GRAY);
            m_view->AppendItem(line);
        } else {

            std::unique_ptr<Compiler::PatternMatch> m(new Compiler::PatternMatch);

            // remove the terminal ascii colouring escape code
            wxString modified_line;
            StringUtils::StripTerminalColouring(line, modified_line);
            bool lineHasColours = (line.length() != modified_line.length());
            if(!m_activeCompiler || !m_activeCompiler->Matches(modified_line, m.get())) {
                m.reset();
            } else {
                switch(m->sev) {
                case Compiler::kSevError:
                    m_error_count++;
                    break;
                case Compiler::kSevWarning:
                    m_warn_count++;
                    break;
                default:
                    break;
                }
            }

            // if this line matches a pattern (error or warning) AND
            // this colour has no colour associated with it (using ASCII escape)
            // add some
            if(!lineHasColours && m.get()) {
                line =
                    WrapLineInColour(line, m->sev == Compiler::kSevError ? eAsciiColours::RED : eAsciiColours::YELLOW);
            }
            // Associate the match info with the line in the view
            // this will be used later when selecting lines
            // Note: its OK to pass null here
            m_view->AppendItem(line, wxNOT_FOUND, wxNOT_FOUND, (wxUIntPtr)m.release());
        }

        lines.pop_back();
    }

    if(last_line) {
        m_buffer.clear();
    } else {
        m_buffer.swap(remainder);
    }
    m_view->Commit();
    // select the last line
    size_t row = m_view->GetItemCount() - 1;
    m_view->SelectRow(row);
    m_view->EnsureVisible(m_view->RowToItem(row));
    m_view->UnselectRow(row);
}

void BuildTab::OnSysColourChanged(clCommandEvent& e)
{
    e.Skip();
    ApplyStyle();
    m_view->Refresh();
}

void BuildTab::Cleanup()
{
    m_buildInProgress = false;
    m_buffer.clear();
    ClearView();
    m_activeCompiler.Reset(nullptr);
    m_error_count = 0;
    m_warn_count = 0;
}

void BuildTab::ApplyStyle()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("text");
    if(lexer) {
        MyAsciiRenderer* r = static_cast<MyAsciiRenderer*>(m_renderer);
        wxFont f = lexer->GetFontForSyle(0, this);
        r->SetFont(f);
        clColours colours;
        colours.InitFromColour(lexer->GetProperty(0).GetBgColour());
        colours.SetItemTextColour(lexer->GetProperty(0).GetFgColour());
        m_view->SetDefaultFont(f);
        m_view->SetColours(colours);
    }
}

void BuildTab::AppendLine(const wxString& text)
{
    m_buffer << text;
    ProcessBuffer();
}

void BuildTab::ClearView()
{
    m_view->UnselectAll();
    m_view->DeleteAllItems([](wxUIntPtr d) {
        if(d) {
            Compiler::PatternMatch* p = reinterpret_cast<Compiler::PatternMatch*>(d);
            wxDELETE(p);
        }
    });
    m_buffer.clear();
}

wxString BuildTab::WrapLineInColour(const wxString& line, eAsciiColours colour) const
{
    wxString text;
    clAsciiEscapeColourBuilder text_builder(text);

    bool is_light = m_view->GetColours().IsLightTheme();
    text_builder.SetTheme(is_light ? eAsciiTheme::LIGHT : eAsciiTheme::DARK).Add(line, colour);
    return text;
}

void BuildTab::OnLineActivated(wxDataViewEvent& e)
{
    clDEBUG() << "Build line double clicked" << endl;
    auto cd = reinterpret_cast<Compiler::PatternMatch*>(m_view->GetItemData(e.GetItem()));
    CHECK_PTR_RET(cd);

    // let the plugins a first chance in handling this line
    clBuildEvent eventErrorClicked(wxEVT_BUILD_OUTPUT_HOTSPOT_CLICKED);
    eventErrorClicked.SetFileName(cd->file_path);
    eventErrorClicked.SetLineNumber(cd->line_number);
    clDEBUG() << "Letting plugins process it first (" << cd->file_path << ":" << cd->line_number << ")" << endl;
    if(EventNotifier::Get()->ProcessEvent(eventErrorClicked)) {
        return;
    }

    wxFileName fn(cd->file_path);
    if(fn.FileExists()) {
        // if we resolved it now, open the file there is no point in searching this file
        // in m_buildInfoPerFile since the key on this map is kept as full name
        clEditor* editor = clMainFrame::Get()->GetMainBook()->FindEditor(fn.GetFullPath());
        if(!editor) {
            editor = clMainFrame::Get()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, cd->line_number);
        }

        CHECK_PTR_RET(editor);
        DoCentreErrorLine(cd, editor, true);
    }
}

void BuildTab::DoCentreErrorLine(Compiler::PatternMatch* match_result, clEditor* editor, bool centerLine)
{
    // We already got compiler markers set here, just goto the line
    clMainFrame::Get()->GetMainBook()->SelectPage(editor);
    CHECK_PTR_RET(match_result);

    // Convert the compiler column to scintilla's position
    if(match_result->column != wxNOT_FOUND) {
        editor->CenterLine(match_result->line_number, match_result->column - 1);
    } else {
        editor->CenterLine(match_result->line_number);
    }

    if(centerLine) {
        auto stc = editor->GetCtrl();
        // If the line in the build error tab is not visible, ensure it is
        int linesOnScreen = stc->LinesOnScreen();

        // Our line is not visible
        int firstVisibleLine = match_result->line_number - (linesOnScreen / 2);
        if(firstVisibleLine < 0) {
            firstVisibleLine = 0;
        }
        stc->EnsureVisible(firstVisibleLine);
        stc->SetFirstVisibleLine(firstVisibleLine);
    }
    SetActive(editor);
}

void BuildTab::OnContextMenu(wxDataViewEvent& e)
{
    wxMenu menu;
    if(e.GetItem().IsOk()) {
        menu.Append(XRCID("copy-current-lines"), _("Copy selected lines to clipboard"));
        menu.AppendSeparator();
    }
    menu.Append(wxID_CLEAR);
    menu.Append(wxID_SAVEAS);
    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            ClearView();
        },
        wxID_CLEAR);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            CallAfter(&BuildTab::SaveBuildLog);
        },
        wxID_SAVEAS);

    menu.Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            wxUnusedVar(event);
            CallAfter(&BuildTab::CopySelections);
        },
        XRCID("copy-current-lines"));

    m_view->PopupMenu(&menu);
}

void BuildTab::SaveBuildLog()
{
    // get the content
    wxString content;
    content.reserve(16 * 1024); // 16K

    for(size_t i = 0; i < m_view->GetItemCount(); ++i) {
        wxString str;
        StringUtils::StripTerminalColouring(m_view->GetItemText(m_view->RowToItem(i)), str);
        content << str << "\n";
    }

    wxString path = ::wxFileSelector();
    if(path.empty()) {
        return;
    }

    if(wxFileName::FileExists(path)) {
        if(::wxMessageBox(_("A file with the same name already exists, continue?"), "CodeLite",
                          wxICON_WARNING | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT) != wxYES) {
            return;
        }
    }
    FileUtils::WriteFileContent(path, content);
}

void BuildTab::CopySelections()
{
    wxDataViewItemArray items;
    if(m_view->GetSelections(items) == 0) {
        return;
    }

    wxString content;
    for(size_t i = 0; i < items.size(); ++i) {
        wxString str;
        StringUtils::StripTerminalColouring(m_view->GetItemText(items[i]), str);
        content << str << "\n";
    }
    ::CopyToClipboard(content);
}
