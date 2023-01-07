#ifndef _WX_MYXH_DATAVIEW_H_
#define _WX_MYXH_DATAVIEW_H_

#include <wx/xrc/xmlres.h>

class wxDataViewCtrl;

class MyWxDataViewCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewCtrlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // _WX_MYXH_DATAVIEW_H_
