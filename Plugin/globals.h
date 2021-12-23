//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : globals.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef GLOBALS_H
#define GLOBALS_H

#include "clThemedMenuBar.hpp"
#include "codelite_exports.h"
#include "macros.h"
#include "window_locker.h"
#include "workspace.h"

#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/ctrlsub.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/infobar.h>
#include <wx/propgrid/propgrid.h>
#include <wx/string.h>
#include <wx/variant.h>

class wxDataViewCtrl;
class IManager;
class wxStyledTextCtrl;
class IProcess;
class IProcessCallback;
class wxListCtrl;
class IEditor;
class IManager;

typedef void (wxObject::*clEventFunc_t)(wxClientData* arg);

class WXDLLIMPEXP_SDK BOM
{
    wxMemoryBuffer m_bom;

public:
    BOM(const char* buffer, size_t len);
    BOM();
    ~BOM();

    void Clear();
    int Len() const;
    wxFontEncoding Encoding();
    static wxFontEncoding Encoding(const char* buff);
    void SetData(const char* buffer, size_t len);
    void* GetData() const { return m_bom.GetData(); }
    bool IsEmpty() const { return m_bom.IsEmpty(); }
};

class WXDLLIMPEXP_SDK clEventDisabler
{
public:
    clEventDisabler();
    ~clEventDisabler();
};

/**
 * \brief send command event to the application (wxTheApp),
 * \param eventId
 * \param clientData any user data. Must NOT be freed by the handler
 * \return same as wxTheApp->ProcessEvent()
 */
WXDLLIMPEXP_SDK bool SendCmdEvent(int eventId, void* clientData = NULL);

/**
 * @brief send command event to the application (wxTheApp),
 * @param eventId
 * @param clientData any user data. Must NOT be freed by the handler
 * @param str this string will be available by the handlers by calling event.GetString()
 * @return same as wxTheApp->ProcessEvent()
 */
WXDLLIMPEXP_SDK bool SendCmdEvent(int eventId, void* clientData, const wxString& str);

/**
 * \brief post command event to the application (wxTheApp),
 * \param eventId
 * \param clientData allocated data on the heap. Must be freed by the handler
 */
WXDLLIMPEXP_SDK void PostCmdEvent(int eventId, void* clientData = NULL);

/**
 * \brief set column text
 * \param list the list
 * \param indx row ID
 * \param column column ID
 * \param rText the text
 * \param imgId image id
 */
WXDLLIMPEXP_SDK void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText,
                                   int imgId = wxNOT_FOUND);

/**
 * \brief return column's text
 * \param list the list control
 * \param index the row ID
 * \param column the column ID
 * \return the column's text
 */
WXDLLIMPEXP_SDK wxString GetColumnText(wxListCtrl* list, long index, long column);

/**
 * \brief append row to list control
 * \param list the list
 * \return new row index
 */
WXDLLIMPEXP_SDK long AppendListCtrlRow(wxListCtrl* list);

/**
 * \brief read file from disk using appropriate file conversion
 * \param fileName file name
 * \param content output string
 * \param encoding
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool ReadFileWithConversion(const wxString& fileName, wxString& content,
                                            wxFontEncoding encoding = wxFONTENCODING_DEFAULT, BOM* bom = NULL);

/**
 * \brief write file using UTF8 converter
 * \param fileName file path
 * \param content file's content
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool WriteFileUTF8(const wxString& fileName, const wxString& content);

/**
 * \brief compare a file with a wxString using md5
 * \param filePath file's full path
 * \param str a wxString, perhaps containing an editor's content
 * \return true if the current content of the file is identical to str, false otherwise
 */
WXDLLIMPEXP_SDK bool CompareFileWithString(const wxString& filePath, const wxString& str);

/**
 * \brief delete directory using shell command
 * \param path directory path
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool RemoveDirectory(const wxString& path);

/**
 * \brief return true of id is a valid cpp identifier
 */
WXDLLIMPEXP_SDK bool IsValidCppIndetifier(const wxString& id);

/**
 * \brief return true of word is a C++ keyword
 * \param word
 * \return
 */
WXDLLIMPEXP_SDK bool IsCppKeyword(const wxString& word);

/**
 * \brief return true of id is a valid cpp file
 */
WXDLLIMPEXP_SDK bool IsValidCppFile(const wxString& id);

/**
 * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
 * Expand variables to their real value, if expanding fails
 * the return value is same as input. The variable is expanded
 * in the project context
 */
