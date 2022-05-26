#ifndef CLCOMBOBOX_HPP
#define CLCOMBOBOX_HPP

#include "clButton.h"
#include "codelite_exports.h"
#include "wx_ordered_map.h"

#include <functional>
#include <unordered_map>
#include <wx/button.h>
#include <wx/combobox.h>
#include <wx/control.h>

#define INVALID_SIZE_T static_cast<size_t>(wxNOT_FOUND)

class clThemedTextCtrl;

class WXDLLIMPEXP_SDK clComboBox : public wxControl
{
    wxArrayString m_choices;
    clThemedTextCtrl* m_textCtrl = nullptr;
    wxButton* m_button = nullptr;
    size_t m_selection = INVALID_SIZE_T;
    size_t m_cbStyle = 0;
    wxOrderedMap<int, wxString> m_custom_commands;

protected:
    void DoCreate(const wxString& value);
    void DoTextEnter();

protected:
    void OnButtonClicked(wxCommandEvent& event);
    void OnText(wxCommandEvent& event);
    void OnCharHook(wxKeyEvent& event);
    void OnFocus(wxFocusEvent& event);

public:
    clComboBox();
    clComboBox(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, size_t n = 0,
               const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);
    clComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
               const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, size_t n = 0,
                const wxString choices[] = NULL, long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    bool Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size,
                const wxArrayString& choices, long style = 0, const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);
    virtual ~clComboBox();

    /**
     * @brief set the focus to the text entry
     */
    void SetFocus();

    /**
     * @brief set text hint
     */
    void SetHint(const wxString& hint);

    /**
     * @brief add custom command to be added to the drop down menu
     */
    void AddCommand(int command_id, const wxString& label);

    /**
     * @brief select item from the list of choices
     */
    void SetSelection(size_t sel);

    /**
     * @brief same as SetSelection
     */
    void Select(size_t sel) { SetSelection(sel); }

    /**
     * @brief Returns true if the list of combobox choices is empty.
     */
    bool IsListEmpty() const { return m_choices.IsEmpty(); }

    /**
     * @brief Returns true if the text of the combobox is empty.
     */
    bool IsTextEmpty() const;

    /**
     * @brief Returns the index of the selected item or wxNOT_FOUND if no item is selected.
     */
    size_t GetSelection() const { return m_selection; }

    /**
     * @brief Changes the text of the specified combobox item.
     * Notice that if the item is the currently selected one, i.e. if its text is displayed in the text part of the
     * combobox, then the text is also replaced with the new text
     */
    void SetString(size_t n, const wxString& text);

    /**
     * @brief Sets the text for the combobox text field.
     * For normal, editable comboboxes with a text entry field calling this method will generate a wxEVT_TEXT event
     */
    void SetValue(const wxString& text);

    /**
     * @brief Gets the text currently selected in the control.
     */
    wxString GetStringSelection() const;

    /**
     * @brief Selects the item with the specified string in the control.
     * This method doesn't cause any command events to be emitted.
     * Notice that this method is case-insensitive, i.e. the string is compared with all the elements of the control
     * case-insensitively and the first matching entry is selected, even if it doesn't have exactly the same case as
     * this string and there is an exact match afterwards.
     */
    void SetStringSelection(const wxString& text);

    void Append(const std::vector<wxString>& strings);
    void Append(const wxArrayString& strings);
    size_t Append(const wxString& text);
    /**
     * @brief Removes all items from the control.
     */
    void Clear();
    /**
     * @brief Deletes an item from the control.
     * The client data associated with the item will be also deleted if it is owned by the control. Note that it is an
     * error (signalled by an assert failure in debug builds) to remove an item with the index negative or greater or
     * equal than the number of items in the control.
     *
     * If there is a currently selected item below the item being deleted, i.e. if GetSelection() returns a valid index
     * greater than or equal to n, the selection is invalidated when this function is called. However if the selected
     * item appears before the item being deleted, the selection is preserved unchanged.
     */
    void Delete(size_t index);

    /**
     * @brief Returns the number of items in the control
     */
    size_t GetCount() const { return m_choices.size(); }
    /**
     * @brief Finds an item whose label matches the given string.
     */
    size_t FindString(const wxString& s, bool bCase = false) const;
    /**
     * @brief Gets the contents of the control.
     */
    wxString GetValue() const;
    /**
     * @brief return the combo box strings
     */
    wxArrayString GetStrings() const;
    wxButton* GetButton() const { return m_button; }
    clThemedTextCtrl* GetTextCtrl() const { return m_textCtrl; }
};

#endif // CLCOMBOBOX_HPP
