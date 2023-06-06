#include "clComboBoxGeneric.hpp"

#include "clThemedTextCtrl.hpp"

#include <wx/button.h>
#include <wx/settings.h>
#include <wx/sizer.h>

namespace
{
class clComboBoxROLocker
{
    clComboBoxGeneric* m_cb = nullptr;
    bool m_isReadOnly = true;

public:
    clComboBoxROLocker(clComboBoxGeneric* cb)
        : m_cb(cb)
        , m_isReadOnly(m_cb && !m_cb->GetTextCtrl()->IsEditable())
    {
        if(m_isReadOnly) {
            m_cb->GetTextCtrl()->SetEditable(true);
        }
    }

    ~clComboBoxROLocker()
    {
        if(!m_isReadOnly) {
            return;
        }
        m_cb->GetTextCtrl()->SetEditable(false);
    }
};
} // namespace

clComboBoxGeneric::clComboBoxGeneric(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                                     const wxSize& size, size_t n, const wxString choices[], long style,
                                     const wxValidator& validator, const wxString& name)
    : wxControl(parent, id, pos, size, wxNO_BORDER)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    m_choices.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        m_choices.Add(choices[i]);
    }
    DoCreate(value);
}

clComboBoxGeneric::clComboBoxGeneric(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                                     const wxSize& size, const wxArrayString& choices, long style,
                                     const wxValidator& validator, const wxString& name)
    : wxControl(parent, id, pos, size, wxNO_BORDER)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    m_choices.reserve(choices.size());
    m_choices = choices;
    DoCreate(value);
}

clComboBoxGeneric::clComboBoxGeneric() {}

clComboBoxGeneric::~clComboBoxGeneric() {}

bool clComboBoxGeneric::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                               const wxSize& size, size_t n, const wxString choices[], long style,
                               const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    bool res = wxControl::Create(parent, id, pos, size, wxNO_BORDER);
    m_choices.reserve(n);
    for(size_t i = 0; i < n; ++i) {
        m_choices.push_back(choices[i]);
    }
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    DoCreate(value);
    return res;
}

bool clComboBoxGeneric::Create(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos,
                               const wxSize& size, const wxArrayString& choices, long style,
                               const wxValidator& validator, const wxString& name)
{
    wxUnusedVar(validator);
    wxUnusedVar(name);
    bool res = wxControl::Create(parent, id, pos, size, wxNO_BORDER);
    m_cbStyle = style & ~wxWINDOW_STYLE_MASK;
    m_choices = choices;
    DoCreate(value);
    return res;
}

void clComboBoxGeneric::DoCreate(const wxString& value)
{
    SetSizer(new wxBoxSizer(wxHORIZONTAL));
    m_textCtrl = new clThemedTextCtrl(this, wxID_ANY, value);
    GetSizer()->Add(m_textCtrl, 1, wxEXPAND | wxALL, 1);

    const wxString arrowSymbol = wxT(" \u25BC ");
    m_button = new wxButton(this, wxID_ANY, arrowSymbol, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);

    wxColour text_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT);
    m_button->SetForegroundColour(DrawingUtils::IsDark(text_colour) ? text_colour.ChangeLightness(120)
                                                                    : text_colour.ChangeLightness(80));
    m_button->Bind(wxEVT_BUTTON, &clComboBoxGeneric::OnButtonClicked, this);
    m_textCtrl->Bind(wxEVT_TEXT, &clComboBoxGeneric::OnText, this);
    m_textCtrl->Bind(wxEVT_CHAR_HOOK, &clComboBoxGeneric::OnCharHook, this);
    Bind(wxEVT_SET_FOCUS, &clComboBoxGeneric::OnFocus, this);

    if(m_cbStyle & wxCB_READONLY) {
        m_textCtrl->SetEditable(false);
    }
    GetSizer()->Add(m_button, 0, wxALIGN_CENTER_VERTICAL);
    GetSizer()->Fit(this);
    wxRect textRect = m_textCtrl->GetSize();
    textRect.Inflate(1);
    m_button->SetSizeHints(textRect.GetHeight(), textRect.GetHeight());
}

namespace
{
void ButtonShowMenu(wxButton* button, wxMenu& menu, wxPoint* point)
{
    wxPoint menuPos;
    if(point) {
        menuPos = *point;
    } else {
        menuPos = button->GetClientRect().GetBottomLeft();
#ifdef __WXOSX__
        menuPos.y += 5;
#endif
    }
    button->PopupMenu(&menu, menuPos);
}
} // namespace

void clComboBoxGeneric::OnButtonClicked(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxMenu menu;
    for(size_t i = 0; i < m_choices.size(); ++i) {
        const wxString& choice = m_choices.Item(i);
        auto item = menu.Append(wxID_ANY, choice, "", wxITEM_CHECK);
        item->Check(i == (size_t)m_selection);
        menu.Bind(
            wxEVT_MENU,
            [this, choice, i](wxCommandEvent& e) {
                wxUnusedVar(e);

                // update the selected text
                clComboBoxROLocker locker(this);
                m_textCtrl->ChangeValue(choice);
                m_selection = i;

                // Notify about selection change
                wxCommandEvent selectionChanged(wxEVT_COMBOBOX);
                selectionChanged.SetEventObject(this);
                selectionChanged.SetInt(i); // the new selection
                GetEventHandler()->AddPendingEvent(selectionChanged);
            },
            item->GetId());
    }

    if(!m_custom_commands.IsEmpty()) {
        if(!m_choices.empty()) {
            menu.AppendSeparator();
        }
        for(auto item : m_custom_commands) {
            menu.Append(item.first, item.second);
            menu.Bind(
                wxEVT_MENU, [this](wxCommandEvent& e) { GetEventHandler()->ProcessEvent(e); }, item.first);
        }
    }

    ButtonShowMenu(m_button, menu, nullptr);
    m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);
}

