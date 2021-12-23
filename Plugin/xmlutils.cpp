//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : xmlutils.cpp
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
#include "xmlutils.h"

// wxXmlNode compatibilty macros
#include "wx_xml_compatibility.h"

wxXmlNode* XmlUtils::FindNodeByName(const wxXmlNode* parent, const wxString& tagName, const wxString& name)
{
    if(!parent) {
        return NULL;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == tagName) {
            if((child->GetPropVal(wxT("Name"), wxEmptyString) == name) ||
               (child->GetPropVal(wxT("name"), wxEmptyString) == name)) {
                return child;
            }
        }
        child = child->GetNext();
    }
    return NULL;
}

wxXmlNode* XmlUtils::FindFirstByTagName(const wxXmlNode* parent, const wxString& tagName)
{
    if(!parent) {
        return NULL;
    }

    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == tagName) {
            return child;
        }
        child = child->GetNext();
    }
    return NULL;
}

wxXmlNode* XmlUtils::FindLastByTagName(const wxXmlNode* parent, const wxString& tagName)
{
    wxXmlNode* last_node = NULL;
    wxXmlNode* child = parent->GetChildren();
    while(child) {
        if(child->GetName() == tagName) {
            last_node = child;
        }
        child = child->GetNext();
    }
    return last_node;
}

void XmlUtils::UpdateProperty(wxXmlNode* node, const wxString& name, const wxString& value)
{
    wxXmlProperty* prop = node->GetProperties();
    while(prop) {
        if(prop->GetName() == name) {
            prop->SetValue(value);
            return;
        }
        prop = prop->GetNext();
    }

    // No such property, create new one and add it
    node->AddProperty(name, value);
}

wxString XmlUtils::ReadString(const wxXmlNode* node, const wxString& propName, const wxString& defaultValue)
{
    return node->GetPropVal(propName, defaultValue);
}

bool XmlUtils::ReadStringIfExists(const wxXmlNode* node, const wxString& propName, wxString& value)
{
    return node->GetPropVal(propName, &value);
}

long XmlUtils::ReadLong(const wxXmlNode* node, const wxString& propName, long defaultValue)
{
    wxString val = node->GetPropVal(propName, wxEmptyString);
    if(val.IsEmpty()) {
        return defaultValue;
    }

    if(val.StartsWith(wxT("\""))) {
        val = val.AfterFirst(wxT('"'));
    }
    if(val.EndsWith(wxT("\""))) {
        val = val.BeforeLast(wxT('"'));
    }
    long retVal = defaultValue;
    val.ToLong(&retVal);
    return retVal;
}

bool XmlUtils::ReadLongIfExists(const wxXmlNode* node, const wxString& propName, long& answer)
{
    wxString value;
    if(!node->GetPropVal(propName, &value)) {
        return false;
    }

    if(value.StartsWith(wxT("\""))) {
        value = value.AfterFirst(wxT('"'));
    }
    if(value.EndsWith(wxT("\""))) {
        value = value.BeforeLast(wxT('"'));
    }

    bool retVal = value.ToLong(&answer);
    return retVal;
}

bool XmlUtils::ReadBool(const wxXmlNode* node, const wxString& propName, bool defaultValue)
{
    wxString val = node->GetPropVal(propName, wxEmptyString);

    if(val.IsEmpty()) {
        return defaultValue;
    }

    bool retVal = defaultValue;
    if(val.CmpNoCase(wxT("yes")) == 0) {
        retVal = true;
    } else {
        retVal = false;
    }
    return retVal;
}

bool XmlUtils::ReadBoolIfExists(const wxXmlNode* node, const wxString& propName, bool& answer)
{
    wxString value;
    if(!node->GetPropVal(propName, &value)) {
        return false;
    }

    if(value.CmpNoCase(wxT("yes")) == 0) {
        answer = true;
    } else {
        answer = false;
    }
    return true;
}

