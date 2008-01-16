#ifndef FILEEXPLORER_H
#define FILEEXPLORER_H

#include "wx/panel.h"
#include "wx/choice.h"
#include "volumelocatorthread.h"

class FileExplorerTree;

class FileExplorer : public wxPanel
{
private:
	FileExplorerTree *m_fileTree;
	wxString m_caption;

#ifdef __WXMSW__
	wxChoice *m_volumes;
	VolumeLocatorThread m_thread;
#endif

private:
	void CreateGUIControls();
#ifdef __WXMSW__
	void OnVolumeChanged(wxCommandEvent &e);
	void OnRootChanged(wxCommandEvent &e);
	void OnVolumes(wxCommandEvent &e);
#endif

public:
	FileExplorer(wxWindow *parent, const wxString &caption);
	virtual ~FileExplorer();

	//setters/getters
	const wxString &GetCaption() const{return m_caption;}
	FileExplorerTree *GetFileTree() {return m_fileTree;}
	void Scan();
};

#endif //FILEEXPLORER_H

