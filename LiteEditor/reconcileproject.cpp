#include "reconcileproject.h"
#include "windowattrmanager.h"
#include "VirtualDirectorySelectorDlg.h"
#include "workspace.h"
#include "manager.h"
#include "frame.h"
#include "tree_node.h"
#include "globals.h"
#include "event_notifier.h"
#include <wx/dirdlg.h>
#include <wx/dir.h>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/busyinfo.h>

wxString VDpathToFilepath(const wxString& vdpath)
{
    // We want to translate projectname:foo:bar to e.g. foo/bar, using the native separator
    wxString fp = vdpath.AfterFirst(':');
    fp.Replace(":", wxString(wxFILE_SEP_PATH));
    return fp;
}

class FindFilesTraverser : public wxDirTraverser
{
public:
    FindFilesTraverser(const wxString types, const wxArrayString& excludes, const wxString& projFP) : m_excludes(excludes), m_projFP(projFP) {
        m_types = wxStringTokenize(types, ";,|"); // The tooltip says use ';' but cover all bases
    }

    virtual wxDirTraverseResult OnFile(const wxString& filename) {
        if (m_types.empty()) {
            m_results.Add(filename); // No types presumably means everything
        } else {
            wxFileName fn(filename);
            for (size_t n = 0; n < m_types.GetCount(); ++n) {
                if (m_types.Item(n) == fn.GetExt()
                        || m_types.Item(n) == "*" || m_types.Item(n) == "*.*") { // Other ways to say "Be greedy"
                    fn.MakeRelativeTo(m_projFP);
                    m_results.Add(fn.GetFullPath());
                    break;
                }
            }
        }
        return wxDIR_CONTINUE;
    }

    virtual wxDirTraverseResult OnDir(const wxString& dirname) {
        // Skip this dir if it's found in the list of excludes
        wxFileName fn = wxFileName::DirName(dirname);
        if (fn.IsAbsolute()) {
            fn.MakeRelativeTo(m_projFP);
        }
        return (m_excludes.Index(fn.GetFullPath()) == wxNOT_FOUND) ? wxDIR_CONTINUE : wxDIR_IGNORE;
    }
    
    wxArrayString GetResults() {
        return m_results;
    }

private:
    wxArrayString m_types;
    wxArrayString m_results;
    const wxArrayString m_excludes;
    const wxString m_projFP;
};

ReconcileProjectDlg::ReconcileProjectDlg(wxWindow* parent, const wxString& projname)
    : ReconcileProjectDlgBaseClass(parent), m_rootPanel(NULL), m_projname(projname)
{
    m_staleFiles = new std::vector< std::pair<wxString, wxArrayString> >;
    
    m_buttonDone->SetLabel("Done");
    
    WindowAttrManager::Load(this, wxT("ReconcileProjectDlg"), NULL);
}

ReconcileProjectDlg::~ReconcileProjectDlg()
{
    delete m_staleFiles;

    WindowAttrManager::Save(this, wxT("ReconcileProjectDlg"), NULL);
}

bool ReconcileProjectDlg::LoadData()
{
    ReconcileProjectFiletypesDlg dlg(this, m_projname);
    dlg.SetData();
    if (dlg.ShowModal() != wxID_OK) {
        return false;
    }
    wxString toplevelDir, types;
    wxArrayString excludes, regexes;
    dlg.GetData(toplevelDir, types, excludes, regexes);
    m_regexes = regexes;

    wxDir dir(toplevelDir);
    if (!dir.IsOpened()) {
        return false;
    }
    
    {
    wxBusyInfo wait("Searching for files...", this);
    wxSafeYield();
    
    FindFilesTraverser traverser(types, excludes, toplevelDir);
    dir.Traverse(traverser);
    m_actualFiles = traverser.GetResults();

    // We now have all the existing files in the project dirs that match the passed filetypes. Prune those that are already known
    PruneExistingItems(toplevelDir);    
    m_actualFiles.Sort();
    }

    m_rootPanel = new ReconcileProjectPanel(GetTreebook(), "", "");
        
    // Now get root to recursively and intelligently fill each VD's panel with appropriate files
    DistributeFiles();
    Layout();

    // Finally ask the project about any files that no longer exist
    FindStaleFiles();

    size_t stalecount = m_rootPanel->GetStaleFilesCount();
    if (!m_actualFiles.GetCount() && !stalecount) {
        wxMessageBox(_("No new or stale files found. The project is up-to-date"), _("CodeLite"), wxICON_INFORMATION|wxOK, this);
        return false;
    }

    clMainFrame::Get()->SetStatusMessage(
                        wxString::Format(_("Found %i new files and %i stale ones"), (int)m_actualFiles.GetCount(), (int)stalecount), 0);
    return true;
}

