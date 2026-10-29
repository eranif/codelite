//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : findusagetab.cpp
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

#include "findusagetab.h"

#include "ColoursAndFontsManager.h"
#include "aui/clAuiToolBarArt.h"
#include "clStrings.h"
#include "clSystemSettings.h"
#include "clWorkspaceManager.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "plugin.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#include <algorithm>
#include <map>
#include <wx/dc.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/srchctrl.h>
#include <wx/stc/stc.h>
#include <wx/textfile.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

/// A row of the table: a match
struct FindUsageEntry {
    const LSP::Location* location = nullptr;
    wxString file;             // "file:line": the file relative to the workspace folder (when it is inside it)
    wxString text;             // the matched line, the tabs replaced by spaces (empty if it could not be read)
    size_t highlightStart = 0; // the range of `text` to highlight
    size_t highlightLength = 0;
};

namespace
{
constexpr int kPadding = 4;
constexpr size_t kTabSize = 4;
// The toolbar buttons (the name is turned into an ID with XRCID)
constexpr const char* kToolClear = "find_usage_clear";

/// Replace the tabs of `line` with spaces. The range [`start`, `start` + `length`) of `line` is moved to the same
/// characters of the result: [`highlightStart`, `highlightStart` + `highlightLength`)
wxString PrepareLine(const wxString& line, size_t start, size_t length, size_t& highlightStart, size_t& highlightLength)
{
    const size_t end = start + length;
    size_t newStart = wxString::npos;
    size_t newEnd = wxString::npos;
    wxString text;
    for (size_t i = 0; i < line.length(); ++i) {
        if (i == start) {
            newStart = text.length();
        }
        if (i == end) {
            newEnd = text.length();
        }
        if (line[i] == '\t') {
            text.Append(' ', kTabSize);
        } else {
            text.Append(line[i]);
        }
    }
    if (newStart == wxString::npos) {
        newStart = text.length();
    }
    if (newEnd == wxString::npos) {
        newEnd = text.length();
    }
    highlightStart = newStart;
    highlightLength = newEnd - newStart;
    return text;
}

/// `path` relative to the folder of the workspace, if it is inside it. The paths are compared as strings, so this
/// works for the paths of a remote workspace too
wxString MakeRelativeToWorkspace(const wxString& path)
{
    if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
        return path;
    }
    wxString dir = clWorkspaceManager::Get().GetWorkspace()->GetDir();
    if (dir.empty()) {
        return path;
    }
    if (!dir.EndsWith("/") && !dir.EndsWith("\\")) {
        dir << wxFileName::GetPathSeparator();
    }
    wxString relative;
    if (path.StartsWith(dir, &relative)) {
        return relative;
    }
    // The other separator (for example, a workspace folder with '\' and a path with '/')
    wxString other = dir;
    other.Last() = other.Last() == '/' ? '\\' : '/';
    if (path.StartsWith(other, &relative)) {
        return relative;
    }
    return path;
}

/// The path of a local copy of `path`: `path` itself, or the file downloaded from the remote machine. Empty on error
wxString GetLocalFile(const wxString& path)
{
    if (wxFileName::Exists(path)) {
        return path;
    }
    // not a local file, request for file download
    clCommandEvent event_download{wxEVT_DOWNLOAD_FILE};
    event_download.SetFileName(path);
    clDEBUG() << "Sending event wxEVT_DOWNLOAD_FILE" << endl;
    if (!EventNotifier::Get()->ProcessEvent(event_download)) {
        return wxEmptyString;
    }
    return event_download.GetFileName();
}

/// The DC that a native control (macOS) gives to a custom renderer has the default font, not the font of the control:
/// use the font of the `view` of the renderer, as GetSize() does
void UseViewFont(const wxWindow* view, wxDC* dc)
{
    if (view != nullptr) {
        dc->SetFont(view->GetFont());
    }
}
} // namespace

