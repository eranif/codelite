#include "clPropertiesPage.hpp"

#include "ColoursAndFontsManager.h"
#include "EditDlg.h"
#include "clSystemSettings.h"
#include "fileextmanager.h"
#include "globals.h"
#include "macros.h"

#include <algorithm>
#include <wx/colordlg.h>
#include <wx/dc.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/numdlg.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxEVT_PROPERTIES_PAGE_MODIFIED, clPropertiesPageEvent);
wxDEFINE_EVENT(wxEVT_PROPERTIES_PAGE_SAVED, clPropertiesPageEvent);
wxDEFINE_EVENT(wxEVT_PROPERTIES_PAGE_ACTION_BUTTON, clPropertiesPageEvent);

/// The data of a line. Both cells of the line hold a pointer to it
struct clPropertiesCell {
    LineKind kind = LineKind::UNKNOWN;
    bool is_header = false;
    wxString label;  // the name of the property
    wxString text;   // the displayed value (also the current selection of a choice)
    wxColour colour; // for LineKind::COLOUR
    bool checked = false;
};

namespace
{
constexpr int kPadding = 4;            // in DIPs
constexpr int kColourSwatchWidth = 24; // in DIPs
constexpr int kCellSlack = 16;         // in DIPs: extra room of a column, so its text is not truncated
constexpr int kFirstChoiceId = wxID_HIGHEST + 1;

/// The width that the value of a line needs, as it is drawn by PropertyRenderer
int MeasureValueWidth(wxWindow* view, const clPropertiesCell& cell)
{
    const int padding = view->FromDIP(kPadding);
    switch (cell.kind) {
    case LineKind::CHECKBOX:
        return wxRendererNative::Get().GetCheckBoxSize(view).x + 2 * padding;
    case LineKind::COLOUR: {
        int width = padding + view->FromDIP(kColourSwatchWidth) + 2 * padding;
        if (cell.colour.IsOk()) {
            width += view->GetTextExtent(cell.colour.GetAsString(wxC2S_HTML_SYNTAX)).x;
        }
        return width + padding;
    }
    case LineKind::BUTTON:
        return view->GetTextExtent(cell.text).x + 6 * padding;
    case LineKind::UNKNOWN:
        return 0;
    default:
        return view->GetTextExtent(cell.text).x + 2 * padding;
    }
}

/// Draws the cells of the page: the name (bold for a header) or the value. A value is drawn according to the kind of
/// its line: a checkbox, a colour swatch with its HTML code, a push button, or a plain text
class PropertyRenderer : public wxDataViewCustomRenderer
{
public:
    using ActivateCallback_t = std::function<void(const wxDataViewItem&)>;

