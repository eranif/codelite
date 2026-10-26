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
#include "cl_editor.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "ctags_manager.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "findresultstab.h"
#include "frame.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "plugin.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#include <algorithm>
#include <map>
#include <wx/dc.h>
#include <wx/ffile.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/textbuf.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

namespace
{
constexpr int kPadding = 4;
constexpr size_t kTabSize = 4;
// The toolbar buttons (the name is turned into an ID with XRCID)
constexpr const char* kToolClear = "find_usage_clear";

/// The data of an item of the tree: a file (the parent) or a match (a child of a file)
struct FindUsageItemData : public wxClientData {
    /// A file entry
    explicit FindUsageItemData(const wxString& path)
        : text(path)
    {
    }

    /// A match. The text of the line is loaded when its file is expanded
    explicit FindUsageItemData(const LSP::Location* loc)
        : location(loc)
    {
    }

    bool IsHeader() const { return location == nullptr; }

    const LSP::Location* location = nullptr; // nullptr for a file entry
    wxString text;                           // a file: its path. A match: the line (empty until it is loaded)
    size_t highlightStart = 0;               // a match: the range of `text` to highlight
    size_t highlightLength = 0;
    bool loaded = false; // a file: the lines of its matches are loaded
};

/// The tree store, but the value of an item is its FindUsageItemData (drawn by FindUsageRenderer)
class FindUsageStore : public wxDataViewTreeStore
{
public:
    wxString GetColumnType(unsigned int col) const override
    {
        wxUnusedVar(col);
        return "void*";
    }

    void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const override
    {
        wxUnusedVar(col);
        variant = static_cast<void*>(static_cast<FindUsageItemData*>(GetItemData(item)));
    }
};

/// Replace the tabs of `line` with spaces. The range [`start`, `start` + `length`) of `line` is moved to the same
/// characters of the result: [`highlightStart`, `highlightStart` + `highlightLength`)
wxString PrepareLine(
    const wxString& line, size_t start, size_t length, size_t& highlightStart, size_t& highlightLength)
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
} // namespace

/// Draws a file (its path) or a match (the line, with the matched range highlighted)
class FindUsageRenderer : public wxDataViewCustomRenderer
{
public:
    struct Colours {
        wxColour header;          // the path of a file
        wxColour match;           // a matched line
        wxColour matchBackground; // the rounded rectangle around the matched range of the line
        wxColour matchText;       // the text of the matched range, it contrasts with matchBackground
        wxColour selection;       // the dotted rectangle around the selected row
    };

    FindUsageRenderer()
        : wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
    {
        m_colours.header = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_colours.match = m_colours.header;
        m_colours.matchBackground = wxColour("GOLD");
        m_colours.matchText = *wxBLACK;
        m_colours.selection = m_colours.header;
    }

    void SetColours(const Colours& colours) { m_colours = colours; }

    bool SetValue(const wxVariant& value) override
    {
        m_data = static_cast<const FindUsageItemData*>(value.GetVoidPtr());
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = static_cast<void*>(const_cast<FindUsageItemData*>(m_data));
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        // An empty line has the height of a text line
        const wxSize size = view->GetTextExtent(m_data != nullptr && !m_data->text.empty() ? m_data->text : "Tp");
        return wxSize(size.x + 2 * kPadding, size.y + kPadding);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        if (m_data == nullptr) {
            return false;
        }

        // The colours of the text do not change on a selected row, so the row is marked with a soft dotted rectangle.
        // It covers the entire line: from the left edge of the view (the cell starts after the indentation of the
        // tree) to its right edge
        if ((state & wxDATAVIEW_CELL_SELECTED) != 0) {
            const int viewWidth = GetView() != nullptr ? GetView()->GetClientSize().x : 0;
            wxRect outline(0, rect.y, std::max(viewWidth, rect.GetRight() + 1), rect.height);
            outline.Deflate(1);
            dc->SetPen(wxPen(m_colours.selection, 1, wxPENSTYLE_DOT));
            dc->SetBrush(*wxTRANSPARENT_BRUSH);
            dc->DrawRectangle(outline);
        }

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

        if (m_data->IsHeader()) {
            draw(m_data->text, m_colours.header);
            return true;
        }

        const wxString before = m_data->text.Left(m_data->highlightStart);
        const wxString match = m_data->text.Mid(m_data->highlightStart, m_data->highlightLength);
        const wxString after = m_data->text.Mid(m_data->highlightStart + m_data->highlightLength);
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
    const FindUsageItemData* m_data = nullptr;
    Colours m_colours;
};

namespace
{
/// The data of `item`, or nullptr
FindUsageItemData* GetNodeData(const wxDataViewTreeCtrl* ctrl, const wxDataViewItem& item)
{
    return item.IsOk() ? dynamic_cast<FindUsageItemData*>(ctrl->GetItemData(item)) : nullptr;
}
} // namespace

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

