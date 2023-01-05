#include "clKeyboardManager.h"

#include "clKeyboardBindingConfig.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "macros.h"
#include "newkeyshortcutdlg.h"
#include "wxCustomControls.hpp"

#include <algorithm>
#include <wx/app.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/tokenzr.h>
#include <wx/xrc/xmlres.h>

wxDEFINE_EVENT(wxEVT_KEYBOARD_ACCEL_INIT_DONE, clCommandEvent);

clKeyboardManager::clKeyboardManager()
{
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clKeyboardManager::OnStartupCompleted, this);

    // A-Z
    for(size_t i = 65; i < 91; ++i) {
        char asciiCode = (char)i;
        m_keyCodes.insert(wxString() << asciiCode);
    }

    // 0-9
    for(size_t i = 48; i < 58; ++i) {
        char asciiCode = (char)i;
        m_keyCodes.insert(wxString() << asciiCode);
    }

    // Special chars
    m_keyCodes.insert("`");
    m_keyCodes.insert("-");
    m_keyCodes.insert("*");
    m_keyCodes.insert("=");
    m_keyCodes.insert("BACK");
    m_keyCodes.insert("TAB");
    m_keyCodes.insert("[");
    m_keyCodes.insert("]");
    m_keyCodes.insert("ENTER");
    m_keyCodes.insert("CAPITAL");
    m_keyCodes.insert("SCROLL_LOCK");
    m_keyCodes.insert("PAUSE");
    m_keyCodes.insert(";");
    m_keyCodes.insert("'");
    m_keyCodes.insert("\\");
    m_keyCodes.insert(",");
    m_keyCodes.insert(".");
    m_keyCodes.insert("/");
    m_keyCodes.insert("SPACE");
    m_keyCodes.insert("INS");
    m_keyCodes.insert("HOME");
    m_keyCodes.insert("PGUP");
    m_keyCodes.insert("PGDN");
    m_keyCodes.insert("DEL");
    m_keyCodes.insert("END");
    m_keyCodes.insert("UP");
    m_keyCodes.insert("DOWN");
    m_keyCodes.insert("RIGHT");
    m_keyCodes.insert("LEFT");
    m_keyCodes.insert("F1");
    m_keyCodes.insert("F2");
    m_keyCodes.insert("F3");
    m_keyCodes.insert("F4");
    m_keyCodes.insert("F5");
    m_keyCodes.insert("F6");
    m_keyCodes.insert("F7");
    m_keyCodes.insert("F8");
    m_keyCodes.insert("F9");
    m_keyCodes.insert("F10");
    m_keyCodes.insert("F11");
    m_keyCodes.insert("F12");

    // There can be the following options:
    // Ctrl-Alt-Shift
    // Ctrl-Alt
    // Ctrl-Shift
    // Ctrl
    // Alt-Shift
    // Alt
    // Shift
    for(const wxString& keyCode : m_keyCodes) {
        m_allShortcuts.insert("Ctrl-Alt-Shift-" + keyCode);
        m_allShortcuts.insert("Ctrl-Alt-" + keyCode);
        m_allShortcuts.insert("Ctrl-Shift-" + keyCode);
        m_allShortcuts.insert("Ctrl-" + keyCode);
        m_allShortcuts.insert("Alt-Shift-" + keyCode);
        m_allShortcuts.insert("Alt-" + keyCode);
        m_allShortcuts.insert("Shift-" + keyCode);
    }
}

clKeyboardManager::~clKeyboardManager()
{
    Save();
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clKeyboardManager::OnStartupCompleted, this);
}

static clKeyboardManager* m_mgr = NULL;
clKeyboardManager* clKeyboardManager::Get()
{
    if(NULL == m_mgr) {
        m_mgr = new clKeyboardManager();
    }
    return m_mgr;
}

void clKeyboardManager::Release()
{
    if(m_mgr) {
        delete m_mgr;
    }
    m_mgr = NULL;
}

void clKeyboardManager::DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames)
{
    frames.push_back(parent);
    const wxWindowList& children = parent->GetChildren();
    wxWindowList::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        wxFrame* frameChild = dynamic_cast<wxFrame*>(*iter);
        if(frameChild) {
            if(std::find(frames.begin(), frames.end(), frameChild) == frames.end()) {
                frames.push_back(frameChild);
                DoGetFrames(frameChild, frames);
            }
        }
    }
}