    PropertyRenderer(bool is_value_column, ActivateCallback_t on_activate)
        : wxDataViewCustomRenderer(
              "void*", is_value_column ? wxDATAVIEW_CELL_ACTIVATABLE : wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
        , m_isValueColumn{is_value_column}
        , m_onActivate{std::move(on_activate)}
    {
    }

    bool SetValue(const wxVariant& value) override
    {
        m_cell = static_cast<const clPropertiesCell*>(value.GetVoidPtr());
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = static_cast<void*>(const_cast<clPropertiesCell*>(m_cell));
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        // All the rows have the same height: it fits a text and a checkbox
        const int height = std::max(view->GetTextExtent("Tp").y, wxRendererNative::Get().GetCheckBoxSize(view).y) +
                           view->FromDIP(kPadding);
        // The renderer draws a text with its own offset and margin, and truncates it ("...") when it does not fit the
        // column: leave some room
        int width = 0;
        if (m_cell != nullptr) {
            if (m_isValueColumn) {
                width = MeasureValueWidth(view, *m_cell);
            } else {
                // a header is drawn in bold
                wxFont font = view->GetFont();
                if (m_cell->is_header) {
                    font.MakeBold();
                }
                view->GetTextExtent(m_cell->label, &width, nullptr, nullptr, nullptr, &font);
                width += 2 * view->FromDIP(kPadding);
            }
        }
        return wxSize(width + view->FromDIP(kCellSlack), height);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxWindow* view = GetView();
        if (m_cell == nullptr || view == nullptr) {
            return false;
        }

        // The DC of a native control (macOS) has the default font, not the font of the control
        wxFont font = view->GetFont();
        if (!m_isValueColumn && m_cell->is_header) {
            font.MakeBold();
        }
        dc->SetFont(font);

        // A long text must not be drawn outside of its cell
        wxDCClipper clipper(*dc, rect);
        const int padding = view->FromDIP(kPadding);
        if (!m_isValueColumn) {
            RenderText(m_cell->label, padding, rect, dc, state);
            return true;
        }

        switch (m_cell->kind) {
        case LineKind::CHECKBOX: {
            const wxSize box = wxRendererNative::Get().GetCheckBoxSize(view);
            const wxRect box_rect(rect.x + padding, rect.y + (rect.height - box.y) / 2, box.x, box.y);
            wxRendererNative::Get().DrawCheckBox(view, *dc, box_rect, m_cell->checked ? wxCONTROL_CHECKED : 0);
            break;
        }
        case LineKind::COLOUR: {
            const int side = rect.height - 2 * padding;
            const wxRect swatch(
                rect.x + padding, rect.y + (rect.height - side) / 2, view->FromDIP(kColourSwatchWidth), side);
            dc->SetPen(clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
            dc->SetBrush(m_cell->colour.IsOk() ? wxBrush(m_cell->colour) : *wxTRANSPARENT_BRUSH);
            dc->DrawRectangle(swatch);
            if (m_cell->colour.IsOk()) {
                RenderText(m_cell->colour.GetAsString(wxC2S_HTML_SYNTAX), swatch.width + 2 * padding, rect, dc, state);
            }
            break;
        }
        case LineKind::BUTTON: {
            const int text_width = view->GetTextExtent(m_cell->text).x;
            wxRect button_rect(rect.x + padding, rect.y + 1, text_width + 4 * padding, rect.height - 2);
            wxRendererNative::Get().DrawPushButton(view, *dc, button_rect, 0);
            RenderText(m_cell->text, 3 * padding, rect, dc, state);
            break;
        }
        case LineKind::UNKNOWN:
            // a header has no value
            break;
        default:
            RenderText(m_cell->text, padding, rect, dc, state);
            break;
        }
        return true;
    }

    /// A click, or the Space key
    bool ActivateCell(const wxRect& cell,
                      wxDataViewModel* model,
                      const wxDataViewItem& item,
                      unsigned int col,
                      const wxMouseEvent* mouseEvent) override
    {
        wxUnusedVar(cell);
        wxUnusedVar(model);
        wxUnusedVar(col);
        wxUnusedVar(mouseEvent);
        if (!m_isValueColumn || m_cell == nullptr || m_cell->kind == LineKind::UNKNOWN || !m_onActivate) {
            return false;
        }
        m_onActivate(item);
        return true;
    }

private:
    bool m_isValueColumn = false;
    ActivateCallback_t m_onActivate;
    const clPropertiesCell* m_cell = nullptr;
};
} // namespace

clPropertiesPage::clPropertiesPage(wxWindow* parent, wxWindowID id)
    : wxPanel(parent, id)
{
    SetSizer(new wxBoxSizer(wxVERTICAL));
    m_view = new wxDataViewListCtrl(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_NO_HEADER | wxDV_SINGLE | wxDV_ROW_LINES);
    ::AdjustDataViewAlternateColour(m_view);
    GetSizer()->Add(m_view, wxSizerFlags(1).Expand());

    // Both columns hold a pointer to the data of the line, the renderers draw it
    auto on_activate = [this](const wxDataViewItem& item) { OnActivateItem(item); };
    m_view->AppendColumn(new wxDataViewColumn(_("Property Name"),
                                              new PropertyRenderer(false, nullptr),
                                              0,
                                              FromDIP(200),
                                              wxALIGN_LEFT,
                                              wxDATAVIEW_COL_RESIZABLE),
                         "void*");
    m_view->AppendColumn(new wxDataViewColumn(_("Property Value"),
                                              new PropertyRenderer(true, on_activate),
                                              1,
                                              FromDIP(200),
                                              wxALIGN_LEFT,
                                              wxDATAVIEW_COL_RESIZABLE),
                         "void*");

    // Enter, or a double click
    m_view->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &clPropertiesPage::OnItemActivated, this);
    GetSizer()->Layout();
}

clPropertiesPage::~clPropertiesPage()
{
    m_view->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &clPropertiesPage::OnItemActivated, this);
    m_view->DeleteAllItems();
}

size_t clPropertiesPage::AppendRow(const wxString& label, std::unique_ptr<clPropertiesCell> cell, bool is_header)
{
    cell->label = label;
    cell->is_header = is_header;

    // The columns are not resized here: Finalize() does it, after all the rows are added
    wxVector<wxVariant> cols;
    cols.push_back(wxVariant(static_cast<void*>(cell.get())));
    cols.push_back(wxVariant(static_cast<void*>(cell.get())));
    m_cells.push_back(std::move(cell));
    m_view->AppendItem(cols);
    return m_cells.size() - 1;
}