/// Draws a matched line, with the matched range highlighted
class FindUsageRenderer : public wxDataViewCustomRenderer
{
public:
    struct Colours {
        wxColour match;           // a matched line
        wxColour matchBackground; // the rounded rectangle around the matched range of the line
        wxColour matchText;       // the text of the matched range, it contrasts with matchBackground
        wxColour selection;       // the dotted rectangle around the selected row
    };

    FindUsageRenderer()
        : wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
    {
        m_colours.match = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_colours.matchBackground = wxColour("GOLD");
        m_colours.matchText = *wxBLACK;
        m_colours.selection = m_colours.match;
    }

    void SetColours(const Colours& colours) { m_colours = colours; }
    /// The entry of the selected row. We do not use the state of the cell: on macOS a row is not drawn as selected
    /// when the view does not have the focus, and selecting a row moves the focus to the editor
    void SetSelectedEntry(const FindUsageEntry* entry) { m_selected = entry; }

    bool SetValue(const wxVariant& value) override
    {
        m_entry = static_cast<const FindUsageEntry*>(value.GetVoidPtr());
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = static_cast<void*>(const_cast<FindUsageEntry*>(m_entry));
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        // An empty line has the height of a text line
        wxString text = "Tp";
        if (m_entry != nullptr && !m_entry->text.empty()) {
            text = m_entry->text;
        }
        const wxSize size = view->GetTextExtent(text);
        return wxSize(size.x + 2 * kPadding, size.y);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxUnusedVar(state);
        if (m_entry == nullptr) {
            return false;
        }
        const bool selected = m_entry == m_selected;

        // The colours of the text do not change on a selected row, so the row is marked with a soft dotted rectangle
        if (selected) {
            wxRect outline = rect; // around the text, in case we cannot find the row
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
            // The control clips the cell, and gives us the box of the text, not of the row. The clipping box has the
            // height of the row: remove the clipping for a moment, to draw around the entire line, from the left edge
            // of the view to its right edge
            wxRect clip;
            const bool hasClip = dc->GetClippingBox(clip) && !clip.IsEmpty();
            if (hasClip) {
                const int viewWidth = GetView() != nullptr ? GetView()->GetClientSize().x : 0;
                outline = wxRect(0, clip.y, std::max(viewWidth, clip.GetRight() + 1), clip.height);
                dc->DestroyClippingRegion();
            }
#endif
            outline.Deflate(1);
            dc->SetPen(wxPen(m_colours.selection, 1, wxPENSTYLE_DOT));
            dc->SetBrush(*wxTRANSPARENT_BRUSH);
            dc->DrawRectangle(outline);
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
            if (hasClip) {
                dc->SetClippingRegion(clip);
            }
#endif
        }

        // A long line must not be drawn outside of its cell
        wxDCClipper clipper(*dc, rect);
        UseViewFont(GetView(), dc);
        rect.Deflate(kPadding, 0);
        int x = rect.x;

        // Draw `text` at `x`, vertically centered. Advance `x`
        auto draw = [&](const wxString& text, const wxColour& colour) {
            if (text.empty()) {
                return;
            }
            const wxSize size = dc->GetTextExtent(text);
            dc->SetTextForeground(colour);
            dc->DrawText(text, x, rect.y + (rect.height - size.y) / 2);
            x += size.x;
        };

        // Only the match of the selected row is highlighted, many highlights are too much
        if (!selected) {
            draw(m_entry->text, m_colours.match);
            return true;
        }

        const wxString before = m_entry->text.Left(m_entry->highlightStart);
        const wxString match = m_entry->text.Mid(m_entry->highlightStart, m_entry->highlightLength);
        const wxString after = m_entry->text.Mid(m_entry->highlightStart + m_entry->highlightLength);
        draw(before, m_colours.match);

        if (!match.empty()) {
            // A rounded rectangle around the match, and a text colour that contrasts with it
            const wxSize size = dc->GetTextExtent(match);
            wxRect matchRect(x - 1, rect.y + 1, size.x + 2, rect.height - 2);
            dc->SetPen(*wxTRANSPARENT_PEN);
            dc->SetBrush(wxBrush(m_colours.matchBackground));
            dc->DrawRoundedRectangle(matchRect, 3.0);
            draw(match, m_colours.matchText);
        }
        draw(after, m_colours.match);
        return true;
    }

private:
    const FindUsageEntry* m_entry = nullptr;
    const FindUsageEntry* m_selected = nullptr;
    Colours m_colours;
};

