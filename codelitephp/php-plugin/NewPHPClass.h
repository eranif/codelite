//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : NewPHPClass.h
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

#ifndef NEWPHPCLASS_H
#define NEWPHPCLASS_H

#include "new_class.h"
#include <wx/filename.h>

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------

class PHPClassDetails
{
public:
    enum {
        GEN_CTOR                 = 0x00000001,
        GEN_DTOR                 = 0x00000002,
        GEN_SINGLETON            = 0x00000004,
        GEN_FOLDER_PER_NAMESPACE = 0x00000008,
    };

protected:
    wxFileName m_filepath;
    wxString   m_name;
    wxString   m_classNamespace;
    size_t     m_flags;
    wxString   m_type;
    wxArrayString m_extends;
    wxArrayString m_implements;

public:
    PHPClassDetails() : m_flags(0), m_type("class") {}
    virtual ~PHPClassDetails() {}

    wxString ToString(const wxString& EOL, const wxString& indent) const;
    void SetType(const wxString& type) {
        this->m_type = type;
    }
    void SetExtends(const wxArrayString& extends) {
        this->m_extends = extends;
    }
    void SetImplements(const wxArrayString& implements) {
        this->m_implements = implements;
    }
    const wxArrayString& GetExtends() const {
        return m_extends;
    }
    const wxArrayString& GetImplements() const {
        return m_implements;
    }
    const wxString& GetType() const {
        return m_type;
    }
    void SetClassNamespace(const wxString& classNamespace) {
        this->m_classNamespace = classNamespace;
    }
    const wxString& GetNamespace() const {
        return m_classNamespace;
    }
    void SetName(const wxString& name) {
        this->m_name = name;
    }
    const wxString& GetName() const {
        return m_name;
    }
    void SetFilepath(const wxFileName& filepath) {
        this->m_filepath = filepath;
    }
    const wxFileName& GetFilepath() const {
        return m_filepath;
    }
    void SetFlags(size_t flags) {
        this->m_flags = flags;
    }
    size_t GetFlags() const {
        return m_flags;
    }

    bool IsClass() const {
        return GetType() == "class";
    }
};

// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------

class NewPHPClass : public NewPHPClassBase
{
    wxString m_outputPath;
public:
    NewPHPClass(wxWindow* parent, const wxString &classPath);
    virtual ~NewPHPClass();

protected:
    virtual void OnEditExtends(wxCommandEvent& event);
    virtual void OnEditImplements(wxCommandEvent& event);
    virtual void OnMakeSingletonUI(wxUpdateUIEvent& event);
    virtual void OnFolderPerNamespace(wxCommandEvent& event);
    virtual void OnNamespaceTextUpdated(wxCommandEvent& event);
    virtual void OnClassNameUpdate(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);

public:
    PHPClassDetails GetDetails() const;
};
#endif // NEWPHPCLASS_H