wxArrayString XmlUtils::ChildNodesContentToArray(const wxXmlNode* node, const wxString& tagName /* = wxT("")*/)
{
    wxArrayString arr;

    if(!node) {
        return arr;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if(tagName.empty() || child->GetName() == tagName) {
            arr.Add(child->GetNodeContent());
        }
        child = child->GetNext();
    }

    return arr;
}

wxString XmlUtils::ChildNodesContentToString(const wxXmlNode* node, const wxString& tagName /* = wxT("")*/,
                                             const wxString& separator /* = wxT(";")*/)
{
    wxString str;

    if(!node) {
        return str;
    }

    wxXmlNode* child = node->GetChildren();
    while(child) {
        if(tagName.empty() || child->GetName() == tagName) {
            str << child->GetNodeContent() << separator;
        }
        child = child->GetNext();
    }

    if(!str.empty()) {
        str.RemoveLast(separator.Len());
    }

    return str;
}

void XmlUtils::SetNodeContent(wxXmlNode* node, const wxString& text)
{
    wxXmlNode* n = node->GetChildren();
    wxXmlNode* contentNode = NULL;
    while(n) {
        if(n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE) {
            contentNode = n;
            break;
        }
        n = n->GetNext();
    }

    if(contentNode) {
        // remove old node
        node->RemoveChild(contentNode);
        delete contentNode;
    }

    if(!text.IsEmpty()) {
        contentNode = new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, text);
        node->AddChild(contentNode);
    }
}

void XmlUtils::RemoveChildren(wxXmlNode* node)
{
    wxXmlNode* child = node->GetChildren();
    while(child) {
        wxXmlNode* nextChild = child->GetNext();
        node->RemoveChild(child);
        delete child;
        child = nextChild;
    }
}

void XmlUtils::SetCDATANodeContent(wxXmlNode* node, const wxString& text)
{
    wxXmlNode* n = node->GetChildren();
    wxXmlNode* contentNode = NULL;
    while(n) {
        if(n->GetType() == wxXML_TEXT_NODE || n->GetType() == wxXML_CDATA_SECTION_NODE) {
            contentNode = n;
            break;
        }
        n = n->GetNext();
    }

    if(contentNode) {
        // remove old node
        node->RemoveChild(contentNode);
        delete contentNode;
    }

    if(!text.IsEmpty()) {
        contentNode = new wxXmlNode(wxXML_CDATA_SECTION_NODE, wxEmptyString, text);
        node->AddChild(contentNode);
    }
}

bool XmlUtils::StaticReadObject(wxXmlNode* root, const wxString& name, SerializedObject* obj)
{
    // find the object node in the xml file

    wxXmlNode* node = XmlUtils::FindNodeByName(root, wxT("ArchiveObject"), name);
    if(node) {

        // Check to see if we need a version check
        wxString objectVersion = obj->GetVersion();
        if(objectVersion.IsEmpty() == false) {
            if(node->GetPropVal(wxT("Version"), wxT("")) != objectVersion) {
                return false;
            }
        }

        Archive arch;
        arch.SetXmlNode(node);
        obj->DeSerialize(arch);
        return true;
    }
    return false;
}

bool XmlUtils::StaticWriteObject(wxXmlNode* root, const wxString& name, SerializedObject* obj)
{
    if(!root)
        return false;

    Archive arch;
    wxXmlNode* child = XmlUtils::FindNodeByName(root, wxT("ArchiveObject"), name);
    if(child) {
        wxXmlNode* n = root;
        n->RemoveChild(child);
        delete child;
    }

    // create new xml node for this object
    child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, wxT("ArchiveObject"));
    root->AddChild(child);

    wxString objectVersion = obj->GetVersion();
    if(objectVersion.IsEmpty() == false)
        child->AddProperty(wxT("Version"), objectVersion);

    child->AddProperty(wxT("Name"), name);

    arch.SetXmlNode(child);
    // serialize the object into the archive
    obj->Serialize(arch);
    return true;
}