    m_ctrl = new wxDataViewTreeCtrl(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_ROW_LINES | wxBORDER_NONE);

    // The tree has one column. Draw it with our renderer, with a store that gives the renderer our item data
    wxObjectDataPtr<FindUsageStore> store(new FindUsageStore());
    m_ctrl->AssociateModel(store.get());
    m_ctrl->ClearColumns();
    m_renderer = new FindUsageRenderer();
    auto* column = new wxDataViewColumn(wxString(), m_renderer, 0, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, 0);
    m_ctrl->AppendColumn(column);
    m_ctrl->SetExpanderColumn(column);

    // A hand cursor over the results. The control draws in its own child window, so set it on the children as well
    const wxCursor handCursor(wxCURSOR_HAND);
    m_ctrl->SetCursor(handCursor);
    for (wxWindow* child : m_ctrl->GetChildren()) {
        child->SetCursor(handCursor);
    }

    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &FindUsageTab::OnItemExpanding, this);
    m_ctrl->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnItemActivated, this);

    // show context menu for the control. The data view does not send wxEVT_CONTEXT_MENU, it sends its own event (also
    // when the user clicks outside of the items)
    m_ctrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, [this](wxDataViewEvent& event) {
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
    m_ctrl->Unbind(wxEVT_DATAVIEW_ITEM_EXPANDING, &FindUsageTab::OnItemExpanding, this);
}

void FindUsageTab::Clear()
{
    // The items point to the locations: delete them first
    m_ctrl->DeleteAllItems();
    m_locations.clear();
}

void FindUsageTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Clear();
}

void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e)
{
    // The files are the children of the (invisible) root
    e.Enable(m_ctrl->GetChildCount(wxDataViewItem()) != 0);
}

void FindUsageTab::InitialiseView(const std::vector<LSP::Location>& locations)
{
    Clear();
    UpdateStyle();

    m_locations = locations;
    // sort by file / location
    std::map<wxString, std::vector<const LSP::Location*>> sorted_entries;
    for (const LSP::Location& location : m_locations) {
        if (sorted_entries.count(location.GetPath()) == 0) {
            sorted_entries.insert({location.GetPath(), {}});
        }
        std::vector<const LSP::Location*>& file_matches = sorted_entries[location.GetPath()];
        file_matches.push_back(&location);
    }

    // add the entries to the view
    for (const auto& vt : sorted_entries) {
        const wxString& filename = vt.first;
        const std::vector<const LSP::Location*>& locations_vec = vt.second;
        DoAddFileEntries(filename, locations_vec);
    }
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
    // The font of the editor
    if (auto lexer = ColoursAndFontsManager::Get().GetLexer("c++")) {
        m_ctrl->SetFont(lexer->GetFontForStyle(0, this));
    }

    // The colours are taken from the palette of the active terminal theme. Some of them have alpha: the view is solid
    auto solid = [](const wxColour& colour) { return wxColour(colour.Red(), colour.Green(), colour.Blue()); };
    auto* terminals = clGetManager()->GetTerminalManager();
    const wxTerminalTheme theme = terminals != nullptr ? terminals->GetActiveTheme() : wxTerminalTheme::MakeDarkTheme();

    m_ctrl->SetBackgroundColour(solid(theme.bg));
    m_ctrl->SetForegroundColour(solid(theme.fg));
    ::AdjustDataViewAlternateColour(m_ctrl);

    FindUsageRenderer::Colours colours;
    colours.header = solid(theme.blue);
    colours.match = solid(theme.fg);
    colours.matchBackground = solid(theme.highlightBg);
    colours.matchText = DrawingUtils::IsDark(colours.matchBackground) ? *wxWHITE : *wxBLACK;
    colours.selection = solid(theme.brightBlack);
    m_renderer->SetColours(colours);
    m_ctrl->Refresh();
}

