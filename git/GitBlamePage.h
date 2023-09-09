#ifndef GITBLAMEPAGE_H
#define GITBLAMEPAGE_H

#include "cl_command_event.h"
#include "git.h"

#include <wx/stc/stc.h>

class GitBlamePage : public wxStyledTextCtrl
{
    GitPlugin* m_plugin = nullptr;

public:
    GitBlamePage(wxWindow* parent, GitPlugin* plugin);
    virtual ~GitBlamePage();

    void SetBlame(const wxString& blame, const wxString& fullpath);
};
#endif // GITBLAMEPAGE_H
