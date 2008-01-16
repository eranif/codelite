#ifndef FILEPICKER_H
#define FILEPICKER_H

#include "wx/panel.h"
#include "wx/button.h"
#include "wx/textctrl.h"
#include "wx/filedlg.h"

/**
 * This control allows the user to select a directory. with a text control on its right side
 */
class FilePicker : public wxPanel {
	wxTextCtrl *m_path;
	wxButton *m_button;
	wxString m_buttonCaption;
	wxString m_dlgCaption;
	long m_dlgStyle;
	wxString m_defaultFile;
	wxString m_wildCard;

private:
	void CreateControls();
	void ConnectEvents();

protected:
	/**
	 * Handle button click event
	 * \param &event 
	 */
	virtual void OnButtonClicked(wxCommandEvent &event);
	
public:
	/**
	 * \param parent control parent
	 * \param id control id
	 * \param message the dialog message
	 * \param buttonCaption the browse button caption
	 * \param style control style, same as wxFileDialog styles
	 */
	FilePicker(wxWindow *parent, 
			   wxWindowID id = wxID_ANY, 
			   const wxString &defaultFile = wxEmptyString,
			   const wxString &message = wxT("Select a file:"), 
			   const wxString &wildCard = wxT("*.*"), 
			   const wxString &buttonCaption = wxT("Browse"), 
			   const wxPoint& pos = wxDefaultPosition, 
			   const wxSize& size = wxDefaultSize, 
			   long style = wxFD_DEFAULT_STYLE );

	virtual ~FilePicker();

	/**
	 * \return the path
	 */
	wxString GetPath() const { return m_path->GetValue(); }

	/**
	 * set a value into the text control
	 * \param path path to set
	 */
	void SetPath(const wxString &path) { m_path->SetValue(path); }
};

#endif // FILEPICKER_H