void clPropertiesPage::SetCellText(size_t line, const wxString& text)
{
    if (line >= m_cells.size()) {
        return;
    }
    m_cells[line]->text = text;
    m_view->GetStore()->RowValueChanged(static_cast<unsigned int>(line), 1);
    // the user changed the value: the new one may be wider
    AutoSizeColumns(m_view);
}

void clPropertiesPage::AddProperty(const wxString& label,
                                   const wxArrayString& choices,
                                   const wxString& string_selection,
                                   clPropertiesPage::Callback_t update_cb)
{
    size_t index = choices.Index(string_selection);
    if (index == wxString::npos) {
        index = 0;
    }
    AddProperty(label, choices, static_cast<int>(index), std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label,
                                   const std::vector<wxString>& choices,
                                   size_t sel,
                                   clPropertiesPage::Callback_t update_cb)
{
    wxArrayString arr;
    arr.reserve(choices.size());
    for (const auto& s : choices) {
        arr.Add(s);
    }
    AddProperty(label, arr, sel, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label,
                                   const std::vector<wxString>& choices,
                                   const wxString& selection,
                                   clPropertiesPage::Callback_t update_cb)
{
    wxArrayString arr;
    arr.reserve(choices.size());
    for (const auto& s : choices) {
        arr.Add(s);
    }
    AddProperty(label, arr, selection, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label,
                                   const wxArrayString& choices,
                                   size_t sel,
                                   clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::CHOICE;
    cell->text = sel < choices.size() ? choices[sel] : wxString();
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::CHOICE, choices, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, bool checked, clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::CHECKBOX;
    cell->checked = checked;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::CHECKBOX, checked, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, long value, clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::INTEGER;
    cell->text << value;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::INTEGER, value, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const wxString& value, clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::TEXT_EDIT;
    cell->text = value;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::TEXT_EDIT, value, std::move(update_cb));
}

void clPropertiesPage::AddProperty(const wxString& label, const wxColour& value, clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::COLOUR;
    cell->colour = value;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::COLOUR, value, std::move(update_cb));
}

void clPropertiesPage::AddPropertyFilePicker(const wxString& label,
                                             const wxString& path,
                                             clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::FILE_PICKER;
    cell->text = path;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::FILE_PICKER, path, std::move(update_cb));
}

void clPropertiesPage::AddPropertyDirPicker(const wxString& label,
                                            const wxString& path,
                                            clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::DIR_PICKER;
    cell->text = path;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::DIR_PICKER, path, std::move(update_cb));
}

void clPropertiesPage::AddPropertyLanguagePicker(const wxString& label,
                                                 const wxArrayString& langs,
                                                 clPropertiesPage::Callback_t update_cb)
{
    wxString languages_str = wxJoin(langs, ';');
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::LANGUAGE_PICKER;
    cell->text = languages_str;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::LANGUAGE_PICKER, languages_str, std::move(update_cb));
}

void clPropertiesPage::AddPropertyButton(const wxString& label,
                                         const wxString& button_label,
                                         clPropertiesPage::Callback_t update_cb)
{
    auto cell = std::make_unique<clPropertiesCell>();
    cell->kind = LineKind::BUTTON;
    cell->text = button_label;
    AppendRow(label, std::move(cell));
    UpdateLastLineData(LineKind::BUTTON, nullptr, std::move(update_cb));
}

void clPropertiesPage::AddHeader(const wxString& label)
{
    // A header has no line data, so it is never activated
    AppendRow(label, std::make_unique<clPropertiesCell>(), true);
}

void clPropertiesPage::OnItemActivated(wxDataViewEvent& event) { OnActivateItem(event.GetItem()); }

void clPropertiesPage::OnActivateItem(const wxDataViewItem& item)
{
    // A double click activates the item twice (and so does a click followed by Enter): handle one activation at a time
    if (m_activating) {
        return;
    }

    const int row = m_view->ItemToRow(item);
    if (row == wxNOT_FOUND) {
        return;
    }

    // The dialogs and the menu are shown after the data view is done with the mouse event
    m_activating = true;
    CallAfter([this, row]() {
        ActivateRow(static_cast<size_t>(row));
        m_activating = false;
    });
}