/// Draws the file of a match, with the colour of the matched lines. A text column would use the colour of the system,
/// which is too bright on the colours of the terminal theme
class FindUsageFileRenderer : public wxDataViewCustomRenderer
{
public:
    FindUsageFileRenderer()
        : wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
    {
    }

    void SetColour(const wxColour& colour) { m_colour = colour; }

    bool SetValue(const wxVariant& value) override
    {
        m_text = value.GetString();
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = m_text;
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        const wxSize size = view->GetTextExtent(m_text.empty() ? wxString("Tp") : m_text);
        return wxSize(size.x + 2 * kPadding, size.y);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxUnusedVar(state);
        wxDCClipper clipper(*dc, rect);
        UseViewFont(GetView(), dc);
        rect.Deflate(kPadding, 0);
        const wxSize size = dc->GetTextExtent(m_text);
        dc->SetTextForeground(m_colour.IsOk() ? m_colour : clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc->DrawText(m_text, rect.x, rect.y + (rect.height - size.y) / 2);
        return true;
    }

private:
    wxString m_text;
    wxColour m_colour;
};

FindUsageTab::FindUsageTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));

    // The toolbar. The tools are created by BuildToolbar() (again when the bitmaps change), the events are bound once
    m_toolbar = new wxAuiToolBar(this);
    m_toolbar->Bind(wxEVT_MENU, &FindUsageTab::OnClearAll, this, XRCID(kToolClear));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &FindUsageTab::OnClearAllUI, this, XRCID(kToolClear));
    GetSizer()->Add(m_toolbar, 0, wxEXPAND);
    BuildToolbar();

    // The filter: a row is shown when its file or its matched line contains the text
    m_filter = new wxSearchCtrl(this, wxID_ANY);
    m_filter->SetHint(_("Filter Results..."));
    m_filter->ShowCancelButton(true);
    m_filter->Bind(wxEVT_TEXT, &FindUsageTab::OnFilter, this);
    m_filter->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN, [this](wxCommandEvent& event) {
        wxUnusedVar(event);
        m_filter->ChangeValue(wxEmptyString);
        ApplyFilter();
    });
    GetSizer()->Add(m_filter, 0, wxEXPAND | wxALL, FromDIP(2));

    // A table: the file, and the matched line (drawn with our renderer, the value of the cell is the entry)
    m_ctrl = new wxDataViewListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxBORDER_NONE);
    m_fileRenderer = new FindUsageFileRenderer();
    m_ctrl->AppendColumn(
        new wxDataViewColumn(_("File"), m_fileRenderer, 0, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE),
        "string");
    m_renderer = new FindUsageRenderer();
    m_ctrl->AppendColumn(
        new wxDataViewColumn(_("Matched Line"), m_renderer, 1, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE),
        "void*");

    // A hand cursor over the results. The control draws in its own child window, so set it on the children as well
    const wxCursor handCursor(wxCURSOR_HAND);
    m_ctrl->SetCursor(handCursor);
    for (wxWindow* child : m_ctrl->GetChildren()) {
        child->SetCursor(handCursor);
    }

    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnItemActivated, this);

    // show context menu for the control. The data view does not send wxEVT_CONTEXT_MENU, it sends its own event (also
    // when the user clicks outside of the items)
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, [this](wxDataViewEvent& event) {
        wxUnusedVar(event);
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                Clear();
            },
            wxID_CLEAR);
        m_ctrl->PopupMenu(&menu);
    });

    GetSizer()->Add(m_ctrl, 1, wxEXPAND);
    GetSizer()->Layout();

    // The colours come from the terminal theme, but the tab is created inside the constructor of the output pane: the
    // terminal pane (and the frame's pointer to the output pane) do not exist yet. Update the style when they do
    CallAfter(&FindUsageTab::UpdateStyle);

    // bind events
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &FindUsageTab::OnBitmapsUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);
}

