/***************************************************************
 * Name:      static.h
 * Purpose:   Header to create static variables.
 * Author:    Vaclav Sprucek
 * Created:   2012-03-04
 * Copyright: Vaclav Sprucek 
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#ifndef _STATIC_H__
#define _STATIC_H__

#include <wx/string.h>
#include <wx/filename.h>
/**
 * @class stvariables
 * @brief Class define static variables.
 */
class stvariables
{	
public:
	/**
	 * @brief Return string with name for picture file.
	 */
	static wxString dotpngname;
	/**
	 * @brief Return string with name for dot text file.
	 */
	static wxString dottxtname;
	/**
	 * @brief Return string with name for dot files folder.
	 */
	static wxString dotfilesdir;
	/**
	 * @brief Return string with name for gmon.out file.
	 */
	static wxString gmonfile;
	/**
	 * @brief Return string with file type.
	 */
	static wxString filetype;
	/**
	 * @brief Return string with separator directory.
	 */
	static wxString sd;
	/**
	 * @brief Return string with white char.
	 */
	static wxString sw;
	/**
	 * @brief Return string with quote char.
	 */
	static wxString sq;
	/**
	 * @brief Return string with gprof name.
	 */
	static wxString gprofname;
	/**
	 * @brief Return string with dot name.
	 */
	static wxString dotname;
	/**
	 * @brief Function inicialize static variables.
	 */	 
	static void InicializeStatic();

};

#endif