void ReconcileProjectDlg::DistributeFiles()
{
    wxCHECK_RET(m_rootPanel, "No root panel");

    // In case this is a redisplay, delete all non-root pages
    for (size_t n = GetTreebook()->GetPageCount(); n > 1; --n) {
        int parentpos = GetTreebook()->GetPageParent(n-1);
        if (parentpos != wxNOT_FOUND) {
            wxStaticCast(GetTreebook()->GetPage(parentpos), ReconcileProjectPanel)->UnstoreChild(GetTreebook()->GetPage(n-1));
        }
        
        GetTreebook()->DeletePage(n-1);
    }

    // Add the child pages, one for each VD
    DisplayVirtualDirectories();

    wxArrayString remainingFiles = m_actualFiles;

    // Before processing the individual VDs, try using any regex as that'll be most likely to reflect the user's choice
    DistributeFilesByRegex(m_rootPanel, m_regexes, remainingFiles);

    m_rootPanel->SetFiles(remainingFiles);
    // Cache any that couldn't be found homes
    m_unallocatedFiles = remainingFiles;
    
    // Especially if there are lots of VDs, the user will probably want to see only those with files. Unfortunately treebook has no API for just hiding
    if (m_rootPanel->GetHideEmpties()) {
        for (size_t n = GetTreebook()->GetPageCount(); n > 1; --n) { // >1 as we never want to delete root
            if (!wxStaticCast(GetTreebook()->GetPage(n-1), ReconcileProjectPanel)->GetHasFiles()) {
                int parentpos = GetTreebook()->GetPageParent(n-1);
                if (parentpos != wxNOT_FOUND) {
                    wxStaticCast(GetTreebook()->GetPage(parentpos), ReconcileProjectPanel)->UnstoreChild(GetTreebook()->GetPage(n-1));
                }
                GetTreebook()->DeletePage(n-1);
            }
        }
    }
}

void ReconcileProjectDlg::DisplayVirtualDirectories()
{
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    ProjectTreePtr tree = proj->AsTree();
    TreeWalker<wxString, ProjectItem> walker(tree->GetRoot());

    // For adding subpages correctly, we need a way to work out the last-added page with a depth of 'n'
    std::map<int, int> depthCache;

    for ( ; !walker.End(); walker++ ) {
        ProjectTreeNode* node = walker.GetNode();
        wxString displayname(node->GetData().GetDisplayName());
        if (node->IsRoot()) {
            wxCHECK_RET(node->GetData().GetKind() == ProjectItem::TypeProject, "Project root node not a project");
            // Create a 'root' page that shows all files
            if (!GetTreebook()->GetPageCount()) { // If this is a redisplay, root will already be there
                GetTreebook()->AddPage(m_rootPanel, displayname);
            }
        } else if (node->GetData().GetKind() == ProjectItem::TypeVirtualDirectory) {
            // Should this be a top-level VD or a subdir, or...?
            // and while we're working it out, grab the chance to deduce its internal path
            size_t count = 0;
            wxString vdPath = displayname;
            ProjectTreeNode* tempnode = node->GetParent();
            while (tempnode) {
                ++count;
                vdPath = tempnode->GetData().GetDisplayName() + ':' + vdPath;
                tempnode = tempnode->GetParent();
            }
            
            ReconcileProjectPanel* panel = new ReconcileProjectPanel(GetTreebook(), node->GetData().GetDisplayName(), vdPath);
            // Save vdPath too; it's needed for FindStaleFiles()
            wxArrayString files;
            m_staleFiles->push_back(std::make_pair(vdPath, files));
            
            if (!count) {
                GetTreebook()->AddSubPage(panel, displayname);
                depthCache[count] = GetTreebook()->GetPageCount() - 1;
            } else {
                GetTreebook()->InsertSubPage(depthCache[count-1], panel, displayname);
                depthCache[count] = GetTreebook()->GetPageCount() - 1;
            }

            // Tell our parent that we exist; it'll be responsible for call our SetFiles()
            int parentPos = GetTreebook()->GetPageParent(GetTreebook()->GetPageCount() - 1);
            wxCHECK_RET(parentPos != wxNOT_FOUND, "Added a non-root page without a valid parent");
            ReconcileProjectPanel* parentPanel = wxStaticCast(GetTreebook()->GetPage(parentPos), ReconcileProjectPanel);
            parentPanel->StoreChild(panel);
        }
    }

    GetTreebook()->ExpandNode(0); // Expand the root node, otherwise the tree section may be given far too small a size, esp. if the project name is short e.g. 'CL'!
}

