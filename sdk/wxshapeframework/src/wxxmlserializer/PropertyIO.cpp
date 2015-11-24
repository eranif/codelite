/***************************************************************
 * Name:      PropertyIO.cpp
 * Purpose:   Implements data types I/O and conversion functions
 * Author:    Michal Bližňák (michal.bliznak@tiscali.cz)
 * Created:   2007-10-28
 * Copyright: Michal Bliňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include "wx/wxxmlserializer/PropertyIO.h"
#include "wx/wxxmlserializer/XmlSerializer.h"

#include <wx/listimpl.cpp>
#include <wx/arrimpl.cpp>
#include <limits>

using namespace wxXS;

WX_DEFINE_EXPORTED_OBJARRAY(RealPointArray);
WX_DEFINE_EXPORTED_LIST(RealPointList);

/////////////////////////////////////////////////////////////////////////////////////
// xsPropertyIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsPropertyIO, wxObject);

wxXmlNode* xsPropertyIO::AddPropertyNode(wxXmlNode* parent, const wxString& name, const wxString& value, wxXmlNodeType type)
{
	if(parent)
	{
		wxXmlNode* child = new wxXmlNode(wxXML_ELEMENT_NODE, name);
		child->AddChild(new wxXmlNode(type, wxT(""), value));
		parent->AddChild(child);
		return child;
	}
	return NULL;
}

void xsPropertyIO::AppendPropertyType(xsProperty *source, wxXmlNode *target)
{
#if wxVERSION_NUMBER < 2900
    target->AddProperty(wxT("name"), source->m_sFieldName);
    target->AddProperty(wxT("type"), source->m_sDataType);
#else
	target->AddAttribute(wxT("name"), source->m_sFieldName);
	target->AddAttribute(wxT("type"), source->m_sDataType);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////
// xsStringPropIO class /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxString, xsStringPropIO);

wxString xsStringPropIO::ToString(const wxString& value)
{
    return value;
}

wxString xsStringPropIO::FromString(const wxString& value)
{
	return value;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsCharPropIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxChar, xsCharPropIO);

wxString xsCharPropIO::ToString(const wxChar& value)
{
    return wxString::Format(wxT("%c"), value);
}

wxChar xsCharPropIO::FromString(const wxString& value)
{
	return value.GetChar(0);
}


/////////////////////////////////////////////////////////////////////////////////////
// xsLongPropIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(long, xsLongPropIO);

wxString xsLongPropIO::ToString(const long& value)
{
    return wxString::Format(wxT("%ld"), value);
}

long xsLongPropIO::FromString(const wxString& value)
{
	long num = 0;
	if(!value.IsEmpty())
	{
		value.ToLong(&num);
	}
	return num;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsIntPropIO class ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(int, xsIntPropIO);

wxString xsIntPropIO::ToString(const int& value)
{
    return wxString::Format(wxT("%d"), value);
}

int xsIntPropIO::FromString(const wxString& value)
{
	long num = 0;
	if(!value.IsEmpty())
	{
		value.ToLong(&num);
	}
	return (int)num;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsBoolPropIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(bool, xsBoolPropIO);

wxString xsBoolPropIO::ToString(const bool& value)
{
    return wxString::Format(wxT("%d"), value);
}

bool xsBoolPropIO::FromString(const wxString& value)
{
	long num = 0;
	if(!value.IsEmpty())
	{
		value.ToLong(&num);
	}
	return (num == 1);
}

/////////////////////////////////////////////////////////////////////////////////////
// xsDoublePropIO class /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(double, xsDoublePropIO);

wxString xsDoublePropIO::ToString(const double& value)
{
    wxString sVal;

    if( wxIsNaN(value) )
    {
        sVal = wxT("NAN");
    }
    else if( wxFinite(value) == 0 )
    {
        sVal = wxT("INF");
    }
    else
    {
        // use '.' decimal point character
        sVal= wxString::Format(wxT("%lf"), value);
        sVal.Replace(wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER), wxT("."));
    }

    return sVal;
}

double xsDoublePropIO::FromString(const wxString& value)
{
	double num = 0;

	if(!value.IsEmpty())
	{
	    if( value == wxT("NAN") )
	    {
	        num = std::numeric_limits<double>::quiet_NaN();
	    }
	    else if( value == wxT("INF") )
	    {
	        num = std::numeric_limits<double>::infinity();
	    }
	    else
	    {
	        // decimal point character used in wxXS is strictly '.'...
#ifdef __WXMSW__
			value.ToDouble(&num);
#else
	        wxString sNum = value;
	        sNum.Replace(wxT("."), wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER) );
	        sNum.ToDouble(&num);
#endif
	    }
	}

	return num;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsFloatPropIO class //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(float, xsFloatPropIO);

wxString xsFloatPropIO::ToString(const float& value)
{
    wxString sVal;
    if( wxIsNaN(value) )
    {
        sVal = wxT("NAN");
    }
    else if( wxFinite(value) == 0 )
    {
        sVal = wxT("INF");
    }
    else
    {
        sVal = wxString::Format(wxT("%f"), value);
        sVal.Replace(wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER), wxT("."));
    }

    return sVal;
}

float xsFloatPropIO::FromString(const wxString& value)
{
	double num = 0;

	if(!value.IsEmpty())
	{
	    if( value == wxT("NAN") )
	    {
	        num = std::numeric_limits<float>::quiet_NaN();
	    }
	    else if( value == wxT("INF") )
	    {
	        num = std::numeric_limits<float>::infinity();
	    }
	    else
	    {
	        // decimal point character used in wxXS is strictly '.'...
#ifdef __WXMSW__
			value.ToDouble(&num);
#else
	        wxString sNum = value;
	        sNum.Replace(wxT("."), wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER) );
	        sNum.ToDouble(&num);
#endif
	    }
	}

	return (float)num;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsPointPropIO class //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxPoint, xsPointPropIO);

wxString xsPointPropIO::ToString(const wxPoint& value)
{
    return wxString::Format(wxT("%d,%d"), value.x, value.y);
}

wxPoint xsPointPropIO::FromString(const wxString& value)
{
	wxPoint pt;

	//long x, y;

	if(!value.IsEmpty())
	{
		wxSscanf( value, wxT("%d,%d"), &pt.x, &pt.y );

//		wxStringTokenizer tokens(value, wxT(","), wxTOKEN_STRTOK);
//
//		tokens.GetNextToken().ToLong(&x);
//		tokens.GetNextToken().ToLong(&y);
//		pt.x = x;
//		pt.y = y;
	}

	return pt;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsSizePropIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxSize, xsSizePropIO);

wxString xsSizePropIO::ToString(const wxSize& value)
{
    return wxString::Format(wxT("%d,%d"), value.x, value.y);
}

wxSize xsSizePropIO::FromString(const wxString& value)
{
	wxPoint pt = xsPointPropIO::FromString(value);
	return wxSize(pt.x, pt.y);
}

/////////////////////////////////////////////////////////////////////////////////////
// xsRealPointPropIO class //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxRealPoint, xsRealPointPropIO);

wxString xsRealPointPropIO::ToString(const wxRealPoint& value)
{
    return wxString::Format(wxT("%s,%s"), xsDoublePropIO::ToString(value.x).c_str(), xsDoublePropIO::ToString(value.y).c_str());
}

wxRealPoint xsRealPointPropIO::FromString(const wxString& value)
{
	wxRealPoint pt;

	if(!value.IsEmpty())
	{
		wxStringTokenizer tokens(value, wxT(","), wxTOKEN_STRTOK);

		pt.x = xsDoublePropIO::FromString(tokens.GetNextToken());
		pt.y = xsDoublePropIO::FromString(tokens.GetNextToken());
	}

	return pt;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsColourPropIO class /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxColour, xsColourPropIO);

wxString xsColourPropIO::ToString(const wxColour& value)
{
    return wxString::Format(wxT("%d,%d,%d,%d"), value.Red(), value.Green(), value.Blue(), value.Alpha());
}

wxColour xsColourPropIO::FromString(const wxString& value)
{
	int nRed = 0;
	int nGreen = 0;
	int nBlue = 0;
	int nAlpha = 0;

	if(!value.IsEmpty())
	{
		if( wxSscanf( value, wxT("%d,%d,%d,%d"), &nRed, &nGreen, &nBlue, &nAlpha ) == 3 ) nAlpha = 255;
	}

	return wxColour(nRed, nGreen, nBlue, nAlpha);
}

/////////////////////////////////////////////////////////////////////////////////////
// xsPenPropIO class ////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxPen, xsPenPropIO);

wxString xsPenPropIO::ToString(const wxPen& value)
{
    return wxString::Format(wxT("%s %d %d"), xsColourPropIO::ToString(value.GetColour()).c_str(), value.GetWidth(), value.GetStyle());
}

wxPen xsPenPropIO::FromString(const wxString& value)
{
	wxPen pen;

	wxStringTokenizer tokens(value, wxT(" "), wxTOKEN_STRTOK);
	pen.SetColour(xsColourPropIO::FromString(tokens.GetNextToken()));
	pen.SetWidth(xsLongPropIO::FromString(tokens.GetNextToken()));
	pen.SetStyle((wxPenStyle)xsLongPropIO::FromString(tokens.GetNextToken()));

	return pen;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsBrushPropIO class //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxBrush, xsBrushPropIO);

wxString xsBrushPropIO::ToString(const wxBrush& value)
{
    return wxString::Format(wxT("%s %d"), xsColourPropIO::ToString(value.GetColour()).c_str(), value.GetStyle());
}

wxBrush xsBrushPropIO::FromString(const wxString& value)
{
	wxBrush brush;

	wxStringTokenizer tokens(value, wxT(" "), wxTOKEN_STRTOK);
	brush.SetColour(xsColourPropIO::FromString(tokens.GetNextToken()));
	brush.SetStyle((wxBrushStyle)xsLongPropIO::FromString(tokens.GetNextToken()));

	return brush;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsFontPropIO class ///////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

XS_DEFINE_IO_HANDLER(wxFont, xsFontPropIO);

wxString xsFontPropIO::ToString(const wxFont& value)
{
    return value.GetNativeFontInfoUserDesc();
}

wxFont xsFontPropIO::FromString(const wxString& value)
{
	wxFont font;

	if( !font.SetNativeFontInfoUserDesc(value) )
	{
		return *wxSWISS_FONT;
	}
	else
		return font;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayStringPropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayStringPropIO, xsPropertyIO);

void xsArrayStringPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((wxArrayString*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((wxArrayString*)property->m_pSourceVariable)->Add(listNode->GetNodeContent());
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayStringPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    wxArrayString& array = *((wxArrayString*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
            AddPropertyNode(newNode, wxT("item"), array[i]);
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayStringPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((wxArrayString*)property->m_pSourceVariable));
}

void xsArrayStringPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((wxArrayString*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayStringPropIO::ToString(const wxArrayString& value)
{
 	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << value[i];
		if( i < value.GetCount()-1 ) out << wxT("|");
	}

	return out;
}

wxArrayString xsArrayStringPropIO::FromString(const wxString& value)
{	
	wxArrayString arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( tokens.GetNextToken() );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayIntPropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayIntPropIO, xsPropertyIO);

void xsArrayIntPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((IntArray*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((IntArray*)property->m_pSourceVariable)->Add(xsIntPropIO::FromString(listNode->GetNodeContent()));
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayIntPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    IntArray& array = *((IntArray*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
            AddPropertyNode(newNode, wxT("item"), xsIntPropIO::ToString(array[i]));
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayIntPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((IntArray*)property->m_pSourceVariable));
}

void xsArrayIntPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((IntArray*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayIntPropIO::ToString(const IntArray& value)
{
 	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << xsIntPropIO::ToString(value[i]);
		if( i < value.GetCount()-1 ) out << wxT("|");
	}

	return out;
}

IntArray xsArrayIntPropIO::FromString(const wxString& value)
{
	IntArray arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( xsIntPropIO::FromString( tokens.GetNextToken() ) );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayLongPropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayLongPropIO, xsPropertyIO);

void xsArrayLongPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((LongArray*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((LongArray*)property->m_pSourceVariable)->Add(xsLongPropIO::FromString(listNode->GetNodeContent()));
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayLongPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    LongArray& array = *((LongArray*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
            AddPropertyNode(newNode, wxT("item"), xsLongPropIO::ToString(array[i]));
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayLongPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((LongArray*)property->m_pSourceVariable));
}

void xsArrayLongPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((LongArray*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayLongPropIO::ToString(const LongArray& value)
{
 	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << xsLongPropIO::ToString(value[i]);
		if( i < value.GetCount()-1 ) out << wxT("|");
	}

	return out;
}

LongArray xsArrayLongPropIO::FromString(const wxString& value)
{
	LongArray arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( xsLongPropIO::FromString( tokens.GetNextToken() ) );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayDoublePropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayDoublePropIO, xsPropertyIO);

void xsArrayDoublePropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((DoubleArray*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((DoubleArray*)property->m_pSourceVariable)->Add(xsDoublePropIO::FromString(listNode->GetNodeContent()));
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayDoublePropIO::Write(xsProperty *property, wxXmlNode *target)
{
    DoubleArray& array = *((DoubleArray*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
            AddPropertyNode(newNode, wxT("item"), xsDoublePropIO::ToString(array[i]));
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayDoublePropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((DoubleArray*)property->m_pSourceVariable));
}

void xsArrayDoublePropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((DoubleArray*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayDoublePropIO::ToString(const DoubleArray& value)
{
 	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << xsDoublePropIO::ToString(value[i]);
		if( i < value.GetCount()-1 ) out << wxT("|");
	}

	return out;
}

DoubleArray xsArrayDoublePropIO::FromString(const wxString& value)
{
	DoubleArray arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( xsDoublePropIO::FromString( tokens.GetNextToken() ) );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayCharPropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayCharPropIO, xsPropertyIO);

void xsArrayCharPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((CharArray*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((CharArray*)property->m_pSourceVariable)->Add(xsCharPropIO::FromString(listNode->GetNodeContent()));
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayCharPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    CharArray& array = *((CharArray*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
            AddPropertyNode(newNode, wxT("item"), xsCharPropIO::ToString(array[i]));
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayCharPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((CharArray*)property->m_pSourceVariable));
}

void xsArrayCharPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((CharArray*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayCharPropIO::ToString(const CharArray& value)
{
 	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << xsCharPropIO::ToString(value[i]);
		if( i < value.GetCount()-1 ) out << wxT("|");
	}
	
	return out;
}

CharArray xsArrayCharPropIO::FromString(const wxString& value)
{
	CharArray arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( xsCharPropIO::FromString( tokens.GetNextToken() ) );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsArrayRealPointPropIO class /////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsArrayRealPointPropIO, xsPropertyIO);

void xsArrayRealPointPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((RealPointArray*)property->m_pSourceVariable)->Clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            ((RealPointArray*)property->m_pSourceVariable)->Add(xsRealPointPropIO::FromString(listNode->GetNodeContent()));
        }

        listNode = listNode->GetNext();
    }
}

void xsArrayRealPointPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    RealPointArray& array = *((RealPointArray*)property->m_pSourceVariable);

    size_t cnt = array.GetCount();
    if(cnt > 0)
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        for(size_t i = 0; i < cnt; i++)
        {
			AddPropertyNode(newNode, wxT("item"), xsRealPointPropIO::ToString(array[i]));
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsArrayRealPointPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((RealPointArray*)property->m_pSourceVariable));
}

void xsArrayRealPointPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((RealPointArray*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsArrayRealPointPropIO::ToString(const RealPointArray& value)
{
	wxString out;

	for( size_t i = 0; i < value.GetCount(); i++)
	{
		out << xsRealPointPropIO::ToString(value[i]);
		if( i < value.GetCount()-1 ) out << wxT("|");
	}

	return out;
}

RealPointArray xsArrayRealPointPropIO::FromString(const wxString& value)
{
	RealPointArray arrData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		arrData.Add( xsRealPointPropIO::FromString( tokens.GetNextToken() ) );
	}
	
	return arrData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsListRealPointPropIO class //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsListRealPointPropIO, xsPropertyIO);

void xsListRealPointPropIO::Read(xsProperty *property, wxXmlNode *source)
{
	RealPointList *list = (RealPointList*)property->m_pSourceVariable;

	// clear previous list content
	bool fDelState = list->GetDeleteContents();

	list->DeleteContents(true);
    list->Clear();
	list->DeleteContents(fDelState);

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
            list->Append(new wxRealPoint(xsRealPointPropIO::FromString(listNode->GetNodeContent())));
        }

        listNode = listNode->GetNext();
    }
}

void xsListRealPointPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    RealPointList *list = (RealPointList*)property->m_pSourceVariable;

    if( !list->IsEmpty() )
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
		RealPointList::compatibility_iterator listNode = list->GetFirst();
        while(listNode)
        {
            AddPropertyNode(newNode, wxT("item"), xsRealPointPropIO::ToString(*(wxRealPoint*)listNode->GetData()));
            listNode = listNode->GetNext();
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsListRealPointPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((RealPointList*)property->m_pSourceVariable));
}

void xsListRealPointPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((RealPointList*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsListRealPointPropIO::ToString(const RealPointList& value)
{
	wxString out;

	RealPointList::compatibility_iterator node = value.GetFirst();
	while( node )
	{
		out << xsRealPointPropIO::ToString(*(wxRealPoint*)node->GetData());
		if( node != value.GetLast() ) out << wxT("|");

		node = node->GetNext();
	}

	return out;
}

RealPointList xsListRealPointPropIO::FromString(const wxString& value)
{
	RealPointList lstData;
	
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		lstData.Append( new wxRealPoint(xsRealPointPropIO::FromString( tokens.GetNextToken() )) );
	}
	
	return lstData;
}

/////////////////////////////////////////////////////////////////////////////////////
// xsListSerializablePropIO class ///////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsListSerializablePropIO, xsPropertyIO);

void xsListSerializablePropIO::Read(xsProperty *property, wxXmlNode *source)
{
	SerializableList& list = *(SerializableList*)property->m_pSourceVariable;

	// clear previous list content
	bool fDelState = list.GetDeleteContents();

	list.DeleteContents(true);
    list.Clear();
	list.DeleteContents(fDelState);

	wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
		if( listNode->GetName() == wxT("object") )
		{
#if wxVERSION_NUMBER < 2900
			xsSerializable* object = (xsSerializable*)wxCreateDynamicObject(listNode->GetPropVal(wxT("type"), wxT("")));
#else
			xsSerializable* object = (xsSerializable*)wxCreateDynamicObject(listNode->GetAttribute(wxT("type"), wxT("")));
#endif
			if(object)
			{
				object->DeserializeObject(listNode);
				list.Append( object );
			}
		}
		
		listNode = listNode->GetNext();
	}
}

void xsListSerializablePropIO::Write(xsProperty *property, wxXmlNode *target)
{
    SerializableList& list = *(SerializableList*)property->m_pSourceVariable;

    if( !list.IsEmpty() )
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
		SerializableList::compatibility_iterator listNode = list.GetFirst();
        while(listNode)
        {
			xsSerializable *object = listNode->GetData();
			newNode->AddChild(object->SerializeObject(NULL));			
			
            listNode = listNode->GetNext();
        }

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsListSerializablePropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((SerializableList*)property->m_pSourceVariable));
}

void xsListSerializablePropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((SerializableList*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsListSerializablePropIO::ToString(const SerializableList& value)
{
	return wxString::Format(wxT("Serializable list at address 0x%x"), &value);
}

SerializableList xsListSerializablePropIO::FromString(const wxString& value)
{
	wxUnusedVar( value );
	
	return SerializableList();
}

/////////////////////////////////////////////////////////////////////////////////////
// xsDynObjPropIO class /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsDynObjPropIO, xsPropertyIO);

void xsDynObjPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    wxXmlNode *objectNode = source->GetChildren();

    if( objectNode && (objectNode->GetName() == wxT("object")) )
    {
#if wxVERSION_NUMBER < 2900
        *(xsSerializable**)(property->m_pSourceVariable) = (xsSerializable*)wxCreateDynamicObject(objectNode->GetPropVal(wxT("type"), wxT("")));
#else
        *(xsSerializable**)(property->m_pSourceVariable) = (xsSerializable*)wxCreateDynamicObject(objectNode->GetAttribute(wxT("type"), wxT("")));
#endif

        xsSerializable* object = *(xsSerializable**)(property->m_pSourceVariable);
        if(object)
        {
            object->DeserializeObject(objectNode);
        }
    }
}

void xsDynObjPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    xsSerializable* object = *(xsSerializable**)(property->m_pSourceVariable);

    if( object && object->IsKindOf(CLASSINFO(xsSerializable)))
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        newNode->AddChild(object->SerializeObject(NULL));

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsDynObjPropIO::GetValueStr(xsProperty *property)
{
    return ToString(**(xsSerializable**)(property->m_pSourceVariable));
}

void xsDynObjPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	**((xsSerializable**)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsDynObjPropIO::ToString(const xsSerializable& value)
{
	return wxString::Format(wxT("Dynamic object at address 0x%x"), &value);
}

xsSerializable xsDynObjPropIO::FromString(const wxString& value)
{
	wxUnusedVar( value );
	
	return xsSerializable();
}

/////////////////////////////////////////////////////////////////////////////////////
// xsDynNCObjPropIO class ///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsDynNCObjPropIO, xsPropertyIO);

void xsDynNCObjPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    wxXmlNode *objectNode = source->GetChildren();

    if( objectNode && (objectNode->GetName() == wxT("object")) )
    {
        xsSerializable* object = *(xsSerializable**)(property->m_pSourceVariable);
        if(object)
        {
            object->DeserializeObject(objectNode);
        }
    }
}

void xsDynNCObjPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    xsSerializable* object = *(xsSerializable**)(property->m_pSourceVariable);

    if( object && object->IsKindOf(CLASSINFO(xsSerializable)))
    {
        wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
        newNode->AddChild(object->SerializeObject(NULL));

        target->AddChild(newNode);
        AppendPropertyType(property, newNode);
    }
}

wxString xsDynNCObjPropIO::GetValueStr(xsProperty *property)
{
    return ToString(**(xsSerializable**)(property->m_pSourceVariable));
}

void xsDynNCObjPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	**((xsSerializable**)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsDynNCObjPropIO::ToString(const xsSerializable& value)
{
	return wxString::Format(wxT("Dynamic object at address 0x%x"), &value);
}

xsSerializable xsDynNCObjPropIO::FromString(const wxString& value)
{
	wxUnusedVar( value );
	
	return xsSerializable();
}

/////////////////////////////////////////////////////////////////////////////////////
// xsStaticObjPropIO class //////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsStaticObjPropIO, xsPropertyIO);

void xsStaticObjPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    wxXmlNode *objectNode = source->GetChildren();

    if( objectNode && (objectNode->GetName() == wxT("object")) )
    {
        (*((xsSerializable*)property->m_pSourceVariable)).DeserializeObject(objectNode);
    }
}

void xsStaticObjPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    wxXmlNode *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
    newNode->AddChild((*((xsSerializable*)property->m_pSourceVariable)).SerializeObject(NULL));

    target->AddChild(newNode);
    AppendPropertyType(property, newNode);
}

wxString xsStaticObjPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*(xsSerializable*)property->m_pSourceVariable);
}

void xsStaticObjPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((xsSerializable*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsStaticObjPropIO::ToString(const xsSerializable& value)
{
	return wxString::Format(wxT("Static object at address 0x%x"), &value);
}

xsSerializable xsStaticObjPropIO::FromString(const wxString& value)
{
	wxUnusedVar( value );
	
	return xsSerializable();
}

/////////////////////////////////////////////////////////////////////////////////////
// xsMapStringPropIO class ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(xsMapStringPropIO, xsPropertyIO);

void xsMapStringPropIO::Read(xsProperty *property, wxXmlNode *source)
{
    ((StringMap*)property->m_pSourceVariable)->clear();

    wxXmlNode *listNode = source->GetChildren();
    while(listNode)
    {
        if(listNode->GetName() == wxT("item"))
        {
#if wxVERSION_NUMBER < 2900
            (*(StringMap*)property->m_pSourceVariable)[listNode->GetPropVal( wxT("key"), wxT("undef_key") )] = listNode->GetNodeContent();
#else
            (*(StringMap*)property->m_pSourceVariable)[listNode->GetAttribute( wxT("key"), wxT("undef_key") )] = listNode->GetNodeContent();
#endif
        }

        listNode = listNode->GetNext();
    }
}

void xsMapStringPropIO::Write(xsProperty *property, wxXmlNode *target)
{
    StringMap& map = *((StringMap*)property->m_pSourceVariable);

	if( !map.empty() )
	{
		wxXmlNode *pXmlNode, *newNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("property"));
		StringMap::iterator it;

		for( it = map.begin(); it != map.end(); ++it )
		{
			wxString key = it->first, value = it->second;
			pXmlNode = AddPropertyNode(newNode, wxT("item"), it->second);
#if wxVERSION_NUMBER < 2900
			pXmlNode->AddProperty(wxT("key"), it->first);
#else
			pXmlNode->AddAttribute(wxT("key"), it->first);
#endif
		}

		target->AddChild(newNode);
		AppendPropertyType(property, newNode);
	}
}

wxString xsMapStringPropIO::GetValueStr(xsProperty *property)
{
	return ToString(*((StringMap*)property->m_pSourceVariable));
}

void xsMapStringPropIO::SetValueStr(xsProperty *property, const wxString& valstr)
{
	*((StringMap*)property->m_pSourceVariable) = FromString(valstr);
}

wxString xsMapStringPropIO::ToString(const StringMap& value)
{
 	wxString out;

	StringMap::const_iterator it;

	for( it = value.begin(); it != value.end(); ++it )
	{
	    if( it != value.begin() ) out << wxT("|");
		out << it->first << wxT("->") << it->second;
	}

	return out;
}

StringMap xsMapStringPropIO::FromString(const wxString& value)
{
	StringMap mapData;
	
	wxString token;
	wxStringTokenizer tokens( value, wxT("|") );
	while( tokens.HasMoreTokens() )
	{
		token = tokens.GetNextToken();
		token.Replace(wxT("->"), wxT("|"));
		mapData[token.BeforeFirst(wxT('|'))] = token.AfterFirst(wxT('|'));
	}
	
	return mapData;
}

