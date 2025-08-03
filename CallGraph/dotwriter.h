//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : dotwriter.h
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
	wxString m_OutputString;
	bool m_writedotfileFlag;
	bool dwhideparams;
	bool dwstripparams;
	bool dwhidenamespaces;
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
	 * @brief Get positions of STL template signs
	 * @param txt
	 * @param start
	 * @param end
	 * @return
	 */
	bool GetOuterTemplate(const wxString& txt, int *start, int *end);

public:
	/**
	 * @brief Default constructor.
	 */
	DotWriter();
	/**
	 * @brief Default destructor.
	 */
    ~DotWriter() = default;
	/**
	 * @brief Function sets object DotWriter and assign the pointer pLines.
	 * @param pLines
	 */	
	void SetLineParser(LineParserList *pLines);
	/**
	 * @brief Function sets object DotWriter from stored configuration data.
	 * @param mgr
	 */	
	void SetDotWriterFromDialogSettings(IManager *mgr);
	
	/**
	 * @brief Function sets object DotWriter from given data.
	 * @param colnode
	 * @param coledge
	 * @param thrnode
	 * @param thredge
	 * @param hideparams
	 * @param stripparams
	 * @param hidenamespaces
	 */
	void SetDotWriterFromDetails(int colnode, int coledge, int thrnode, int thredge, bool hideparams, bool stripparams, bool hidenamespaces);
	//
	/**
	 * @brief Function create data in the DOT language and prepare it to write.
	 */
	void WriteToDotLanguage();
	/**
	 * @brief Function write data in the DOT language to file dot.txt.
	 * @param path for file where write file with DOT language.
	 */
	bool SendToDotAppOutputDirectory(const wxString& path);
	
	/**
	 * @brief Function return string modified by the options in the dialog settings of the plugin.
	 * @param name of the function stored in the list of objects.
	 */
	wxString OptionsShortNameAndParameters(const wxString& name);
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
	 * @brief Function return bool value if index exists in the array.
	 * @param index of the function stored in the list of objects.
	 * @param array of index by nodes added to the call graph.
	 */
	bool IsInArray(int index, const wxArrayInt& array);
	/**
	 * @brief Function return optimal index for color by the value time and options in the dialog settings of the plugin.
	 * @param time of the function stored in the list of objects.
	 * @param dwc is value from dialog settings node level colors
	 */
	int ReturnIndexForColor(float time, int dwc);
};