FindUsageTab::~FindUsageTab()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &FindUsageTab::OnBitmapsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);

    m_ctrl->Unbind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);

    // The rows point to the entries: delete them first
    m_ctrl->DeleteAllItems();
}

void FindUsageTab::Clear()
{
    // The rows point to the entries, and the entries to the locations: delete them in this order
    m_renderer->SetSelectedEntry(nullptr);
    m_ctrl->DeleteAllItems();
    m_entries.clear();
    m_locations.clear();
}

void FindUsageTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Clear();
}

void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(!m_entries.empty()); }

void FindUsageTab::InitialiseView(const std::vector<LSP::Location>& locations)
{
    Clear();
    UpdateStyle();

    m_locations = locations;
    // sort by file / location
    std::map<wxString, std::vector<const LSP::Location*>> sorted_entries;
    for (const LSP::Location& location : m_locations) {
        sorted_entries[location.GetPath()].push_back(&location);
    }

    // The lines are read now: the filter needs them
    wxBusyCursor bc;
    for (const auto& [filename, file_matches] : sorted_entries) {
        DoAddFileEntries(filename, file_matches);
    }

    // A new search: show all its results
    m_filter->ChangeValue(wxEmptyString);
    ApplyFilter();
}

void FindUsageTab::DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches)
{
    // The file is shown relative to the workspace. Use the path from the language server: a remote file is read from
    // a local copy
    const wxString display_file = MakeRelativeToWorkspace(filename);

    wxTextFile text_buffer;
    const wxString local_file = GetLocalFile(filename);
    if (local_file.empty() || !text_buffer.Open(local_file)) {
        clWARNING() << "Find Usage: failed to read file:" << filename << endl;
    }

    for (const LSP::Location* location : matches) {
        auto entry = std::make_unique<FindUsageEntry>();
        entry->location = location;
        // The numbers of the lines of the locations start at 0
        entry->file =
            wxString::Format("%s:%d", display_file, static_cast<int>(location->GetRange().GetStart().GetLine()) + 1);

        // in case this was a remote file, open the local copy
        if (!local_file.empty()) {
            const_cast<LSP::Location*>(location)->SetPath(local_file);
        }

        const auto& start = location->GetRange().GetStart();
        const auto& end = location->GetRange().GetEnd();
        const size_t line_number = start.GetLine();
        if (text_buffer.IsOpened() && line_number < text_buffer.GetLineCount()) {
            // the text and the highlight information (a match that spans several lines is not highlighted)
            const bool single_line = start.GetLine() == end.GetLine() && end.GetCharacter() >= start.GetCharacter();
            const size_t length = single_line ? end.GetCharacter() - start.GetCharacter() : 0;
            entry->text = PrepareLine(text_buffer.GetLine(line_number),
                                      start.GetCharacter(),
                                      length,
                                      entry->highlightStart,
                                      entry->highlightLength);
            if (!single_line) {
                entry->highlightLength = 0;
            }
        }
        m_entries.push_back(std::move(entry));
    }
}

void FindUsageTab::ApplyFilter()
{
    wxWindowUpdateLocker locker{m_ctrl};
    // The rows are created again, without a selection
    m_renderer->SetSelectedEntry(nullptr);
    m_ctrl->DeleteAllItems();

    const wxString filter = m_filter->GetValue();
    for (const auto& entry : m_entries) {
        if (!filter.empty() && !entry->file.Contains(filter) && !entry->text.Contains(filter)) {
            continue;
        }
        wxVector<wxVariant> values;
        values.push_back(wxVariant(entry->file));
        values.push_back(wxVariant(static_cast<void*>(entry.get())));
        m_ctrl->AppendItem(values, reinterpret_cast<wxUIntPtr>(entry.get()));
    }
}

