#ifndef _WX_MYXH_DVTREECTRL_H_
#define _WX_MYXH_DVTREECTRL_H_

#include <wx/xrc/xmlres.h>

class wxDataViewTreeCtrl;

class MyWxDataViewTreeCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewTreeCtrlHandler();
    virtual wxObject* DoCreateResource();
    virtual bool CanHandle(wxXmlNode* node);

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewTreeCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // _WX_MYXH_DVTREECTRL_H_
