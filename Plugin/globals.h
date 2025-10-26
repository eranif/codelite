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

#include "Notebook.h"
#include "codelite_exports.h"
#include "imanager.h"
#include "window_locker.h"
#include "workspace.h"

#include <wx/arrstr.h>
#include <wx/bitmap.h>
#include <wx/ctrlsub.h>
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
    BOM() = default;
    ~BOM() = default;

    void Clear();
    int Len() const;
    wxFontEncoding Encoding();
    static wxFontEncoding Encoding(const char* buff);
    void SetData(const char* buffer, size_t len);
    void* GetData() const { return m_bom.GetData(); }
    bool IsEmpty() const { return m_bom.IsEmpty(); }
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
WXDLLIMPEXP_SDK void
SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText, int imgId = wxNOT_FOUND);

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
WXDLLIMPEXP_SDK bool ReadFileWithConversion(const wxString& fileName,
                                            wxString& content,
                                            wxFontEncoding encoding = wxFONTENCODING_DEFAULT,
                                            BOM* bom = NULL);

/**
 * \brief compare a file with a wxString using md5
 * \param filePath file's full path
 * \param str a wxString, perhaps containing an editor's content
 * \return true if the current content of the file is identical to str, false otherwise
 */
WXDLLIMPEXP_SDK bool CompareFileWithString(const wxString& filePath, const wxString& str);

/**
 * \brief return true of id is a valid cpp identifier
 */
WXDLLIMPEXP_SDK bool IsValidCppIdentifier(const wxString& id);

/**
 * \brief write file content with optional backup
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
 * @brief return the current user name without any special characters
 * @return
 */
WXDLLIMPEXP_SDK wxString clGetUserName();

/**
 * @brief set the native Windows theme for the application
 * @param win [input]
 */
WXDLLIMPEXP_SDK void MSWSetNativeTheme(wxWindow* win, const wxString& theme = "Explorer");

/**
 * @brief under Windows 10 and later, enable dark mode controls (where it is implemented)
 * based on the selected editor theme. This is don't recursively on win
 */
WXDLLIMPEXP_SDK void MSWSetWindowDarkTheme(wxWindow* win);

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
WXDLLIMPEXP_SDK wxArrayString ReturnWithStringPrepended(const wxArrayString& oldarray,
                                                        const wxString& str,
                                                        const size_t maxsize);

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
 * @brief prompt the user with a wxRichMessageDialog with a checkbox "Don't show this message again"
 * @param message the message to show to the user
 * @param checkboxLabel the message to display next to the checkbox
 * @param dlgId a unique string ID which will be used for storing the user value in case he checks the don't annoy me
 * again' checkbox
 * @param yesLabel set a different label to the "Yes" button
 * @param noLabel set a different label to the "No" button
 * @param cancelLabel set a different label to the "Cancel" button
 * @param style the wxRichMessageDialog style
 * @param checkboxInitialValue
 * @return wxRichMessageDialog::ShowModal() return value
 */
WXDLLIMPEXP_SDK wxStandardID
PromptForYesNoCancelDialogWithCheckbox(const wxString& message,
                                       const wxString& dlgId,
                                       const wxString& yesLabel = _("Yes"),
                                       const wxString& noLabel = _("No"),
                                       const wxString& cancelLabel = _("Cancel"),
                                       const wxString& checkboxLabel = _("Remember my answer and don't ask me again"),
                                       long style = wxYES_NO | wxCANCEL | wxICON_QUESTION | wxYES_DEFAULT,
                                       bool checkboxInitialValue = false);

/**
 * @brief prompt the user with a PromptForYesNoCancelDialogWithCheckbox minus the 'Cancel' button
 * @param message the message to show to the user
 * @param checkboxLabel the message to display next to the checkbox
 * @param dlgId a unique string ID which will be used for storing the user value in case he checks the don't annoy me
 * again' checkbox
 * @param yesLabel set a different label to the "Yes" button
 * @param noLabel set a different label to the "No" button
 * @param style the wxRichMessageDialog style
 * @param checkboxInitialValue
 * @return wxRichMessageDialog::ShowModal() return value
 */