void ReconcileProjectDlg::PruneExistingItems(const wxString& toplevelDir)
{
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    StringSet_t knownfiles;
    proj->GetFiles(knownfiles, toplevelDir);

    for (size_t n = m_actualFiles.GetCount(); n > 0; --n) {
        wxFileName actual(m_actualFiles[n-1]);
        if (actual.IsAbsolute()) {
            actual.MakeRelativeTo(toplevelDir);
        }
        if (knownfiles.find(actual.GetFullPath()) != knownfiles.end()) {
            m_actualFiles.RemoveAt(n-1);
        }
    }
}

void ReconcileProjectDlg::FindStaleFiles()
{
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    for (size_t n = 0; n < m_staleFiles->size(); ++n) {
        wxArrayString vdStaleFiles;
        wxString vdPath = m_staleFiles->at(n).first.AfterFirst(':'); // We don't want the projectname here
        wxArrayString& vdFiles = m_staleFiles->at(n).second;
        // Get an array of all the files in this VD, and check each against reality
        proj->GetFilesByVirtualDir(vdPath, vdFiles);
        for (size_t n = 0; n < vdFiles.GetCount(); ++n) {
            if (!wxFileName::Exists(vdFiles[n])) {
                // We've found a stale file
                vdStaleFiles.Add(vdFiles[n]);
            }
        }

        if (!vdStaleFiles.IsEmpty()) {
            vdStaleFiles.Sort();
            // Add each file to the stale-files checklistbox
            for (size_t n = 0; n < vdStaleFiles.GetCount(); ++n) {
                wxString entry(vdPath + ": " + vdStaleFiles[n]);
                m_rootPanel->SetStaleFiles(entry);
            }
        }
    }
}

wxArrayString ReconcileProjectDlg::RemoveStaleFiles(const wxArrayString& StaleFiles) const
{
    wxArrayString removals;
    
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_MSG(proj, removals, "Can't find a Project with the supplied name");
    
    for (size_t n = 0; n < StaleFiles.GetCount(); ++n) {
        // Reconstruct the VD path in projectname:foo:bar format
        int index = StaleFiles[n].Find(": ");
        wxCHECK_MSG(index != wxNOT_FOUND, removals, "Badly-formed stalefile string");
        wxString vdPath = StaleFiles[n].Left(index);
        wxString filepath = StaleFiles[n].Mid(index+2);

        if (proj->RemoveFile(filepath, vdPath)) {
            removals.Add(StaleFiles[n]);
        }
    }
    
    return removals;
}

wxArrayString ReconcileProjectDlg::AddMissingFiles(const wxArrayString& files, const wxString& vdPath)
{
    wxArrayString additions;

    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_MSG(proj, additions, "Can't find a Project with the supplied name");

    wxString VD = vdPath;
    if (VD.empty()) {
        // If we were called from the root panel (so the user is trying to add unallocated files, or all files at once) we need to know which VD to use
        VirtualDirectorySelectorDlg selector(this, WorkspaceST::Get(), "", m_projname);
        selector.SetText("Please choose the Virtual Directory to which to add the files");
        if (selector.ShowModal() == wxID_OK) {
            VD = selector.GetVirtualDirectoryPath();
        } else {
            return additions;
        }
    }

    VD = VD.AfterFirst(':'); // Remove the projectname

    for (size_t n = 0; n < files.GetCount(); ++n) {
        if (proj->FastAddFile(files[n], VD)) {
            additions.Add(files[n]);
        }
    }
    
    return additions;
}

