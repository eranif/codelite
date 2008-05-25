//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah                            
// file name            : attribute_style.h              
//                                                                          
// -------------------------------------------------------------------------
// A                                                                        
//              _____           _      _     _ _                            
//             /  __ \         | |    | |   (_) |                           
//             | /  \/ ___   __| | ___| |    _| |_ ___                      
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ \                     
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
 #ifndef ATTRIBUTE_STYLE_H
#define ATTRIBUTE_STYLE_H

#include "wx/string.h"
#include "list"

#ifdef WXMAKINGDLL_LE_SDK
#    define WXDLLIMPEXP_LE_SDK WXEXPORT
#elif defined(WXUSINGDLL_LE_SDK)
#    define WXDLLIMPEXP_LE_SDK WXIMPORT
#else /* not making nor using FNB as DLL */
#    define WXDLLIMPEXP_LE_SDK
#endif // WXMAKINGDLL_LE_SDK

class WXDLLIMPEXP_LE_SDK StyleProperty
{
	int m_id;
	wxString m_fgColour;
	wxString m_bgColour;
	long m_fontSize;
	wxString m_name;
	wxString m_faceName;
	bool m_bold;
	
public:
	StyleProperty(int id, const wxString& fgColour, const wxString& bgColour, const int fontSize, const wxString& name, const wxString& face, bool bold)
		: m_id(id)
		, m_fgColour(fgColour)
		, m_bgColour(bgColour)
		, m_fontSize(fontSize)
		, m_name(name)
		, m_faceName(face)
		, m_bold(bold)
	{
	};

	StyleProperty()
		: m_id(0)
		, m_fgColour(_T("BLACK"))
		, m_bgColour(_T("WHITE"))
		, m_fontSize(10)
		, m_name(wxEmptyString)
		, m_faceName(_T("Courier"))
		, m_bold(false)
	{
	};

	StyleProperty(const StyleProperty& rhs)
	{
		*this = rhs;
	};

	StyleProperty& operator=(const StyleProperty& rhs){
		m_fgColour = rhs.m_fgColour;
		m_bgColour = rhs.m_bgColour;
		m_faceName = rhs.m_faceName;
		m_bold = rhs.m_bold;
		m_fontSize = rhs.m_fontSize;
		m_name = rhs.m_name;
		m_id = rhs.m_id;
		return *this;
	}

	virtual ~StyleProperty(){}

	const wxString& GetFgColour() const { return m_fgColour; }
	const wxString& GetBgColour() const { return m_bgColour; }
	const long GetFontSize() const { return m_fontSize; }
	const wxString& GetFaceName() const { return m_faceName; }
	bool IsBold() const { return m_bold; }
	const wxString& GetName() const { return m_name; }
	int GetId() const { return m_id; }

	void SetBgColour(const wxString& colour) { m_bgColour = colour; }
	void SetFgColour(const wxString& colour) { m_fgColour = colour; }
	void SetFontSize(long size) { m_fontSize = size; }
	void SetFaceName(const wxString& face){ m_faceName = face; }
	void SetBold(bool bold) { m_bold = bold; }
	void SetId(int id){m_id = id;}
};

typedef std::list<StyleProperty> StylePropertyList;
#endif 
