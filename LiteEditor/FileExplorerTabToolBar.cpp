#include "FileExplorerTabToolBar.h"
#include "FileExplorerTab.h"

FileExplorerTabToolBar::FileExplorerTabToolBar(wxWindow* parent, FileExplorerTab* fileExplorer)
    : FileExplorerTabToolBarBase(parent)
    , m_fileExplorer(fileExplorer)
{
}

FileExplorerTabToolBar::~FileExplorerTabToolBar() {}

void FileExplorerTabToolBar::OnFindInFilesUI(wxUpdateUIEvent& event) 
{
    event.Enable(m_fileExplorer->GetSelectionCount());
}
