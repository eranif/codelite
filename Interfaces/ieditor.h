//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : ieditor.h              
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
 #ifndef IEDITOR_H
#define IEDITOR_H

#include "wx/filename.h"
#include "wx/string.h"

//------------------------------------------------------------------
// Defines the interface to the editor control
//------------------------------------------------------------------
/**
 * @class IEditor
 * @author Eran
 * @date 05/07/08
 * @file ieditor.h
 * @brief an interface for the plugin to provide clean access to the editor class 
 */
class IEditor {
public:
	IEditor(){}
	virtual ~IEditor(){}
	
	/**
	 * @brief return true if the editor is modified
	 */
	virtual bool IsModified() = 0;
	
	/**
	 * \brief return the current editor content
	 */
	virtual wxString GetEditorText() = 0;
	
	/**
	 * \brief sets the current editor's content with 'text'
	 */
	virtual void SetEditorText(const wxString &text) = 0;
	
	/**
	 * @brief append text to the editor
	 * @param text text to append
	 */
	virtual void AppendText(const wxString &text) = 0;
	
	/**
	 * @brief insert text at a given position
	 * @param text test to insert
	 * @param pos position to insert it
	 */
	virtual void InsertText(int pos, const wxString &text) = 0;
	
	/**
	 * @brief return the document length
	 * @return document length
	 */
	virtual int GetLength() = 0;
	
	/**
	 * \brief place the carte at a given position
	 * \param pos position to place the caret
	 */
	virtual void SetCaretAt(long pos) = 0;
	
	/**
	 * \brief reload the current file from disk - this function discards all changes made
	 * to the  current file
	 */
	virtual void ReloadFile() = 0;
	
	/**
	 * \brief return the current position of the caret
	 */
	virtual long GetCurrentPosition() = 0;
	
	/**
	 * \brief return the current file name 
	 */
	virtual const wxFileName &GetFileName() const = 0;
	
	/**
	 * \brief return the project which owns the current file name. return wxEmptyString
	 * if this file has not project owner 
	 */
	virtual const wxString &GetProjectName() const = 0;

	/**
	 * \brief return the current word under the caret. May return wxEmptyString
	 */
	virtual wxString GetWordAtCaret() = 0;
	
	/**
	 * @brief return the EOL mode of the editor. 
	 * 	wxSCI_EOL_CRLF 	0
	 *	wxSCI_EOL_CR 	1
	 *	wxSCI_EOL_LF 	2
	 */
	virtual int GetEOL() = 0;
};

#endif //IEDITOR_H


