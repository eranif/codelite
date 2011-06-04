#ifndef VIEWSETTINGS_H
#define VIEWSETTINGS_H

#include <wx/wx.h>
#include <wx/validate.h>
#include "GUI.h" // Base class: _TableSettings
#include <wx/wxsf/DiagramManager.h>

#include "IDbAdapter.h"
#include "view.h"

/*! \brief Dialog for editing database view */
class ViewSettings : public _ViewSettings {

public:
	/*! \brief Default constructor */
	ViewSettings( wxWindow* parent,IDbAdapter* pDbAdapter);
	/*! \brief Default desctructor */
	virtual ~ViewSettings();
	/*! \brief Load view into dialog */
	void SetView(View* pView, wxSFDiagramManager* pManager);

	virtual void OnOKClick(wxCommandEvent& event);
	
protected:
	IDbAdapter* m_pDbAdapter;
	View* m_pView;
	wxSFDiagramManager* m_pDiagramManager;
};

#endif // VIEWSETTINGS_H