void clKeyboardManager::DoUpdateMenu(wxMenu* menu, MenuItemDataIntMap_t& accels, std::vector<wxAcceleratorEntry>& table)
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetSubMenu()) {
            DoUpdateMenu(item->GetSubMenu(), accels, table);
            continue;
        }

        MenuItemDataIntMap_t::iterator where = accels.find(item->GetId());
        if(where != accels.end()) {
            wxString itemText = item->GetItemLabel();
            // remove the old shortcut
            itemText = itemText.BeforeFirst('\t');
            itemText << "\t" << where->second.accel.ToString();

            // Replace the item text (mnemonics + accel + label)
            item->SetItemLabel(itemText);

            // remove the matches entry from the accels map
            accels.erase(where);
        }

        wxAcceleratorEntry* a = wxAcceleratorEntry::Create(item->GetItemLabel());
        if(a) {
            a->Set(a->GetFlags(), a->GetKeyCode(), item->GetId());
            table.push_back(*a);
            wxDELETE(a);
        }
    }
}

void clKeyboardManager::DoUpdateFrame(wxFrame* frame, MenuItemDataIntMap_t& accels)
{
    std::vector<wxAcceleratorEntry> table;

    // Update menus. If a match is found remove it from the 'accel' table
    auto menuBar = clGetManager()->GetMenuBar();
    if(!menuBar) {
        clDEBUG() << "No menu bar found!" << clEndl;
        return;
    }

    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        LOG_IF_TRACE { clDEBUG1() << "clKeyboardManager: updating menu" << menuBar->GetMenuLabel(i) << clEndl; }
        DoUpdateMenu(menu, accels, table);
    }

    if(!table.empty() || !accels.empty()) {
        wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + accels.size()];
        // append the globals
        clDEBUG() << "clKeyboardManager: appending global entries" << clEndl;
        for(MenuItemDataIntMap_t::iterator iter = accels.begin(); iter != accels.end(); ++iter) {
            wxString dummyText;
            dummyText << iter->second.action << "\t" << iter->second.accel.ToString();
            wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
            if(entry) {
                entry->Set(entry->GetFlags(), entry->GetKeyCode(), wxXmlResource::GetXRCID(iter->second.resourceID));
                table.push_back(*entry);
                wxDELETE(entry);
            }
        }

        for(size_t i = 0; i < table.size(); ++i) {
            entries[i] = table.at(i);
        }

        wxAcceleratorTable acceleTable(table.size(), entries);
        frame->SetAcceleratorTable(acceleTable);
        wxDELETEA(entries);
    }
}

void clKeyboardManager::Save()
{
    clKeyboardBindingConfig config;
    config.SetBindings(m_accelTable).Save();
}

void clKeyboardManager::Initialize()
{
    m_accelTable.clear();
    clDEBUG() << "Keyboard manager: Initializing keyboard manager" << endl;
    // Load old format
    clKeyboardBindingConfig config;
    if(config.Exists()) {
        config.Load();
        m_accelTable = config.GetBindings();
    }

    // Load the default settings and add any new entries
    for(const MenuItemDataMap_t::value_type& vt : m_defaultAccelTable) {
        MenuItemDataMap_t::iterator iter = m_accelTable.find(vt.first);
        if(iter == m_accelTable.end()) {
            // Add new entry
            MenuItemData mid = vt.second;
            if(Exists(mid.accel)) {
                // This shortcut is already in use, disable it
                mid.accel.Clear();
            }
            m_accelTable.emplace(mid.resourceID, mid);
        } else {
            // Update the description to reflect localization changes
            iter->second.parentMenu = vt.second.parentMenu;
            iter->second.action = vt.second.action;
        }
    }

    // Store the correct configuration
    config.SetBindings(m_accelTable).Save();

    // And apply the changes
    Update();

    m_initialized = true;
}

void clKeyboardManager::GetAllAccelerators(MenuItemDataMap_t& accels) const { accels = m_accelTable; }

void clKeyboardManager::SetAccelerators(const MenuItemDataMap_t& accels)
{
    m_accelTable = accels;

    Update();
    Save();
}

void clKeyboardManager::Update(wxFrame* frame)
{
    // Since we keep the accelerators with their original resource ID in the form of string
    // we need to convert the map into a different integer with integer as the resource ID
    MenuItemDataIntMap_t intAccels;
    DoConvertToIntMap(m_accelTable, intAccels);

    if(!frame) {
        // update all frames
        frame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        CHECK_PTR_RET(frame);
    }
    // update only the requested frame
    DoUpdateFrame(frame, intAccels);
}

