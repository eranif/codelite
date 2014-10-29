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

clKeyboardManager::clKeyboardManager()
{
    EventNotifier::Get()->Connect(
        wxEVT_INIT_DONE, wxCommandEventHandler(clKeyboardManager::OnCodeLiteStarupDone), NULL, this);
}

clKeyboardManager::~clKeyboardManager()
{
    Save();
    EventNotifier::Get()->Disconnect(
        wxEVT_INIT_DONE, wxCommandEventHandler(clKeyboardManager::OnCodeLiteStarupDone), NULL, this);
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

void clKeyboardManager::DoUpdateMenu(wxMenu* menu, MenuItemDataMap_t& accels, std::vector<wxAcceleratorEntry>& table)
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetSubMenu()) {
            DoUpdateMenu(item->GetSubMenu(), accels, table);
            continue;
        }

        MenuItemDataMap_t::iterator where = accels.find(item->GetId());
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

void clKeyboardManager::DoUpdateFrame(wxFrame* frame, MenuItemDataMap_t& accels)
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
        for(MenuItemDataMap_t::iterator iter = accels.begin(); iter != accels.end(); ++iter) {
            wxString dummyText;
            dummyText << iter->second.action << "\t" << iter->second.accel;
            wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
            if(entry) {
                entry->Set(entry->GetFlags(), entry->GetKeyCode(), iter->second.id);
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
    config.SetBindings(m_menuTable).SetGlobalBindings(m_globalTable).Save();
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
                binding.id = wxXmlResource::GetXRCID(parts.Item(0));
                binding.action = parts.Item(2);
                if(parts.GetCount() == 4) {
                    binding.accel = parts.Item(3);
                }
                m_menuTable.insert(std::make_pair(binding.id, binding));
            }
            Update();
            m_menuTable.clear();
            
            if(canDeleteOldSettings) {
                wxLogNull noLog;
                ::wxRemoveFile(fnFileToLoad.GetFullPath());
            }
        }
    }

    config.Load();
    m_menuTable = config.GetBindings();

    // Load the accelerator table from the menu itself
    // Get the list of frames we have
    wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
    if(!topFrame) {
        CL_DEBUG("Keyboard manager: no top level Window found!");
    }
    CHECK_PTR_RET(topFrame);

    // load accelerators by scanning the top level frame and go over all menus
    // in the menu bar
    MenuItemDataMap_t accels;
    CL_DEBUG("Keyboard manager: scanning top frame for keyboard shortcuts...");
    DoGetFrameAccels(accels, topFrame);
    CL_DEBUG("Keyboard manager: scanning top frame for keyboard shortcuts...done. %d keyboard shortcuts loaded",
              (int)accels.size());

    // insert what we have found on the actual menus
    // insert will fail if an entry is already exists with the same
    // key in the table. This way we give the user preferences a priority
    // over the menu set by the plugin
    m_menuTable.insert(accels.begin(), accels.end());

    // Load global bindings
    // Same logic here: load the user settings first and then
    // try to add entries from the plugin
    MenuItemDataMap_t globals;
    globals = config.GetGlobalBindings();
    globals.insert(m_globalTable.begin(), m_globalTable.end());
    m_globalTable.swap(globals);

    // Store the correct configuration
    config.SetBindings(m_menuTable).SetGlobalBindings(m_globalTable).Save();
    
    // And apply the changes
    Update();
}

void clKeyboardManager::GetAllAccelerators(MenuItemDataMap_t& accels) const
{
    accels.clear();
    accels.insert(m_menuTable.begin(), m_menuTable.end());
    accels.insert(m_globalTable.begin(), m_globalTable.end());
}

void clKeyboardManager::DoGetMenuAccels(MenuItemDataMap_t& accels, wxMenu* menu, const wxString& parentMenuTitle) const
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetItemLabelText() == _("Recent Workspaces") || item->GetItemLabelText() == _("Recent Files") ||
           item->GetItemLabelText() == _("View As"))
            continue;

        if(item->GetKind() == wxITEM_SEPARATOR) {
            // Skip separators
            continue;
        }

        if(item->GetSubMenu()) {
            DoGetMenuAccels(accels, item->GetSubMenu(), parentMenuTitle + "::" + item->GetItemLabelText());
            continue;
        }

        MenuItemData mid;
        mid.accel = item->GetItemLabel().Find("\t") == wxNOT_FOUND ? "" : item->GetItemLabel().AfterLast('\t');
        mid.action = item->GetItemLabelText();
        mid.id = item->GetId();
        mid.parentMenu = parentMenuTitle;
        mid.parentMenu.Replace("&", "");
        accels.insert(std::make_pair(mid.id, mid));
    }
}

void clKeyboardManager::DoGetFrameAccels(MenuItemDataMap_t& accels, wxFrame* frame) const
{
    wxMenuBar* menuBar = frame->GetMenuBar();
    if(!menuBar) {
        CL_DEBUG("Keyboard manager: frame has no menu bar");
        return;
    }

    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        DoGetMenuAccels(accels, menu, menu->GetTitle());
    }
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
    if(!frame) {
        // update all frames
        wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        CHECK_PTR_RET(topFrame);

        FrameList_t frames;
        DoGetFrames(topFrame, frames);
        for(FrameList_t::iterator iter = frames.begin(); iter != frames.end(); ++iter) {
            MenuItemDataMap_t accels = m_menuTable;
            accels.insert(m_globalTable.begin(), m_globalTable.end());
            DoUpdateFrame(*iter, accels);
        }
    } else {
        // update only the requested frame
        MenuItemDataMap_t accels = m_menuTable;
        accels.insert(m_globalTable.begin(), m_globalTable.end());
        DoUpdateFrame(frame, accels);
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

void
clKeyboardManager::AddGlobalAccelerator(int actionId, const wxString& keyboardShortcut, const wxString& description)
{
    MenuItemData mid;
    mid.action = description;
    mid.accel = keyboardShortcut;
    mid.id = actionId;
    m_globalTable.insert(std::make_pair(actionId, mid));
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
        ::wxRemoveFile(fnOldSettings.GetFullPath());
    }

    if(fnNewSettings.Exists()) {
        ::wxRemoveFile(fnNewSettings.GetFullPath());
    }

    // Call initialize again
    Initialize();
}

void clKeyboardManager::AddGlobalAccelerators(wxMenu* menu, const wxString& prefix)
{
    MenuItemDataMap_t accels;
    DoGetMenuAccels(accels, menu, "");

    // Remove the parentMenu entry
    std::for_each(accels.begin(), accels.end(), MenuItemData::ClearParentMenu());
    // Prepend the prefix
    if(!prefix.IsEmpty()) {
        std::for_each(accels.begin(), accels.end(), MenuItemData::PrependPrefix(prefix));
    }
    m_globalTable.insert(accels.begin(), accels.end());
}

void clKeyboardManager::OnCodeLiteStarupDone(wxCommandEvent& event)
{
    event.Skip();
    this->Initialize();
}
