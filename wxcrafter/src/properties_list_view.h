#ifndef PROPERTIESLISTVIEW_H
#define PROPERTIESLISTVIEW_H

#include "custom_pg_properties.h"
#include "wxc_widget.h"
#include "wxgui_helpers.h"
#include <map>
#include <vector>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>

class wxcWidget;
class PropertiesListView : public wxPanel
{
public:
    typedef std::vector<PropertyBase*> PropVec_t;
    enum DisplayType { Type_Control, Type_Project_Metadata };

protected:
    wxcWidget* m_wxcWidget;
    PropVec_t m_properties;
    wxPropertyGrid* m_pg;
    wxPropertyGridManager* m_pgMgr;
    DisplayType m_type;

    void DoClear();

    void OnCellChanged(wxPropertyGridEvent& e);
    void OnCellChanging(wxPropertyGridEvent& e);
    void OnPropertyGridUpdateUI(wxUpdateUIEvent& event);

public:
    PropertiesListView(wxWindow* win);
    virtual ~PropertiesListView();

    wxPGProperty* AddBoolProp(const wxString& label, bool checked, const wxString& tip);
    wxPGProperty* AddIntegerProp(const wxString& label, int value, const wxString& tip);
    wxPGProperty* AddIntegerProp(const wxString& label, const wxString& tip);
    wxPGProperty* AddTextProp(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddChoiceProp(const wxString& label, const wxArrayString& value, int sel, const wxString& tip);
    wxPGProperty* AddFilePicker(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddLongTextProp(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddColorProp(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddComboxProp(const wxString& label, const wxArrayString& options, const wxString& value,
                                const wxString& tip);
    wxPGProperty* AddFlags(const wxString& label, const wxArrayString& strings, const wxArrayInt& values, long value,
                           const wxString& tip);
    wxPGProperty* AddFontProp(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddVDPicker(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddBmpTextPicker(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddDirPicker(const wxString& label, const wxString& value, const wxString& tip);
    wxPGProperty* AddBitmapPicker(const wxString& label, const wxString& value, const wxString& tip);

    void Construct(wxcWidget* wb);
    void ConstructProjectSettings();
};

#endif // PROPERTIESLISTVIEW_H
