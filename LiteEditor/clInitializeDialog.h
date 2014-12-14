#ifndef CODELITEUPGRADEFRAME_H
#define CODELITEUPGRADEFRAME_H

#include "CodeLiteUpgradeFrameBase.h"
#include "cl_command_event.h"

class clInitializeDialog : public clInitializeDialogBase
{
public:
    clInitializeDialog(wxWindow* parent);
    virtual ~clInitializeDialog();
    void StartUpgrade();
    
protected:
    void OnLexersUpgradeStart(clCommandEvent &e);
    void OnLexersUpgradeProgress(clCommandEvent &e);
    void OnLexersUpgradeEnd(clCommandEvent &e);
};
#endif // CODELITEUPGRADEFRAME_H