void ReconcileProjectDlg::DistributeFilesByRegex(ReconcileProjectPanel* rootpanel, const wxArrayString& regexes, wxArrayString& files) const
{
    for (size_t n = 0; n < regexes.GetCount() ; ++n) {
        wxString VD(regexes[n].BeforeFirst('|'));
        wxRegEx regex(regexes[n].AfterFirst('|'));
        for (size_t f = files.GetCount(); f > 0; --f) {
            if (regex.Matches(files[f-1]) && rootpanel->AllocateFileByVD(files[f-1], VD)) {
                files.RemoveAt(f-1);
            }
        }
    }
}



ReconcileProjectPanel::ReconcileProjectPanel(wxWindow* parent, const wxString& displayname, const wxString& vdPath)
    : ReconcileProjectPanelBaseClass(parent), m_displayname(displayname), m_vdPath(vdPath), m_hasItems(false)
{
    if (!VDpathToFilepath(m_vdPath).empty()) {
        // We're not the 'root' panel, so hide the 'stale' subpanel
        m_panelStale->Hide();
        // and the Show All/Show Unallocated radiobuttons and Hide empties checkbox
        wxSizer* radiosizer = m_radioShowAll->GetContainingSizer();
        if (radiosizer) {
            radiosizer->ShowItems(false);
        }
        m_checkBoxShowAllVDs->Hide();
    }
}

void ReconcileProjectPanel::StoreChild(ReconcileProjectPanel* childpanel)
{
    wxString childname = childpanel->GetVDDisplayName();
    if (IsSourceVD(childname.Lower()) || IsHeaderVD(childname.Lower()) || IsResourceVD(childname.Lower())) {
        m_children.push_back(childpanel); // We want these processed last, so push_back
    } else {
        m_children.push_front(childpanel);
    }
}

void ReconcileProjectPanel::UnstoreChild(wxWindow* childpanel)
{
    std::deque<ReconcileProjectPanel*>::iterator iter = m_children.begin();
    for ( ; iter != m_children.end(); ++iter) {
        if (*iter == childpanel) {
            m_children.erase(iter);
            return;
        }
    }
    wxASSERT("Failed to find the childpanel to be unstored");
}

bool ReconcileProjectPanel::GetHasItems() // Does this panel have children or grandchildren? Caches the result internally
{
    for (size_t n = 0; n < m_children.size(); ++n) {
        if (m_children[n]->GetHasItems()) {
            m_hasItems = true; // Don't return here: all children need to be processed
        }
    }
    if (m_hasItems) {
        return true;
    }

    if (m_checkListBoxMissing->GetCount()) {
        m_hasItems = true;
        return true;
    }

    m_hasItems = false;
    return false;
}

void ReconcileProjectPanel::SetFiles(wxArrayString& files)
{
    // Process any children first
    for (size_t n = 0; n < m_children.size(); ++n) {
        m_children[n]->SetFiles(files);
    }
    
    if (VDpathToFilepath(m_vdPath).empty()) {
        m_checkListBoxMissing->Append(files);   // This is the project root panel, so display all unallocated files
        if (!m_checkBoxShowAllVDs->IsChecked()) {
            // Mark for removal all panels without either files or children with files
            for (size_t n = 0; n < m_children.size(); ++n) {
                m_children[n]->GetHasItems();
            }
        }
    } else {
        // Put files in their best-guess virtual dir
        DistributeFilesToScrInclude(files);     // If there are VDs called 'src', 'include' or similar, put things there
        DistributeFilesToExactMatches(files);   // Otherwise try to match the file's path with this panel's VD
    }
}

void ReconcileProjectPanel::SetStaleFiles(wxString& file)
{
    m_checkListBoxStale->Append(file);
}