void clComboBoxGeneric::SetHint(const wxString& hint) { wxUnusedVar(hint); }

void clComboBoxGeneric::SetSelection(size_t sel)
{
    if(sel == INVALID_SIZE_T) {
        m_textCtrl->Clear();
        m_selection = INVALID_SIZE_T;
    } else {
        if(sel >= m_choices.GetCount()) {
            return;
        }
        m_selection = sel;
        // else, selection is a valid index
        SetValue(m_choices.Item(m_selection));
    }
}

void clComboBoxGeneric::OnText(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxCommandEvent textEvent(wxEVT_TEXT);
    textEvent.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(textEvent);
    m_selection = m_choices.Index(m_textCtrl->GetValue());
}

void clComboBoxGeneric::DoTextEnter()
{
    wxCommandEvent textEvent(wxEVT_TEXT_ENTER);
    textEvent.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(textEvent);
}

void clComboBoxGeneric::SetString(size_t n, const wxString& text)
{
    if(n >= m_choices.size()) {
        return;
    }
    m_choices[n] = text;
    if(GetSelection() == (size_t)n) {
        SetValue(m_choices[n]);
    }
}

void clComboBoxGeneric::SetValue(const wxString& text)
{
    clComboBoxROLocker locker(this);
    m_textCtrl->SetValue(text);
    SetStringSelection(text);
}

wxString clComboBoxGeneric::GetStringSelection() const { return m_textCtrl->GetValue(); }

void clComboBoxGeneric::SetStringSelection(const wxString& text)
{
    clComboBoxROLocker locker(this);
    for(size_t i = 0; i < m_choices.size(); ++i) {
        if(m_choices.Item(i).CmpNoCase(text) == 0) {
            m_textCtrl->ChangeValue(m_choices.Item(i));
            m_selection = i;
            break;
        }
    }
}

void clComboBoxGeneric::SetFocus() { m_textCtrl->SetFocus(); }

void clComboBoxGeneric::Append(const std::vector<wxString>& strings)
{
    if(strings.empty()) {
        return;
    }
    m_choices.reserve(strings.size() + m_choices.size());
    for(const auto& str : strings) {
        m_choices.Add(str);
    }
}

void clComboBoxGeneric::Append(const wxArrayString& strings)
{
    if(strings.empty()) {
        return;
    }
    m_choices.reserve(strings.size() + m_choices.size());
    m_choices.insert(m_choices.end(), strings.begin(), strings.end());
}

size_t clComboBoxGeneric::Append(const wxString& text)
{
    m_choices.Add(text);
    return m_choices.size() - 1;
}

void clComboBoxGeneric::Clear()
{
    m_choices.Clear();
    m_selection = INVALID_SIZE_T;
    clComboBoxROLocker locker(this);
    m_textCtrl->ChangeValue(wxEmptyString);
}

void clComboBoxGeneric::Delete(size_t index)
{
    if(index >= m_choices.size()) {
        return;
    }
    if(index <= m_selection) {
        // the removed item is _before_ the selected item
        // invalidate the selection
        m_selection = INVALID_SIZE_T;
        clComboBoxROLocker locker(this);
        m_textCtrl->ChangeValue(wxEmptyString);
    }
    m_choices.RemoveAt(index);
}

size_t clComboBoxGeneric::FindString(const wxString& s, bool bCase) const { return m_choices.Index(s, bCase); }

wxString clComboBoxGeneric::GetValue() const { return m_textCtrl->GetValue(); }

wxArrayString clComboBoxGeneric::GetStrings() const
{
    wxArrayString strings;
    strings.reserve(m_choices.size() + 1);
    if(GetSelection() == INVALID_SIZE_T && !GetValue().empty()) {
        strings.Add(GetValue());
    }
    strings.insert(strings.end(), m_choices.begin(), m_choices.end());
    return strings;
}

void clComboBoxGeneric::OnCharHook(wxKeyEvent& event)
{
    if(event.GetKeyCode() == WXK_TAB) {
        Navigate(event.ShiftDown() ? wxNavigationKeyEvent::IsBackward : wxNavigationKeyEvent::IsForward);
    } else if(event.GetKeyCode() == WXK_NUMPAD_ENTER || event.GetKeyCode() == WXK_RETURN) {
        DoTextEnter();
    } else {
        event.Skip();
    }
}

void clComboBoxGeneric::OnFocus(wxFocusEvent& event)
{
    event.Skip();
    m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);
}

bool clComboBoxGeneric::IsTextEmpty() const { return m_textCtrl->IsEmpty(); }

void clComboBoxGeneric::AddCommand(int command_id, const wxString& label)
{
    if(m_custom_commands.Contains(command_id)) {
        m_custom_commands.Remove(command_id);
    }
    m_custom_commands.PushBack(command_id, label);
}

void clComboBoxGeneric::SelectAll() { GetTextCtrl()->SelectAll(); }
