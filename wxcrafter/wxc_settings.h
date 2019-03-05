#ifndef WXCSETTINGS_H
#define WXCSETTINGS_H

#include "wxcLib/json_node.h"
#include <map>

// ----------------------------------------------------------------------
// CustomControlTemplate
// ----------------------------------------------------------------------
class CustomControlTemplate
{
protected:
    wxString m_includeFile;
    wxString m_allocationLine;
    wxString m_className;
    wxString m_xrcPreviewClass;
    int m_controlId;
    wxStringMap_t m_events;

public:
    CustomControlTemplate();
    ~CustomControlTemplate();

    JSONElement ToJSON() const;
    void FromJSON(const JSONElement& json);

    void SetEvents(const wxStringMap_t& events) { this->m_events = events; }
    const wxStringMap_t& GetEvents() const { return m_events; }
    void SetAllocationLine(const wxString& allocationLine) { this->m_allocationLine = allocationLine; }
    void SetIncludeFile(const wxString& includeFile) { this->m_includeFile = includeFile; }
    const wxString& GetAllocationLine() const { return m_allocationLine; }
    const wxString& GetIncludeFile() const { return m_includeFile; }
    void SetControlId(int controlId) { this->m_controlId = controlId; }
    void SetClassName(const wxString& className) { this->m_className = className; }
    const wxString& GetClassName() const { return m_className; }
    int GetControlId() const { return m_controlId; }
    bool IsValid() const { return m_controlId != wxNOT_FOUND; }
    void SetXrcPreviewClass(const wxString& xrcPreviewClass) { this->m_xrcPreviewClass = xrcPreviewClass; }
    const wxString& GetXrcPreviewClass() const { return m_xrcPreviewClass; }
};
typedef std::map<wxString, CustomControlTemplate> CustomControlTemplateMap_t;

// ----------------------------------------------------------------------
// wxcSettings
// ----------------------------------------------------------------------

class wxcSettings
{
public:
    enum {
        DLG_CODE_GENERATED = (1 << 0),
        USE_TABBED_MODE = (1 << 1),
        SIZERS_AS_MEMBERS = (1 << 2),
        LAYOUT_RESET_DONE = (1 << 3),
        LICENSE_ACTIVATED = (1 << 4),
        DONT_PROMPT_ABOUT_MISSING_SUBCLASS = (1 << 5),
        DISPLAY_EVENTS_PANE = (1 << 6),
        DUPLICATE_KEEPS_USERSET_NAMES = (1 << 7),
        DUPLICATE_KEEPS_ALL_NAMES = (1 << 8),
        DUPLICATE_EVENTHANDLERS_TOO = (1 << 9),
        FORMAT_INHERITED_FILES = (1 << 10),
        DONT_USE_RELATIVE_BITMAPS = (1 << 11),
        EXIT_MINIMIZE_TO_TRAY = (1 << 12),
    };

protected:
    size_t m_flags;
    CustomControlTemplateMap_t m_templateClasses;
    int m_sashPosition;
    int m_secondarySashPos;
    int m_treeviewSashPos;
    wxString m_serialNumber;
    wxString m_username;
    wxArrayString m_history;
    time_t m_ts;
    bool m_initCompleted;

public:
    static wxcSettings& Get();

private:
    wxcSettings();
    ~wxcSettings();

public:
    void Load();
    void Save();

    void EnableFlag(int flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }

    void ShowNagDialogIfNeeded();
    void SetInitCompleted(bool initCompleted) { this->m_initCompleted = initCompleted; }
    bool IsInitCompleted() const { return m_initCompleted; }

    bool HasFlag(int flag) const { return m_flags & flag; }
    /**
     * @brief merge custom controls to the current list
     */
    void MergeCustomControl(const JSONElement& arr);

    JSONElement GetCustomControlsAsJSON(const wxArrayString& controls) const;
    void SetTemplateClasses(const CustomControlTemplateMap_t& templateClasses)
    {
        this->m_templateClasses = templateClasses;
    }
    size_t GetAnnoyDialogs() const { return m_flags; }
    const CustomControlTemplateMap_t& GetTemplateClasses() const { return m_templateClasses; }

    void RegisterCustomControl(CustomControlTemplate& cct);
    void DeleteCustomControl(const wxString& name);

    bool GetShowDialog(int flag) const { return !(m_flags & flag); }

    CustomControlTemplate FindByControlId(int controlId) const;
    CustomControlTemplate FindByControlName(const wxString& name) const;

    void DontShowDialog(int flag) { m_flags |= flag; }

    void SetAnnoyDialogs(size_t annoyDialogs) { this->m_flags = annoyDialogs; }
    void SetSashPosition(int sashPosition) { this->m_sashPosition = sashPosition; }
    void SetSecondarySashPos(int secondarySashPos) { this->m_secondarySashPos = secondarySashPos; }
    void SetTreeviewSashPos(int treeviewSashPos) { this->m_treeviewSashPos = treeviewSashPos; }
    int GetSashPosition() const { return m_sashPosition; }
    int GetSecondarySashPos() const { return m_secondarySashPos; }
    int GetTreeviewSashPos() const { return m_treeviewSashPos; }

    /**
     * @brief is licensed with v1 license
     * @return
     */
    bool IsLicensed() const;
    bool IsRegistered() const;

    /**
     * @brief is licensed with v2 license
     * @return
     */
    bool IsLicensed2() const;

    void SetSerialNumber(const wxString& serialNumber) { this->m_serialNumber = serialNumber; }
    void SetUsername(const wxString& username) { this->m_username = username; }
    const wxString& GetSerialNumber() const { return m_serialNumber; }
    const wxString& GetUsername() const { return m_username; }
    void SetHistory(const wxArrayString& history) { this->m_history = history; }
    const wxArrayString& GetHistory() const { return m_history; }
};

#endif // WXCSETTINGS_H
