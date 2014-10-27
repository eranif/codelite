#include "clKeyboardManager.h"
#include <wx/app.h>
#include "macros.h"
#include <algorithm>
#include "clKeyboardBindingConfig.h"

clKeyboardManager::clKeyboardManager() { Load(); }

clKeyboardManager::~clKeyboardManager() { Save(); }

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

bool clKeyboardManager::AddAccelerator(const wxString& actionId,
                                       const wxString& keyboardShortcut,
                                       const wxString& description)
{
    return AddAccelerator(wxXmlResource::GetXRCID(actionId), keyboardShortcut, description);
}

bool clKeyboardManager::AddAccelerator(int actionId, const wxString& keyboardShortcut, const wxString& description)
{
    KeyBinding::Map_t accel;
    KeyBinding accelerator;
    accelerator.accelerator = keyboardShortcut;
    accelerator.actionId = actionId;
    accelerator.description = description;
    accel.insert(std::make_pair(accelerator.actionId, accelerator));
    return AddAccelerators(accel);
}

bool clKeyboardManager::AddAccelerators(clKeyboardManager::KeyBinding::Map_t& accels)
{
    // Get the list of frames we have
    wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
    CHECK_PTR_RET_FALSE(topFrame);

    // Get list of all wxFrames codelite has
    clKeyboardManager::FrameList_t frames;
    DoGetFrames(topFrame, frames);

    // Loop over the menubars + the internal map we keep and update the shortcuts
    for(clKeyboardManager::FrameList_t::const_iterator iter = frames.begin(); iter != frames.end(); ++iter) {
        DoUpdateFrame(*iter, accels);
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
                                     clKeyboardManager::KeyBinding::Map_t& accels,
                                     std::vector<wxAcceleratorEntry>& table)
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetSubMenu()) {
            DoUpdateMenu(item->GetSubMenu(), accels, table);
            continue;
        }

        clKeyboardManager::KeyBinding::Map_t::iterator where = accels.find(item->GetId());
        if(where != accels.end()) {
            wxString itemText = item->GetItemLabel();
            // remove the old shortcut
            itemText = itemText.BeforeFirst('\t');
            itemText << "\t" << where->second.accelerator;

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

void clKeyboardManager::DoUpdateFrame(wxFrame* frame, clKeyboardManager::KeyBinding::Map_t& accels)
{
    std::vector<wxAcceleratorEntry> table;

    // Update menus. If a match is found remove it from the 'accel' table
    wxMenuBar* menuBar = frame->GetMenuBar();
    if(!menuBar) return;
    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        DoUpdateMenu(menu, accels, table);
    }

    if(!accels.empty()) {
        // We still have some accelerators that we could not map them to menu entries
        // We can however, associate them to the frame(s)
        m_accelTable.insert(accels.begin(), accels.end());
    }

    if(!table.empty() || !m_accelTable.empty()) {
        wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + m_accelTable.size()];
        // append the globals
        for(KeyBinding::Map_t::iterator iter = m_accelTable.begin(); iter != m_accelTable.end(); ++iter) {
            wxString dummyText;
            dummyText << iter->second.description << "\t" << iter->second.accelerator;
            wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
            if(entry) {
                entry->Set(entry->GetFlags(), entry->GetKeyCode(), iter->second.actionId);
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

void clKeyboardManager::Load()
{
    clKeyboardBindingConfig config;
    m_accelTable = config.Load().GetBindings();
}
