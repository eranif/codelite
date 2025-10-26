//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : globals.cpp
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
#include "globals.h"

#include "Debugger/debuggermanager.h"
#include "FileSystemWorkspace/clFileSystemWorkspace.hpp"
#include "SelectFileTypesDialog.hpp"
#include "StringUtils.h"
#include "clDataViewListCtrl.h"
#include "clGetTextFromUserDialog.h"
#include "clTreeCtrl.h"
#include "debugger.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "imanager.h"
#include "macros.h"
#include "md5/wxmd5.h"
#include "precompiled_header.h"
#include "procutils.h"
#include "workspace.h"

#include <vector>
#include <wx/app.h>
#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dcscreen.h>
#include <wx/display.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/richmsgdlg.h>
#include <wx/settings.h>
#include <wx/window.h>

#ifdef __WXMSW__
#include "MSWDarkMode.hpp"

#include <UxTheme.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#ifdef __WXGTK20__
#include <gtk/gtk.h>
#endif

#if USE_SFTP
#include "SFTPBrowserDlg.h"
#endif

const wxEventType wxEVT_COMMAND_CL_INTERNAL_0_ARGS = ::wxNewEventType();
const wxEventType wxEVT_COMMAND_CL_INTERNAL_1_ARGS = ::wxNewEventType();

void MSWSetWindowDarkTheme(wxWindow* win)
{
#if defined(__WXMSW__) && 0
    MSWDarkMode::Get().SetDarkMode(win);
#else
    wxUnusedVar(win);
#endif
}

// --------------------------------------------------------
// Internal handler to handle queuing requests... end
// --------------------------------------------------------

#if defined(__WXGTK__)
#include <dirent.h>
#include <unistd.h>
#endif

static bool IsBOMFile(const char* file_name)
{
    bool res(false);
    FILE* fp = fopen(file_name, "rb");
    if (fp) {
        struct stat buff;
        if (stat(file_name, &buff) == 0) {

            // Read the first 4 bytes (or less)
            size_t size = buff.st_size;
            if (size > 4) {
                size = 4;
            }
            char* buffer = new char[size];
            if (fread(buffer, sizeof(char), size, fp) == size) {
                BOM bom(buffer, size);
                res = (bom.Encoding() != wxFONTENCODING_SYSTEM);
            }
            delete[] buffer;
        }
        fclose(fp);
    }
    return res;
}

static bool ReadBOMFile(const char* file_name, wxString& content, BOM& bom)
{
    content.Empty();

    FILE* fp = fopen(file_name, "rb");
    if (fp) {
        struct stat buff;
        if (stat(file_name, &buff) == 0) {
            size_t size = buff.st_size;
            char* buffer = new char[size + 1];
            if (fread(buffer, sizeof(char), size, fp) == size) {
                buffer[size] = 0;

                wxFontEncoding encoding(wxFONTENCODING_SYSTEM);
                size_t bomSize(size);

                if (bomSize > 4) {
                    bomSize = 4;
                }
                bom.SetData(buffer, bomSize);
                encoding = bom.Encoding();

                if (encoding != wxFONTENCODING_SYSTEM) {
                    wxCSConv conv(encoding);
                    // Skip the BOM
                    char* ptr = buffer;
                    ptr += bom.Len();
                    content = wxString(ptr, conv);

                    if (content.IsEmpty()) {
                        content = wxString::From8BitData(ptr);
                    }
                }
            }
            delete[] buffer;
        }
        fclose(fp);
    } // From8BitData
    return content.IsEmpty() == false;
}

static bool ReadFile8BitData(const char* file_name, wxString& content)
{
    content.Empty();

    FILE* fp = fopen(file_name, "rb");
    if (fp) {
        struct stat buff;
        if (stat(file_name, &buff) == 0) {
            size_t size = buff.st_size;
            char* buffer = new char[size + 1];
            if (fread(buffer, sizeof(char), size, fp) == size) {
                buffer[size] = 0;
                content = wxString::From8BitData(buffer);
            }
            delete[] buffer;
        }
        fclose(fp);
    }
    return content.IsEmpty() == false;
}

