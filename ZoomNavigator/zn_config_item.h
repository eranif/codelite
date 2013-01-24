#ifndef ZNCONFIGITEM_H
#define ZNCONFIGITEM_H

#include "cl_config.h" // Base class: clConfigItem

class znConfigItem : public clConfigItem
{
    wxString m_highlightColour;
    bool     m_enabled;
    int      m_zoomFactor;

public:
    znConfigItem();
    virtual ~znConfigItem();

public:
    virtual void FromJSON(const JSONElement& json);
    virtual JSONElement ToJSON() const;

    void SetEnabled(bool enabled) {
        this->m_enabled = enabled;
    }
    void SetHighlightColour(const wxString& highlightColour) {
        this->m_highlightColour = highlightColour;
    }
    bool IsEnabled() const {
        return m_enabled;
    }
    const wxString& GetHighlightColour() const {
        return m_highlightColour;
    }
    void SetZoomFactor(int zoomFactor) {
        this->m_zoomFactor = zoomFactor;
    }
    int GetZoomFactor() const {
        return m_zoomFactor;
    }
};

#endif // ZNCONFIGITEM_H
