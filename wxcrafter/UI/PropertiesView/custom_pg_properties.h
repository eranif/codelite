#ifndef CUSTOM_PG_PROPS_H
#define CUSTOM_PG_PROPS_H

#include "BitmapSelectorDlg.h"
#include "BmpTextSelectorDlg.h"
#include "UI/FontPickerDlg.h"
#include "VirtualDirectorySelectorDlg.h"
#include "enter_strings_dlg.h"
#include "workspace.h"
#include "wxgui_defs.h"
#include "wxgui_helpers.h"

#include <wx/app.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/editors.h>
#include <wx/propgrid/propgrid.h>

///////////////////////////////////////////////////////////////////////
// Custom editors
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
// Multi string editor
///////////////////////////////////////////////////////////////////////

class EnterStringsDlgAdapter : public wxPGEditorDialogAdapter
{
public:
    EnterStringsDlgAdapter() = default;
    ~EnterStringsDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {
        wxUnusedVar(propGrid);
        wxString delim = ";";
        wxString str;

        wxString origlbl = property->GetLabel();
        wxString lbl = origlbl;
        if(lbl.EndsWith(":") == false) { lbl << ":"; }

        if(lbl == PROP_TOOLTIP || lbl == PROP_LABEL || lbl == PROP_MESSAGE || lbl == PROP_TITLE ||
           lbl == PROP_CB_CHOICES) {
            delim = "\\n";
        }

        bool wordsSplit = false;
        if(origlbl == PROP_KEYWORDS_SET_1 || origlbl == PROP_KEYWORDS_SET_2 || origlbl == PROP_KEYWORDS_SET_3 ||
           origlbl == PROP_KEYWORDS_SET_4 || origlbl == PROP_KEYWORDS_SET_5) {
            wordsSplit = true;
            delim = " ";
        }

        wxArrayString arr = wxCrafter::SplitByString(property->GetValueAsString(), delim, true);

        for(size_t i = 0; i < arr.GetCount(); i++) {
            str << arr.Item(i) << wxT("\n");
        }

        if(str.IsEmpty() == false) { str.RemoveLast(); }

        EnterStringsDlg dlg(wxCrafter::TopFrame(), str);
        dlg.SetMessage(wxT(""));
        if(dlg.ShowModal() == wxID_OK) {

            wxString value = dlg.GetValue();

            if(wordsSplit) {
                wxArrayString strings = wxCrafter::Split(value, wxT("\n\r; \t"), wxTOKEN_STRTOK);
                value = wxCrafter::Join(strings, delim);

            } else {
                wxArrayString strings = wxCrafter::Split(value, wxT("\n\r"), wxTOKEN_RET_EMPTY_ALL);
                value = wxCrafter::Join(strings, delim);
            }

            SetValue(wxVariant(value));
            return true;
        }
        return false;
    }
};

///////////////////////////////////////////////////////////////////////
// Font picker
///////////////////////////////////////////////////////////////////////

class FontPickerDlgAdapter : public wxPGEditorDialogAdapter
{
public:
    FontPickerDlgAdapter() = default;
    ~FontPickerDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {
        FontPickerDlg dlg(wxCrafter::TopFrame(), property->GetValueAsString());
        if(dlg.ShowModal() == wxID_OK) {
            wxString fontName = dlg.GetFontName();
            SetValue(fontName);
            return true;
        }
        return false;
    }
};

///////////////////////////////////////////////////////////////////////
// Bitmap picker
///////////////////////////////////////////////////////////////////////

class BitmapPickerDlgAdapter : public wxPGEditorDialogAdapter
{
public:
    BitmapPickerDlgAdapter() = default;
    ~BitmapPickerDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {
        BitmapSelectorDlg dlg(wxCrafter::TopFrame(), property->GetValueAsString());
        if(dlg.ShowModal() == wxID_OK) {
            wxString bmpname = dlg.GetBitmapFile();
            SetValue(bmpname);
            return true;
        }
        return false;
    }
};

class wxPG_MultiStringProperty : public wxStringProperty
{
    wxString m_value;

public:
    wxPG_MultiStringProperty(const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = "")
        : wxStringProperty(label, name, value)
    {
    }

    ~wxPG_MultiStringProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new EnterStringsDlgAdapter(); }
};

class wxPG_FontProperty : public wxStringProperty
{
    wxString m_value;

public:
    wxPG_FontProperty(const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = "")
        : wxStringProperty(label, name, value)
    {
    }

    ~wxPG_FontProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new FontPickerDlgAdapter(); }
};

///////////////////////////////////////////////////////////////////////
// Bitmap picker
///////////////////////////////////////////////////////////////////////

class wxPG_BitmapProperty : public wxStringProperty
{
    wxString m_value;

public:
    wxPG_BitmapProperty(const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = "")
        : wxStringProperty(label, name, value)
    {
    }

    ~wxPG_BitmapProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new BitmapPickerDlgAdapter(); }
};

///////////////////////////////////////////////////////////////////////
// File Picker
///////////////////////////////////////////////////////////////////////

class FilePickerDlgAdapter : public wxPGEditorDialogAdapter
{
protected:
    wxString m_projectPath;

public:
    FilePickerDlgAdapter(const wxString& projectPath)
        : wxPGEditorDialogAdapter()
        , m_projectPath(projectPath)
    {
    }