int clKeyboardManager::PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData)
{
    NewKeyShortcutDlg dlg(parent, menuItemData);
    if(dlg.ShowModal() == wxID_OK) {
        menuItemData.accel = dlg.GetAccel();
        return wxID_OK;
    }
    return wxID_CANCEL;
}

bool clKeyboardManager::Exists(const clKeyboardShortcut& accel) const
{
    if(!accel.IsOk()) {
        return false;
    }

    MenuItemDataMap_t::const_iterator iter = m_accelTable.begin();
    for(; iter != m_accelTable.end(); ++iter) {
        if(iter->second.accel == accel) {
            return true;
        }
    }
    return false;
}

void clKeyboardManager::AddAccelerator(const wxString& resourceID, const wxString& parentMenu, const wxString& action,
                                       const clKeyboardShortcut& accel)
{
    wxASSERT_MSG(m_defaultAccelTable.count(resourceID) == 0, "An accelerator with this resourceID already exists");

    MenuItemData mid;
    mid.resourceID = resourceID;
    mid.parentMenu = parentMenu;
    mid.action = action;
    mid.accel = accel;

    // Is the keyboard manager already initialized?
    // (this may indicate that the accelerator was dynamically generated from e.g. the SnipWiz plugin)
    if(m_initialized) {
        // We assume this accelerator as a 'non-persistent' accelerator,
        // which will be gone when the user restores default accelerators
        if(Exists(mid.accel)) {
            mid.accel.Clear();
        }
        m_accelTable[mid.resourceID] = mid;
    } else {
        // Otherwise, make it a persistent accelerator
        m_defaultAccelTable.emplace(mid.resourceID, mid);
    }
}

void clKeyboardManager::AddAccelerator(const wxString& parentMenu, const std::vector<AddAccelData>& table)
{
    for(const AddAccelData& data : table) {
        AddAccelerator(data.m_resourceID, parentMenu, data.m_action, data.m_accel);
    }
}

void clKeyboardManager::RestoreDefaults()
{
    // Decide which file we want to load, take the user settings file first
    wxFileName fnOldSettings(clStandardPaths::Get().GetUserDataDir(), "accelerators.conf");
    fnOldSettings.AppendDir("config");

    wxFileName fnNewSettings(clStandardPaths::Get().GetUserDataDir(), "keybindings.conf");
    fnNewSettings.AppendDir("config");

    wxLogNull nl;
    if(fnOldSettings.Exists()) {
        clRemoveFile(fnOldSettings.GetFullPath());
    }

    if(fnNewSettings.Exists()) {
        clRemoveFile(fnNewSettings.GetFullPath());
    }

    m_initialized = false;

    // Call initialize again
    Initialize();
}

void clKeyboardManager::OnStartupCompleted(wxCommandEvent& event)
{
    event.Skip();
    this->Initialize();
}

void clKeyboardManager::DoConvertToIntMap(const MenuItemDataMap_t& strMap, MenuItemDataIntMap_t& intMap)
{
    // Convert the string map into int based map
    MenuItemDataMap_t::const_iterator iter = strMap.begin();
    for(; iter != strMap.end(); ++iter) {
        intMap.insert(std::make_pair(wxXmlResource::GetXRCID(iter->second.resourceID), iter->second));
    }
}

clKeyboardShortcut::Vec_t clKeyboardManager::GetAllUnassignedKeyboardShortcuts() const
{
    clKeyboardShortcut::Set_t usedShortcuts;
    for(std::pair<wxString, MenuItemData> p : m_accelTable) {
        if(p.second.accel.IsOk()) {
            usedShortcuts.insert(p.second.accel);
        }
    }

    // Remove all duplicate entries
    clKeyboardShortcut::Vec_t allUnassigned;
    std::set_difference(m_allShortcuts.begin(), m_allShortcuts.end(), usedShortcuts.begin(), usedShortcuts.end(),
                        std::back_inserter(allUnassigned));
    return allUnassigned;
}