WXDLLIMPEXP_SDK wxString ExpandVariables(const wxString& expression, ProjectPtr proj, IEditor* editor,
                                         const wxString& filename = wxEmptyString);

/**
 * * [DEPRECATED] DONT USE THIS METHOD ANYMORE - USE IMacroManager
 * \brief accepts expression string and expand all known marcos (e.g. $(ProjectName))
 * \param expression expression
 * \param projectName project name (to be used for $(ProjectName) macro)
 * \param fileName file name, to help expand the $(CurrentFile) macro family
 * \return an expanded string. If a macro is unknown it is replaced by empty string
 */
WXDLLIMPEXP_SDK wxString ExpandAllVariables(const wxString& expression, clCxxWorkspace* workspace,
                                            const wxString& projectName, const wxString& selConf,
                                            const wxString& fileName);

/**
 * \brief copy entire directory content (recursievly) from source to target
 * \param src source path
 * \param target target path
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool CopyDir(const wxString& src, const wxString& target);

/**
 * \brief create a directory
 * \param path directory path
 */
WXDLLIMPEXP_SDK void Mkdir(const wxString& path);

/**
 * \brief write file content with optinal backup
 * \param file_name
 * \param content
 * \param backup
 * \return true on success, false otherwise
 */
WXDLLIMPEXP_SDK bool WriteFileWithBackup(const wxString& file_name, const wxString& content, bool backup);

/**
 * \brief copy text to the clipboard
 * \return true on success false otherwise
 */
WXDLLIMPEXP_SDK bool CopyToClipboard(const wxString& text);

/**
 * \brief make colour lighter
 * \param color
 * \param level
 * \return modified colour
 */
WXDLLIMPEXP_SDK wxColour MakeColourLighter(wxColour color, float level);

/**
 * @brief return true if filename is readonly false otherwise
 */
WXDLLIMPEXP_SDK bool IsFileReadOnly(const wxFileName& filename);

/**
 * \brief fill an array with a semi-colon separated string
 * \param arr [out] the array to fill
 * \param str the string to split
 */
WXDLLIMPEXP_SDK void FillFromSmiColonString(wxArrayString& arr, const wxString& str);

/**
 * \brief return a string semi-colon separated of the given array
 */
WXDLLIMPEXP_SDK wxString ArrayToSmiColonString(const wxArrayString& array);

/**
 * \brief Remove all semi colons of the given string
 */
WXDLLIMPEXP_SDK void StripSemiColons(wxString& str);

/**
 * \brief Normalize the given path (change all \ by /)
 */
WXDLLIMPEXP_SDK wxString NormalizePath(const wxString& path);

/**
 * \brief Returns the file modification time in seconds after the epoch.
 */
WXDLLIMPEXP_SDK time_t GetFileModificationTime(const wxString& filename);
WXDLLIMPEXP_SDK time_t GetFileModificationTime(const wxFileName& filename);

/**
 * @brief wrap a given command in the shell command (e.g. cmd /c "command")
 */
WXDLLIMPEXP_SDK void WrapInShell(wxString& cmd);

/**
 * @brief return the current user name without any special characters
 * @return
 */
WXDLLIMPEXP_SDK wxString clGetUserName();

/**
 * @brief return list of projects available based on the installed tempaltes
 * @param list list of projects
 * @param imageMap when provided, returns the image index (set in the lstImages) mapped to the project type
 */
WXDLLIMPEXP_SDK void GetProjectTemplateList(std::list<ProjectPtr>& list);

/**
 * @brief set the native Windows theme for the application
 * @param win [input]
 */
WXDLLIMPEXP_SDK void MSWSetNativeTheme(wxWindow* win, const wxString& theme = wxT("Explorer"));

/**
 * @brief under Windows 10 and later, enable dark mode controls (where it is implemented)
 * based on the selected editor theme. This is dont recursievly on win
 */
WXDLLIMPEXP_SDK void MSWSetWindowDarkTheme(wxWindow* win);

/**
 * @brief make relative only if a subpath of reference_path (or is reference_path itself)
 * @brief also, make normalise first, and abolish any symlink
 * @param fn wxFileName to alter
 * @param reference_path the path to which to make relative
 */
WXDLLIMPEXP_SDK bool MakeRelativeIfSensible(wxFileName& fn, const wxString& reference_path);

/**
 * @brief joins array element into a string using 'glue' as the array elements
 * separator
 */
WXDLLIMPEXP_SDK wxString wxImplode(const wxArrayString& arr, const wxString& glue = wxT("\n"));

