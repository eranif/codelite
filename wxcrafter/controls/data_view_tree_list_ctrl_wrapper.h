#ifndef DATAVIEWTREELISTCTRLWRAPPER_H
#define DATAVIEWTREELISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewTreeListCtrlWrapper : public wxcWidget
{
public:
    DataViewTreeListCtrlWrapper();
    ~DataViewTreeListCtrlWrapper() override = default;

protected:
    wxString DoGenerateClassMember() const override;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
    void GenerateAdditionalFiles(wxStringMap_t& extraFile) const override;
    wxString AssociateModelCode() const;
    wxString GetModelName() const;
};

#endif // DATAVIEWTREELISTCTRLWRAPPER_H