MenuItemDataMap_t clKeyboardManager::DoLoadAccelerators(const wxFileName& filename) const
{
    MenuItemDataMap_t entries;
    if(filename.Exists()) {
        wxString content;
        if(!FileUtils::ReadFileContent(filename, content)) {
            return entries;
        }
        wxArrayString lines = ::wxStringTokenize(content, "\r\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            wxArrayString parts = ::wxStringTokenize(lines.Item(i), "|", wxTOKEN_RET_EMPTY);
            if(parts.GetCount() < 3) {
                continue;
            }
            MenuItemData binding;
            binding.resourceID = parts.Item(0);
            binding.parentMenu = parts.Item(1);
            binding.action = parts.Item(2);
            if(parts.GetCount() == 4) {
                binding.accel.FromString(parts.Item(3));
            }
            entries.insert(std::make_pair(binding.resourceID, binding));
        }
    }
    return entries;
}

bool clKeyboardShortcut::operator==(const clKeyboardShortcut& rhs) const
{
    return this->GetControl() == rhs.GetControl() && this->GetAlt() == rhs.GetAlt() &&
           this->GetShift() == rhs.GetShift() && this->GetKeyCode() == rhs.GetKeyCode();
}

bool clKeyboardShortcut::operator<(const clKeyboardShortcut& rhs) const
{
    if(this->GetShift() != rhs.GetShift()) {
        return this->GetShift() < rhs.GetShift();
    }
    if(this->GetAlt() != rhs.GetAlt()) {
        return this->GetAlt() < rhs.GetAlt();
    }
    if(this->GetControl() != rhs.GetControl()) {
        return this->GetControl() < rhs.GetControl();
    }
    return this->GetKeyCode() < rhs.GetKeyCode();
}

bool clKeyboardShortcut::IsOk() const { return !m_keyCode.IsEmpty(); }

void clKeyboardShortcut::Clear()
{
    m_control_type = WXK_NONE;
    m_alt = false;
    m_shift = false;
    m_keyCode.Clear();
}

wxArrayString clKeyboardShortcut::Tokenize(const wxString& accelString) const
{
    /**
     * For example, "Ctrl-Alt-1" will be tokenized into:
     *   [0] Ctrl
     *   [1] -
     *   [2] Alt
     *   [3] -
     *   [4] 1
     *
     * Note that we also need the delimiter itself (in this case, '-')
     * to properly interpret accelerators like "Ctrl--" or just "-",
     * so we can't use wxStringTokenizer here
     */
    wxString token;
    wxArrayString tokens;
    for(const wxUniChar& ch : accelString) {
        if(ch == '-' || ch == '+') {
            if(!token.IsEmpty()) {
                tokens.Add(token);
                token.Clear();
            }
            tokens.Add(ch);
        } else {
            token << ch;
        }
    }
    if(!token.IsEmpty()) {
        tokens.Add(token);
    }
    return tokens;
}

void clKeyboardShortcut::FromString(const wxString& accelString)
{
    Clear();
    if(accelString.IsEmpty()) {
        return;
    }

    wxArrayString tokens = Tokenize(accelString);
    for(size_t i = 0; i < tokens.GetCount(); ++i) {
        wxString token = tokens.Item(i);
        if(token.IsSameAs("rawctrl", false)) {
            // WXK_RAW_CONTROL == WXK_CONTROL on non macOS
            m_control_type = WXK_RAW_CONTROL;
            ++i;

        } else if(token.IsSameAs("ctrl", false)) {
            // CMD or Control
            m_control_type = WXK_CONTROL;
            ++i;

        } else if(token.IsSameAs("alt", false)) {
            m_alt = true;
            ++i;

        } else if(token.IsSameAs("shift", false)) {
            m_shift = true;
            ++i;

        } else {
            m_keyCode = token.MakeUpper();
        }
    }
}

wxString clKeyboardShortcut::to_string(bool for_ui) const
{
    wxUnusedVar(for_ui);

    // An accelerator must contain a key code
    if(!IsOk()) {
        return "";
    }

    wxString str;
    if(m_control_type == WXK_CONTROL) {
#ifdef __WXMAC__
        if(for_ui) {
            str << "Cmd-";

        } else {
            str << "Ctrl-";
        }
#else
        str << "Ctrl-";
#endif

    } else if(m_control_type == WXK_RAW_CONTROL) {
#ifdef __WXMAC__
        if(for_ui) {
            str << "Ctrl-";
        } else {
            str << "RawCtrl-";
        }
#else
        str << "Ctrl-";
#endif
    }

    if(m_alt) {
        str << "Alt-";
    }
    if(m_shift) {
        str << "Shift-";
    }
    str << m_keyCode;
    return str;
}

wxString clKeyboardShortcut::ToString() const { return to_string(false); }
wxString clKeyboardShortcut::DisplayString() const { return to_string(true); }