/**
 * @brief executes a command under the proper shell and return string as the output
 */
WXDLLIMPEXP_SDK wxString wxShellExec(const wxString& cmd, const wxString& projectName);

/**
 * @class StringManager
 * @brief Stores unlocalised strings for serialisation, while managing localised ones in the gui
 */
class WXDLLIMPEXP_SDK StringManager
{
protected:
    wxArrayString m_unlocalisedStringArray;
    wxControlWithItems* p_control;
    size_t m_size;

public:
    StringManager()
        : m_size(0)
    {
    }

    /**
     * @brief Store the data, and load the strings into the control, localised
     * @param size size of the string array
     * @param strings the string array
     * @param current the option currently used
     * @param control the gui element, probably a wxChoice
     */
    void AddStrings(size_t size, const wxString* const strings, const wxString& current, wxControlWithItems* control);
    /**
     * @brief Returns the unlocalised string corresponding to the selection
     * @return an unlocalised string
     */
    wxString GetStringSelection() const;
    /**
     * @brief Sets m_control's selection to that corresponding to the unlocalised string
     * @param str an unlocalised string
     * @param dfault the default used if str is not found
     */
    void SetStringSelection(const wxString& str, size_t dfault = 0);
};

/**
 * @brief Prepends the wxString to the wxArrayString, first removing any matching entry
 * @param oldarray contains any entries
 * @param str the new entry
 * @param maxsize the maximum number of items allowed in the arraystring. 0 means no maximum
 * @return the amended entries
 */
WXDLLIMPEXP_SDK wxArrayString ReturnWithStringPrepended(const wxArrayString& oldarray, const wxString& str,
                                                        const size_t maxsize);

/**
 * @brief return true if filename is a symbolic link
 */
WXDLLIMPEXP_SDK bool wxIsFileSymlink(const wxFileName& filename);

/**
 * @brief convert filename to the real path if filename is a symbolic link
 */
WXDLLIMPEXP_SDK wxFileName wxReadLink(const wxFileName& filename);

/**
 * @brief makes-absolute filepath, and dereferences it and any symlinked dirs in the path
 */
WXDLLIMPEXP_SDK wxString CLRealPath(const wxString& filepath);

/**
 * @brief convert string to integer using range validation and default value
 */
WXDLLIMPEXP_SDK int wxStringToInt(const wxString& str, int defval, int min = -1, int max = -1);

/**
 * @brief convert integer to string
 */
WXDLLIMPEXP_SDK wxString wxIntToString(int val);

WXDLLIMPEXP_SDK unsigned int clUTF8Length(const wchar_t* uptr, unsigned int tlen);

WXDLLIMPEXP_SDK wxString DbgPrependCharPtrCastIfNeeded(const wxString& expr, const wxString& exprType);

/**
 * @brief create wxVariant from wxString + wxBitmap
 */
WXDLLIMPEXP_SDK wxVariant MakeIconText(const wxString& text, const wxBitmap& bmp);

/**
 * @brief create wxVariant from wxString + int
 */
WXDLLIMPEXP_SDK wxVariant MakeBitmapIndexText(const wxString& text, int imgIndex);

/**
 * @brief create wxVariant from label+checkbox+imgindex
 */
WXDLLIMPEXP_SDK wxVariant MakeCheckboxVariant(const wxString& label, bool checked, int imgIndex);

/**
 * @brief queue a call to a function to be executed on the next event loop
 */
WXDLLIMPEXP_SDK void PostCall(wxObject* instance, clEventFunc_t func, wxClientData* arg);
WXDLLIMPEXP_SDK void PostCall(wxObject* instance, clEventFunc_t func);

/**
 * @brief split lines (using CR|LF as the separator), taking into considertaion line continuation
 * @param trim trim the lines with set to true
 */
WXDLLIMPEXP_SDK wxArrayString SplitString(const wxString& inString, bool trim = true);

/**
 * @brief launch terminal for debugging purposes and return its TTY. This function does nothing under Windows
 */
WXDLLIMPEXP_SDK void LaunchTerminalForDebugger(const wxString& title, wxString& tty, wxString& realPts, long& pid);

/**
 * @brief prompt the user with a wxRichMessageDialog with a checkbox "Don't show this message again"
 * @param message the message to show to the user
 * @param checkboxLabel the message to display next to the checkbox
 * @param dlgId a unique string ID which will be used for storing the user value in case he checks the 'dont annoy me
 * again' checkbox
 * @param yesLabel set a different label to the "Yes" button
 * @param noLabel set a different label to the "No" button
 * @param cancelLabel set a different label to the "Cancel" button
 * @param style the wxRichMessageDialog style
 * @param checkboxInitialValue
 * @return wxRichMessageDialog::ShowModal() return value
 */
