#ifndef DATAVIEWLISTCTRLWRAPPER_H
#define DATAVIEWLISTCTRLWRAPPER_H

#include "wxc_widget.h" // Base class: wxcWidget

class DataViewListCtrlWrapper : public wxcWidget
{
public:
    DataViewListCtrlWrapper();
    virtual ~DataViewListCtrlWrapper();

public:
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
    virtual void GetIncludeFile(wxArrayString& headers) const;
    virtual wxString GetWxClassName() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
};

#endif // DATAVIEWLISTCTRLWRAPPER_H