void clPropertiesPage::ActivateRow(size_t row)
{
    const LineData* data = nullptr;
    if (row >= m_cells.size() || !GetLineData(row, &data)) {
        return;
    }

    clPropertiesPageEvent event{wxEVT_PROPERTIES_PAGE_ACTION_BUTTON};
    event.SetItem(m_view->RowToItem(static_cast<int>(row)));
    event.SetEventObject(this);
    event.SetPage(this);
    event.SetLineData(data);
    event.SetLabel(m_cells[row]->label);
    if (GetEventHandler()->ProcessEvent(event)) {
        // Handled by the caller.
        return;
    }

    // Do default behavior
    switch (data->line_kind) {
    case LineKind::TEXT_EDIT: {
        wxString value_str;
        if (!data->value.GetAs(&value_str))
            return;
        ShowTextEditor(row, value_str);
        break;
    }
    case LineKind::COLOUR: {
        wxColour colour_value;
        if (!data->value.GetAs(&colour_value))
            return;
        ShowColourPicker(row, colour_value);
        break;
    }
    case LineKind::CHOICE: {
        wxArrayString arr_value;
        if (!data->value.GetAs(&arr_value))
            return;
        ShowStringSelectionMenu(row, arr_value);
        break;
    }
    case LineKind::FILE_PICKER: {
        wxString path;
        if (!data->value.GetAs(&path))
            return;
        ShowFilePicker(row, path);
        break;
    }
    case LineKind::LANGUAGE_PICKER: {
        wxString curlangs;
        if (!data->value.GetAs(&curlangs))
            return;
        ShowLanguagePicker(row, curlangs);
        break;
    }
    case LineKind::DIR_PICKER: {
        wxString path;
        if (!data->value.GetAs(&path))
            return;
        ShowDirPicker(row, path);
        break;
    }
    case LineKind::INTEGER: {
        long value;
        if (!data->value.GetAs(&value))
            return;
        ShowNumberPicker(row, value);
        break;
    }
    case LineKind::BUTTON: {
        DoButtonClicked(row);
        break;
    }
    case LineKind::CHECKBOX: {
        // toggle the value
        clPropertiesCell& cell = *m_cells[row];
        cell.checked = !cell.checked;
        m_view->GetStore()->RowValueChanged(static_cast<unsigned int>(row), 1);
        UpdateLineData(row, LineKind::CHECKBOX, cell.checked, nullptr);
        // call the user callback for this change
        NotifyChange(row);
        SetModified();
        break;
    }
    case LineKind::UNKNOWN:
    default:
        break;
    }
}

