#include "Scripting/CodeLiteLUA.hpp"

#include "codelite_events.h"
#include "globals.h"

extern "C" {
#include "lualib.h"
}

#include "FileManager.hpp"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"

#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

namespace
{
enum MessageType {
    kInfo,
    kWarn,
    kError,
};
}

CodeLiteLUA& CodeLiteLUA::Get()
{
    static CodeLiteLUA codelite_lua;
    return codelite_lua;
}

void CodeLiteLUA::Reset() { m_menu_items.clear(); }

void CodeLiteLUA::Initialise()
{
    auto& self = Get();
    self.m_state = luaL_newstate();
    luaL_openlibs(self.m_state);

    try {
        clDEBUG() << "Registering codelite with LUA" << endl;
        luabridge::getGlobalNamespace(self.m_state)
            .beginNamespace("codelite")
            .addFunction("message_box", &CodeLiteLUA::message_box)
            .addFunction("add_menu_item", &CodeLiteLUA::add_menu_item)
            .endNamespace();

    } catch (const std::exception& e) {
        self.m_state = nullptr; // it's ok to have the memory leak
        clERROR() << "Error occurred while initialising LUA." << e.what() << endl;
        return;
    }
    self.InitialiseInternal();
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &CodeLiteLUA::OnFileSaved, &self);
}

void CodeLiteLUA::InitialiseInternal()
{
    clDEBUG() << "InitialiseInternal is called" << endl;
    Reset();

    // Load and compile CodeLite's main lua script.
    auto options = WriteOptions{.force_global = true};

    wxString codelite_lua = FileManager::GetSettingFileFullPath("codelite.lua", options);
    if (!wxFileName::FileExists(codelite_lua)) {
        clDEBUG() << "File: '" << codelite_lua << "' does not exist" << endl;
        return;
    }

    clDEBUG() << "Running file:" << codelite_lua << endl;
    auto status = LoadScriptFile(codelite_lua);
    if (!status.ok()) {
        wxString errmsg;
        errmsg << _("Failed to run LUA file: ") << codelite_lua << "\n" << status.message();
        ::wxMessageBox(errmsg, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    clDEBUG() << "Successfully executed script file:" << codelite_lua << endl;
}

CodeLiteLUA::CodeLiteLUA() {}

CodeLiteLUA::~CodeLiteLUA()
{
    // free the state
    m_menu_items.clear();
    if (m_state) {
        lua_close(m_state);
        m_state = nullptr;
    }
}

void CodeLiteLUA::message_box(const std::string& message, int type)
{
    switch (static_cast<MessageType>(type)) {
    case MessageType::kWarn:
        ::wxMessageBox(wxString::FromUTF8(message), "CodeLite Lua", wxOK | wxCENTER | wxICON_WARNING);
        break;
    case MessageType::kError:
        ::wxMessageBox(wxString::FromUTF8(message), "CodeLite Lua", wxOK | wxCENTER | wxICON_ERROR);
        break;
    default:
    case MessageType::kInfo:
        ::wxMessageBox(wxString::FromUTF8(message), "CodeLite Lua", wxOK | wxCENTER | wxICON_INFORMATION);
        break;
    }
}

void CodeLiteLUA::add_menu_item(const std::string& menu_name, const std::string& label, luabridge::LuaRef action)
{
    auto& self = Get();

    clDEBUG() << "Adding item:" << label << "for menu:" << menu_name << endl;
    LuaMenuItem menu_item{.label = label, .action = std::move(action)};
    if (!menu_item.IsOk()) {
        clWARNING() << "Failed to add menu item:" << label << "to menu:" << menu_name << ". Action is not a function"
                    << endl;
        return;
    }

    if (!self.m_menu_items.contains(menu_name)) {
        self.m_menu_items.insert({menu_name, std::vector<LuaMenuItem>{}});
    }
    self.m_menu_items[menu_name].push_back(std::move(menu_item));
}

clStatus CodeLiteLUA::LoadScriptString(const wxString& script)
{
    if (m_state == nullptr) {
        return StatusInavalidArgument("Could not run LUA script. Engine is not initialised.");
    }

    std::string script_content = script.ToStdString(wxConvUTF8);
    clDEBUG() << "Loading LUA script:\n" << script << endl;
    if (luaL_loadstring(m_state, script.c_str()) != LUA_OK) {
        return StatusOther(wxString::FromUTF8(lua_tostring(m_state, -1)));
    }

    if (lua_pcall(m_state, 0, 0, -2) != LUA_OK) {
        return StatusOther(wxString::FromUTF8(lua_tostring(m_state, -1)));
    }

    return StatusOk();
}

clStatus CodeLiteLUA::LoadScriptFile(const wxString& path)
{
    wxString content;
    if (!FileUtils::ReadFileContent(path, content)) {
        return StatusNotFound(path);
    }
    return LoadScriptString(content);
}

void CodeLiteLUA::UpdateMenu(const wxString& menu_name, wxMenu* menu)
{
    std::string name = menu_name.ToStdString(wxConvUTF8);
    if (!m_menu_items.contains(name)) {
        return;
    }

    auto& menu_items = m_menu_items[name];
    for (auto& item : menu_items) {
        wxString label = wxString::FromUTF8(item.label);
        auto menu_id = wxXmlResource::GetXRCID(label);
        menu->Append(menu_id, label);
        menu->Bind(wxEVT_MENU, [&item](wxCommandEvent&) { item.RunAction(); }, menu_id);
    }
}

void CodeLiteLUA::OnFileSaved(clCommandEvent& event)
{
    event.Skip(); // Always call this.

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    const WriteOptions opts{.converter = nullptr, .force_global = true};
    wxString codelite_lua = FileManager::GetSettingFileFullPath("codelite.lua", opts);

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath != codelite_lua) {
        return;
    }

    InitialiseInternal();
}