void ReconcileProjectPanel::DistributeFilesToScrInclude(wxArrayString& files)
{
    if (!VDpathToFilepath(m_vdPath)) {
        return;
    }
    
    wxString vdName = GetVDDisplayName();
    wxFileName vdFn(VDpathToFilepath(m_vdPath).Lower());

    if (IsSourceVD(vdName.Lower())) {
        for (size_t n = files.GetCount(); n > 0; --n) {
            wxFileName fn(files[n-1].Lower());
            if (fn.GetExt() == "cpp" || fn.GetExt() == "c" || fn.GetExt() == "cc") {
                // If the paths match, or if the file is from a subdir of the parent, insert into this VD.
                // NB subdirs will already have been processed once and any absolute matches allocated, so we'll only get not-yet-matched files here
                // i.e. if there's a VD foo/src, then add cpp files from foo/ and also any still-unallocated ones from foo/bar/
                // Check for an exact match too, just in case someone has a real dir called src/
                if (fn.GetPath().StartsWith(vdFn.GetPath()) || (fn.GetPath() == vdFn.GetFullPath())) {
                    m_checkListBoxMissing->Append(files[n-1]);
                    files.RemoveAt(n-1);
                }
            }
        }
    }

    if (IsHeaderVD(vdName.Lower())) {
        for (size_t n = files.GetCount(); n > 0 ; --n) {
            wxFileName fn(files[n-1].Lower());
            if (fn.GetExt() == "h" || fn.GetExt() == "hpp" || fn.GetExt() == "hh") {
                if (fn.GetPath().StartsWith(vdFn.GetPath()) || (fn.GetPath() == vdFn.GetFullPath())) {
                    m_checkListBoxMissing->Append(files[n-1]);
                    files.RemoveAt(n-1);
                }
            }
        }
    }

    if (IsResourceVD(vdName.Lower())) {
        for (size_t n = files.GetCount(); n > 0 ; --n) {
            wxFileName fn(files[n-1].Lower());
            if (fn.GetExt() == "rc") {
                if ((fn.GetPath() == vdFn.GetPath()) || fn.GetPath().StartsWith(vdFn.GetPath())) {
                    m_checkListBoxMissing->Append(files[n-1]);
                    files.RemoveAt(n-1);
                }
            }
        }
    }
}

void ReconcileProjectPanel::DistributeFilesToExactMatches(wxArrayString& files)
{
    if (!VDpathToFilepath(m_vdPath)) {
        return;
    }

    wxFileName vdFn(VDpathToFilepath(m_vdPath).Lower());

    for (size_t n = files.GetCount(); n > 0 ; --n) {
        wxFileName fn(files[n-1].Lower());
        // See if we have a file called foo/bar/baz.cpp and a VD called foo/bar/
        if (fn.GetPath() == vdFn.GetFullPath()) {
            m_checkListBoxMissing->Append(files[n-1]);
            files.RemoveAt(n-1);
        }
    }
}

bool ReconcileProjectPanel::AllocateFileByVD(const wxString& file, const wxString& VD)
{
    if (VD == m_vdPath) {
        m_checkListBoxMissing->Append(file);
        return true;
    }
    
    // Otherwise try any children
    for (size_t n = 0; n < m_children.size(); ++n) {
        if (m_children[n]->AllocateFileByVD(file, VD)) {
            return true;
        }
    }

    return false;
}

size_t ReconcileProjectPanel::GetStaleFiles(wxArrayString& files) const
{
    wxCHECK_MSG(m_checkListBoxStale && m_checkListBoxStale->IsShown(), 0, "Calling GetStaleFiles() on non-root panel");
    for (size_t n = 0; n < m_checkListBoxStale->GetCount(); ++n) {
        if (m_checkListBoxStale->IsChecked(n)) {
            files.Add(m_checkListBoxStale->GetString(n));
        }
    }

    return files.GetCount();
}

size_t ReconcileProjectPanel::GetStaleFilesCount() const
{
    wxCHECK_MSG(m_checkListBoxStale && m_checkListBoxStale->IsShown(), 0, "Calling GetStaleFilesCount() on non-root panel");

    return m_checkListBoxStale->GetCount();
}

void ReconcileProjectPanel::DoShowAllInRoot()
{
    wxArrayString files = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->GetAllNewFiles();
    m_checkListBoxMissing->Clear();
    m_checkListBoxMissing->Append(files);
}

