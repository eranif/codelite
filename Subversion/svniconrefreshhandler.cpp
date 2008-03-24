#include "svniconrefreshhandler.h"
#include "procutils.h"
#include "subversion.h"
#include "workspace.h"
#include "imanager.h"
#include <vector>

struct ProjectIconInfo {
	wxString path;
	wxString name;
	SvnXmlParser::FileState state;
};

typedef std::map< wxString, ProjectIconInfo > ProjectIconInfoMap;

extern int ProjectModifiedIconId;
extern int ProjectConflictIconId;
extern int ProjectOkIconId;
extern int WorkspaceModifiedIconId;
extern int WorkspaceConflictIconId;
extern int WorkspaceOkIconId;
extern int FileModifiedIconId;
extern int FileConflictIconId;
extern int FileOkIconId;
extern int FolderModifiedIconId;
extern int FolderConflictIconId;
extern int FolderOkIconId;

SvnIconRefreshHandler::SvnIconRefreshHandler(IManager *mgr, SubversionPlugin *plugin)
		: m_mgr(mgr)
		, m_plugin(plugin)
{

}

SvnIconRefreshHandler::~SvnIconRefreshHandler()
{
}

void SvnIconRefreshHandler::UpdateIcons()
{
	SvnOptions options;
	m_mgr->GetConfigTool()->ReadObject(wxT("SubversionOptions"), &options);

	bool useIcons ( true );
	options.GetFlags() & SvnUseIcons ? useIcons = true : useIcons = false;

	if (!m_plugin->IsWorkspaceUnderSvn() || !useIcons) {
		return;
	}

	//get list of paths to check
	std::vector<wxFileName> fileNames;
	wxString errMsg;

	wxArrayString projects;
	ProjectIconInfoMap pi;

	m_mgr->GetWorkspace()->GetProjectList(projects);
	for (size_t i=0; i<projects.GetCount(); i++) {
		ProjectPtr p = m_mgr->GetWorkspace()->FindProjectByName(projects.Item(i), errMsg);
		fileNames.push_back(p->GetFileName());

		ProjectIconInfo info;
		info.name = p->GetName();
		info.path = p->GetFileName().GetPath();
		info.state = SvnXmlParser::StateOK;
		pi[info.name] =  info;
	}

	//get list of files
	wxString command;
	command << wxT("\"") << m_plugin->GetOptions().GetExePath() << wxT("\" ");
	command << wxT("status --xml --non-interactive --no-ignore -q ");
	//concatenate list of files here
	for (size_t i=0; i< fileNames.size(); i++) {
		command << wxT("\"") <<  fileNames.at(i).GetPath() << wxT("\" ");
	}

	wxArrayString output;
	ProcUtils::ExecuteCommand(command, output);

	//parse the resutls
	wxString rawData;
	for (size_t i=0; i< output.GetCount(); i++) {
		rawData << output.Item(i);
	}

	wxArrayString modifiedPaths;
	wxArrayString conflictedPaths;

	//by default all paths are OK
	SvnXmlParser::GetFiles(rawData, modifiedPaths, SvnXmlParser::StateModified);
	SvnXmlParser::GetFiles(rawData, conflictedPaths, SvnXmlParser::StateConflict);

	//sort the results for better search later
	modifiedPaths.Sort();
	conflictedPaths.Sort();


	wxTreeCtrl *tree = m_mgr->GetTree(TreeFileView);
	if (!tree) {
		return;
	}

	wxTreeItemId root = tree->GetRootItem();
	if (!root.IsOk()) {
		return;
	}

	//avoid flickering
	tree->Freeze();

	//reset parents icons back to normal
	ResetIcons(tree, root);

	//we now have two lists containing the modified and conflict files in the workspace
	//we recurse into the file view tree, every item of type File that we encounter, we
	//colour according to the match
	ColourTree(tree, root, modifiedPaths, conflictedPaths);

	//unfreeze it
	tree->Thaw();
}

void SvnIconRefreshHandler::ColourTree(wxTreeCtrl *tree, wxTreeItemId &parent, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths)
{
	if (parent.IsOk() == false) {
		return;
	}

	//get the item data
	FilewViewTreeItemData *data = (FilewViewTreeItemData *) tree->GetItemData(parent);
	if (data && data->GetData().GetKind() == ProjectItem::TypeFile) {
		//we found a leaf of the tree
		wxString fileName = data->GetData().GetFile();
		ColourPath(tree, parent, fileName, modifiedPaths, conflictedPaths);
		return;
	}
	
	//container, might be workspace, project or virtual folder
	if (tree->ItemHasChildren(parent)) {
		//loop over the children
		wxTreeItemIdValue cookie;
		wxTreeItemId child = tree->GetFirstChild(parent, cookie);
		while ( child.IsOk() ) {
			ColourTree(tree, child, modifiedPaths, conflictedPaths);
			child = tree->GetNextChild(parent, cookie);
		}
	}
	
	if (data && data->GetData().GetKind() == ProjectItem::TypeProject) {
		//we found a leaf of the tree
		wxString fileName = data->GetData().GetFile();
		
		SvnXmlParser::FileState state ( SvnXmlParser::StateOK );

		if (conflictedPaths.Index(fileName) != wxNOT_FOUND) {
			state = SvnXmlParser::StateConflict;

		} else if ( modifiedPaths.Index(fileName) != wxNOT_FOUND) {
			state = SvnXmlParser::StateModified;
		}

		DoColourPath(tree, parent, state);
		return;
	}
}