WXDLLIMPEXP_SDK wxStandardID PromptForYesNoCancelDialogWithCheckbox(
    const wxString& message, const wxString& dlgId, const wxString& yesLabel = _("Yes"),
    const wxString& noLabel = _("No"), const wxString& cancelLabel = _("Cancel"),
    const wxString& checkboxLabel = _("Remember my answer and don't ask me again"),
    long style = wxYES_NO | wxCANCEL | wxICON_QUESTION | wxYES_DEFAULT, bool checkboxInitialValue = false);

/**
 * @brief prompt the user with a PromptForYesNoCancelDialogWithCheckbox minus the 'Cancel' button
 * @param message the message to show to the user
 * @param checkboxLabel the message to display next to the checkbox
 * @param dlgId a unique string ID which will be used for storing the user value in case he checks the 'dont annoy me
 * again' checkbox
 * @param yesLabel set a different label to the "Yes" button
 * @param noLabel set a different label to the "No" button
 * @param style the wxRichMessageDialog style
 * @param checkboxInitialValue
 * @return wxRichMessageDialog::ShowModal() return value
 */
WXDLLIMPEXP_SDK wxStandardID PromptForYesNoDialogWithCheckbox(
    const wxString& message, const wxString& dlgId, const wxString& yesLabel = _("Yes"),
    const wxString& noLabel = _("No"), const wxString& checkboxLabel = _("Remember my answer and don't ask me again"),
    long style = wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, bool checkboxInitialValue = false);

/**
 * @brief wrap string with quotes if needed
 */
WXDLLIMPEXP_SDK wxString& WrapWithQuotes(wxString& str);

/**
 * @brief wrap string with quotes if needed
 */
WXDLLIMPEXP_SDK wxString& EscapeSpaces(wxString& str);

/**
 * @brief return an expression from a given position.
 * e.g. if the caret is on a line:
 * variable.m_name.m_value|
 * the | represents the cart, this function will return the entire expression: variable.m_name.m_value
 */
WXDLLIMPEXP_SDK wxString GetCppExpressionFromPos(long pos, wxStyledTextCtrl* ctrl, bool forCC);

/**
 * @brief save an xml document to file
 */
WXDLLIMPEXP_SDK bool SaveXmlToFile(wxXmlDocument* doc, const wxString& filename);

/**
 * @brief return true if running under Cygwin environment
 * This function returns false under Linux/OSX and under Windows it checks the
 * output of the command 'uname -s'
 */
WXDLLIMPEXP_SDK bool clIsCygwinEnvironment();

/**
 * @brief return true if running under MSYS environment
 * This function returns false under Linux/OSX and under Windows it checks the
 * output of the command 'uname -s'
 */
WXDLLIMPEXP_SDK bool clIsMSYSEnvironment();

/**
 * @brief change the command so it will run in the background
 */
WXDLLIMPEXP_SDK wxString MakeCommandRunInBackground(const wxString& cmd);

/**
 * @brief enable use of checkbox for boolean properties
 */
WXDLLIMPEXP_SDK void wxPGPropertyBooleanUseCheckbox(wxPropertyGrid* grid);

/**
 * @brief adjust the h-scroll bar of the control to fit the content
 * @param ctrl
 */
WXDLLIMPEXP_SDK void clRecalculateSTCHScrollBar(wxStyledTextCtrl* ctrl);

WXDLLIMPEXP_SDK wxString clGetTextFromUser(const wxString& title, const wxString& message,
                                           const wxString& initialValue = "", int charsToSelect = wxNOT_FOUND,
                                           wxWindow* parent = NULL);

/**
 * @brief similar to wxDirSelector, but on a remote machine
 */
WXDLLIMPEXP_SDK pair<wxString, wxString>
clRemoteFolderSelector(const wxString& title, const wxString& accountName = wxEmptyString, wxWindow* parent = NULL);

/**
 * @brief similar to wxFileSelector, but on a remote machine
 */
WXDLLIMPEXP_SDK pair<wxString, wxString> clRemoteFileSelector(const wxString& title,
                                                              const wxString& accountName = wxEmptyString,
                                                              const wxString& filter = wxEmptyString,
                                                              wxWindow* parent = NULL);
