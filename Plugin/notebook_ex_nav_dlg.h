//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : notebooknavdialog.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )                     
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/                     
//              \____/\___/ \__,_|\___\_____/_|\__\___|                     
//                                                                          
//                                                  F i l e                 
//                                                                          
//    This program is free software; you can redistribute it and/or modify  
//    it under the terms of the GNU General Public License as published by  
//    the Free Software Foundation; either version 2 of the License, or     
//    (at your option) any later version.                                   
//                                                                          
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
 #ifndef __notebooknavdialog__
#define __notebooknavdialog__

#include <wx/dialog.h>
#include <map>
#include <wx/bitmap.h>

class wxListBox;
class Notebook;

class NotebookNavDialog : public wxDialog
{
protected:
	wxListBox *                m_listBox;
	size_t                     m_selectedItem;
	std::map< int, wxWindow* > m_tabsIndex;
	wxWindow*                  m_selTab;
	
protected:
	void CloseDialog();

public:
	/**
	 * Parameterized constructor
	 * \param parent dialog parent window
	 */
	NotebookNavDialog(wxWindow* parent);

	/**
	 * Default constructor
	 */
	NotebookNavDialog();

	/**
	 * Destructor
	 */
	virtual ~NotebookNavDialog();

	/**
	 * Create the dialog, usually part of the two steps construction of a 
	 * dialog
	 * \param parent dialog parent window
	 */
	void Create(wxWindow* parent);
	
	wxWindow *GetSelection(){return m_selTab;}
	
	/// Event handling
	void OnKeyUp(wxKeyEvent &event);
	void OnNavigationKey(wxNavigationKeyEvent &event);
	void OnItemSelected(wxCommandEvent &event);
	void PopulateListControl(Notebook *book);
};

#endif // __notebooknavdialog__
