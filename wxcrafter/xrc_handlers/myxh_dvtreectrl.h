#ifndef _WX_MYXH_DVTREECTRL_H_
#define _WX_MYXH_DVTREECTRL_H_

#include <wx/xrc/xmlreshandler.h>

class wxDataViewTreeCtrl;

class MyWxDataViewTreeCtrlHandler : public wxXmlResourceHandler
{
public:
    MyWxDataViewTreeCtrlHandler();
    wxObject* DoCreateResource() override;
    bool CanHandle(wxXmlNode* node) override;

private:
    // handlers for wxListCtrl itself and its listcol and listitem children
    wxDataViewTreeCtrl* HandleListCtrl();
    void HandleListCol();
};

#endif // _WX_MYXH_DVTREECTRL_H_
