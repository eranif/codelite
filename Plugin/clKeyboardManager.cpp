#include "clKeyboardManager.h"
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include "macros.h"
#include <algorithm>
#include "clKeyboardBindingConfig.h"
#include "cl_standard_paths.h"
#include "fileutils.h"
#include "newkeyshortcutdlg.h"
#include <wx/log.h>
#include <algorithm>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include "file_logger.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <algorithm>

clKeyboardManager::clKeyboardManager()
{
    EventNotifier::Get()->Connect(
        wxEVT_INIT_DONE, wxCommandEventHandler(clKeyboardManager::OnStartupCompleted), NULL, this);

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
    m_keyCodes.insert("PASUE");
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
    std::for_each(m_keyCodes.begin(), m_keyCodes.end(), [&](const wxString& keyCode) {
        m_allShorcuts.insert("Ctrl-Alt-Shift-" + keyCode);
        m_allShorcuts.insert("Ctrl-Alt-" + keyCode);
        m_allShorcuts.insert("Ctrl-Shift-" + keyCode);
        m_allShorcuts.insert("Ctrl-" + keyCode);
        m_allShorcuts.insert("Alt-Shift-" + keyCode);
        m_allShorcuts.insert("Alt-" + keyCode);
        m_allShorcuts.insert("Shift-" + keyCode);
    });
}

clKeyboardManager::~clKeyboardManager()
{
    Save();
    EventNotifier::Get()->Disconnect(
        wxEVT_INIT_DONE, wxCommandEventHandler(clKeyboardManager::OnStartupCompleted), NULL, this);
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
            itemText << "\t" << where->second.accel;

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
    wxMenuBar* menuBar = frame->GetMenuBar();
    if(!menuBar) return;
    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        DoUpdateMenu(menu, accels, table);
    }

    if(!table.empty() || !accels.empty()) {
        wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + accels.size()];
        // append the globals
        for(MenuItemDataIntMap_t::iterator iter = accels.begin(); iter != accels.end(); ++iter) {
            wxString dummyText;
            dummyText << iter->second.action << "\t" << iter->second.accel;
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
    config.SetBindings(m_menuTable, m_globalTable).Save();
}

void clKeyboardManager::Initialize()
{
    m_menuTable.clear();
    CL_DEBUG("Keyboard manager: Initializing keyboard manager");
    // Load old format
    clKeyboardBindingConfig config;
    if(!config.Exists()) {
        CL_DEBUG("Keyboard manager: No configurtion found - importing old settings");
        // Decide which file we want to load, take the user settings file first
        wxFileName fnOldSettings(clStandardPaths::Get().GetUserDataDir(), "accelerators.conf");
        fnOldSettings.AppendDir("config");

        wxFileName fnDefaultOldSettings(clStandardPaths::Get().GetDataDir(), "accelerators.conf.default");
        fnDefaultOldSettings.AppendDir("config");
        wxFileName fnFileToLoad;
        bool canDeleteOldSettings(false);
        if(fnOldSettings.Exists()) {
            fnFileToLoad = fnOldSettings;
            canDeleteOldSettings = true;
        } else {
            fnFileToLoad = fnDefaultOldSettings;
        }

        if(fnFileToLoad.Exists()) {
            CL_DEBUG("Keyboard manager: Importing settings from '%s'", fnFileToLoad.GetFullPath());

            // Apply the old settings to the menus
            wxString content;
            if(!FileUtils::ReadFileContent(fnFileToLoad, content)) return;
            wxArrayString lines = ::wxStringTokenize(content, "\r\n", wxTOKEN_STRTOK);
            for(size_t i = 0; i < lines.GetCount(); ++i) {
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), "|", wxTOKEN_RET_EMPTY);
                if(parts.GetCount() < 3) continue;
                MenuItemData binding;
                binding.resourceID = parts.Item(0);
                binding.parentMenu = parts.Item(1);
                binding.action = parts.Item(2);
                if(parts.GetCount() == 4) {
                    binding.accel = parts.Item(3);
                }
                m_menuTable.insert(std::make_pair(binding.resourceID, binding));
            }

            if(canDeleteOldSettings) {
                wxLogNull noLog;
                clRemoveFile(fnFileToLoad.GetFullPath());
            }
        }
    } else {
        config.Load();
        m_menuTable = config.GetBindings();
    }

    // Load the default settings and add any new entries
    MenuItemDataMap_t defaultEntries = DoLoadDefaultAccelerators();
    std::for_each(defaultEntries.begin(), defaultEntries.end(), [&](const MenuItemDataMap_t::value_type& vt) {
        if(m_menuTable.count(vt.first) == 0) {
            m_menuTable.insert(vt);
        }
    });
    
    // Store the correct configuration
    config.SetBindings(m_menuTable, m_globalTable).Save();

    // And apply the changes
    Update();
}

void clKeyboardManager::GetAllAccelerators(MenuItemDataMap_t& accels) const
{
    accels.clear();
    accels.insert(m_menuTable.begin(), m_menuTable.end());
    accels.insert(m_globalTable.begin(), m_globalTable.end());
}

