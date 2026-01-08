#ifndef NEWMCPSERVERDLG_HPP
#define NEWMCPSERVERDLG_HPP

#include "UI.hpp"
#include "clPropertiesPage.hpp"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK NewMCPServerDlg : public NewMCPServerDlgBase
{
public:
    NewMCPServerDlg(wxWindow* parent);
    ~NewMCPServerDlg() override;

private:
    void Initialise();
    void UpdateUIForStdio();
    void UpdateUIForSSE();
    
    clPropertiesPage* m_view{nullptr};
};

#endif // NEWMCPSERVERDLG_HPP
