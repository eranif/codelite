#pragma once

#include <wx/version.h>

#if wxCHECK_VERSION(3, 1, 0)

#include "codelite_exports.h"
#include "file_logger.h"

#include <wx/aui/serializer.h>
#include <wx/sstream.h>
#include <wx/stream.h>
#include <wx/string.h>
#include <wx/xml/xml.h>

// Sample serializer and deserializer implementations for saving and loading notebook tabs.
class clAuiSerializer : public wxAuiBookSerializer
{
public:
    clAuiSerializer()
    {
        m_book = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "wxAuiNotebook");
        m_doc.SetRoot(m_book);
    }

    virtual ~clAuiSerializer() {}

    wxString GetXML() const
    {
        wxStringOutputStream oss;
        if (!m_doc.Save(oss)) {
            clERROR() << "Failed to save XML document." << endl;
        }

        return oss.GetString();
    }

    virtual void BeforeSaveNotebook(const wxString& name) override { m_book->AddAttribute("name", name); }

    virtual void SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) override
    {
        auto node = new wxXmlNode(wxXML_ELEMENT_NODE, "tab");

        AddDockLayout(node, tab);
        AddPagesList(node, "pages", tab.pages);
        AddPagesList(node, "pinned", tab.pinned);
        AddChild(node, "active", tab.active);

        m_book->AddChild(node);
    }

private:
    void AddChild(wxXmlNode* parent, const wxString& name, const wxString& value)
    {
        auto node = new wxXmlNode(parent, wxXML_ELEMENT_NODE, name);
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, {}, value));
    }

    void AddChild(wxXmlNode* parent, const wxString& name, int value)
    {
        // Don't save 0 values, they're the default.
        if (value)
            AddChild(parent, name, wxString::Format("%u", value));
    }

    void AddChild(wxXmlNode* parent, const wxString& name, const wxRect& rect)
    {
        if (rect.GetPosition() != wxDefaultPosition || rect.GetSize() != wxDefaultSize) {
            AddChild(parent, name, wxString::Format("%d,%d %dx%d", rect.x, rect.y, rect.width, rect.height));
        }
    }

    // Common helper of SavePane() and SaveNotebookTabControl() which both need
    // to save the same layout information.
    void AddDockLayout(wxXmlNode* node, const wxAuiDockLayoutInfo& layout)
    {
        AddChild(node, "direction", layout.dock_direction);
        AddChild(node, "layer", layout.dock_layer);
        AddChild(node, "row", layout.dock_row);
        AddChild(node, "position", layout.dock_pos);
        AddChild(node, "proportion", layout.dock_proportion);
        AddChild(node, "size", layout.dock_size);
    }

    // Helper of SaveNotebookTabControl(): add a node with the given name
    // containing the comma-separated list of page indices if there are any.
    void AddPagesList(wxXmlNode* node, const wxString& name, const std::vector<int>& pages)
    {
        if (!pages.empty()) {
            wxString pagesList;
            for (auto page : pages) {
                if (!pagesList.empty())
                    pagesList << ',';

                pagesList << page;
            }

            AddChild(node, name, pagesList);
        }
    }

    wxXmlDocument m_doc;

    // Non-owning pointer to the root node of m_doc.
    wxXmlNode* m_book{nullptr};
};

class clAuiDeserializer : public wxAuiBookDeserializer
{
public:
    explicit clAuiDeserializer(const wxString& xml)
    {
        wxStringInputStream iss(xml);
        if (!m_doc.Load(iss)) {
            throw std::runtime_error("Failed to load XML document.");
        }
        m_book = m_doc.GetRoot();
    }

    virtual ~clAuiDeserializer() {}
    virtual std::vector<wxAuiTabLayoutInfo> LoadNotebookTabs(const wxString& name) override
    {
        return LoadNotebookTabs(m_book);
    }

private:
    int GetInt(const wxString& str)
    {
        int value;
        if (!str.ToInt(&value))
            throw std::runtime_error("Failed to parse integer");

        return value;
    }

    wxSize GetSize(const wxString& str)
    {
        wxString strH;
        const wxString strW = str.BeforeFirst('x', &strH);

        // Special case which wouldn't be parse by ToUInt() below.
        if (strW == "-1" && strH == strW)
            return wxDefaultSize;

        unsigned int w, h;
        if (!strW.ToUInt(&w) || !strH.ToUInt(&h))
            throw std::runtime_error("Failed to parse size");

        return wxSize(w, h);
    }

    wxRect GetRect(const wxString& str)
    {
        wxString strWH;
        const wxString strXY = str.BeforeFirst(' ', &strWH);

        wxString strY;
        const wxString strX = strXY.BeforeFirst(',', &strY);

        int x, y;
        if (!strX.ToInt(&x) || !strY.ToInt(&y))
            throw std::runtime_error("Failed to parse position");

        return wxRect(wxPoint(x, y), GetSize(strWH));
    }

    // Common helper of LoadPanes() and LoadNotebookTabs() which both need to
    // load the dock layout information.
    //
    // Returns true if we processed this node.
    bool LoadDockLayout(wxXmlNode* node, wxAuiDockLayoutInfo& info)
    {
        const wxString& name = node->GetName();
        const wxString& content = node->GetNodeContent();

        if (name == "direction") {
            info.dock_direction = GetInt(content);
        } else if (name == "layer") {
            info.dock_layer = GetInt(content);
        } else if (name == "row") {
            info.dock_row = GetInt(content);
        } else if (name == "position") {
            info.dock_pos = GetInt(content);
        } else if (name == "proportion") {
            info.dock_proportion = GetInt(content);
        } else if (name == "size") {
            info.dock_size = GetInt(content);
        } else {
            return false;
        }

        return true;
    }

    std::vector<wxAuiTabLayoutInfo> LoadNotebookTabs(wxXmlNode* book)
    {
        std::vector<wxAuiTabLayoutInfo> tabs;

        for (wxXmlNode* node = book->GetChildren(); node; node = node->GetNext()) {
            if (node->GetName() != "tab")
                throw std::runtime_error("Unexpected tab node name");

            wxAuiTabLayoutInfo tab;
            for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext()) {
                if (LoadDockLayout(child, tab))
                    continue;

                const auto& pageIndices = wxSplit(child->GetNodeContent(), ',');
                if (child->GetName() == "pages") {
                    for (const auto& s : pageIndices)
                        tab.pages.push_back(GetInt(s));
                } else if (child->GetName() == "pinned") {
                    for (const auto& s : pageIndices)
                        tab.pinned.push_back(GetInt(s));
                } else if (child->GetName() == "active") {
                    tab.active = GetInt(child->GetNodeContent());
                } else {
                    throw std::runtime_error("Unexpected tab child node name");
                }
            }

            tabs.push_back(tab);
        }

        return tabs;
    }

    wxXmlDocument m_doc;

    // Non-owning pointers to the nodes in m_doc.
    wxXmlNode* m_book{nullptr};
};

#endif
