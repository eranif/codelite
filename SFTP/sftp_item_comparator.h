#ifndef SFTPITEMCOMPARATOR_H
#define SFTPITEMCOMPARATOR_H

#include <wx/treelist.h>
#include <vector>

class MyClientData : public wxClientData
{
    wxString m_path;
    bool     m_initialized;
    bool     m_isFolder;

public:
    typedef std::vector<MyClientData*> Vector_t;

public:
    MyClientData(const wxString &path)
        : m_path(path)
        , m_initialized(false)
        , m_isFolder(false) {
        while (m_path.Replace("//", "/")) {}
        while (m_path.Replace("\\\\", "\\")) {}
    }

    virtual ~MyClientData() {}
    
    wxString GetBasename() const {
        return GetFullPath().BeforeLast('/');
    }
    wxString GetFullName() const {
        return GetFullPath().AfterLast('/');
    }
    void SetFullName( const wxString &fullname ) {
        wxString base = GetBasename();
        base << "/" << fullname;
        m_path.swap( base );
    }
    
    void SetInitialized(bool initialized) {
        this->m_initialized = initialized;
    }
    bool IsInitialized() const {
        return m_initialized;
    }
    void SetPath(const wxString& path) {
        this->m_path = path;
    }
    const wxString& GetFullPath() const {
        return m_path;
    }
    void SetIsFolder(bool isFolder) {
        this->m_isFolder = isFolder;
    }
    bool IsFolder() const {
        return m_isFolder;
    }
};


class SFTPItemComparator : public wxTreeListItemComparator
{
public:
    SFTPItemComparator();
    virtual ~SFTPItemComparator();
    virtual int Compare(wxTreeListCtrl* treelist, unsigned column, wxTreeListItem first, wxTreeListItem second);
};

#endif // SFTPITEMCOMPARATOR_H