bool SendCmdEvent(int eventId, void* clientData) { return EventNotifier::Get()->SendCommandEvent(eventId, clientData); }

bool SendCmdEvent(int eventId, void* clientData, const wxString& str)
{
    return EventNotifier::Get()->SendCommandEvent(eventId, clientData, str);
}

void PostCmdEvent(int eventId, void* clientData) { EventNotifier::Get()->PostCommandEvent(eventId, clientData); }

void SetColumnText(wxListCtrl* list, long indx, long column, const wxString& rText, int imgId)
{
    wxListItem list_item;
    list_item.SetId(indx);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list_item.SetText(rText);
    list_item.SetImage(imgId);
    list->SetItem(list_item);
}

wxString GetColumnText(wxListCtrl* list, long index, long column)
{
    wxListItem list_item;
    list_item.SetId(index);
    list_item.SetColumn(column);
    list_item.SetMask(wxLIST_MASK_TEXT);
    list->GetItem(list_item);
    return list_item.GetText();
}

bool ReadFileWithConversion(const wxString& fileName, wxString& content, wxFontEncoding encoding, BOM* bom)
{
    wxLogNull noLog;
    content.Clear();
    wxFile file(fileName, wxFile::read);

    const wxCharBuffer name = _C(fileName);
    if (file.IsOpened()) {

        // If we got a BOM pointer, test to see whether the file is BOM file
        if (bom && IsBOMFile(name.data())) {
            return ReadBOMFile(name.data(), content, *bom);
        }

        if (encoding == wxFONTENCODING_DEFAULT) {
            encoding = EditorConfigST::Get()->GetOptions()->GetFileFontEncoding();
        }

        // first try the user defined encoding (except for UTF8: the UTF8 builtin appears to be faster)
        if (encoding != wxFONTENCODING_UTF8) {
            wxCSConv fontEncConv(encoding);
            if (fontEncConv.IsOk()) {
                file.ReadAll(&content, fontEncConv);
            }
        }

        if (content.IsEmpty()) {
            // now try the Utf8
            file.ReadAll(&content, wxConvUTF8);
            if (content.IsEmpty()) {
                // try local 8 bit data
                ReadFile8BitData(name.data(), content);
            }
        }
    }
    return !content.IsEmpty();
}

