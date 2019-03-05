#ifndef CLDOCKERSETTINGS_H
#define CLDOCKERSETTINGS_H

#include "cl_config.h"
#include <wx/filename.h>

class clDockerSettings : public clConfigItem
{
    wxFileName m_docker;
    wxFileName m_dockerCompose;
    size_t m_flags = 0;

public:
    enum {
        kLinkEditor = (1 << 0),
        kRemoveAllImages = (1 << 1),
    };

    void EnableFlag(int flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    bool HasFlag(int flag) const { return (m_flags & flag); }

public:
    virtual void FromJSON(const JSONItem& json);
    virtual JSONItem ToJSON() const;

    clDockerSettings();
    virtual ~clDockerSettings();

    void Load();
    void Save();

    void SetDocker(const wxFileName& docker) { this->m_docker = docker; }
    void SetDockerCompose(const wxFileName& dockerCompose) { this->m_dockerCompose = dockerCompose; }
    const wxFileName& GetDocker() const { return m_docker; }
    const wxFileName& GetDockerCompose() const { return m_dockerCompose; }
    void SetLinkEditor(bool b) { EnableFlag(kLinkEditor, b); }
    bool IsLinkEditor() const { return HasFlag(kLinkEditor); }
    bool IsRemoveAllImages() const { return HasFlag(kRemoveAllImages); }
    void SetRemoveAllImages(bool b) { EnableFlag(kRemoveAllImages, b); }
};
#endif // CLDOCKERSETTINGS_H