void FindUsageTab::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches)
{
    // The matches are added now, their text is loaded when the file is expanded (see DoExpandItem())
    wxDataViewItem child =
        m_ctrl->AppendContainer(wxDataViewItem(), filename, wxWithImages::NO_IMAGE, wxWithImages::NO_IMAGE, new FindUsageItemData(filename));
    for (const LSP::Location* location : matches) {
        m_ctrl->AppendItem(child, wxEmptyString, wxWithImages::NO_IMAGE, new FindUsageItemData(location));
    }
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

    FindUsageItemData* item_data = GetNodeData(m_ctrl, event.GetItem());
    CHECK_PTR_RET(item_data);
    if (item_data->IsHeader()) {
        // header entry
        DoExpandItem(event.GetItem());
        if (!m_ctrl->IsExpanded(event.GetItem())) {
            m_ctrl->Expand(event.GetItem());
        }
        return;
    }

    // Open the file
    wxFileName fn(item_data->location->GetPath());

    // prepare the "after-file-is-loaded" callback
    auto range = item_data->location->GetRange();
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
    open_file_event.SetFileName(item_data->location->GetPath());
    if (!EventNotifier::Get()->ProcessEvent(open_file_event)) {
        ::wxMessageBox(_("Failed to open file: ") + item_data->location->GetPath(),
                       "CodeLite",
                       wxOK | wxOK_DEFAULT | wxICON_ERROR);
        event.Veto();
        return;
    }
    clGetManager()->OpenFileAndAsyncExecute(open_file_event.GetFileName(), std::move(callback));
}

void FindUsageTab::OnItemExpanding(wxDataViewEvent& event)
{
    event.Skip();

    wxBusyCursor bc;
    if (!DoExpandItem(event.GetItem())) {
        event.Veto();
    }
}

bool FindUsageTab::DoExpandItem(const wxDataViewItem& item)
{
    wxBusyCursor bc;
    CHECK_ITEM_RET_FALSE(item);

    // Only a file that was not loaded yet
    FindUsageItemData* header = GetNodeData(m_ctrl, item);
    if (header == nullptr || !header->IsHeader() || header->loaded || m_ctrl->GetChildCount(item) == 0) {
        return true;
    }

    wxString content;
    wxString filepath = header->text;
    if (!wxFileName::Exists(filepath)) {
        // not a local file, request for file download
        clCommandEvent event_download{wxEVT_DOWNLOAD_FILE};
        event_download.SetFileName(filepath);
        clDEBUG() << "Sending event wxEVT_DOWNLOAD_FILE" << endl;
        if (!EventNotifier::Get()->ProcessEvent(event_download)) {
            ::wxMessageBox(_("Failed to download file: ") + filepath, "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
            return false;
        }
        filepath = event_download.GetFileName();
    }

    // load the file into wxTextBuffer
    wxTextFile text_buffer(filepath);
    if (!text_buffer.Open()) {
        ::wxMessageBox(_("Failed to open file: ") + filepath, "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
        return false;
    }

    // go over the entries and set the actual line
    const int child_count = m_ctrl->GetChildCount(item);
    for (int i = 0; i < child_count; ++i) {
        FindUsageItemData* item_data = GetNodeData(m_ctrl, m_ctrl->GetNthChild(item, i));
        if (item_data == nullptr || item_data->IsHeader()) {
            continue;
        }

        // in case this was a remote file, update the filepath
        const_cast<LSP::Location*>(item_data->location)->SetPath(filepath);

        size_t line_number = item_data->location->GetRange().GetStart().GetLine();
        if (line_number < text_buffer.GetLineCount()) {
            const auto& start = item_data->location->GetRange().GetStart();
            const auto& end = item_data->location->GetRange().GetEnd();

            // update the text and the highlight information (a match that spans several lines is not highlighted)
            const bool single_line = start.GetLine() == end.GetLine() && end.GetCharacter() >= start.GetCharacter();
            const size_t length = single_line ? end.GetCharacter() - start.GetCharacter() : 0;
            item_data->text = PrepareLine(text_buffer.GetLine(line_number),
                                          start.GetCharacter(),
                                          length,
                                          item_data->highlightStart,
                                          item_data->highlightLength);
            if (!single_line) {
                item_data->highlightLength = 0;
            }
        }
    }
    header->loaded = true;
    m_ctrl->Refresh();
    return true;
}