void SvnIconRefreshHandler::ColourPath(wxTreeCtrl *tree, wxTreeItemId &item, const wxString &fileName, const wxArrayString &modifiedPaths, const wxArrayString &conflictedPaths)
{
	SvnXmlParser::FileState state ( SvnXmlParser::StateOK );

	if (conflictedPaths.Index(fileName) != wxNOT_FOUND) {
		state = SvnXmlParser::StateConflict;

	} else if ( modifiedPaths.Index(fileName) != wxNOT_FOUND) {
		state = SvnXmlParser::StateModified;
	}

	DoColourPath(tree, item, state);
}

void SvnIconRefreshHandler::DoColourPath(wxTreeCtrl *tree, wxTreeItemId &item, SvnXmlParser::FileState state)
{
	int imgid;
	FilewViewTreeItemData *data = (FilewViewTreeItemData *) tree->GetItemData(item);
	if (data) {
		imgid = GetIcon(data->GetData().GetKind(), state);
		int curimgid_ = tree->GetItemImage(item);
		if (imgid != wxNOT_FOUND && imgid > curimgid_) {
			tree->SetItemImage(item, imgid, wxTreeItemIcon_Normal);
			tree->SetItemImage(item, imgid, wxTreeItemIcon_Selected);
		}

		//colour the items parents as well
		wxTreeItemId parent = tree->GetItemParent(item);
		while ( parent.IsOk() ) {
			//get the parent type
			data = NULL;
			data = (FilewViewTreeItemData *) tree->GetItemData(parent);
			if (data) {
				imgid = GetIcon(data->GetData().GetKind(), state);
				int curimgid = tree->GetItemImage(parent);
				//replce the icon only if the severity is higher, that is only these state shifts are allowed:
				//non-versionsed -> ok
				//ok -> modified
				//modified -> conflict
				if (imgid != wxNOT_FOUND && imgid > curimgid) {
					tree->SetItemImage(parent, imgid, wxTreeItemIcon_Normal);
					tree->SetItemImage(parent, imgid, wxTreeItemIcon_Selected);
				}
			}
			parent = tree->GetItemParent(parent);
		}
	}
}

int SvnIconRefreshHandler::GetIcon(int kind, SvnXmlParser::FileState state)
{
	switch (kind) {

	case ProjectItem::TypeFile:
		if (state == SvnXmlParser::StateConflict)
			return FileConflictIconId;
		if (state == SvnXmlParser::StateModified)
			return FileModifiedIconId;
		return FileOkIconId;

	case ProjectItem::TypeProject:
		if (state == SvnXmlParser::StateConflict)
			return ProjectConflictIconId;
		if (state == SvnXmlParser::StateModified)
			return ProjectModifiedIconId;
		return ProjectOkIconId;

	case ProjectItem::TypeWorkspace:
		if (state == SvnXmlParser::StateConflict)
			return WorkspaceConflictIconId;
		if (state == SvnXmlParser::StateModified)
			return WorkspaceModifiedIconId;
		return WorkspaceOkIconId;

	case ProjectItem::TypeVirtualDirectory:
		if (state == SvnXmlParser::StateConflict)
			return FolderConflictIconId;
		if (state == SvnXmlParser::StateModified)
			return FolderModifiedIconId;
		return FolderOkIconId;

	}
	return wxNOT_FOUND;
}

void SvnIconRefreshHandler::ResetIcons(wxTreeCtrl *tree, wxTreeItemId &item)
{
	if (item.IsOk() == false) {
		return;
	}

	//get the item data
	FilewViewTreeItemData *data = (FilewViewTreeItemData *) tree->GetItemData(item);
	if (data) {
		int imgid = GetIcon(data->GetData().GetKind(), SvnXmlParser::StateOK);
		if (imgid != wxNOT_FOUND) {
			tree->SetItemImage(item, imgid, wxTreeItemIcon_Normal);
			tree->SetItemImage(item, imgid, wxTreeItemIcon_Selected);
		}
	}

	if (tree->ItemHasChildren(item)) {
		//loop over the children
		wxTreeItemIdValue cookie;
		wxTreeItemId child = tree->GetFirstChild(item, cookie);
		while ( child.IsOk() ) {
			ResetIcons(tree, child);
			child = tree->GetNextChild(item, cookie);
		}
	}
}

void SvnIconRefreshHandler::DoCommand()
{
	UpdateIcons();
}
