#ifndef ENVVAR_H
#define ENVVAR_H

class wxXmlNode;

#include "wx/string.h"
#include "smart_ptr.h"
#include <map>

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * \ingroup SDK
 * A manager class for the environment variables of the workspace
 *
 * \version 1.0
 * first version
 *
 * \date 05-18-2007
 *
 * \author Eran
 *
 */
class WXDLLIMPEXP_LE_SDK EnvironmentVariebles {

	std::map<wxString, wxString> m_variables;

public:
	typedef std::map<wxString, wxString>::const_iterator ConstIterator;

public:	
	/**
	 * Constructor that initializes itself from an XML node
	 * \param node XML node, can be null to create empty Environment variables class
	 */
	EnvironmentVariebles(wxXmlNode *node);
	/**
	 * destructor
	 */
	virtual ~EnvironmentVariebles();
	
	/**
	 * Add new environment variable. if environment exist, this function updates its value
	 * \param name environment variable name
	 * \param value environment variable value
	 */
	void SetEnv(const wxString &name, const wxString &value);
	/**
	 * Find environment variable. This function will first try to match the environment
	 * variable name in the workspace settings, if the variable does not exist, it will try
	 * to find a match in the system environment variables.
	 * \param name environment variable name
	 * \return value or wxEmptyString if not found
	 */
	wxString GetEnv(const wxString &name) const ;

	/**
	 * Enumerating function
	 * \return const iterator to the start of the internal container of type std::map<wxString, wxString>
	 */
	ConstIterator Begin() const;

	/** 
	 * delete environemt variable
	 */
	void DeleteEnv(const wxString &name);

	/**
	 * Enumerating function
	 * \return const iterator to the end of the internal container of type std::map<wxString, wxString>
	 */
	ConstIterator End() const;

	/** 
	 * Return an xml representation of this object
	 */
	wxXmlNode *ToXml() const;
};

typedef SmartPtr<EnvironmentVariebles> EnvironmentVarieblesPtr;

#endif // ENVVAR_H
