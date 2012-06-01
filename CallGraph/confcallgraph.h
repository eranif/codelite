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
	
	bool m_boxName;
	bool m_boxParam;

public:
	/**
	 * @brief Defautl constructor.
	 */
	ConfCallGraph();
	/**
	 * @brief Defautl destructor.
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
	
	void SetBoxName(bool boxName){
		this->m_boxName = boxName;
	}
	
	void SetBoxParam(bool boxParam){
		this->m_boxParam = boxParam;
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
	
	bool GetBoxName() {
		return m_boxName;
	}
	
	bool GetBoxParam() {
		return m_boxParam;
	}
	
	
};
#endif // __confcallgraph__