void clPropertiesPage::ShowColourPicker(size_t line, const wxColour& colour)
{
    wxColour c = ::wxGetColourFromUser(wxGetTopLevelParent(this), colour);
    if (!c.IsOk()) {
        return;
    }

    m_cells[line]->colour = c;
    m_view->GetStore()->RowValueChanged(static_cast<unsigned int>(line), 1);
    UpdateLineData(line, LineKind::COLOUR, c, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowTextEditor(size_t line, const wxString& text)
{
    // Guess the language by checking the input text.
    FileExtManager::FileType ft = FileExtManager::FileType::TypeText;
    FileExtManager::GetContentType(text, ft);
    auto lexer = ColoursAndFontsManager::Get().GetLexerForFileType(ft);

    // use EditDlg directly and not clGetTextFromUser() method
    // since empty string is a valid value
    EditDlg dlg(wxGetTopLevelParent(this), text, lexer.value_or(nullptr));
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    wxString new_text = dlg.GetText();

    // update the cell value
    SetCellText(line, new_text);
    UpdateLineData(line, LineKind::TEXT_EDIT, new_text, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowStringSelectionMenu(size_t line, const wxArrayString& options)
{
    const wxString current = m_cells[line]->text;

    wxMenu menu;
    for (size_t i = 0; i < options.size(); ++i) {
        wxString label = options[i];
        label.Replace("&", "&&"); // do not turn a '&' into a mnemonic
        wxMenuItem* item = menu.AppendRadioItem(kFirstChoiceId + static_cast<int>(i), label);
        if (options[i] == current) {
            item->Check();
        }
    }

    // show the menu below the value, or at the mouse position
    wxPoint pos = wxDefaultPosition;
    const wxRect cell_rect = m_view->GetItemRect(m_view->RowToItem(static_cast<int>(line)), m_view->GetColumn(1));
    if (!cell_rect.IsEmpty()) {
        pos = wxPoint(cell_rect.x, cell_rect.y + cell_rect.height);
    }

    const int selection = m_view->GetPopupMenuSelectionFromUser(menu, pos);
    if (selection == wxID_NONE) {
        return;
    }
    const size_t index = static_cast<size_t>(selection - kFirstChoiceId);
    if (index >= options.size() || options[index] == current) {
        return;
    }

    // update the view, and call the user callback with the new selection
    const wxString new_selection = options[index];
    SetCellText(line, new_selection);

    const LineData* line_data = nullptr;
    if (GetLineData(line, &line_data) && line_data->callback) {
        line_data->callback(m_cells[line]->label, wxAny(new_selection));
    }
    SetModified();
}

void clPropertiesPage::ShowFilePicker(size_t line, const wxString& path)
{
    wxString default_path = wxEmptyString;
    wxString default_name = wxEmptyString;
    wxString default_ext = wxEmptyString;
    if (!path.empty() && wxFileName(path).IsOk()) {
        default_path = wxFileName(path).GetPath();
        default_name = wxFileName(path).GetFullName();
        default_ext = wxFileName(path).GetExt();
    }

    wxString new_path = wxFileSelector(
        _("Choose a file"), default_path, default_name, default_ext, wxEmptyString, 0, wxGetTopLevelParent(this));
    if (new_path.empty()) {
        return;
    }

    // update the view
    SetCellText(line, new_path);
    UpdateLineData(line, LineKind::FILE_PICKER, new_path, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowDirPicker(size_t line, const wxString& path)
{
    wxString new_path = wxDirSelector(_("Choose a directory"), path, 0, wxDefaultPosition, wxGetTopLevelParent(this));
    if (new_path.empty()) {
        return;
    }

    // update the view
    SetCellText(line, new_path);
    UpdateLineData(line, LineKind::DIR_PICKER, new_path, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::ShowLanguagePicker(size_t line, const wxString& langs)
{
    wxArrayString selected;
    if (!::clShowFileTypeSelectionDialog(
            wxGetTopLevelParent(this), wxStringTokenize(langs, ";", wxTOKEN_STRTOK), &selected)) {
        return;
    }

    // update the view
    wxString new_label = wxJoin(selected, ';');
    SetCellText(line, new_label);
    UpdateLineData(line, LineKind::LANGUAGE_PICKER, new_label, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

void clPropertiesPage::DoButtonClicked(size_t line)
{
    // call the user provided callback
    NotifyChange(line);
}

void clPropertiesPage::ShowNumberPicker(size_t line, long number)
{
    wxNumberEntryDialog dlg(
        wxGetTopLevelParent(this), m_cells[line]->label, wxEmptyString, _("Choose a number"), number, -10000, 10000);
    if (dlg.ShowModal() != wxID_OK)
        return;

    int new_number = dlg.GetValue();
    SetCellText(line, wxString() << new_number);
    UpdateLineData(line, LineKind::INTEGER, new_number, nullptr);
    // call the user callback for this change
    NotifyChange(line);
    SetModified();
}

bool clPropertiesPage::GetLineData(size_t line, const LineData** data) const
{
    if (!data) {
        return false;
    }
    auto iter = m_lines.find(line);
    if (iter == m_lines.end()) {
        return false;
    }

    *data = &(iter->second);
    return true;
}

void clPropertiesPage::SetModified()
{
    m_isModified = true;
    clPropertiesPageEvent modified_event(wxEVT_PROPERTIES_PAGE_MODIFIED);
    modified_event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(modified_event);
}

void clPropertiesPage::ClearModified()
{
    m_isModified = false;
    clPropertiesPageEvent modified_event(wxEVT_PROPERTIES_PAGE_SAVED);
    modified_event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(modified_event);
}

void clPropertiesPage::NotifyChange(size_t line)
{
    const LineData* line_data = nullptr;
    if (line >= m_cells.size() || !GetLineData(line, &line_data))
        return;

    if (line_data->callback) {
        line_data->callback(m_cells[line]->label, line_data->value);
    }
}

void clPropertiesPage::Finalize()
{
    if (m_frozen) {
        m_view->Thaw();
        m_frozen = false;
    }
    // All the rows are added: fit the columns to their content
    AutoSizeColumns(m_view);
}

void clPropertiesPage::Begin()
{
    if (!m_frozen) {
        m_view->Freeze();
        m_frozen = true;
    }
}

void clPropertiesPage::Clear()
{
    CHECK_PTR_RET(m_view);
    m_view->DeleteAllItems();
    m_cells.clear();
    m_lines.clear();
    m_isModified = false;
}
