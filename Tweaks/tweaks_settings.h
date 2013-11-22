#ifndef TWEAKSSETTINGS_H
#define TWEAKSSETTINGS_H

#include "cl_config.h"
#include <wx/colour.h>
#include <map>

class ProjectTweaks : public clConfigItem
{
    wxColour m_tabFgColour;
    wxColour m_tabBgColour;
    wxString m_projectName;

public:
    typedef std::map<wxString, ProjectTweaks> Map_t;

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;
    ProjectTweaks();
    virtual ~ ProjectTweaks();
    
    bool IsOk() const {
        return !m_projectName.IsEmpty();
    }
    void SetTabBgColour(const wxColour& tabBgColour) {
        this->m_tabBgColour = tabBgColour;
    }
    void SetTabFgColour(const wxColour& tabFgColour) {
        this->m_tabFgColour = tabFgColour;
    }
    const wxColour& GetTabBgColour() const {
        return m_tabBgColour;
    }
    const wxColour& GetTabFgColour() const {
        return m_tabFgColour;
    }
    void SetProjectName(const wxString& projectName) {
        this->m_projectName = projectName;
    }
    const wxString& GetProjectName() const {
        return m_projectName;
    }
};

class TweaksSettings : public clConfigItem
{
    ProjectTweaks::Map_t m_projects;
public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    TweaksSettings();
    virtual ~TweaksSettings();

    TweaksSettings& Load();
    void Clear();
    void Save();
    
    const ProjectTweaks::Map_t& GetProjects() const {
        return m_projects;
    }
    
    const ProjectTweaks& GetProjectTweaks(const wxString &project) const;
    void UpdateProject(const ProjectTweaks& pt);
    void DeleteProject(const wxString& pt);
};

#endif // TWEAKSSETTINGS_H
