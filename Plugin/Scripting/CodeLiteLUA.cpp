#include "Scripting/CodeLiteLUA.hpp"

#include "DefaultLuaScript.cpp"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "FileSystemWorkspace/clFileSystemWorkspaceView.hpp"
#include "ai/LLMManager.hpp"
#include "codelite_events.h"
#include "fileextmanager.h"
#include "globals.h"

extern "C" {
#include "lualib.h"
}

#include "FileManager.hpp"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"

#include <wx/frame.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include <wx/textdlg.h>
#include <wx/xrc/xmlres.h>

namespace
{
enum class MessageType {
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

void CodeLiteLUA::Shutdown()
{
    auto& self = Get();

    // free the state
    self.m_menu_items.clear();
    if (self.m_state) {
        lua_close(self.m_state);
        self.m_state = nullptr;
    }

    self.m_textGenerationFrame->Destroy();
    self.m_textGenerationFrame = nullptr;
}

void CodeLiteLUA::Initialise()
{
    auto& self = Get();
    self.m_state = luaL_newstate();

    // Load basic functionality, but do not load the "package" library to reduce security risks.
    luaopen_base(self.m_state);
    luaopen_debug(self.m_state);
    luaopen_io(self.m_state);
    luaopen_math(self.m_state);
    luaopen_os(self.m_state);
    luaopen_string(self.m_state);
    luaopen_table(self.m_state);
    luaopen_utf8(self.m_state);

    try {
        clDEBUG() << "Registering codelite with LUA" << endl;
        luabridge::getGlobalNamespace(self.m_state)
            .beginNamespace("codelite")
            // Functions
            .addFunction("message_box", &CodeLiteLUA::message_box)
            .addFunction("user_text", &CodeLiteLUA::user_text)
            .addFunction("add_menu_item", &CodeLiteLUA::add_menu_item)
            .addFunction("add_menu_separator", &CodeLiteLUA::add_menu_separator)
            .addFunction("editor_selection", &CodeLiteLUA::editor_selection)
            .addFunction("chat", &CodeLiteLUA::chat)
            .addFunction("generate", &CodeLiteLUA::generate)
            .addFunction("editor_language", &CodeLiteLUA::editor_language)
            .addFunction("editor_text", &CodeLiteLUA::editor_text)
            .addFunction("editor_filepath", &CodeLiteLUA::editor_filepath)
            .addFunction("log_warn", &CodeLiteLUA::log_warn)
            .addFunction("log_error", &CodeLiteLUA::log_error)
            .addFunction("log_system", &CodeLiteLUA::log_system)
            .addFunction("log_debug", &CodeLiteLUA::log_debug)
            .addFunction("log_trace", &CodeLiteLUA::log_trace)
            .addFunction("str_replace_all", &CodeLiteLUA::str_replace_all)
            .addFunction("file_system_workspace_selected_folders", &CodeLiteLUA::file_system_workspace_selected_folders)
            .addFunction("file_system_workspace_selected_files", &CodeLiteLUA::file_system_workspace_selected_files)
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
    auto options = WriteOptions{.ignore_workspace = true};

    wxString codelite_lua = FileManager::GetSettingFileFullPath("codelite.lua", options);
    if (!wxFileName::FileExists(codelite_lua)) {
        // First time opening it, create it with default content.
        clSYSTEM() << "File: '" << codelite_lua << "' does not exist" << endl;
        clSYSTEM() << "Creating a default file" << endl;
        if (!FileManager::WriteSettingsFileContent("codelite.lua", wxString::FromUTF8(kDefaultCodeLiteLUA), options)) {
            clERROR() << "Failed to write file:" << codelite_lua << endl;
            return;
        }
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

    if (m_textGenerationFrame == nullptr) {
        m_textGenerationFrame = std::make_shared<TextGenerationPreviewFrame>(PreviewKind::kDefault);
        m_textGenerationFrame->Hide();
    }
}

CodeLiteLUA::CodeLiteLUA() {}

CodeLiteLUA::~CodeLiteLUA() {}

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

std::string CodeLiteLUA::user_text(const std::string& title)
{
    wxString text = wxGetTextFromUser(title, "CodeLite");
    if (text.empty()) {
        return {};
    }
    return text.ToStdString(wxConvUTF8);
}

void CodeLiteLUA::add_menu_item(const std::string& menu_name, const std::string& label, luabridge::LuaRef action)
{
    auto& self = Get();

    auto menu_name_lc = StringUtils::Lowercase(menu_name);
    clDEBUG() << "Adding item:" << label << "for menu:" << menu_name_lc << endl;
    LuaMenuItem menu_item{.label = label, .action = std::move(action)};
    if (!menu_item.IsOk()) {
        clWARNING() << "Failed to add menu item:" << label << "to menu:" << menu_name_lc << ". Action is not a function"
                    << endl;
        return;
    }

    // If "menu_name_lc" is missing a default entry is added.
    self.m_menu_items[menu_name_lc].push_back(std::move(menu_item));
}

void CodeLiteLUA::add_menu_separator(const std::string& menu_name)
{
    auto& self = Get();

    clDEBUG() << "Adding separator for menu:" << menu_name << endl;
    LuaMenuItem menu_item;

    // If "menu_name" is missing a default entry is added.
    self.m_menu_items[menu_name].push_back(std::move(menu_item));
}

std::string CodeLiteLUA::editor_selection()
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor == nullptr) {
        return {};
    }

    return editor->GetSelection().ToStdString(wxConvUTF8);
}

std::string CodeLiteLUA::editor_language()
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor == nullptr) {
        return {};
    }
    auto lang = FileExtManager::GetLanguageFromType(FileExtManager::GetType(editor->GetRemotePathOrLocal()));
    lang = lang.Lower();
    lang.Replace(" ", "");
    return lang.ToStdString(wxConvUTF8);
}

