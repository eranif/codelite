#ifndef FILEEXPLORERTABTOOLBAR_H
#define FILEEXPLORERTABTOOLBAR_H
#include "wxcrafter.h"

class FileExplorerTab;
class FileExplorerTabToolBar : public FileExplorerTabToolBarBase
{
    FileExplorerTab* m_fileExplorer;
    
public:
    FileExplorerTabToolBar(wxWindow* parent, FileExplorerTab* fileExplorer);
    virtual ~FileExplorerTabToolBar();
protected:
    virtual void OnFindInFilesUI(wxUpdateUIEvent& event);
};
#endif // FILEEXPLORERTABTOOLBAR_H
