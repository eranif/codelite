#ifndef DOCKEROUTPUTPANE_H
#define DOCKEROUTPUTPANE_H

#include "UI.h"
#include "clGenericSTCStyler.h"

class DockerOutputPane : public DockerOutputPaneBase
{
    clGenericSTCStyler::Ptr_t m_styler;

public:
    DockerOutputPane(wxWindow* parent);
    virtual ~DockerOutputPane();

    void Clear();
    void AddTextWithEOL(const wxString& msg);
    void AddTextRaw(const wxString& msg);
};
#endif // DOCKEROUTPUTPANE_H
