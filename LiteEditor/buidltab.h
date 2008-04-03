#ifndef __buidltab__
#define __buidltab__

#include "wx/string.h"
#include "map"
#include "outputtabwindow.h"
#include "compiler.h"

struct LineInfo {
	wxString project;
	wxString configuration;
};

class BuildTab : public OutputTabWindow {
	std::map<int, LineInfo> m_lineInfo;
	// holds the index of the last line in the build window
	// that was reached, using F4 (last build error)
	int m_nextBuildError_lastLine;
	bool m_skipWarnings;
	
protected:
	bool OnBuildWindowDClick(const wxString &line, int lineClicked);
	void OnNextBuildError(wxCommandEvent &event);
	void OnMouseDClick(wxScintillaEvent &event);
	void OnHotspotClicked(wxScintillaEvent &event);
	void OnCompilerColours(wxCommandEvent &e);
	void Initialize();
	
	
public:
	BuildTab(wxWindow *parent, wxWindowID id, const wxString &name);
	~BuildTab();

	virtual void AppendText(const wxString &text);
	virtual void Clear();
	
	void OnBuildEnded();
	void ReloadSettings();
	CompilerPtr GetCompilerByLine(int lineClicked);
	int LineFromPosition(int pos);
	
	static int ColourGccLine(int startLine, const char *line, size_t &fileNameStart, size_t &fileNameLen);
	
	DECLARE_EVENT_TABLE()
};
#endif // __buidltab__

