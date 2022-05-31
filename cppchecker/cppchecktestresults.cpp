//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : cppchecktestresults.cpp
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

#include "cppchecktestresults.h"

#include "wx_xml_compatibility.h"

#include <wx/filename.h>
#include <wx/sstream.h>
#include <wx/xml/xml.h>

CppCheckResult::~CppCheckResult() {}

//------------------------------------------------------------

CppCheckTestResults::CppCheckTestResults()
    : m_errorCount(0)
{
}

CppCheckTestResults::~CppCheckTestResults() {}

void CppCheckTestResults::AddResultsForFile(const wxString& xmlOutput)
{
    // Create XML parser
    wxStringInputStream xmlStream(xmlOutput);
    wxXmlDocument xmlDoc(xmlStream);
    if(!xmlDoc.IsOk()) {
        return;
    }

    wxXmlNode* root = xmlDoc.GetRoot();
    if(!root || root->GetName() != wxT("results")) {
        return;
    }

    std::vector<CppCheckResult>* v(NULL);
    wxString fileName;
    for(wxXmlNode* child = root->GetChildren(); child; child = child->GetNext()) {
        if(child->GetName() == wxT("error")) {
            CppCheckResult cppCheckResult;
            child->GetAttribute(wxT("id"), &cppCheckResult.id);
            child->GetAttribute(wxT("file"), &fileName);

            // Normalize the path
            wxFileName fn(fileName);
            fn.Normalize(wxPATH_NORM_ALL & ~(wxPATH_NORM_LONG));
            cppCheckResult.filename = fn.GetFullPath();

            wxString sLine;
            child->GetAttribute(wxT("line"), &sLine);
            sLine.ToLong(&cppCheckResult.lineno);
            child->GetAttribute(wxT("severity"), &cppCheckResult.severity);
            child->GetAttribute(wxT("msg"), &cppCheckResult.msg);

            // Locate the entry that matches this file name
            std::map<wxString, std::vector<CppCheckResult>*>::iterator iter = m_results.find(fileName);
            if(iter == m_results.end()) {
                v = new std::vector<CppCheckResult>();
                m_results[fn.GetFullPath()] = v;
            } else {
                v = iter->second;
            }

            v->push_back(cppCheckResult);
            m_errorCount++;
            v = NULL;
        }
    }
}

std::vector<CppCheckResult>* CppCheckTestResults::GetResultsForFile(const wxString& filename)
{
    std::map<wxString, std::vector<CppCheckResult>*>::iterator iter = m_results.find(filename);
    if(iter != m_results.end())
        return iter->second;

    return NULL;
}

void CppCheckTestResults::ClearAll()
{
    std::map<wxString, std::vector<CppCheckResult>*>::iterator iter = m_results.begin();
    for(; iter != m_results.end(); iter++)
        delete iter->second;
    m_results.clear();
    m_errorCount = 0;
}

wxArrayString CppCheckTestResults::GetFiles()
{
    wxArrayString files;
    std::map<wxString, std::vector<CppCheckResult>*>::iterator iter = m_results.begin();
    for(; iter != m_results.end(); iter++)
        files.Add(iter->first);
    return files;
}
