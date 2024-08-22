#include "XVariable.h"

#include "xmlutils.h"

#include <wx/base64.h>

///-----------------------------------------------------
/// XVariable 
///-----------------------------------------------------

XVariable::XVariable(wxXmlNode* xmlProp, bool useDoubleQoutesOnStrings)
    : numchildren(0)
{
    FromXML( xmlProp, useDoubleQoutesOnStrings );
}

void XVariable::FromXML(wxXmlNode* xmlProp, bool useDoubleQoutesOnStrings)
{
    this->fullname    = XmlUtils::ReadString(xmlProp, "fullname");
    this->name        = XmlUtils::ReadString(xmlProp, "name");
    this->classname   = XmlUtils::ReadString(xmlProp, "classname");
    this->type        = XmlUtils::ReadString(xmlProp, "type");
    this->numchildren = XmlUtils::ReadLong(xmlProp, "numchildren", 0);
    
    wxString encoding = XmlUtils::ReadString(xmlProp, "encoding");
    this->value = xmlProp->GetNodeContent();
    
    // Check if decode is needed
    if ( encoding.IsEmpty() == false ) {
        wxMemoryBuffer memBuf = ::wxBase64Decode(this->value);
        this->value = wxString((char*)memBuf.GetData(), memBuf.GetDataLen());
    }

    // escape newlines/whitespaces with readable chars
    this->value.Replace("\n", "\\n");
    this->value.Replace("\r", "\\r");
    this->value.Replace("\t", "\\t");
    this->value.Replace("\v", "\\v");
    this->value.Replace("\b", "\\b");
    
    if ( this->type == "string" && useDoubleQoutesOnStrings ) {
        // wrap the value in ""
        this->value.Prepend("\"").Append("\"");
    }
    
    wxXmlNode* child = xmlProp->GetChildren();
    while ( child ) {
        if ( child->GetName() == "property" ) {
            XVariable c;
            c.FromXML( child, useDoubleQoutesOnStrings );
            children.push_back( c );
        }
        child = child->GetNext();
    }
}

wxString XVariable::ToString() const
{
    wxString tostr;
    tostr   << "Name     :" << this->name << "\n"
            << "Fullname :" << this->fullname << "\n"
            << "Class    :" << this->classname << "\n"
            << "Value    :" << "\n"
            << "[" << this->value << "]\n";
    return tostr;
}

