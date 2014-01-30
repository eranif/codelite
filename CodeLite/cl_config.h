#ifndef CLCONFIG_H
#define CLCONFIG_H

#include "codelite_exports.h"
#include "json_node.h"

////////////////////////////////////////////////////////

class WXDLLIMPEXP_CL clConfigItem
{
protected:
    wxString m_name;

public:
    clConfigItem(const wxString &name)
        : m_name(name)
    {}

    virtual ~clConfigItem()
    {}

    const wxString& GetName() const {
        return m_name;
    }

    void SetName(const wxString& name) {
        this->m_name = name;
    }
    virtual void FromJSON(const JSONElement& json) = 0;
    virtual JSONElement ToJSON() const = 0;
};



////////////////////////////////////////////////////////

class WXDLLIMPEXP_CL clConfig
{
protected:
    wxFileName m_filename;
    JSONRoot* m_root;

protected:
    void DoDeleteProperty(const wxString &property);
    JSONElement GetGeneralSetting();

public:
    // We provide a global configuration
    // and the ability to allocate a private copy with a different file
    clConfig(const wxString& filename = "codelite.conf");
    virtual ~clConfig();
    static clConfig& Get();

    // Re-read the content from the disk
    void Reload();
    // Save the content to a give file name
    void Save(const wxFileName& fn);
    // Save the content the file passed on the construction
    void Save();

    // Utility functions
    //------------------------------

    // Merge 2 arrays of strings into a single array with all duplicate entries removed
    wxArrayString MergeArrays(const wxArrayString& arr1, const wxArrayString &arr2) const;
    JSONElement::wxStringMap_t MergeStringMaps(const JSONElement::wxStringMap_t& map1, const JSONElement::wxStringMap_t &map2) const;

    // Workspace tab order
    //------------------------------
    void SetWorkspaceTabOrder( const wxArrayString& tabs, int selected );
    bool GetWorkspaceTabOrder( wxArrayString& tabs, int &selected );

    // General objects
    // -----------------------------
    bool ReadItem(clConfigItem* item, const wxString &differentName = wxEmptyString);
    void WriteItem(const clConfigItem* item, const wxString &differentName = wxEmptyString);
    // bool
    bool Read (const wxString& name, bool defaultValue);
    void Write(const wxString& name, bool value);
    // int
    int  Read (const wxString& name, int defaultValue);
    void Write(const wxString& name, int value);
    // wxString
    wxString Read (const wxString& name, const wxString& defaultValue);
    void Write(const wxString& name, const wxString& value);
    
    // standard IDs for annoying dialogs
    int GetAnnoyingDlgAnswer(const wxString &name, int defaultValue = wxNOT_FOUND);
    void SetAnnoyingDlgAnswer(const wxString &name, int value);
    void ClearAnnoyingDlgAnswers();
};


#endif // CLCONFIG_H