/**
 * @brief return the instance to the plugin manager. A convinience method
 */
WXDLLIMPEXP_SDK IManager* clGetManager();
/**
 * @brief set the plugin manager
 * @param manager
 */
WXDLLIMPEXP_SDK void clSetManager(IManager* manager);

/**
 * @brief remove terminal colours from buffer
 */
WXDLLIMPEXP_SDK void clStripTerminalColouring(const wxString& buffer, wxString& modbuffer);

/**
 * @brief return true if the name is valid
 */
WXDLLIMPEXP_SDK bool clIsVaidProjectName(const wxString& name);

/**
 * @brief support for Hi-Res displays
 */
WXDLLIMPEXP_SDK double clGetContentScaleFactor();

/**
 * @brief return the real bitmap size for Hi-DPI screens
 */
WXDLLIMPEXP_SDK int clGetScaledSize(int size);

/**
 * @brief return the real size for based on Window DIP
 */
WXDLLIMPEXP_SDK int clGetSize(int size, const wxWindow* win);

/**
 * @param signo singal number
 * @brief send signo to the
 * @param processID the process ID to kill
 * @param kill_whole_group kill the process group
 * @param as_superuser send the signal as superuser
 */
WXDLLIMPEXP_SDK void clKill(int processID, wxSignal signo, bool kill_whole_group = false, bool as_superuser = false);

/**
 * @brief set the new font encoding name for the editors
 */
WXDLLIMPEXP_SDK void clSetEditorFontEncoding(const wxString& encoding);

/**
 * @brief locate an executable on the system using the PATH environment variable
 * @param name the exe name to locate (you can ommit the .exe on Windows, it will be added automatically)
 * @param exepath [output]
 * @param hint list of directories to search
 */
WXDLLIMPEXP_SDK bool clFindExecutable(const wxString& name, wxFileName& exepath, const wxArrayString& hint = {},
                                      const wxArrayString& suffix_list = {});

/**
 * @brief given a menu and an item ID, return its position
 * @return return the position or wxNOT_FOUND
 */
WXDLLIMPEXP_SDK int clFindMenuItemPosition(wxMenu* menu, int menuItemId);

/**
 * @brief an efficient way to tokenize string into words (separated by SPACE and/or TAB)
 * @code
 * wxString str = "My String That Requires Tokenize";
 * wxString word; // The output
 * size_t offset = 0;
 * while(clNextWord(str, offset, word)) {
 *      // Do something with "word" here
 * }
 * @codeend
 * @param str the string to tokenize
 * @param offset used internally, allocate one on the stack and initialise it to 0
 * @param word [output]
 * @return true if a word was found
 */
WXDLLIMPEXP_SDK bool clNextWord(const wxString& str, size_t& offset, wxString& word);

/**
 * @brief join strings with "\n" or "\r\n" (depends on eol)
 * eol can be wxSTC_EOL_CRLF, wxSTC_EOL_LF etc
 */
WXDLLIMPEXP_SDK wxString clJoinLinesWithEOL(const wxArrayString& lines, int eol);

/**
 * @brief fit the dataview columns width to match their content
 */
WXDLLIMPEXP_SDK void clFitColumnWidth(wxDataViewCtrl* ctrl);

/**
 * @brief return the screen size, taking dual or more screens into account
 */
WXDLLIMPEXP_SDK wxSize clGetDisplaySize();

/**
 * @brief returna top level window best size using its parent's size as reference
 */
WXDLLIMPEXP_SDK void clSetTLWindowBestSizeAndPosition(wxWindow* win);

/**
 * @brief set dialog best size and position
 * @param win
 */
WXDLLIMPEXP_SDK void clSetDialogBestSizeAndPosition(wxDialog* win);

/**
 * @brief similar to clSetDialogBestSizeAndPosition but use a smaller default size
 */
WXDLLIMPEXP_SDK void clSetSmallDialogBestSizeAndPosition(wxDialog* win);

/**
 * @brief set a dialog size and position. Ratio is the size of the dialog compared to its parent
 * ration <= 0.0 the same size as the parent
 */
WXDLLIMPEXP_SDK void clSetDialogSizeAndPosition(wxDialog* win, double ratio);

/**
 * @brief return true if a C++ workspace is opened
 */
WXDLLIMPEXP_SDK bool clIsCxxWorkspaceOpened();

/**
 * @brief return true if this is a Wayland session
 */
WXDLLIMPEXP_SDK bool clIsWaylandSession();

#endif // GLOBALS_H