bool IsValidCppIdentifier(const wxString& id)
{
    if (id.IsEmpty()) {
        return false;
    }
    // first char can be only _A-Za-z
    wxString first(id.Mid(0, 1));
    if (first.find_first_not_of("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ") != wxString::npos) {
        return false;
    }
    // make sure that rest of the id contains only a-zA-Z0-9_
    if (id.find_first_not_of("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != wxString::npos) {
        return false;
    }
    return true;
}

long AppendListCtrlRow(wxListCtrl* list)
{
    long item;
    list->GetItemCount() ? item = list->GetItemCount() : item = 0;

    wxListItem info;
    // Set the item display name
    info.SetColumn(0);
    info.SetId(item);
    item = list->InsertItem(info);
    return item;
}

bool CompareFileWithString(const wxString& filePath, const wxString& str)
{
    wxString content;
    if (!ReadFileWithConversion(filePath, content)) {
        return false;
    }

    wxString diskMD5 = wxMD5::GetDigest(content);
    wxString mem_MD5 = wxMD5::GetDigest(str);
    return diskMD5 == mem_MD5;
}

bool WriteFileWithBackup(const wxString& file_name, const wxString& content, bool backup)
{
    wxUnusedVar(backup);
    wxCSConv fontEncConv(EditorConfigST::Get()->GetOptions()->GetFileFontEncoding());
    return FileUtils::WriteFileContent(file_name, content, fontEncConv);
}

bool CopyToClipboard(const wxString& text)
{
    bool ret(true);

#if wxUSE_CLIPBOARD
    if (wxTheClipboard->Open()) {
        wxTheClipboard->UsePrimarySelection(false);
        if (!wxTheClipboard->SetData(new wxTextDataObject(text))) {
            ret = false;
        }
        wxTheClipboard->Close();
    } else {
        ret = false;
    }
#else // wxUSE_CLIPBOARD
    ret = false;
#endif
    return ret;
}

namespace
{
#ifdef __WXMSW__
/// helper method:
/// run `uname -s` command and cache the output
const wxString& __uname()
{
    static wxString uname_output;
    static bool firstTime = true;

    if (firstTime) {
        firstTime = false;
        wxFileName uname;
        if (FileUtils::FindExe("uname", uname)) {
            firstTime = false;
            clDEBUG() << "Running `uname -s`..." << endl;
            wxString cmd;
            cmd << uname.GetFullPath();
            StringUtils::WrapWithQuotes(cmd);
            cmd << " -s";
            uname_output = ProcUtils::SafeExecuteCommand(cmd);
            clDEBUG() << uname_output << endl;
        }
    }
    return uname_output;
}
#endif
} // namespace
bool clIsMSYSEnvironment()
{
#ifdef __WXMSW__
    wxString uname_output = __uname();
    return uname_output.Lower().Contains("MSYS_NT");
#else
    return false;
#endif
}

bool clIsCygwinEnvironment()
{
#ifdef __WXMSW__
    wxString uname_output = __uname();
    return uname_output.Lower().Contains("CYGWIN_NT");
#else
    return false;
#endif
}

wxString clGetUserName()
{
    wxString squashedname, name = wxGetUserId();

    // The wx doc says that 'name' may now be e.g. "Mr. John Smith"
    // So try to make it more suitable to be an extension
    name.MakeLower();
    name.Replace(" ", "_");
    for (size_t i = 0; i < name.Len(); ++i) {
        wxChar ch = name.GetChar(i);
        if ((ch < 'a' || ch > 'z') && ch != '_') {
            // Non [a-z_] character: skip it
        } else {
            squashedname << ch;
        }
    }

    return (squashedname.IsEmpty() ? wxString("someone") : squashedname);
}

void MSWSetNativeTheme(wxWindow* win, const wxString& theme)
{
#if defined(__WXMSW__) && defined(_WIN64) && 0
    SetWindowTheme((HWND)win->GetHWND(), theme.c_str(), NULL);
#endif
}

void StringManager::AddStrings(size_t size,
                               const wxString* strings,
                               const wxString& current,
                               wxControlWithItems* control)
{
    m_size = size;
    m_unlocalisedStringArray = wxArrayString(size, strings);
    p_control = control;
    p_control->Clear();

    // Add each item to the control, localising as we go
    for (size_t n = 0; n < size; ++n) {
        p_control->Append(wxGetTranslation(strings[n]));
    }

    // Select in the control the currently used string
    SetStringSelection(current);
}

wxString StringManager::GetStringSelection() const
{
    wxString selection;
    // Find which localised string was selected
    int sel = p_control->GetSelection();
    if (sel != wxNOT_FOUND) {
        selection = m_unlocalisedStringArray.Item(sel);
    }

    return selection;
}

void StringManager::SetStringSelection(const wxString& str, size_t dfault /*= 0*/)
{
    if (str.IsEmpty() || m_size == 0) {
        return;
    }
    int sel = m_unlocalisedStringArray.Index(str);
    if (sel != wxNOT_FOUND) {
        p_control->SetSelection(sel);
    } else {
        if (dfault < m_size) {
            p_control->SetSelection(dfault);
        } else {
            p_control->SetSelection(0);
        }
    }
}

wxArrayString ReturnWithStringPrepended(const wxArrayString& oldarray, const wxString& str, const size_t maxsize)
{
    wxArrayString array(oldarray);
    if (!str.empty()) {
        // Remove any pre-existing identical entry
        // This avoids duplication, and allows us to prepend the current string
        // As a result, the array will be suitable for 'recently-used-strings' situations
        int index = array.Index(str);
        if (index != wxNOT_FOUND) {
            array.RemoveAt(index);
        }
        array.Insert(str, 0);
    }

    // Truncate the array if needed
    if (maxsize) {
        while (array.GetCount() > maxsize) {
            array.RemoveAt(array.GetCount() - 1);
        }
    }
    return array;
}

////////////////////////////////////////
// BOM
////////////////////////////////////////

BOM::BOM(const char* buffer, size_t len) { m_bom.AppendData(buffer, len); }

wxFontEncoding BOM::Encoding()
{
    wxFontEncoding encoding = Encoding((const char*)m_bom.GetData());
    if (encoding != wxFONTENCODING_SYSTEM) {
        switch (encoding) {

        case wxFONTENCODING_UTF32BE:
        case wxFONTENCODING_UTF32LE:
            m_bom.SetDataLen(4);
            break;

        case wxFONTENCODING_UTF8:
            m_bom.SetDataLen(3);
            break;

        case wxFONTENCODING_UTF16BE:
        case wxFONTENCODING_UTF16LE:
        default:
            m_bom.SetDataLen(2);
            break;
        }
    }
    return encoding;
}

wxFontEncoding BOM::Encoding(const char* buff)
{
    // Support for BOM:
    //----------------------------------
    // 00 00 FE FF UTF-32, big-endian
    // FF FE 00 00 UTF-32, little-endian
    // FE FF       UTF-16, big-endian
    // FF FE       UTF-16, little-endian
    // EF BB BF    UTF-8
    //----------------------------------
    wxFontEncoding encoding = wxFONTENCODING_SYSTEM; /* -1 */

    static const char UTF32be[] = { 0x00, 0x00, (char)0xfe, (char)0xff };
    static const char UTF32le[] = { (char)0xff, (char)0xfe, 0x00, 0x00 };
    static const char UTF16be[] = { (char)0xfe, (char)0xff };
    static const char UTF16le[] = { (char)0xff, (char)0xfe };
    static const char UTF8[] = { (char)0xef, (char)0xbb, (char)0xbf };

    if (memcmp(buff, UTF32be, sizeof(UTF32be)) == 0) {
        encoding = wxFONTENCODING_UTF32BE;

    } else if (memcmp(buff, UTF32le, sizeof(UTF32le)) == 0) {
        encoding = wxFONTENCODING_UTF32LE;

    } else if (memcmp(buff, UTF16be, sizeof(UTF16be)) == 0) {
        encoding = wxFONTENCODING_UTF16BE;

    } else if (memcmp(buff, UTF16le, sizeof(UTF16le)) == 0) {
        encoding = wxFONTENCODING_UTF16LE;

    } else if (memcmp(buff, UTF8, sizeof(UTF8)) == 0) {
        encoding = wxFONTENCODING_UTF8;
    }
    return encoding;
}

void BOM::SetData(const char* buffer, size_t len)
{
    m_bom = wxMemoryBuffer();
    m_bom.SetDataLen(0);
    m_bom.AppendData(buffer, len);
}

int BOM::Len() const { return m_bom.GetDataLen(); }

void BOM::Clear()
{
    m_bom = wxMemoryBuffer();
    m_bom.SetDataLen(0);
}

wxString DbgPrependCharPtrCastIfNeeded(const wxString& expr, const wxString& exprType)
{
    static wxRegEx reConstArr("(const )?[ ]*(w)?char(_t)? *[\\[0-9\\]]*");

    bool arrayAsCharPtr = false;
    DebuggerInformation info;
    IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
    if (dbgr) {
        DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), info);
        arrayAsCharPtr = info.charArrAsPtr;
    }

    wxString newExpr;
    if (arrayAsCharPtr && reConstArr.Matches(exprType)) {
        // array
        newExpr << "(char*)" << expr;

    } else {
        newExpr << expr;
    }
    return newExpr;
}

