#ifndef DATAVIEWLISTCTRLWRAPPER_H
#define DATAVIEWLISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewListCtrlWrapper : public wxcWidget
{
public:
    DataViewListCtrlWrapper();
    ~DataViewListCtrlWrapper() override = default;

public:
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;
};

#endif // DATAVIEWLISTCTRLWRAPPER_H
