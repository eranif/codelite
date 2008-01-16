#ifndef XMLUTILS_H
#define XMLUTILS_H

#include "wx/xml/xml.h"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/// A collection of XML utils
class WXDLLIMPEXP_LE_SDK XmlUtils {
public:
	/// Find a child node by name by iterating the parent children. NULL if no childs exist
	/// \param parent  the parent node whom to be searched
	/// \param tagName the element tag name
	/// \param name	   the element 'Name' property value to search
	static wxXmlNode *FindNodeByName(const wxXmlNode *parent, const wxString &tagName, const wxString &name);

	/// Find the last child node of parent with a given name. NULL if no childs exist
	/// \param parent  the parent node whom to be searched
	/// \param name	   the element's tag name 
	static wxXmlNode *FindLastByTagName(const wxXmlNode *parent, const wxString &tagName);

	
	/**
	 * update node property. If multiple properties with the same name exist, this function updates
     * the first occurance
	 * \param node 
	 * \param name property name
	 * \param value new value to set
	 */
	static void UpdateProperty(wxXmlNode *node, const wxString &name, const wxString &value);

	/**
	 * Find the first child node of parent with a given name. NULL if no childs exist
	 * \param the parent node whom to be searched
	 * \param the element's tag name 
	 */
	static wxXmlNode *FindFirstByTagName(const wxXmlNode *parent, const wxString &tagName);

	
	/**
	 * Set the content of node. This function replaces any existing content of node
	 * \param node 
	 * \param text content
	 */
	static void SetNodeContent(wxXmlNode *node, const wxString &text);

	/**
	 * Read string property from the given node
	 * \param propName the property name
	 * \param defaultValue default value to return if no property exist
	 */
	static wxString ReadString(wxXmlNode *node, const wxString &propName, const wxString &defaultValue = wxEmptyString);

	/**
	 * Read long property from the given node
	 * \param propName the property name
	 * \param defaultValue default value to return if no property exist
	 */
	static long ReadLong(wxXmlNode *node, const wxString &propName, long defaultValue = -1);

	/**
	 * Read a boolean property from the given node
	 * \param propName the property name
	 * \param defaultValue default value to return if no property exist
	 */
	static bool ReadBool(wxXmlNode *node, const wxString &propName, bool defaultValue = false);
	
	/**
	 * Remove all children of xml node
	 * \param node xml node
	 */
	static void RemoveChildren(wxXmlNode *node);

};

#endif // XMLUTILS_H