wxVariant MakeIconText(const wxString& text, const wxBitmap& bmp)
{
    wxIcon icn;
    icn.CopyFromBitmap(bmp);
    wxDataViewIconText ict(text, icn);
    wxVariant v;
    v << ict;
    return v;
}

wxStandardID PromptForYesNoCancelDialogWithCheckbox(const wxString& message,
                                                    const wxString& dlgId,
                                                    const wxString& yesLabel,
                                                    const wxString& noLabel,
                                                    const wxString& cancelLabel,
                                                    const wxString& checkboxLabel,
                                                    long style,
                                                    bool checkboxInitialValue)
{
    int res = clConfig::Get().GetAnnoyingDlgAnswer(dlgId, wxNOT_FOUND);
    if (res == wxNOT_FOUND) {

        // User did not save his answer
        wxRichMessageDialog d(EventNotifier::Get()->TopFrame(), message, "CodeLite", style);
        d.ShowCheckBox(checkboxLabel);
        if (cancelLabel.empty()) {
            d.SetYesNoLabels(yesLabel, noLabel);
        } else {
            d.SetYesNoCancelLabels(yesLabel, noLabel, cancelLabel);
        }

        res = d.ShowModal();
        if (d.IsCheckBoxChecked() && (res != wxID_CANCEL)) {
            // store the user result
            clConfig::Get().SetAnnoyingDlgAnswer(dlgId, res);
        }
    }
    return static_cast<wxStandardID>(res);
}

