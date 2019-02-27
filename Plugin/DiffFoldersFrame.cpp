#include "DiffFoldersFrame.h"
#include "DiffSelectFoldersDlg.h"
#include "clFilesCollector.h"
#include <wx/dir.h>
#include <algorithm>

DiffFoldersFrame::DiffFoldersFrame(wxWindow* parent)
    : DiffFoldersBaseFrame(parent)
{
}

DiffFoldersFrame::~DiffFoldersFrame() {}

void DiffFoldersFrame::OnLeftFolderExpanding(wxTreeEvent& event) {}
void DiffFoldersFrame::OnLeftItemActivated(wxTreeEvent& event) {}
void DiffFoldersFrame::OnLeftSelectionChanged(wxTreeEvent& event) {}
void DiffFoldersFrame::OnRightFolderExpanding(wxTreeEvent& event) {}
void DiffFoldersFrame::OnRightItemActivated(wxTreeEvent& event) {}
void DiffFoldersFrame::OnRightSelectionChanged(wxTreeEvent& event) {}

void DiffFoldersFrame::OnClose(wxCommandEvent& event) { Close(); }

void DiffFoldersFrame::OnNewCmparison(wxCommandEvent& event)
{
    DiffSelectFoldersDlg dlg(this);
    if(dlg.ShowModal() == wxID_OK) {
        CallAfter(&DiffFoldersFrame::BuildTrees, dlg.GetDirPickerLeft()->GetPath(), dlg.GetDirPickerRight()->GetPath());
    }
}
#define LEFT_COL 0
#define RIGHT_COL 1

void DiffFoldersFrame::BuildTrees(const wxString& left, const wxString& right)
{
    wxBusyCursor bc;
    m_treeCtrlLeft->DeleteAllItems();
    m_treeCtrlRight->DeleteAllItems();

    wxTreeItemId leftRoot = m_treeCtrlLeft->AddRoot(left);
    wxTreeItemId rightRoot = m_treeCtrlRight->AddRoot(right);

    m_treeCtrlLeft->SetSortFunction(nullptr);
    m_treeCtrlRight->SetSortFunction(nullptr);

    wxArrayString leftFiles;
    wxArrayString rightFiles;

    // Get list of all files in the given folders
    wxDir::GetAllFiles(left, &leftFiles, wxEmptyString, wxDIR_FILES);
    wxDir::GetAllFiles(right, &rightFiles, wxEmptyString, wxDIR_FILES);

    // merge the lists and then sort them
    std::vector<std::pair<wxString, int> > mergedList;
    while(!leftFiles.IsEmpty()) {
        mergedList.push_back({ leftFiles.Item(0), LEFT_COL });
        leftFiles.RemoveAt(0, 1);
    }

    while(!rightFiles.IsEmpty()) {
        mergedList.push_back({ rightFiles.Item(0), RIGHT_COL });
        rightFiles.RemoveAt(0, 1);
    }

    std::sort(mergedList.begin(), mergedList.end());

    std::vector<std::pair<wxString, wxString> > pairs;
    while(!mergedList.empty()) {
        // an entry in the new view
        //          LEFT     RIGHT
        std::pair<wxString, wxString> p;

        auto entry = mergedList.back();
        wxString filename = entry.first;
        int col = entry.second;
        if(col == LEFT_COL) {
            // LEFT_COL
            p.first = filename;
        } else {
            // RIGHT_COL
            p.second = filename;
        }
        mergedList.pop_back();

        // Check if the next item in the list has the same file name
        if(!mergedList.empty()) {
            auto entry2 = mergedList.back();
            if(entry2.first == filename) {
                col = entry2.second;
                if(col == LEFT_COL) {
                    // LEFT_COL
                    p.first = filename;
                } else {
                    // RIGHT_COL
                    p.second = filename;
                }
                mergedList.pop_back();
            }
        }
        pairs.push_back(p);
    }

    for(size_t i = 0; i < pairs.size(); ++i) {
        const std::pair<wxString, wxString>& line = pairs[i];
        m_treeCtrlLeft->AppendItem(leftRoot, line.first);
        m_treeCtrlRight->AppendItem(rightRoot, line.second);
    }
}