void ReconcileProjectPanel::OnShowUnallocdInRoot(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString files = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->GetUnallocatedFiles();
    m_checkListBoxMissing->Clear();
    m_checkListBoxMissing->Append(files);
}

void ReconcileProjectPanel::OnShowVDsClicked(wxCommandEvent& WXUNUSED(event))
{
    wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->DistributeFiles();
}

void ReconcileProjectPanel::OnProcessButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString NewFiles;
    for (size_t n = 0; n < m_checkListBoxMissing->GetCount(); ++n) {
        if (m_checkListBoxMissing->IsChecked(n)) {
            NewFiles.Add(m_checkListBoxMissing->GetString(n));
        }
    }
    
    wxArrayString additions = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->AddMissingFiles(NewFiles, m_vdPath);

    // Update our display, and also the new-files list in the parent dialog
    wxArrayString& allnew = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->GetAllNewFiles();
    for (size_t n = 0; n < additions.GetCount(); ++n) {
        int index = m_checkListBoxMissing->FindString(additions[n]);
        if (index != wxNOT_FOUND) {
            m_checkListBoxMissing->Delete(index);
        }
        
        index = allnew.Index(additions[n]);
        if (index != wxNOT_FOUND) {
            allnew.RemoveAt(index);
        }
    }

    // If we're not it, tell the root panel about the change if it's in ShowAll mode
    if (!m_radioShowAll->IsShown()) {
        ReconcileProjectPanel* rootpanel = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->GetRootPanel();
        if (rootpanel && rootpanel->GetIsShowingAll()) {
            rootpanel->DoShowAllInRoot();
        }
    }

    int failures = NewFiles.GetCount() - additions.GetCount();
    if (additions.IsEmpty()) {
        clMainFrame::Get()->SetStatusMessage(_("Failed to add files to project"), 0);
    } else if (!failures) {
        clMainFrame::Get()->SetStatusMessage(wxString::Format(_("%i files successfully added to project"), (int)additions.GetCount()), 0);
    } else {
        clMainFrame::Get()->SetStatusMessage(wxString::Format(_("%i of %i files added to project"), (int)additions.GetCount(), (int)NewFiles.GetCount()), 0);
    }

    if (additions.GetCount()) {
        wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
        EventNotifier::Get()->AddPendingEvent(buildTree);
    }
}

void ReconcileProjectPanel::OnRemoveStaleButtonClicked(wxCommandEvent& WXUNUSED(event))
{
    wxArrayString StaleFiles;
    GetStaleFiles(StaleFiles);
    
    wxArrayString removals = wxStaticCast(GetGrandParent(), ReconcileProjectDlg)->RemoveStaleFiles(StaleFiles);
    for (size_t n = 0; n < removals.GetCount(); ++n) {
        int index = m_checkListBoxStale->FindString(removals[n]);
        if (index != wxNOT_FOUND) {
            m_checkListBoxStale->Delete(index);
        }
    }

    int failures = StaleFiles.GetCount() - removals.GetCount();
    if (removals.IsEmpty()) {
        clMainFrame::Get()->SetStatusMessage(_("Failed to add files to project"), 0);
    } else if (!failures) {
        clMainFrame::Get()->SetStatusMessage(wxString::Format(_("%i files successfully added to project"), (int)removals.GetCount()), 0);
    } else {
        clMainFrame::Get()->SetStatusMessage(wxString::Format(_("%i of %i files added to project"), (int)removals.GetCount(), (int)StaleFiles.GetCount()), 0);
    }

    if (removals.GetCount()) {
        wxCommandEvent buildTree(wxEVT_REBUILD_WORKSPACE_TREE);
        EventNotifier::Get()->AddPendingEvent(buildTree);
    }
}

void ReconcileProjectPanel::OnProcessButtonUpdateUI(wxUpdateUIEvent& event)
{
    for (size_t n = 0; n < GetActiveChkListBox()->GetCount(); ++n) {
        if (GetActiveChkListBox()->IsChecked(n)) {
            return event.Enable(true);
        }
    }
    event.Enable(false) ;
}

