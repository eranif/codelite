#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "wx/string.h"
#include "wx/hashmap.h"
#include <wx/arrstr.h>
#include "wx/filename.h"
#include <wx/gdicmn.h>

class wxXmlNode;

#ifndef WXDLLIMPEXP_CL
#ifdef WXMAKINGDLL_CODELITE
#    define WXDLLIMPEXP_CL WXEXPORT
#elif defined(WXUSINGDLL_CODELITE)
#    define WXDLLIMPEXP_CL WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_CL
#endif // WXMAKINGDLL_CODELITE
#endif // WXDLLIMPEXP_CL

class SerializedObject;

WX_DECLARE_STRING_HASH_MAP( wxString, StringMap );

/**
 * \class Archive
 * \brief an auxulariy class which serializes variables into XML format
 * \author Eran
 * \date 07/20/07
 */
class WXDLLIMPEXP_CL Archive
{
	wxXmlNode *m_root;

public:
	Archive();
	virtual ~Archive();

	/**
	 * \brief set a root node for this Archive object, all Write operations will append their nodes
	 * to this node
	 * \param node wxXmlNode to act as the root
	 */
	void SetXmlNode(wxXmlNode *node);

	//--------------------
	// Write API
	//--------------------
	void Write(const wxString &name, SerializedObject *obj);
	void Write(const wxString &name, int value);
	void Write(const wxString &name, bool value);
	void Write(const wxString &name, long value);
	void Write(const wxString &name, const wxString &str);
	void Write(const wxString &name, const wxArrayString &arr);
	void Write(const wxString &name, const wxFileName &fileName);
	void Write(const wxString &name, size_t value);
	void Write(const wxString &name, wxSize size);
	void Write(const wxString &name, wxPoint pt);
	void Write(const wxString &name, const StringMap &str_map);
	void Write(const wxString &name, const wxColour &colour);
	
	//--------------------
	// Read API
	//--------------------
	void Read(const wxString &name, int &value);
	void Read(const wxString &name, bool &value);
	void Read(const wxString &name, long &value);
	void Read(const wxString &name, wxString &value);
	void Read(const wxString &name, wxArrayString &arr);
	void Read(const wxString &name, wxFileName &fileName);
	void Read(const wxString &name, size_t &value);
	void Read(const wxString &name, wxSize &size);
	void Read(const wxString &name, wxPoint &pt);
	void Read(const wxString &name, StringMap &str_map);
	void Read(const wxString &name, SerializedObject *obj);
	void Read(const wxString &name, wxColour &colour);	
	
private:
	void WriteSimple(long value, const wxString &typeName, const wxString &name);
	void ReadSimple(long &value, const wxString &typeName, const wxString &name);
};
#endif //ARCHIVE_H
