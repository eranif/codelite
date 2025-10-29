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
    clStatus LoadScriptString(const wxString& script);

    /**
     * @brief Executes a Lua script file.
     *
     * Reads the content of the file specified by @a path. If the file cannot be read,
     * returns a @c StatusNotFound error. Otherwise, the content is passed to @c LoadScriptString
     * for execution, and the resulting status is returned.
     *
     * @param path The full path to the Lua script file to be executed.
     *
     * @return A @c clStatus indicating the result of the operation:
     *         - @c StatusNotFound if the file could not be read.
     *         - The status returned by @c LoadScriptString for the executed script.
     */
    clStatus LoadScriptFile(const wxString& path);

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

    /**
     * @brief Retrieves the current selection text from the active editor.
     *
     * This function obtains the active editor via the global manager. If there is
     * no active editor, it returns an empty string. Otherwise, it returns the
     * selected text converted to a UTF‑8 encoded `std::string`.
     *
     * @return A `std::string` containing the selected text, or an empty string if
     *         no editor is active.
     */
    static std::string editor_selection();

    /**
     * @brief Initiates a chat session using the given prompt.
     *
     * This method creates a menu event that triggers the chat window
     * within the application and attaches the provided prompt string
     * to the event.
     *
     * @param prompt The initial text to be displayed in the chat input.
     */
    static void chat(const std::string& prompt);
    /**
     * @brief Retrieves the programming language associated with the currently active editor.
     *
     * Obtains the active editor from the manager, determines the file type from the editor's
     * path, maps it to a language, and returns the language name encoded as a UTF‑8
     * `std::string`. If there is no active editor, an empty string is returned.
     *
     * @return The language name as a UTF‑8 `std::string`, or an empty string if no editor is active.
     */
    static std::string editor_language();

    /**
     * @brief Returns the text content of the currently active editor.
     *
     * This method retrieves the active editor from the manager and returns its
     * contents as a UTF‑8 encoded {@code std::string}. If no editor is active,
     * an empty string is returned.
     *
     * @return The full text of the active editor as a UTF‑8 string,
     *         or an empty string if there is no active editor.
     */
    static std::string editor_text();

    /**
     * @brief Retrieves the file path of the currently active editor.
     *
     * This method obtains the active editor from the manager. If an editor is
     * present, it returns the editor's remote or local file path converted to a
     * UTF‑8 encoded std::string. If no editor is active, an empty string is
     * returned.
     *
     * @return std::string The file path in UTF‑8 encoding, or an empty string if
     *         no active editor is available.
     */
    static std::string editor_filepath();
    /**
     * @brief Logs an error message using the FileLogger.
     *
     * This inline function forwards the supplied message to the
     * FileLogger at the {@link FileLogger::LogLevel::Error} level.
     * It is intended for reporting error conditions within the
     * application and should be called whenever an error needs to
     * be recorded.
     *
     * @param msg The error message to be logged.
     */
    static inline void log_error(const std::string& msg) { log_message(msg, FileLogger::LogLevel::Error); }
    /**
     * @brief Logs a system message.
     *
     * This function logs the provided message at the System log level using
     * the FileLogger. It is intended for messages that are relevant to system
     * operations and diagnostics.
     *
     * @param msg The message to be logged.
     */
    static inline void log_system(const std::string& msg) { log_message(msg, FileLogger::LogLevel::System); }
    /**
     * @brief Logs a warning message.
     *
     * This inline function forwards the provided message to the
     * underlying logging system with a warning severity level.
     *
     * @param msg The message to be logged.
     */
    static inline void log_warn(const std::string& msg) { log_message(msg, FileLogger::LogLevel::Warning); }
    /**
     * @brief Logs a debug-level message.
     *
     * This static inline helper forwards the supplied message to the underlying
     * `log_message` function, specifying the debug severity (`FileLogger::LogLevel::Dbg`).
     * It is intended for quick debugging output and has no side effects on program flow.
     *
     * @param msg The message to be logged. The string should contain information
     *            relevant for debugging purposes.
     *
     * @note The function returns no value and is inexpensive to call.
     */
    static inline void log_debug(const std::string& msg) { log_message(msg, FileLogger::LogLevel::Dbg); }
    /**
     * Logs a trace message at the Developer log level.
     *
     * This helper forwards the given message to {@code log_message} with the
     * {@code FileLogger::LogLevel::Developer} level.
     *
     * @param msg The message to log.
     */
    static inline void log_trace(const std::string& msg) { log_message(msg, FileLogger::LogLevel::Developer); }

private:
    CodeLiteLUA();
    ~CodeLiteLUA();
    /**
     * Logs a message at the specified log level using the CodeLite logger.
     *
     * <p>The message is converted from UTF-8 to a wxString and output to the
     * appropriate log stream based on the {@link FileLogger::LogLevel} value:
     * <ul>
     *   <li>{@link FileLogger::LogLevel::Developer} - logs with {@code clDEBUG1()}.</li>
     *   <li>{@link FileLogger::LogLevel::Dbg}        - logs with {@code clDEBUG()}.</li>
     *   <li>{@link FileLogger::LogLevel::Warning}   - logs with {@code clWARNING()}.</li>
     *   <li>{@link FileLogger::LogLevel::System}    - logs with {@code clSYSTEM()}.</li>
     *   <li>{@link FileLogger::LogLevel::Error}     - logs with {@code clERROR()}.</li>
     * </ul>
     *
     * @param msg   The message to log. It is expected to be a UTF-8 encoded string.
     * @param level The severity level of the log message.
     */
    static void log_message(const std::string& msg, FileLogger::LogLevel level);

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