void ReconcileProjectPanel::OnSelectAll(wxCommandEvent& event)
{
    for (size_t n = 0; n < GetActiveChkListBox()->GetCount(); ++n) {
        GetActiveChkListBox()->Check(n, true);
    }
}

void ReconcileProjectPanel::OnSelectAllUpdateUI(wxUpdateUIEvent& event)
{
    for (size_t n = 0; n < GetActiveChkListBox()->GetCount(); ++n) {
        if (!GetActiveChkListBox()->IsChecked(n)) {
            return event.Enable(true);
        }
    }
    event.Enable(false) ;
}

void ReconcileProjectPanel::OnUnselectAll(wxCommandEvent& event)
{
    for (size_t n = 0; n < GetActiveChkListBox()->GetCount(); ++n) {
        GetActiveChkListBox()->Check(n, false);
    }
}

void ReconcileProjectPanel::OnUnselectAllUpdateUI(wxUpdateUIEvent& event)
{
    for (size_t n = 0; n < GetActiveChkListBox()->GetCount(); ++n) {
        if (GetActiveChkListBox()->IsChecked(n)) {
            return event.Enable(true);
        }
    }
    event.Enable(false) ;
}

bool ReconcileProjectPanel::IsSourceVD(const wxString& name) const
{
    return (name == "src" || name == "source" || name == "cpp" || name == "c" || name == "cc");
}

bool ReconcileProjectPanel::IsHeaderVD(const wxString& name) const
{
    return (name == "include" || name == "includes" || name == "header" || name == "headers" || name == "hpp" || name == "h");
}

bool ReconcileProjectPanel::IsResourceVD(const wxString& name) const
{
    return (name == "rc" || name == "resource" || name == "resources");
}

wxCheckListBox* ReconcileProjectPanel::GetActiveChkListBox() const
{
    return (m_notebook88->GetSelection() == 1) ? m_checkListBoxStale : m_checkListBoxMissing;
}



ReconcileProjectFiletypesDlg::ReconcileProjectFiletypesDlg(wxWindow* parent, const wxString& projname)
    : ReconcileProjectFiletypesDlgBaseClass(parent), m_projname(projname)
{
    m_listCtrlRegexes->AppendColumn("Regex");
    m_listCtrlRegexes->AppendColumn("Virtual Directory");
    
    WindowAttrManager::Load(this, wxT("ReconcileProjectFiletypesDlg"), NULL);
}

ReconcileProjectFiletypesDlg::~ReconcileProjectFiletypesDlg()
{
    WindowAttrManager::Save(this, wxT("ReconcileProjectFiletypesDlg"), NULL);
}

void ReconcileProjectFiletypesDlg::SetData()
{
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    wxString topleveldir, types;
    wxArrayString excludes, regexes;
    proj->GetReconciliationData(topleveldir, types, excludes, regexes);

    if (topleveldir.empty()) {
        topleveldir = proj->GetFileName().GetPath();
    }
    m_dirPickerToplevel->SetPath(topleveldir);

    if (types.empty()) {
        types << "cpp;c;h;hpp;xrc;wxcp;fbp";
    }
    m_textExtensions->ChangeValue(types);

    m_listExclude->Clear();
    m_listExclude->Append(excludes);
    
    m_listCtrlRegexes->DeleteAllItems();
    for (size_t n = 0; n < regexes.GetCount(); ++n) {
        SetRegex(regexes[n]);
    }
}

void ReconcileProjectFiletypesDlg::GetData(wxString& toplevelDir, wxString& types, wxArrayString& excludePaths, wxArrayString& regexes) const
{
    toplevelDir = m_dirPickerToplevel->GetPath();
    types = m_textExtensions->GetValue();
    excludePaths = m_listExclude->GetStrings();
    regexes = GetRegexes();
    
    // While we're here, save the current data
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");
    proj->SetReconciliationData(toplevelDir, types, excludePaths, regexes);
}

void ReconcileProjectFiletypesDlg::SetRegex(const wxString& regex)
{
    int n = m_listCtrlRegexes->GetItemCount();
    AppendListCtrlRow(m_listCtrlRegexes);
    SetColumnText(m_listCtrlRegexes, n, 0, regex.AfterFirst('|'));
    SetColumnText(m_listCtrlRegexes, n, 1, regex.BeforeFirst('|'));
}

