#ifndef DATAVIEWTREELISTCTRLWRAPPER_H
#define DATAVIEWTREELISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewTreeListCtrlWrapper : public wxcWidget
{
public:
    DataViewTreeListCtrlWrapper();
    virtual ~DataViewTreeListCtrlWrapper();

protected:
    virtual wxString DoGenerateClassMember() const;

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual void GenerateAdditionalFiles(wxStringMap_t& extraFile) const;
    wxString AssociateModelCode() const;
    wxString GetModelName() const;
};

#endif // DATAVIEWTREELISTCTRLWRAPPER_H
