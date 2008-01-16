#ifndef OUTPUT_PANE_H
#define OUTPUT_PANE_H


#include "wx/panel.h"
#include "wx/wxscintilla.h"
#include "wx/wxFlatNotebook/wxFlatNotebook.h"
#include "outputpanewindow.h"
#include "map"

class OutputPane;
class ShellWindow;

struct LineInfo {
	wxString project;
	wxString configuration;
};

/**
 * \ingroup LiteEditor
 * This class represents the default bottom pane in the editor
 *
 * \date 04-14-2007
 *
 * \author Eran
 *
 * \par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \todo 
 *
 * \bug 
 *
 */
class OutputPane : public wxPanel 
{
public:
	static const wxString FIND_IN_FILES_WIN;
	static const wxString BUILD_WIN;
	static const wxString OUTPUT_WIN;
	static const wxString OUTPUT_DEBUG;

	wxFlatNotebook *m_book;
	wxString m_caption;
	bool m_canFocus;
	wxFlatNotebookImageList m_images;
	OutputPaneWinodw *m_outputWind;
	wxLog *m_logTargetOld;
	OutputPaneWinodw *m_outputDebug;
	std::map<int, LineInfo> m_lineInfo;

private:
	void CreateGUIControls();
	
	void OnFindInFilesDClick(const wxString &line);
	// returns true if line contained warning/error, false otherwise
	bool OnBuildWindowDClick(const wxString &line, int lineClicked = wxNOT_FOUND);
	// holds the index of the last line in the build window
	// that was reached, using F4 (last build error)
	int m_nextBuildError_lastLine;

public:
	/**
	 * Return the index of a given window by its caption
	 */
	int CaptionToIndex(const wxString &caption);

	/**
	 * Destructor
	 * \param parent parent window for this pane
	 * \param caption the caption
	 */
	OutputPane(wxWindow *parent, const wxString &caption);

	/**
	 * Destructor
	 */
	virtual ~OutputPane();

	//-----------------------------------------------
	// Operations
	//-----------------------------------------------
	void AppendText(const wxString &winName, const wxString &text);
	void Clear();

	//-----------------------------------------------
	// Setters/Getters
	//-----------------------------------------------
	wxFlatNotebook *GetNotebook() { return m_book; }
	const wxString &GetCaption() const { return m_caption; }
	void CanFocus(bool can) { m_canFocus = can; }
	void SelectTab(const wxString &tabName);
	ShellWindow *GetOutputWindow() {return (ShellWindow *)m_outputWind->GetWindow();}
	ShellWindow *GetDebugWindow() {return (ShellWindow *)m_outputDebug->GetWindow();}

	//-----------------------------------------------
	// Event handlers
	//-----------------------------------------------
	void OnSetFocus(wxFocusEvent &event);
	void OnMouseDClick(wxScintillaEvent &event);
	void OnPaint(wxPaintEvent &event);
	void OnEraseBg(wxEraseEvent &){};
	void OnNextBuildError(wxCommandEvent &event);
	
	DECLARE_EVENT_TABLE()
};

#endif // OUTPUT_PANE_H