wxArrayString ReconcileProjectFiletypesDlg::GetRegexes() const
{ 
    wxArrayString array;
    for (int n = 0; n < m_listCtrlRegexes->GetItemCount(); ++n) {
        wxString regex = GetColumnText(m_listCtrlRegexes, n, 0);
        wxString VD = GetColumnText(m_listCtrlRegexes, n, 1);
        array.Add(VD + '|' + regex); // Store the data as a VD|regex string, as the regex might contain a '|' but the VD won't
    }
    return array;
}

void ReconcileProjectFiletypesDlg::OnIgnoreBrowse(wxCommandEvent& WXUNUSED(event))
{
    ProjectPtr proj = ManagerST::Get()->GetProject(m_projname);
    wxCHECK_RET(proj, "Can't find a Project with the supplied name");

    wxString topleveldir, types;
    wxArrayString excludes, regexes;
    proj->GetReconciliationData(topleveldir, types, excludes, regexes);

    if (topleveldir.empty()) {
        topleveldir = proj->GetFileName().GetPath();
    }
	wxString new_exclude = wxDirSelector(_("Select a directory to ignore:"), topleveldir, wxDD_DEFAULT_STYLE, wxDefaultPosition, this);

    if (!new_exclude.empty()) {
        wxFileName fn = wxFileName::DirName(new_exclude);
        fn.MakeRelativeTo(topleveldir);
        new_exclude = fn.GetFullPath();

        if (m_listExclude->FindString(new_exclude) == wxNOT_FOUND) {
            m_listExclude->Append(new_exclude);
        }
    }
}

void ReconcileProjectFiletypesDlg::OnIgnoreRemove(wxCommandEvent& WXUNUSED(event))
{
	int sel = m_listExclude->GetSelection();
    if (sel != wxNOT_FOUND) {
        m_listExclude->Delete(sel);
    }
}

void ReconcileProjectFiletypesDlg::OnIgnoreRemoveUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(m_listExclude->GetSelection() != wxNOT_FOUND);
}

void ReconcileProjectFiletypesDlg::OnAddRegex(wxCommandEvent& event)
{
    ReconcileByRegexDlg dlg(this, m_projname);
    if (dlg.ShowModal() == wxID_OK) {
        SetRegex(dlg.GetRegex());
    }
}

void ReconcileProjectFiletypesDlg::OnRemoveRegex(wxCommandEvent& event)
{
    wxUnusedVar(event);

	long selecteditem = m_listCtrlRegexes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (selecteditem != wxNOT_FOUND) {
        m_listCtrlRegexes->DeleteItem(selecteditem);
    }
}

void ReconcileProjectFiletypesDlg::OnRemoveRegexUpdateUI(wxUpdateUIEvent& event)
{
	long selecteditem = m_listCtrlRegexes->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    event.Enable(selecteditem != wxNOT_FOUND);
}



ReconcileByRegexDlg::ReconcileByRegexDlg(wxWindow* parent, const wxString& projname) : ReconcileByRegexDlgBaseClass(parent), m_projname(projname)
{
    WindowAttrManager::Load(this, wxT("ReconcileByRegexDlg"), NULL);
}

ReconcileByRegexDlg::~ReconcileByRegexDlg()
{
    WindowAttrManager::Save(this, wxT("ReconcileByRegexDlg"), NULL);
}

void ReconcileByRegexDlg::OnTextEnter(wxCommandEvent& event)
{
    if (m_buttonOK->IsEnabled()) {
        EndModal(wxID_OK);
    }
}

void ReconcileByRegexDlg::OnVDBrowse(wxCommandEvent& WXUNUSED(event))
{    
    VirtualDirectorySelectorDlg selector(this, WorkspaceST::Get(), m_textCtrlVirtualFolder->GetValue(), m_projname);
    if (selector.ShowModal() == wxID_OK) {
        m_textCtrlVirtualFolder->ChangeValue(selector.GetVirtualDirectoryPath());
    }
}

void ReconcileByRegexDlg::OnRegexOKCancelUpdateUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_textCtrlRegex->IsEmpty() && !m_textCtrlVirtualFolder->IsEmpty());
}

