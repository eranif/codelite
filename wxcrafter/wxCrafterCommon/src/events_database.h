#ifndef EVENTSDATABASE_H
#define EVENTSDATABASE_H

#include "JSON.h"
#include "wx_ordered_map.h"

#include <unordered_map>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

class ConnectDetails
{
protected:
    wxString m_eventName;
    wxString m_eventClass;
    wxString m_description;
    wxString m_functionNameAndSignature;
    wxString m_ifBlock; // In case this event should be wrapped with #if / #endif block, mark it here

public:
    ConnectDetails() = default;

    ConnectDetails(const wxString& eventName,
                   const wxString& eventClass,
                   const wxString& description)
        : m_eventName(eventName)
        , m_eventClass(eventClass)
        , m_description(description)
    {
    }

    void SetIfBlock(const wxString& ifBlock) { this->m_ifBlock = ifBlock; }
    const wxString& GetIfBlock() const { return m_ifBlock; }

    JSONItem ToJSON() const
    {
        return nlohmann::json{{"m_eventName", m_eventName.ToStdString(wxConvUTF8)},
                              {"m_eventClass", m_eventClass.ToStdString(wxConvUTF8)},
                              {"m_functionNameAndSignature", m_functionNameAndSignature.ToStdString(wxConvUTF8)},
                              {"m_description", m_description.ToStdString(wxConvUTF8)}};
    }

    void FromJSON(const JSONItem& json)
    {
        m_eventName = json.namedObject(wxT("m_eventName")).toString();
        m_eventClass = json.namedObject(wxT("m_eventClass")).toString();
        m_functionNameAndSignature = json.namedObject(wxT("m_functionNameAndSignature")).toString();
        m_description = json.namedObject(wxT("m_description")).toString();
    }

    const wxString& GetDescription() const { return m_description; }
    const wxString& GetEventClass() const { return m_eventClass; }

    void SetEventName(const wxString& eventName) { this->m_eventName = eventName; }
    void SetFunctionNameAndSignature(const wxString& functionNameAndSignature);
    const wxString& GetEventName() const { return m_eventName; }
    const wxString& GetFunctionNameAndSignature() const { return m_functionNameAndSignature; }
    int GetMenuItemId() const { return wxXmlResource::GetXRCID(m_eventName); }
    void GenerateFunctionName(const wxString& controlName);
    void MakeSignatureForName(const wxString& name);
    wxString GetFunctionImpl(const wxString& classname) const;
    wxString GetFunctionDecl() const;
};

/////////////////////////////////////////////////////////////////////////////////////////

class EventsDatabase
{
public:
    using MapEvents_t = wxOrderedMap<wxString, ConnectDetails>;
    using MapMenuIdToName_t = std::unordered_map<int, wxString>;

protected:
    MapEvents_t m_events;
    MapMenuIdToName_t m_menuIdToName;

public:
    EventsDatabase() = default;
    virtual ~EventsDatabase();

    // API
    void FillCommonEvents();

    void Add(const ConnectDetails& ed);
    void Add(const wxString& eventName,
             const wxString& className,
             const wxString& description);
    void Clear();
    bool Exists(int menuId) const;
    ConnectDetails Item(int menuId) const;

    MapEvents_t& GetEvents() { return m_events; }
    const MapEvents_t& GetEvents() const { return m_events; }
};

#endif // EVENTSDATABASE_H