wxStandardID PromptForYesNoDialogWithCheckbox(const wxString& message,
                                              const wxString& dlgId,
                                              const wxString& yesLabel,
                                              const wxString& noLabel,
                                              const wxString& checkboxLabel,
                                              long style,
                                              bool checkboxInitialValue)
{
    return PromptForYesNoCancelDialogWithCheckbox(
        message, dlgId, yesLabel, noLabel, "", checkboxLabel, style, checkboxInitialValue);
}

void wxPGPropertyBooleanUseCheckbox(wxPropertyGrid* grid)
{
    grid->SetPropertyAttributeAll(wxPG_BOOL_USE_CHECKBOX, true);

    wxColour bg_colour = clSystemSettings::GetDefaultPanelColour();
    wxColour line_colour = bg_colour.ChangeLightness(60);
    wxColour text_colour = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    wxColour caption_colour = bg_colour;

    bool is_dark = DrawingUtils::IsDark(bg_colour);
    if (is_dark) {
        line_colour = line_colour.ChangeLightness(130);
        caption_colour = caption_colour.ChangeLightness(120);

    } else {
        line_colour = line_colour.ChangeLightness(70);
        caption_colour = caption_colour.ChangeLightness(80);
    }
    grid->SetCaptionBackgroundColour(caption_colour);
    grid->SetCaptionTextColour(text_colour);
    grid->SetLineColour(line_colour);
    grid->SetMarginColour(caption_colour);
}

void clRecalculateSTCHScrollBar(wxStyledTextCtrl* ctrl)
{
    // recalculate and set the length of horizontal scrollbar
    int maxPixel = 0;
    int startLine = ctrl->GetFirstVisibleLine();
    int endLine = startLine + ctrl->LinesOnScreen();
    if (endLine >= (ctrl->GetLineCount() - 1)) {
        endLine--;
    }

    wxString text;
    for (int i = startLine; i <= endLine; i++) {
        int visibleLine = (int)ctrl->DocLineFromVisible(i); // get actual visible line, folding may offset lines
        wxString line_text = ctrl->GetLine(visibleLine);
        text = line_text.length() > text.length() ? line_text : text;
    }

    maxPixel = ctrl->TextWidth(0, text);
    if (maxPixel == 0) {
        maxPixel++; // make sure maxPixel is valid
    }

    int currentLength = ctrl->GetScrollWidth(); // Get current scrollbar size
    if (currentLength != maxPixel) {
        // And if it is not the same, update it
        ctrl->SetScrollWidth(maxPixel);
    }
}
wxString clGetTextFromUser(
    const wxString& title, const wxString& message, const wxString& initialValue, int charsToSelect, wxWindow* parent)
{
    clGetTextFromUserDialog dialog(
        parent == NULL ? EventNotifier::Get()->TopFrame() : parent, title, message, initialValue, charsToSelect);
    if (dialog.ShowModal() == wxID_OK) {
        return dialog.GetValue();
    }
    return "";
}

