#ifndef NODEJSEXECUTABLE_H
#define NODEJSEXECUTABLE_H

#include <wx/filename.h>

class NodeJSExecutable
{
    wxFileName m_exePath;

public:
    NodeJSExecutable();
    NodeJSExecutable(const wxFileName& exePath);
    virtual ~NodeJSExecutable();
    bool Exists() const;
    int GetMajorVersion() const;
};

#endif // NODEJSEXECUTABLE_H