void FindUsageTab::OnFilter(wxCommandEvent& event)
{
    event.Skip();
    ApplyFilter();
}

void FindUsageTab::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    UpdateStyle();
}

void FindUsageTab::OnBitmapsUpdated(clCommandEvent& e)
{
    e.Skip();
    BuildToolbar();
}

void FindUsageTab::BuildToolbar()
{
    auto* icons = clGetManager()->GetStdIcons();
    m_toolbar->Clear();
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID(kToolClear), _("Clear"), icons->LoadBitmap("clear"), _("Clear the results"));
    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();
    // The new tools can have another size (a new theme has other icons): tell the sizer, or the toolbar keeps its old
    // size until the window is resized
    m_toolbar->InvalidateBestSize();
    Layout();
    m_toolbar->Refresh();
}

void FindUsageTab::UpdateStyle()
{
    // The font of the editor, with the size of the font of the system, a bit bigger
    if (auto lexer = ColoursAndFontsManager::Get().GetLexer("c++")) {
        wxFont font = lexer->GetFontForStyle(0, this);
        font.SetFractionalPointSize(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetFractionalPointSize() * 1.25);
        m_ctrl->SetFont(font);
    }

    // The colours are taken from the palette of the active terminal theme. Some of them have alpha: the view is solid
    auto solid = [](const wxColour& colour) { return wxColour(colour.Red(), colour.Green(), colour.Blue()); };
    auto* terminals = clGetManager()->GetTerminalManager();
    const wxTerminalTheme theme = terminals != nullptr ? terminals->GetActiveTheme() : wxTerminalTheme::MakeDarkTheme();

    ::AdjustDataViewAlternateColour(m_ctrl);

    FindUsageRenderer::Colours colours;
    colours.match = solid(theme.fg);
    colours.matchBackground = solid(theme.highlightBg);
    colours.matchText = DrawingUtils::IsDark(colours.matchBackground) ? *wxWHITE : *wxBLACK;
    colours.selection = solid(theme.brightBlack);
    m_renderer->SetColours(colours);
    m_fileRenderer->SetColour(colours.match);
    m_ctrl->Refresh();
}

void FindUsageTab::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::OnReferences(LSPEvent& event)
{
    event.Skip();
    InitialiseView(event.GetLocations());

    // ensure that this view is shown
    clGetManager()->ShowOutputPane(SHOW_USAGE);
}

void FindUsageTab::OnReferencesInProgress(LSPEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    CHECK_ITEM_RET(event.GetItem());

    const auto* entry = reinterpret_cast<const FindUsageEntry*>(m_ctrl->GetItemData(event.GetItem()));
    CHECK_PTR_RET(entry);

    // Keep the match of this row highlighted, also after the editor takes the focus
    m_renderer->SetSelectedEntry(entry);
    m_ctrl->Refresh();

    // Open the file
    wxFileName fn(entry->location->GetPath());

    // prepare the "after-file-is-loaded" callback
    auto range = entry->location->GetRange();
    auto callback = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        editor->SelectRangeAfter(range);
    };

    if (fn.FileExists()) {
        // default behaviour
        clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(callback));
        return;
    }

    // the file does not exist
    clCommandEvent open_file_event{wxEVT_OPEN_FILE};
    open_file_event.SetFileName(entry->location->GetPath());
    if (!EventNotifier::Get()->ProcessEvent(open_file_event)) {
        ::wxMessageBox(
            _("Failed to open file: ") + entry->location->GetPath(), "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
        event.Veto();
        return;
    }
    clGetManager()->OpenFileAndAsyncExecute(open_file_event.GetFileName(), std::move(callback));
}