static IManager* s_pluginManager = NULL;

void clSetManager(IManager* manager) { s_pluginManager = manager; }
IManager* clGetManager() { return s_pluginManager; }

bool clIsValidProjectName(const wxString& name)
{
    return name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == wxString::npos;
}

double clGetContentScaleFactor()
{
    static bool once = false;
    static double res = 1.0;
    if (!once) {
        once = true;
#ifdef __WXGTK__
        GdkScreen* screen = gdk_screen_get_default();
        if (screen) {
            res = gdk_screen_get_resolution(screen) / 96.;
        }
#else
        res = (wxScreenDC().GetPPI().y / 96.);
#endif
    }
    return res;
}

int clGetScaledSize(int size)
{
    if (clGetContentScaleFactor() >= 1.5) {
        return size * 2;
    } else {
        return size;
    }
}

void clKill(int processID, wxSignal signo, bool kill_whole_group, bool as_superuser)
{
#ifdef __WXMSW__
    wxUnusedVar(as_superuser);
    ::wxKill(processID, signo, NULL, kill_whole_group ? wxKILL_CHILDREN : wxKILL_NOCHILDREN);
#else
    wxString sudoAskpass = ::wxGetenv("SUDO_ASKPASS");
    const char* sudo_path = "/usr/bin/sudo";
    if (!wxFileName::Exists(sudo_path)) {
        sudo_path = "/usr/local/bin/sudo";
    }
    if (as_superuser && wxFileName::Exists(sudo_path) && wxFileName::Exists(sudoAskpass)) {
        wxString cmd;
        cmd << sudo_path << " --askpass kill -" << (int)signo << " ";
        if (kill_whole_group) {
            cmd << "-";
        }
        cmd << processID;
        int rc = system(cmd.mb_str(wxConvUTF8).data());
        wxUnusedVar(rc);
    } else {
        ::wxKill(processID, signo, NULL, kill_whole_group ? wxKILL_CHILDREN : wxKILL_NOCHILDREN);
    }
#endif
}

void clSetEditorFontEncoding(const wxString& encoding)
{
    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    options->SetFileFontEncoding(encoding);
    EditorConfigST::Get()->SetOptions(options);
}

int clFindMenuItemPosition(wxMenu* menu, int menuItemId)
{
    if (!menu) {
        return wxNOT_FOUND;
    }
    const wxMenuItemList& list = menu->GetMenuItems();
    wxMenuItemList::const_iterator iter = list.begin();
    for (int pos = 0; iter != list.end(); ++iter, ++pos) {
        if ((*iter)->GetId() == menuItemId) {
            return pos;
        }
    }
    return wxNOT_FOUND;
}

wxSize clGetDisplaySize()
{
    // Calculate the display size only once. If the user changes the display size, he will need to restart CodeLite
    static wxSize displaySize;
    if (displaySize.GetHeight() == 0) {

        int displayWidth = ::wxGetDisplaySize().GetWidth();
        int displayHeight = ::wxGetDisplaySize().GetHeight();
        for (size_t i = 0; i < wxDisplay::GetCount(); ++i) {
            wxDisplay display(i);
            displayWidth = wxMax(display.GetClientArea().GetWidth(), displayWidth);
            displayHeight = wxMax(display.GetClientArea().GetHeight(), displayHeight);
        }
        displaySize = wxSize(displayWidth, displayHeight);
    }
    return displaySize;
}

wxVariant MakeBitmapIndexText(const wxString& text, int imgIndex)
{
    clDataViewTextBitmap tb(text, imgIndex);
    wxVariant vr;
    vr << tb;
    return vr;
}

