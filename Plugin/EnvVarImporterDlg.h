#ifndef ENVVARIMPORTERDLG_H
#define ENVVARIMPORTERDLG_H

#include "WSImporterDlgs.h"
#include "workspace.h"
#include <set>

class EnvVarImporterDlg : public EnvVarImporterDlgBase
{
public:
    EnvVarImporterDlg(wxWindow* parent,
                      const wxString& projectName,
                      const wxString& cfgName,
                      std::set<wxString> listEnvVar,
                      BuildConfigPtr le_conf,
                      bool* showDlg);
    virtual ~EnvVarImporterDlg();

protected:
    virtual void OnImport(wxCommandEvent& event);
    virtual void OnContinue(wxCommandEvent& event);
    virtual void OnSkip(wxCommandEvent& event);

private:
    BuildConfigPtr le_conf;
    bool* showDlg;
};

#endif // ENVVARIMPORTERDLG_H
