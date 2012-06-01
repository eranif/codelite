/***************************************************************
 * Name:      dotwriter.h
 * Purpose:   Header to create writer to DOT language
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "lineparser.h"
#include "confcallgraph.h"
#include "plugin.h"
#include "static.h"
#include <wx/wx.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/dir.h> 
#include <wx/filefn.h>
#include <wx/file.h>
/**
 * @class DotWriter
 * @brief Class write data from lineparser structure to dot language.
 */
class DotWriter
{	
private:
	wxString graph, end_graph, begin_graph;
	wxString style, shape, fontname;
	wxString cwhite, cblack;
	wxString dlabel, dedge, hedge, hnode;
	LineParserList *mlines;
	wxString output;
	bool writedotfile;
	bool dwbname;
	bool dwbparam;
	int dwcn;
	int dwce;
	int dwtn;
	int dwte;
	
protected:
	/**
	 * @brief Object confData type ConfCallGraph with stored configuration data.
	 */
	ConfCallGraph confData;
	/**
	 * @brief Pointer m_mgr type of IManager.
	 */
	IManager *m_mgr;

public:
	/**
	 * @brief Defautl constructor.
	 */
	DotWriter();
	/**
	 * @brief Defautl destructor.
	 */
	~DotWriter();
	/**
	 * @brief Function setup object DotWriter and assign the pointer pLines.
	 * @param pLines
	 */	
	void setLineParser(LineParserList *pLines);
	/**
	 * @brief Function setup object DotWriter from stored configuration data.
	 * @param mgr
	 */	
	void setDotWriterFromDialogSettings(IManager *mgr);
	//
	/**
	 * @brief Function create data in the DOT language and prepare it to write.
	 */
	void WriteToDotLanguade();
	/**
	 * @brief Function write data in the DOT language to file dot.txt.
	 * @param path for file where write file with DOT language.
	 */
	void SendToDotAppOutputDirectory(wxString path);
	/**
	 * @brief Function return bool value if the file in the DOT language is exist.
	 * @param path for file where is file with DOT language placed.
	 */
	bool DotFileExist(wxString path);
	//
	/**
	 * @brief Function return string modified by the options in the dialog settings of the plugin.
	 * @param name of the function stored in the list of objects.
	 */
	wxString OptionsShortNameAndParameters(wxString name);
	/**
	 * @brief Function return string of color by the index value.
	 * @param index of the color, this value return function ReturnIndexForColor.
	 * */
	wxString DefineColorForNodeEdge(int index);
	/**
	 * @brief Function return string of color by the index value.
	 * @param index of the color, this value return function ReturnIndexForColor.
	 */
	wxString DefineColorForLabel(int index);
	/**
	 * @brief Function return bool value if index is exist in the array.
	 * @param index of the function stored in the list of objects.
	 * @param array of index by nodes added to the call graph.
	 */
	bool IsInArray(int index, wxArrayInt array);
	/**
	 * @brief Function return optimal index for color by the value time and options in the dialog settings of the plugin.
	 * @param time of the function stored in the list of objects.
	 * @param dwc is value from dialog settings node level colors
	 */
	int ReturnIndexForColor(float time, int dwc);
};