wxVariant MakeCheckboxVariant(const wxString& label, bool checked, int imgIndex)
{
    clDataViewCheckbox cb(checked, imgIndex, label);
    wxVariant vr;
    vr << cb;
    return vr;
}

void clSetTLWindowBestSizeAndPosition(wxWindow* win)
{
    CHECK_PTR_RET(win);

    // confirm that this window is top level
    wxTopLevelWindow* tlw = dynamic_cast<wxTopLevelWindow*>(win);

    // find its parent
    wxWindow* parent_tlw = EventNotifier::Get()->TopFrame();

    if (!tlw || !parent_tlw) {
        return;
    }

    wxRect frameSize = parent_tlw->GetSize();
    frameSize.Deflate(100);
    tlw->SetMinSize(frameSize.GetSize());
    tlw->SetSize(frameSize.GetSize());
    tlw->CentreOnParent();

#if defined(__WXMAC__) || defined(__WXMSW__)
    tlw->Move(wxNOT_FOUND, parent_tlw->GetPosition().y);
#endif
    tlw->PostSizeEvent();
}

static void DoSetDialogSize(wxDialog* win, double factor)
{
    if (!win) {
        return;
    }

    if (factor <= 0.0) {
        factor = 1.0;
    }

    wxWindow* parent = win->GetParent();
    if (!parent) {
        parent = wxTheApp->GetTopWindow();
    }
    if (parent) {
        wxSize parentSize = parent->GetSize();

        double dlgWidth = (double)parentSize.GetWidth() * factor;
        double dlgHeight = (double)parentSize.GetHeight() * factor;
        parentSize.SetWidth(dlgWidth);
        parentSize.SetHeight(dlgHeight);
        win->SetSize(parentSize);
        win->GetSizer()->Layout();
        win->CentreOnParent();
#if defined(__WXMAC__) || defined(__WXMSW__)
        win->Move(wxNOT_FOUND, parent->GetPosition().y);
#endif
    }
}

std::pair<wxString, wxString>
clRemoteFolderSelector(const wxString& title, const wxString& accountName, wxWindow* parent)
{
#if USE_SFTP
    SFTPBrowserDlg dlg(parent, title, wxEmptyString, clSFTP::SFTP_BROWSE_FOLDERS, accountName);
    if (dlg.ShowModal() != wxID_OK) {
        return {};
    }
    return { dlg.GetAccount(), dlg.GetPath() };
#else
    return {};
#endif
}

std::pair<wxString, wxString>
clRemoteFileSelector(const wxString& title, const wxString& accountName, const wxString& filter, wxWindow* parent)
{
#if USE_SFTP
    SFTPBrowserDlg dlg(parent, title, filter, clSFTP::SFTP_BROWSE_FOLDERS | clSFTP::SFTP_BROWSE_FILES, accountName);
    if (dlg.ShowModal() != wxID_OK) {
        return {};
    }
    return { dlg.GetAccount(), dlg.GetPath() };
#else
    return {};
#endif
}

void clSetDialogBestSizeAndPosition(wxDialog* win) { DoSetDialogSize(win, 0.66); }

void clSetSmallDialogBestSizeAndPosition(wxDialog* win) { DoSetDialogSize(win, 0.5); }

void clSetDialogSizeAndPosition(wxDialog* win, double ratio) { DoSetDialogSize(win, ratio); }

bool clIsCxxWorkspaceOpened() { return clCxxWorkspaceST::Get()->IsOpen() || clFileSystemWorkspace::Get().IsOpen(); }

int clGetSize(int size, const wxWindow* win)
{
    if (!win) {
        return size;
    }
#ifdef __WXGTK__
    wxString dpiscale = "1.0";
    if (wxGetEnv("GDK_DPI_SCALE", &dpiscale)) {
        double scale = 1.0;
        if (dpiscale.ToDouble(&scale)) {
            double scaledSize = scale * size;
            return scaledSize;
        }
    }
#endif
#if wxCHECK_VERSION(3, 1, 0)
    return win->FromDIP(size);
#else
    return size;
#endif
}