std::string CodeLiteLUA::editor_text()
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor == nullptr) {
        return {};
    }
    return editor->GetEditorText().ToStdString(wxConvUTF8);
}

std::string CodeLiteLUA::editor_filepath()
{
    auto editor = clGetManager()->GetActiveEditor();
    if (editor == nullptr) {
        return {};
    }
    return editor->GetRemotePathOrLocal().ToStdString(wxConvUTF8);
}

void CodeLiteLUA::log_message(const std::string& msg, FileLogger::LogLevel level)
{
    switch (level) {
    case FileLogger::LogLevel::Developer:
        clDEBUG1() << wxString::FromUTF8(msg) << endl;
        break;
    case FileLogger::LogLevel::Dbg:
        clDEBUG() << wxString::FromUTF8(msg) << endl;
        break;
    case FileLogger::LogLevel::Warning:
        clWARNING() << wxString::FromUTF8(msg) << endl;
        break;
    case FileLogger::LogLevel::System:
        clSYSTEM() << wxString::FromUTF8(msg) << endl;
        break;
    case FileLogger::LogLevel::Error:
        clERROR() << wxString::FromUTF8(msg) << endl;
        break;
    }
}

std::string
CodeLiteLUA::str_replace_all(const std::string& str, const std::string& find_what, const std::string& replace_with)
{
    if (find_what.empty()) {
        return str;
    }

    std::string result;
    result.reserve(str.size()); // Pre-allocate to reduce reallocations

    size_t pos = 0;
    size_t last_pos = 0;

    while ((pos = str.find(find_what, last_pos)) != std::string::npos) {
        result.append(str, last_pos, pos - last_pos);
        result.append(replace_with);
        last_pos = pos + find_what.length();
    }

    result.append(str, last_pos, std::string::npos);
    return result;
}

void CodeLiteLUA::generate(const std::string& prompt)
{
    if (Get().m_generationInProgress) {
        ::wxMessageBox(
            _("Another generation is in progress, try again later"), "CodeLite", wxOK | wxICON_WARNING | wxCENTER);
        return;
    }

    Get().m_generationInProgress = true;
    Get().m_textGenerationFrame->InitialiseFor(PreviewKind::kDefault);
    llm::Manager::GetInstance().ShowTextGenerationDialog(
        prompt, Get().m_textGenerationFrame, std::nullopt, []() { CodeLiteLUA::Get().m_generationInProgress = false; });
}