    ~FilePickerDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {

        wxString wildcard =
            wxT("PNG files (*.png)|*.png|BMP files (*.bmp)|*.bmp|GIF files (*.gif)|*.gif|All files (*)|*");
        wxString newPath = ::wxFileSelector(_("Select bitmap"), wxT(""), wxT(""), wxEmptyString, wildcard, wxFD_OPEN,
                                            wxCrafter::TopFrame());
        if(newPath.IsEmpty() == false) {
            wxFileName newFilePath(newPath);
            if(m_projectPath.IsEmpty() == false) { newFilePath.MakeRelativeTo(m_projectPath); }
            SetValue(newFilePath.GetFullPath(wxPATH_UNIX));
            return true;
        }
        return false;
    }
};

///////////////////////////////////////////////////////////////////////
// File Picker
///////////////////////////////////////////////////////////////////////

class DirPickerDlgAdapter : public wxPGEditorDialogAdapter
{
protected:
    wxString m_projectPath;

public:
    DirPickerDlgAdapter(const wxString& projectPath)
        : wxPGEditorDialogAdapter()
        , m_projectPath(projectPath)
    {
    }

    ~DirPickerDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {
        wxString newPath = ::wxDirSelector("Select a folder", wxEmptyString, wxDD_DEFAULT_STYLE, wxDefaultPosition,
                                           wxCrafter::TopFrame());
        if(newPath.IsEmpty() == false) {
            wxFileName newFilePath(newPath, "");
            if(m_projectPath.IsEmpty() == false) { newFilePath.MakeRelativeTo(m_projectPath); }
            wxString new_path = newFilePath.GetFullPath(wxPATH_UNIX);
            if(new_path.IsEmpty()) new_path = ".";
            SetValue(new_path);
            return true;
        }
        return false;
    }
};

class wxPG_DirPickerProperty : public wxStringProperty
{
    wxString m_value;
    wxString m_projectPath;

public:
    wxPG_DirPickerProperty(const wxString& label, const wxString& projectPath, const wxString& name = wxPG_LABEL,
                           const wxString& value = "")
        : wxStringProperty(label, name, value)
        , m_projectPath(projectPath)
    {
    }

    ~wxPG_DirPickerProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new DirPickerDlgAdapter(m_projectPath); }
};

class wxPG_FilePickerProperty : public wxStringProperty
{
    wxString m_value;
    wxString m_projectPath;

public:
    wxPG_FilePickerProperty(const wxString& label, const wxString& projectPath, const wxString& name = wxPG_LABEL,
                            const wxString& value = "")
        : wxStringProperty(label, name, value)
        , m_projectPath(projectPath)
    {
    }

    ~wxPG_FilePickerProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new FilePickerDlgAdapter(m_projectPath); }
};

///////////////////////////////////////////////////////////////////////
// Virtual Folder Picker
///////////////////////////////////////////////////////////////////////

class VDPickerDlgAdapter : public wxPGEditorDialogAdapter
{
protected:
    wxString m_vdPath;

public:
    VDPickerDlgAdapter(const wxString& vdPath)
        : wxPGEditorDialogAdapter()
        , m_vdPath(vdPath)
    {
    }

    ~VDPickerDlgAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {

        VirtualDirectorySelectorDlg selector(wxCrafter::TopFrame(), clCxxWorkspaceST::Get(), m_vdPath);
        if(selector.ShowModal() == wxID_OK) {
            m_vdPath = selector.GetVirtualDirectoryPath();
            SetValue(m_vdPath);
            return true;
        }
        return false;
    }
};

class wxPG_VDPickerProperty : public wxStringProperty
{
public:
    wxPG_VDPickerProperty(const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = "")
        : wxStringProperty(label, name, value)
    {
    }

    ~wxPG_VDPickerProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new VDPickerDlgAdapter(GetValueAsString()); }
};

///////////////////////////////////////////////////////////////////////
// Bitmap and Text pairs selectors
///////////////////////////////////////////////////////////////////////

class BmpTextDialogAdapter : public wxPGEditorDialogAdapter
{
protected:
    wxString m_initialValue;

public:
    BmpTextDialogAdapter(const wxString& value)
        : wxPGEditorDialogAdapter()
        , m_initialValue(value)
    {
    }

    ~BmpTextDialogAdapter() override = default;

    bool DoShowDialog(wxPropertyGrid* propGrid, wxPGProperty* property) override
    {
        BmpTextSelectorDlg selector(wxCrafter::TopFrame(), m_initialValue);
        if(selector.ShowModal() == wxID_OK) {
            m_initialValue = selector.GetValue();
            SetValue(m_initialValue);
            return true;
        }
        return false;
    }
};

class wxPG_BmpTextProperty : public wxStringProperty
{
public:
    wxPG_BmpTextProperty(const wxString& label, const wxString& name = wxPG_LABEL, const wxString& value = "")
        : wxStringProperty(label, name, value)
    {
    }

    ~wxPG_BmpTextProperty() override = default;

    // Set editor to have button
    const wxPGEditor* DoGetEditorClass() const override { return wxPGEditor_TextCtrlAndButton; }

    // Set what happens on button click
    wxPGEditorDialogAdapter* GetEditorDialog() const override { return new BmpTextDialogAdapter(GetValueAsString()); }
};

class wxPG_Colour : public wxSystemColourProperty
{
public:
    wxPG_Colour(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL,
                const wxColourPropertyValue& value = wxColourPropertyValue())
        : wxSystemColourProperty(label, name, value)
    {
    }
    ~wxPG_Colour() override = default;
    bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) override;
};

//////////////////////////////////////////////////////////////////
class wxcPGChoiceAndButtonEditor : public wxPGChoiceEditor
{
public:
    wxcPGChoiceAndButtonEditor() = default;
    ~wxcPGChoiceAndButtonEditor() override = default;
    wxString GetName() const override;

    wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                  wxPGProperty* property,
                                  const wxPoint& pos,
                                  const wxSize& size) const override;
};

#endif // CUSTOM_PG_PROPS_H
