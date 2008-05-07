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
	 * \brief return the current editor content
	 */
	virtual wxString GetEditorText() = 0;
	
	/**
	 * \brief sets the current editor's content with 'text'
	 */
	virtual void SetEditorText(const wxString &text) = 0;
	
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
};

#endif //IEDITOR_H