void CodeLiteLUA::chat(const std::string& prompt)
{
    // send an event requesting to initiate a chat
    wxCommandEvent event_chat{wxEVT_MENU, XRCID("ai_show_chat_window")};
    event_chat.SetString(wxString::FromUTF8(prompt));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(event_chat);
}

bool CodeLiteLUA::is_workspace_opened() { return clWorkspaceManager::Get().IsWorkspaceOpened(); }

std::vector<std::string> CodeLiteLUA::file_system_workspace_selected_files()
{
    if (!clFileSystemWorkspace::Get().IsOpen()) {
        return {};
    }
    wxArrayString files, folders;
    clFileSystemWorkspace::Get().GetView()->GetSelections(folders, files);
    wxUnusedVar(folders);
    return StringUtils::ToStdStrings(files);
}

std::vector<std::string> CodeLiteLUA::file_system_workspace_selected_folders()
{
    if (!clFileSystemWorkspace::Get().IsOpen()) {
        return {};
    }
    wxArrayString files, folders;
    clFileSystemWorkspace::Get().GetView()->GetSelections(folders, files);
    wxUnusedVar(files);
    return StringUtils::ToStdStrings(folders);
}

clStatus CodeLiteLUA::LoadScriptString(const wxString& script)
{
    if (m_state == nullptr) {
        return StatusInvalidArgument("Could not run LUA script. Engine is not initialised.");
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

void CodeLiteLUA::CleanupDynamicMenuItems()
{
    auto frame = EventNotifier::Get()->TopFrame();
    auto menu_bar = frame->GetMenuBar();
    CHECK_PTR_RET(menu_bar);

    for (size_t i = 0; i < menu_bar->GetMenuCount(); ++i) {
        auto menu = menu_bar->GetMenu(i);
        wxString title = menu->GetTitle();
        title = wxStripMenuCodes(title).Lower();
        std::string name = title.ToStdString(wxConvUTF8);

        if (!m_items_added_to_menu_bar_menus.contains(name)) {
            continue;
        }

        const auto& ids_to_remove = m_items_added_to_menu_bar_menus[name];
        for (int id : ids_to_remove) {
            menu->Delete(id);
        }
        m_items_added_to_menu_bar_menus.erase(name);
    }
}

void CodeLiteLUA::UpdateMenu(const wxString& menu_name, wxMenu* menu)
{
    CHECK_PTR_RET(menu);
    CleanupDynamicMenuItems();

    std::string name = menu_name.Lower().ToStdString(wxConvUTF8);
    bool is_menu_bar_menu{false};
    if (name.empty()) {
        wxString title = menu->GetTitle();
        title = wxStripMenuCodes(title).Lower();
        name = title.ToStdString(wxConvUTF8);
        is_menu_bar_menu = true;
    }

    CHECK_COND_RET(!name.empty());
    CHECK_COND_RET(m_menu_items.contains(name));

    auto& menu_items = m_menu_items[name];
    for (const auto& item : menu_items) {
        if (item.IsSeparator()) {
            if (is_menu_bar_menu) {
                // Note: Dynamically-added separators cannot be removed from menus individually,
                // as they share the same identifier. Menu bar menus are allocated once during
                // initialization, which would result in duplicate separators being added each
                // time the menu is displayed. To prevent this issue, separator addition to
                // menu bar items is explicitly disabled.
                continue;
            }
            menu->AppendSeparator();
        } else {
            wxString label = wxString::FromUTF8(item.label);
            auto menu_id = wxXmlResource::GetXRCID(label);
            menu->Append(menu_id, label);
            if (is_menu_bar_menu) {
                m_items_added_to_menu_bar_menus[name].push_back(menu_id);
            }
            menu->Bind(wxEVT_MENU, [&item](wxCommandEvent&) { item.RunAction(); }, menu_id);
        }
    }
}

void CodeLiteLUA::OnFileSaved(clCommandEvent& event)
{
    event.Skip(); // Always call this.

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    const WriteOptions opts{.converter = nullptr, .ignore_workspace = true};
    wxString codelite_lua = FileManager::GetSettingFileFullPath("codelite.lua", opts);

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath != codelite_lua) {
        return;
    }

    InitialiseInternal();
}
