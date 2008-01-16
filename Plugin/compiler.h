#ifndef COMPILER_H
#define COMPILER_H

#include "configuration_object.h"
#include "smart_ptr.h"
#include "map"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

/**
 * \ingroup LiteEditor
 * This class represenets a compiler entry in the configuration file
 *
 * \version 1.0
 * first version
 *
 * \date 05-25-2007
 *
 * \author Eran
 */
class WXDLLIMPEXP_LE_SDK Compiler : public ConfObject {
	wxString m_name;
	std::map<wxString, wxString> m_switches;
	wxString m_objectSuffix;

	wxString m_errorPattern;
	wxString m_errorLineNubmerIndex;
	wxString m_errorFileNameIndex;

	wxString m_warningPattern;
	wxString m_warningLineNubmerIndex;
	wxString m_warningFileNameIndex;

	std::map<wxString, wxString> m_tools;

public:
	typedef std::map<wxString, wxString>::const_iterator ConstIterator;

	Compiler(wxXmlNode *node);
	virtual ~Compiler();

	wxXmlNode *ToXml() const;

	//iteration over switches
	Compiler::ConstIterator SwitchesBegin() const { return m_switches.begin(); }
	Compiler::ConstIterator SwitchesEnd() const { return m_switches.end(); }

	//---------------------------------------------------
	//setters/getters
	//---------------------------------------------------
	wxString GetTool(const wxString &name) const;
	void SetTool(const wxString &name, const wxString &tool){ m_tools[name] = tool; }

	wxString GetSwitch(const wxString &name) const;
	void SetSwitch(const wxString &name, const wxString &value) { m_switches[name] = value; }
	const wxString &GetObjectSuffix() const { return m_objectSuffix; }
	void SetObjectSuffix(const wxString &suffix) { m_objectSuffix = suffix; }
	void SetName(const wxString &name) { m_name = name;}
	const wxString &GetName() const { return m_name; }
	const wxString &GetErrPattern() const { return m_errorPattern; }
	const wxString &GetErrFileNameIndex() const { return m_errorFileNameIndex; }
	const wxString &GetErrLineNumberIndex() const { return m_errorLineNubmerIndex; }
	const wxString &GetWarnPattern() const { return m_warningPattern; }
	const wxString &GetWarnFileNameIndex() const { return m_warningFileNameIndex; }
	const wxString &GetWarnLineNumberIndex() const { return m_warningLineNubmerIndex; }
	
	void SetErrPattern(const wxString &s) { m_errorPattern = s; }
	void SetErrFileNameIndex(const wxString &s) { m_errorFileNameIndex = s; }
	void SetErrLineNumberIndex(const wxString &s) { m_errorLineNubmerIndex = s; }
	void SetWarnPattern(const wxString &s) { m_warningPattern = s; }
	void SetWarnFileNameIndex(const wxString &s) { m_warningFileNameIndex = s; }
	void SetWarnLineNumberIndex(const wxString &s) { m_warningLineNubmerIndex = s; };
};	

typedef SmartPtr<Compiler> CompilerPtr;
#endif // COMPILER_H
