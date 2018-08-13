#ifndef DBEXPLORERFRAME_H
#define DBEXPLORERFRAME_H
#include "GUI.h"

class DbExplorerFrame : public DbExplorerFrameBase
{
public:
    DbExplorerFrame(wxWindow* parent, wxWindow* page, const wxString& label);
    virtual ~DbExplorerFrame();
};
#endif // DBEXPLORERFRAME_H
