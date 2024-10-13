//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : confcallgraph.h
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

/***************************************************************
 * Name:      confcallgraph.h
 * Purpose:   Header to store values.
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef __confcallgraph__
#define __confcallgraph__

#include "serialized_object.h"
#include <wx/msgdlg.h> 
/**
 * @class ConfCallGraph
 * @brief Class define structure for save settings of dialog.
 */
class ConfCallGraph : public SerializedObject
{
	wxString m_gprofPath;
	wxString m_dotPath;
	
	int m_colorsNode;
	int m_colorsEdge;
	
	int m_tresholdNode;
	int m_tresholdEdge;
	
	bool m_boxHideParams;
	bool m_boxStripParams;
	bool m_boxHideNamespaces;

public:
	/**
	 * @brief Default constructor.
	 */
	ConfCallGraph();
	/**
	 * @brief Default destructor.
	 */
	virtual ~ConfCallGraph();

public:
	virtual void DeSerialize(Archive &arch);
	virtual void Serialize(Archive &arch);
	//
	//
	//
	// just wxStrings are pointers
	void SetGprofPath(const wxString& gprofPath) {
		this->m_gprofPath = gprofPath;
	}
	
	void SetDotPath(const wxString& dotPath) {
		this->m_dotPath = dotPath;
	}
	
	void SetColorsNode(int colorsNode){
		this->m_colorsNode = colorsNode;
	}
	
	void SetColorsEdge(int colorsEdge){
		this->m_colorsEdge = colorsEdge;
	}
	
	void SetTresholdNode(int trashNode){
		this->m_tresholdNode = trashNode;
	}
	
	void SetTresholdEdge(int trashEdge){
		this->m_tresholdEdge = trashEdge;
	}
	
	void SetHideParams(bool hide){
		this->m_boxHideParams = hide;
	}
	
	void SetStripParams(bool hide){
		this->m_boxStripParams = hide;
	}
	void SetHideNamespaces(bool hide){
		this->m_boxHideNamespaces = hide;
	}
	
	////////////////////////////////////////////
	// just wxStrings are pointers
	const wxString& GetGprofPath() const {
		return m_gprofPath;
	}
	
	const wxString& GetDotPath() const {
		return m_dotPath;
	}
	
	int GetColorsNode() {
		return m_colorsNode;
	}
	
	int GetColorsEdge() {
		return m_colorsEdge;
	}
	
	int GetTresholdNode() {
		return m_tresholdNode;
	}
	
	int GetTresholdEdge() {
		return m_tresholdEdge;
	}
	
	bool GetHideParams() {
		return m_boxHideParams;
	}
	
	bool GetStripParams() {
		return m_boxStripParams;
	}
	
	bool GetHideNamespaces() {
		return m_boxHideNamespaces;
	}
	
};
#endif // __confcallgraph__
