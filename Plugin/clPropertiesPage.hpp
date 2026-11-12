#pragma once

#include "cl_command_event.h"
#include "codelite_exports.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <wx/any.h>
#include <wx/arrstr.h>
#include <wx/colour.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <wx/window.h>

enum class LineKind {
    UNKNOWN,
    COLOUR,
    CHOICE,
    TEXT_EDIT,
    CHECKBOX,
    FILE_PICKER,
    DIR_PICKER,
    INTEGER,
    BUTTON,
    LANGUAGE_PICKER,
};

struct WXDLLIMPEXP_SDK LineData {
    LineKind line_kind = LineKind::UNKNOWN;
    wxAny value;
    std::function<void(const wxString&, const wxAny&)> callback = nullptr;
};

struct clPropertiesCell; // the data of a row (private to clPropertiesPage.cpp)

/// A page of "name | value" properties, shown in a native wxDataViewListCtrl. A value is activated by a click (or by
/// pressing Enter on its row): a checkbox is toggled, any other value is changed with a menu or a dialog
class WXDLLIMPEXP_SDK clPropertiesPage : public wxPanel
{
public:
    using Callback_t = std::function<void(const wxString&, const wxAny&)>;
    clPropertiesPage(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clPropertiesPage();

    void AddProperty(const wxString& label,
                     const wxArrayString& choices,
                     size_t sel,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label,
                     const std::vector<wxString>& choices,
                     size_t sel,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label,
                     const std::vector<wxString>& choices,
                     const wxString& selection,
                     clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label,
                     const wxArrayString& choices,
                     const wxString& selection,
                     clPropertiesPage::Callback_t update_cb);

    void AddProperty(const wxString& label, bool checked, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const wxString& value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, const char* value, clPropertiesPage::Callback_t update_cb)
    {
        AddProperty(label, wxString(value), std::move(update_cb));
    }
    void AddProperty(const wxString& label, const wxColour& value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, long value, clPropertiesPage::Callback_t update_cb);
    void AddProperty(const wxString& label, int value, clPropertiesPage::Callback_t update_cb)
    {
        AddProperty(label, (long)value, std::move(update_cb));
    }

    void AddPropertyFilePicker(const wxString& label, const wxString& path, clPropertiesPage::Callback_t update_cb);
    void AddPropertyDirPicker(const wxString& label, const wxString& path, clPropertiesPage::Callback_t update_cb);
    void AddPropertyLanguagePicker(const wxString& label,
                                   const wxArrayString& langs,
                                   clPropertiesPage::Callback_t update_cb);
    void AddPropertyButton(const wxString& label, const wxString& button_label, clPropertiesPage::Callback_t update_cb);
    void AddHeader(const wxString& label);

    bool IsModified() const { return m_isModified; }
    void ClearModified();

    /**
     * @brief call this method to tell the control what we are done adding items
     */
    void Finalize();
    /**
     * @brief tell the control that we are about to add multiple items, so avoid un-needed drawings
     * once done, call `Finalize`
     */
    void Begin();

    /**
     * @brief clear the view and discard all caches
     */
    virtual void Clear();

    void NotifyChange(size_t line);

protected:
    void OnItemActivated(wxDataViewEvent& event);

    /// Called by the renderers when the user activates (clicks) an item
    void OnActivateItem(const wxDataViewItem& item);
    /// Do the action of a row: toggle it, or show its menu or dialog
    void ActivateRow(size_t line);

    // Helpers
    void ShowColourPicker(size_t line, const wxColour& colour);
    void ShowLanguagePicker(size_t line, const wxString& langs);
    void ShowTextEditor(size_t line, const wxString& text);
    void ShowStringSelectionMenu(size_t line, const wxArrayString& options);
    void ShowFilePicker(size_t line, const wxString& path);
    void ShowDirPicker(size_t line, const wxString& path);
    void ShowNumberPicker(size_t line, long number);
    void DoButtonClicked(size_t line);
    void SetModified();

    bool GetLineData(size_t line, const LineData** data) const;

    template <typename T>
    void UpdateLineData(size_t line, LineKind kind, const T& data, clPropertiesPage::Callback_t update_cb)
    {
        if (m_lines.count(line) == 0) {
            m_lines.insert({line, {}});
        }

        // update the values
        LineData& d = m_lines[line];
        d.value = data;
        d.line_kind = kind;
        if (update_cb != nullptr) {
            d.callback = std::move(update_cb);
        }
    }

    template <typename T>
    void UpdateLastLineData(LineKind kind, const T& data, clPropertiesPage::Callback_t update_cb)
    {
        if (m_cells.empty()) {
            return;
        }

        size_t line = m_cells.size() - 1;
        UpdateLineData(line, kind, data, update_cb);
    }

private:
    /// Add a row to the view. Returns its line number
    size_t AppendRow(const wxString& label, std::unique_ptr<clPropertiesCell> cell, bool is_header = false);
    /// Change the displayed text of the value of a line and repaint it
    void SetCellText(size_t line, const wxString& text);

    wxDataViewListCtrl* m_view{nullptr};
    std::vector<std::unique_ptr<clPropertiesCell>> m_cells; // one for each line
    std::unordered_map<size_t, LineData> m_lines;
    bool m_isModified{false};
    bool m_activating{false}; // a dialog or a menu of a line is pending or shown
    bool m_frozen{false};
};

/// Source control event
class WXDLLIMPEXP_SDK clPropertiesPageEvent : public clCommandEvent
{
public:
    clPropertiesPageEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : clCommandEvent(commandType, winid)
    {
    }
    clPropertiesPageEvent(const clPropertiesPageEvent& event) = default;
    clPropertiesPageEvent& operator=(const clPropertiesPageEvent&) = delete;
    ~clPropertiesPageEvent() override = default;
    wxEvent* Clone() const override { return new clPropertiesPageEvent(*this); }

    const LineData* GetLineKind() const { return m_lineData; }
    void SetLineData(const LineData* kind) { m_lineData = kind; }

    void SetPage(clPropertiesPage* page) { this->m_page = page; }
    clPropertiesPage* GetPage() { return m_page; }

    void SetItem(const wxDataViewItem& item) { this->m_item = item; }
    const wxDataViewItem& GetItem() const { return m_item; }

    void SetLabel(const wxString& label) { this->m_label = label; }
    const wxString& GetLabel() const { return m_label; }

private:
    const LineData* m_lineData{nullptr};
    clPropertiesPage* m_page{nullptr};
    wxDataViewItem m_item{nullptr};
    wxString m_label;
};

using clPropertiesPageEventFunction = void (wxEvtHandler::*)(clPropertiesPageEvent&);
#define clPropertiesPageEventHandler(func) wxEVENT_HANDLER_CAST(clPropertiesPageEventFunction, func)

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_MODIFIED, clPropertiesPageEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_SAVED, clPropertiesPageEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_SDK, wxEVT_PROPERTIES_PAGE_ACTION_BUTTON, clPropertiesPageEvent);
