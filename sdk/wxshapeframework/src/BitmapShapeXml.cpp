/***************************************************************
 * Name:      BitmapShapeXml.cpp
 * Purpose:   Implements bitmap shape's serialization capability
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-07-22
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxsf/BitmapShape.h"
#include "wx/wxsf/CommonFcn.h"

//----------------------------------------------------------------------------------//
// Serialization
//----------------------------------------------------------------------------------//

wxXmlNode* wxSFBitmapShape::Serialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	if(node)
	{
		node = wxSFRectShape::Serialize(node);
	}

	return node;
}

void wxSFBitmapShape::Deserialize(wxXmlNode* node)
{
	// HINT: overload it for custom actions...

	wxSFRectShape::Deserialize(node);

	wxRealPoint prevSize = m_nRectSize;

    if(!m_sBitmapPath.IsEmpty())
    {
        CreateFromFile(m_sBitmapPath);
    }

	if(m_fCanScale)
	{
		if(m_nRectSize != prevSize)
		{
			m_nRectSize = prevSize;
			RescaleImage(prevSize);
		}
		else
			Scale(1, 1);
	}
}
