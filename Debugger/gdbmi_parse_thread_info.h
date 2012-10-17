#ifndef GDBMIPARSETHREADINFO_H
#define GDBMIPARSETHREADINFO_H

#include <wx/string.h>
#include <vector>
#include "debugger.h"

class GdbMIThreadInfo
{
public:
    wxString threadId;
    wxString extendedName;
    wxString function;
    wxString file;
    wxString line;
    wxString active;
    
public:
    GdbMIThreadInfo() {}
    virtual ~GdbMIThreadInfo() {}

    ThreadEntry ToThreadEntry() const {
        ThreadEntry te;
        this->threadId.ToLong(&te.dbgid);
        te.active   = (this->active == "Yes");
        te.file     = this->file;
        te.function = this->function;
        te.line     = this->line;
        return te;
    }
};

typedef std::vector<GdbMIThreadInfo> GdbMIThreadInfoVec_t;

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

class GdbMIThreadInfoParser
{
    GdbMIThreadInfoVec_t m_threads;
protected:
    bool ReadBlock(wxString &input, const wxString &pair, wxString& block);
    bool ReadKeyValuePair(const wxString &input, const wxString &key, wxString &value);

public:
    GdbMIThreadInfoParser();
    virtual ~GdbMIThreadInfoParser();

    void Parse(const wxString &info);
    
    const GdbMIThreadInfoVec_t& GetThreads() const {
        return m_threads;
    }
};

#endif // GDBMIPARSETHREADINFO_H
