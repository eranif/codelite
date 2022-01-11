#ifndef EVENTSDATABASE_H
#define EVENTSDATABASE_H

#include "wx_ordered_map.h"
#include "wxcLib/json_node.h"

#include <map>
#include <unordered_map>
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/xrc/xmlres.h>

class ConnectDetails
{
protected:
    wxString m_eventName;
    wxString m_eventClass;
    wxString m_description;
    wxString m_functionNameAndSignature;
    bool m_noBody;
    wxString m_ifBlock; // In case this event should be wrapped with #if / #endif block, mark it here

public:
    ConnectDetails() {}

    ConnectDetails(const wxString& eventName, const wxString& eventClass, const wxString& description,
                   bool noBody = false, const wxString& functionNameAndSignature = "")
        : m_eventName(eventName)
        , m_eventClass(eventClass)
        , m_description(description)
        , m_functionNameAndSignature(functionNameAndSignature)
        , m_noBody(noBody)
    {
    }

    void SetIfBlock(const wxString& ifBlock) { this->m_ifBlock = ifBlock; }
    const wxString& GetIfBlock() const { return m_ifBlock; }

    JSONElement ToJSON() const
    {
        JSONElement element = JSONElement::createObject();
        element.addProperty(wxT("m_eventName"), m_eventName);
        element.addProperty(wxT("m_eventClass"), m_eventClass);
        element.addProperty(wxT("m_functionNameAndSignature"), m_functionNameAndSignature);
        element.addProperty(wxT("m_description"), m_description);
        element.addProperty(wxT("m_noBody"), m_noBody);
        return element;
    }

    void FromJSON(const JSONElement& json)
    {
        m_eventName = json.namedObject(wxT("m_eventName")).toString();
        m_eventClass = json.namedObject(wxT("m_eventClass")).toString();
        m_functionNameAndSignature = json.namedObject(wxT("m_functionNameAndSignature")).toString();
        m_description = json.namedObject(wxT("m_description")).toString();
        m_noBody = json.namedObject(wxT("m_noBody")).toBool();
    }

    void SetDescription(const wxString& description) { this->m_description = description; }
    const wxString& GetDescription() const { return m_description; }
    void SetEventClass(const wxString& eventClass) { this->m_eventClass = eventClass; }

    const wxString& GetEventClass() const { return m_eventClass; }

    void SetEventName(const wxString& eventName) { this->m_eventName = eventName; }
    void SetFunctionNameAndSignature(const wxString& functionNameAndSignature);
    void SetNoBody(bool noBody) { this->m_noBody = noBody; }
    const wxString& GetEventName() const { return m_eventName; }
    const wxString& GetFunctionNameAndSignature() const { return m_functionNameAndSignature; }
    bool GetNoBody() const { return m_noBody; }
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
    typedef wxOrderedMap<wxString, ConnectDetails> MapEvents_t;
    typedef std::unordered_map<int, wxString> MapMenuIdToName_t;

protected:
    MapEvents_t m_events;
    MapMenuIdToName_t m_menuIdToName;

public:
    EventsDatabase();
    virtual ~EventsDatabase();

    // API
    void FillCommonEvents();

    void Add(const ConnectDetails& ed);
    void Add(const wxString& eventName, const wxString& className, const wxString& description,
             const wxString& functionNameAndSig = "", bool noBody = false);
    void Clear();
    bool Exists(int menuId) const;
    ConnectDetails Item(int menuId) const;

    MapEvents_t& GetEvents() { return m_events; }
    const MapEvents_t& GetEvents() const { return m_events; }
    wxMenu* CreateMenu() const;
};

#endif // EVENTSDATABASE_H
