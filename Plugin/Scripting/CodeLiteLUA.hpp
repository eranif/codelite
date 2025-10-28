#pragma once
#include "clResult.hpp"
#include "cl_command_event.h"
extern "C" {
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
}

#include "LuaBridge/LuaBridge.h"
#include "codelite_exports.h"

#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/string.h>

struct WXDLLIMPEXP_SDK LuaMenuItem {
    std::string label;
    luabridge::LuaRef action;
    /**
     * @brief Executes the stored action if it is a valid function.
     *
     * The method checks whether `action.isFunction()` returns true before invoking
     * `action()`. If the action throws a `std::exception`, the exception is caught,
     * a warning is logged, and an error message box is displayed containing the
     * action's label and the exception message.
     *
     * @throws std::exception If the invoked action throws (the exception is handled
     *         internally by logging and showing a message box).
     */
    inline void RunAction()
    {
        try {
            if (!action.isFunction()) {
                return;
            }
            action();
        } catch (const std::exception& e) {
            wxString errmsg;
            errmsg << "Failed to execute LUA menu action: " << label << ". " << e.what();
            clWARNING() << errmsg << endl;
            ::wxMessageBox(errmsg, "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
        }
    }
    /**
     * @brief Checks if the object is in a valid state.
     *
     * Validates that the label is not empty and that the action is not null
     * and is a valid function.
     *
     * @return true if the label is non-empty and action is a valid function, false otherwise
     */
    inline bool IsOk() const { return !label.empty() && action.isFunction(); }
};

class WXDLLIMPEXP_SDK CodeLiteLUA : public wxEvtHandler
{
public:
    static CodeLiteLUA& Get();

    // Non-copyable
    CodeLiteLUA& operator=(const CodeLiteLUA&) = delete;
    CodeLiteLUA(const CodeLiteLUA&) = delete;

    /**
     * @brief Executes a Lua script string in the current Lua state.
     *
     * This function loads and executes the provided Lua script string. It first
     * attempts to load the script using luaL_loadstring, and if successful,
     * executes it using lua_pcall. Any errors during loading or execution are
     * captured and returned as a status object.
     *
     * @param script The Lua script string to execute
     * @return clStatus StatusOk() if execution succeeds, or StatusOther() with
     *         error message if loading or execution fails
     */
    clStatus Run(const wxString& script);
    clStatus RunFile(const wxString& path);

    /**
     * @brief Updates a menu by appending menu items from the internal menu_items collection.
     *
     * This function looks up menu items associated with the given menu name and appends them to the provided
     * wxMenu object. Each menu item is bound to an event handler that executes the associated Lua action
     * function when triggered.
     *
     * @param menu_name The name/identifier of the menu to update
     * @param menu Pointer to the wxMenu object to be populated with menu items
     */
    void UpdateMenu(const wxString& menu_name, wxMenu* menu);

    /**
     * @brief Initializes the CodeLiteLUA singleton instance by calling its internal initialization method.
     */
    static void Initialise();

protected:
    /**
     * @brief Displays a message dialog box with the specified message and type.
     *
     * This function shows a wxWidgets message box with "CodeLite Lua" as the title.
     * The appearance and icon of the dialog depend on the message type provided.
     *
     * @param message The message text to display in the dialog (UTF-8 encoded string)
     * @param type The type of message dialog to display (MessageType::kWarn, MessageType::kError, or
     * MessageType::kInfo)
     */
    static void message_box(const std::string& message, int type);

    /**
     * @brief Adds a menu item with an associated Lua action to a specified menu.
     *
     * This function registers a new menu item under the given menu name. If the menu
     * does not exist in the internal menu items map, it will be created. The menu item
     * consists of a label and a Lua callback function that will be executed when the
     * menu item is triggered.
     *
     * @param menu_name the name of the menu to which the item will be added
     * @param label the display label for the menu item
     * @param action a Lua function reference that will be called when the menu item is activated
     */
    static void add_menu_item(const std::string& menu_name, const std::string& label, luabridge::LuaRef action);

private:
    CodeLiteLUA();
    ~CodeLiteLUA();

    /**
     * @brief Initializes the CodeLite LUA environment by resetting the state and loading the main codelite.lua script.
     *
     * This method resets the LUA environment, locates and reads the codelite.lua settings file,
     * and attempts to execute it. If the file cannot be read or execution fails, an error
     * message is displayed to the user.
     */
    void InitialiseInternal();

    /**
     * @brief Reset the CodeLiteLUA object by clearing all menu items.
     */
    void Reset();
    /**
     * @brief Handles the file saved event and reinitializes the plugin if the codelite.lua configuration file was
     * saved.
     *
     * This function checks if the saved file is the codelite.lua settings file. If it is,
     * the plugin is reinitialized to apply the new configuration. The event is always skipped
     * to allow other handlers to process it.
     *
     * @param event The command event triggered when a file is saved
     */
    void OnFileSaved(clCommandEvent& event);
    lua_State* m_state{nullptr};
    std::unordered_map<std::string, std::vector<LuaMenuItem>> m_menu_items;
};