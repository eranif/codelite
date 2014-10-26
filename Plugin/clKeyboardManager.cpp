#include "clKeyboardManager.h"
#include <wx/app.h>
#include "macros.h"
#include <algorithm>

clKeyboardManager::clKeyboardManager() {}

clKeyboardManager::~clKeyboardManager() {}

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

bool clKeyboardManager::AddAccelerator(const wxString& actionId, const wxString& keyboardShortcut)
{
    return AddAccelerator(wxXmlResource::GetXRCID(actionId), keyboardShortcut);
}

bool clKeyboardManager::AddAccelerator(int actionId, const wxString& keyboardShortcut)
{
    // Get the list of frames we have
    wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
    CHECK_PTR_RET_FALSE(topFrame);

    // Get list of all wxFrames codelite has
    clKeyboardManager::FrameList_t frames;
    DoGetFrames(topFrame, frames);

    // Loop over the menubars and search for the action ID
    bool matchFound = false;
    for(clKeyboardManager::FrameList_t::const_iterator iter = frames.begin(); iter != frames.end(); ++iter) {
        DoUpdateFrame(*iter, actionId, keyboardShortcut, matchFound);
    }
    return true;
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

void clKeyboardManager::DoUpdateMenu(wxMenu* menu,
                                     int actionId,
                                     const wxString& keyboardShortcut,
                                     std::vector<wxAcceleratorEntry>& table,
                                     bool& matchFound)
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetSubMenu()) {
            DoUpdateMenu(item->GetSubMenu(), actionId, keyboardShortcut, table, matchFound);
            continue;
        }

        if(item->GetId() == actionId) {
            wxString itemText = item->GetItemLabel();
            // remove the old shortcut
            itemText = itemText.BeforeFirst('\t');
            itemText << "\t" << keyboardShortcut;

            // Replace the item text (mnemonics + accel + label)
            item->SetItemLabel(itemText);
            matchFound = true;
        }

        wxAcceleratorEntry* a = wxAcceleratorEntry::Create(item->GetItemLabel());
        if(a) {
            a->Set(a->GetFlags(), a->GetKeyCode(), item->GetId());
            table.push_back(*a);
            wxDELETE(a);
        }
    }
}

void clKeyboardManager::DoUpdateFrame(wxFrame* frame, int actionId, const wxString& keyboardShortcut, bool& matchFound)
{
    std::vector<wxAcceleratorEntry> table;
    
    // Go over the menus and scan each menu item, if one of the menu items matches the requested actionId
    // update the accelerator to the new keyboard shortcut and mark 'matchFound' with true
    wxMenuBar* menuBar = frame->GetMenuBar();
    if(!menuBar) return;
    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        DoUpdateMenu(menu, actionId, keyboardShortcut, table, matchFound);
    }

    // Now that we are done going over the menus. Add the global accelerators
    // global accelerators are accelerators requested by the user that have no matching
    // menu entries
    if(!matchFound && m_accelTable.count(actionId) == 0) {
        // Could not find a match - add one to the global accel table
        m_accelTable.insert(std::make_pair(actionId, keyboardShortcut));
    }

    if(!table.empty() || !m_accelTable.empty()) {
        wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + m_accelTable.size()];
        // append the globals
        for(std::map<int, wxString>::iterator iter = m_accelTable.begin(); iter != m_accelTable.end(); ++iter) {
            wxString dummyText;
            dummyText << "Stam\t" << keyboardShortcut;
            wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
            if(entry) {
                entry->Set(entry->GetFlags(), entry->GetKeyCode(), actionId);
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