bool clIsWaylandSession()
{
#ifdef __WXGTK__
    // Try to detect if this is a Wayland session; we have some Wayland-workaround code
    wxString sesstype("XDG_SESSION_TYPE"), session_type;
    wxGetEnv(sesstype, &session_type);
    return session_type.Lower().Contains("wayland");
#else
    return false;
#endif
}

bool clShowFileTypeSelectionDialog(wxWindow* parent, const wxArrayString& initial_selection, wxArrayString* selected)
{
    SelectFileTypesDialog dlg(parent, initial_selection);
    if (dlg.ShowModal() != wxID_OK) {
        return false;
    }

    auto res = dlg.GetValue();
    selected->swap(res);
    return true;
}

bool SetBestFocus(wxWindow* win)
{
    if (win->IsEnabled()) {
        if (dynamic_cast<wxBookCtrlBase*>(win)) {
            auto book = dynamic_cast<wxBookCtrlBase*>(win);
            if (book->GetPageCount()) {
                book->GetPage(book->GetSelection())->CallAfter(&wxWindow::SetFocus);
            }
            return true;
        } else if (dynamic_cast<Notebook*>(win)) {
            auto book = dynamic_cast<Notebook*>(win);
            if (book->GetCurrentPage()) {
                book->GetCurrentPage()->CallAfter(&wxWindow::SetFocus);
            }
            return true;
        } else if (dynamic_cast<wxStyledTextCtrl*>(win)) {
            win->CallAfter(&wxStyledTextCtrl::SetFocus);
            return true;
        } else if (dynamic_cast<clTreeCtrl*>(win)) {
            win->CallAfter(&clTreeCtrl::SetFocus);
            return true;
        }
    }

    // try the children
    auto children = win->GetChildren();
    for (auto c : children) {
        if (SetBestFocus(c)) {
            return true;
        }
    }
    return false;
}

Notebook* FindNotebookParentOf(wxWindow* child)
{
    if (!child) {
        return nullptr;
    }

    wxWindow* parent = child->GetParent();
    while (parent) {
        Notebook* book = dynamic_cast<Notebook*>(parent);
        if (book) {
            return book;
        }
        parent = parent->GetParent();
    }
    return nullptr;
}

bool IsChildOf(wxWindow* child, wxWindow* parent)
{
    if (child == nullptr || parent == nullptr) {
        return false;
    }

    wxWindow* curparent = child->GetParent();
    while (curparent) {
        if (curparent == parent) {
            return true;
        }
        curparent = curparent->GetParent();
    }
    return false;
}

wxString clGetVisibleSelection(wxStyledTextCtrl* ctrl)
{
    CHECK_PTR_RET_EMPTY_STRING(ctrl);

    int start_pos = ctrl->GetSelectionStart();
    int end_pos = ctrl->GetSelectionEnd();
    CHECK_COND_RET_EMPTY_STRING(end_pos > start_pos);

    // Make sure we only pick visible chars (embedded ANSI colour can break the selected word)
    wxString res;
    res.reserve(end_pos - start_pos + 1);
    for (; start_pos < end_pos; start_pos++) {
        if (ctrl->StyleGetVisible(ctrl->GetStyleAt(start_pos))) {
            res << (wxChar)ctrl->GetCharAt(start_pos);
        }
    }
    return res;
}

int GetClangFormatIntProperty(const wxString& clang_format_content, const wxString& name)
{
    auto lines = ::wxStringTokenize(clang_format_content, "\r\n", wxTOKEN_STRTOK);
    for (auto& line : lines) {
        auto prop = line.BeforeFirst(':');
        auto value = line.AfterFirst(':');

        prop.Trim(false).Trim();
        value.Trim(false).Trim();
        if (prop == name) {
            long nValue = wxNOT_FOUND;
            if (value.ToCLong(&nValue)) {
                return nValue;
            }
            break;
        }
    }
    return wxNOT_FOUND;
}