void clKeyboardManager::SetAccelerators(const MenuItemDataMap_t& accels)
{
    // separate the globals from the menu accelerators
    // The process is done by checking each item's parentMenu
    // If the parentMenu is empty -> global accelerator
    MenuItemDataMap_t globals, menus;
    MenuItemDataMap_t::const_iterator iter = accels.begin();
    for(; iter != accels.end(); ++iter) {
        if(iter->second.parentMenu.IsEmpty()) {
            globals.insert(std::make_pair(iter->first, iter->second));
        } else {
            menus.insert(std::make_pair(iter->first, iter->second));
        }
    }

    m_menuTable.swap(menus);
    m_globalTable.swap(globals);
    Update();
    Save();
}

void clKeyboardManager::Update(wxFrame* frame)
{
    // Since we keep the accelerators with their original resource ID in the form of string
    // we need to convert the map into a different integer with integer as the resource ID

    // Note that we place the items from the m_menuTable first and then we add the globals
    // this is because menu entries takes precedence over global accelerators
    MenuItemDataMap_t accels = m_menuTable;
    accels.insert(m_globalTable.begin(), m_globalTable.end());

    MenuItemDataIntMap_t intAccels;
    DoConvertToIntMap(accels, intAccels);

    if(!frame) {
        // update all frames
        wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        CHECK_PTR_RET(topFrame);

        FrameList_t frames;
        DoGetFrames(topFrame, frames);
        for(FrameList_t::iterator iter = frames.begin(); iter != frames.end(); ++iter) {

            DoUpdateFrame(*iter, intAccels);
        }
    } else {
        // update only the requested frame
        DoUpdateFrame(frame, intAccels);
    }
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

bool clKeyboardManager::Exists(const wxString& accel) const
{
    if(accel.IsEmpty()) return false;

    MenuItemDataMap_t accels;
    GetAllAccelerators(accels);

    MenuItemDataMap_t::const_iterator iter = accels.begin();
    for(; iter != accels.end(); ++iter) {
        if(iter->second.accel == accel) {
            return true;
        }
    }
    return false;
}

void clKeyboardManager::AddGlobalAccelerator(const wxString& resourceID,
                                             const wxString& keyboardShortcut,
                                             const wxString& description)
{
    MenuItemData mid;
    mid.action = description;
    mid.accel = keyboardShortcut;
    mid.resourceID = resourceID;
    m_globalTable.insert(std::make_pair(mid.resourceID, mid));
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

wxArrayString clKeyboardManager::GetAllUnasignedKeyboardShortcuts() const
{
    MenuItemDataMap_t accels;
    GetAllAccelerators(accels);

    wxStringSet_t usedShortcuts;
    std::for_each(accels.begin(), accels.end(), [&](const std::pair<wxString, MenuItemData>& p) {
        if(!p.second.accel.IsEmpty()) {
            usedShortcuts.insert(p.second.accel);
        }
    });

    // Remove all duplicate entries
    wxArrayString allUnasigned;
    std::set_difference(m_allShorcuts.begin(),
                        m_allShorcuts.end(),
                        usedShortcuts.begin(),
                        usedShortcuts.end(),
                        std::back_inserter(allUnasigned));
    return allUnasigned;
}

MenuItemDataMap_t clKeyboardManager::DoLoadDefaultAccelerators()
{
    MenuItemDataMap_t entries;
    wxFileName fnDefaultOldSettings(clStandardPaths::Get().GetDataDir(), "accelerators.conf.default");
    fnDefaultOldSettings.AppendDir("config");

    if(fnDefaultOldSettings.Exists()) {
        wxString content;
        if(!FileUtils::ReadFileContent(fnDefaultOldSettings, content)) {
            return entries;
        }
        wxArrayString lines = ::wxStringTokenize(content, "\r\n", wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i) {
            wxArrayString parts = ::wxStringTokenize(lines.Item(i), "|", wxTOKEN_RET_EMPTY);
            if(parts.GetCount() < 3) continue;
            MenuItemData binding;
            binding.resourceID = parts.Item(0);
            binding.parentMenu = parts.Item(1);
            binding.action = parts.Item(2);
            if(parts.GetCount() == 4) {
                binding.accel = parts.Item(3);
            }
            entries.insert(std::make_pair(binding.resourceID, binding));
        }
    }
    return entries;
}

void clKeyboardShortcut::Clear()
{
    m_ctrl = false;
    m_alt = false;
    m_shift = false;
    m_keyCode.Clear();
}

void clKeyboardShortcut::FromString(const wxString& accelString)
{
    Clear();
    wxArrayString tokens = ::wxStringTokenize(accelString, "-+", wxTOKEN_STRTOK);
    for(size_t i = 0; i < tokens.GetCount(); ++i) {
        wxString token = tokens.Item(i);
        token.MakeLower();
        if(token == "shift") {
            m_shift = true;
        } else if(token == "alt") {
            m_alt = true;
        } else if(token == "ctrl") {
            m_ctrl = true;
        } else {
            m_keyCode = tokens.Item(i);
        }
    }
}

wxString clKeyboardShortcut::ToString() const
{
    // An accelerator must contain a key code
    if(m_keyCode.IsEmpty()) {
        return "";
    }

    wxString str;
    if(m_ctrl) {
        str << "Ctrl-";
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