WXDLLIMPEXP_SDK wxStandardID
PromptForYesNoDialogWithCheckbox(const wxString& message,
                                 const wxString& dlgId,
                                 const wxString& yesLabel = _("Yes"),
                                 const wxString& noLabel = _("No"),
                                 const wxString& checkboxLabel = _("Remember my answer and don't ask me again"),
                                 long style = wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT,
                                 bool checkboxInitialValue = false);

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
 * @brief enable use of checkbox for boolean properties
 */
WXDLLIMPEXP_SDK void wxPGPropertyBooleanUseCheckbox(wxPropertyGrid* grid);

/**
 * @brief adjust the h-scroll bar of the control to fit the content
 * @param ctrl
 */
WXDLLIMPEXP_SDK void clRecalculateSTCHScrollBar(wxStyledTextCtrl* ctrl);

WXDLLIMPEXP_SDK wxString clGetTextFromUser(const wxString& title,
                                           const wxString& message,
                                           const wxString& initialValue = "",
                                           int charsToSelect = wxNOT_FOUND,
                                           wxWindow* parent = NULL);

/**
 * @brief similar to wxDirSelector, but on a remote machine
 */
WXDLLIMPEXP_SDK std::pair<wxString, wxString>
clRemoteFolderSelector(const wxString& title, const wxString& accountName = wxEmptyString, wxWindow* parent = NULL);

/**
 * @brief similar to wxFileSelector, but on a remote machine
 */
WXDLLIMPEXP_SDK std::pair<wxString, wxString> clRemoteFileSelector(const wxString& title,
                                                                   const wxString& accountName = wxEmptyString,
                                                                   const wxString& filter = wxEmptyString,
                                                                   wxWindow* parent = NULL);
/**
 * @brief return the instance to the plugin manager. A convenience method
 */
WXDLLIMPEXP_SDK IManager* clGetManager();
/**
 * @brief set the plugin manager
 * @param manager
 */
WXDLLIMPEXP_SDK void clSetManager(IManager* manager);

/**
 * @brief return true if the name is valid
 */
WXDLLIMPEXP_SDK bool clIsValidProjectName(const wxString& name);

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
 * @param signo signal number
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
 * @brief given a menu and an item ID, return its position
 * @return return the position or wxNOT_FOUND
 */
WXDLLIMPEXP_SDK int clFindMenuItemPosition(wxMenu* menu, int menuItemId);

/**
 * @brief return the screen size, taking dual or more screens into account
 */
WXDLLIMPEXP_SDK wxSize clGetDisplaySize();

/**
 * @brief return a top level window best size using its parent's size as reference
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

/**
 * @brief get list of file types from the user
 */
WXDLLIMPEXP_SDK bool
clShowFileTypeSelectionDialog(wxWindow* parent, const wxArrayString& initial_selection, wxArrayString* selected);

/// Find the best window starting from `win` and give it the focus
WXDLLIMPEXP_SDK bool SetBestFocus(wxWindow* win);

/// Find Notebook parent of a `child`
WXDLLIMPEXP_SDK Notebook* FindNotebookParentOf(wxWindow* child);

/// Return true if `child` is a child (does not have to be a direct child) of `parent`
WXDLLIMPEXP_SDK bool IsChildOf(wxWindow* child, wxWindow* parent);

/// Return the selected text in a wxSTC. This function ensures that only
/// visible text is returned (for example, if the selected text contains
/// ANSI code style hidden - it will be dropped)
WXDLLIMPEXP_SDK wxString clGetVisibleSelection(wxStyledTextCtrl* ctrl);

/// Parse `clang_format_content` content and return the property's "name" value.
WXDLLIMPEXP_SDK int GetClangFormatIntProperty(const wxString& clang_format_content, const wxString& name);

#endif // GLOBALS_H
