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
	if(!m_plugin->IsWorkspaceUnderSvn()) {
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
	ProjectIconInfoMap::iterator iter = pi.begin();
	for (; iter != pi.end(); iter++) {
		for (size_t i=0; i< modifiedPaths.GetCount(); i++) {
			wxString s1 = modifiedPaths.Item(i);
			wxString s2 = iter->second.path;
			if (s1.StartsWith(s2)) {
				//our project is modified
				iter->second.state = SvnXmlParser::StateModified;
				break;
			}
		}
	}

	SvnXmlParser::GetFiles(rawData, conflictedPaths, SvnXmlParser::StateConflict);
	iter = pi.begin();
	for (; iter != pi.end(); iter++) {
		for (size_t i=0; i< conflictedPaths.GetCount(); i++) {
			if (conflictedPaths.Item(i) == iter->second.path) {
				//our project is modified
				iter->second.state = SvnXmlParser::StateConflict;
				break;
			}
		}
	}

	wxTreeCtrl *tree = m_mgr->GetTree(TreeFileView);
	if (!tree) {
		return;
	}

	wxTreeItemId root = tree->GetRootItem();
	if (!root.IsOk()) {
		return;
	}

	if (!tree->ItemHasChildren(root)) {
		return;
	}

	int wspImgId(WorkspaceOkIconId);
	wxTreeItemIdValue cookie;
	wxTreeItemId child = tree->GetFirstChild(root, cookie);
	while ( child.IsOk() ) {
		wxString text = tree->GetItemText(child);
		ProjectIconInfoMap::iterator it = pi.find(text);
		if (it != pi.end()) {
			ProjectIconInfo info = it->second;
			if (info.name == text) {
				int imgId(wxNOT_FOUND);

				switch (info.state) {
				case SvnXmlParser::StateConflict:
					imgId = ProjectConflictIconId;
					wspImgId = WorkspaceConflictIconId;
					break;

				case SvnXmlParser::StateModified:
					imgId = ProjectModifiedIconId;
					if (wspImgId != WorkspaceConflictIconId) {
						wspImgId = WorkspaceModifiedIconId;
					}
					break;

				default:
					imgId = ProjectOkIconId;
					break;
				}

				if (imgId != wxNOT_FOUND) {
					tree->SetItemImage(child, imgId, wxTreeItemIcon_Normal);
					tree->SetItemImage(child, imgId, wxTreeItemIcon_Selected);
				}

			}
		}

		child = tree->GetNextChild(root, cookie);
	}
	tree->SetItemImage(root, wspImgId, wxTreeItemIcon_Normal);
	tree->SetItemImage(root, wspImgId, wxTreeItemIcon_Selected);
}
