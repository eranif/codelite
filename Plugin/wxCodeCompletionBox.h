#ifndef WXCODECOMPLETIONBOX_H
#define WXCODECOMPLETIONBOX_H

#include "wxCodeCompletionBoxBase.h"
#include <wx/arrstr.h>
#include <wx/sharedptr.h>
#include <vector>
#include <list>
#include <wx/bitmap.h>
#include <wx/stc/stc.h>
#include <wx/font.h>

class wxCodeCompletionBox;
class wxStyledTextCtrl;
class WXDLLIMPEXP_SDK wxCodeCompletionBoxEntry
{
    wxString m_text;
    int m_imgIndex;
    wxClientData* m_clientData;
    wxRect m_itemRect;
    friend class wxCodeCompletionBox;
public:
    typedef wxSharedPtr<wxCodeCompletionBoxEntry> Ptr_t;
    typedef std::vector<wxCodeCompletionBoxEntry::Ptr_t> Vec_t;

public:
    wxCodeCompletionBoxEntry(const wxString& text, int imgId = wxNOT_FOUND, wxClientData* userData = NULL)
        : m_text(text)
        , m_imgIndex(imgId)
        , m_clientData(userData)
    {
    }

    virtual ~wxCodeCompletionBoxEntry()
    {
        wxDELETE(m_clientData);
        m_imgIndex = wxNOT_FOUND;
        m_text.Clear();
    }

    /**
     * @brief helper method for allocating wxCodeCompletionBoxEntry::Ptr
     * @return
     */
    static wxCodeCompletionBoxEntry::Ptr_t
    New(const wxString& text, int imgId = wxNOT_FOUND, wxClientData* userData = NULL)
    {
        wxCodeCompletionBoxEntry::Ptr_t pEntry(new wxCodeCompletionBoxEntry(text, imgId, userData));
        return pEntry;
    }

    void SetImgIndex(int imgIndex) { this->m_imgIndex = imgIndex; }
    void SetText(const wxString& text) { this->m_text = text; }
    int GetImgIndex() const { return m_imgIndex; }
    const wxString& GetText() const { return m_text; }
    /**
     * @brief set client data, deleting the old client data
     * @param clientData
     */
    void SetClientData(wxClientData* clientData)
    {
        wxDELETE(this->m_clientData);
        this->m_clientData = clientData;
    }
    wxClientData* GetClientData() { return m_clientData; }
};

class WXDLLIMPEXP_SDK wxCodeCompletionBox : public wxCodeCompletionBoxBase
{
public:
    typedef std::vector<wxBitmap> BmpVec_t;

protected:
    wxCodeCompletionBoxEntry::Vec_t m_allEntries;
    wxCodeCompletionBoxEntry::Vec_t m_entries;
    wxCodeCompletionBox::BmpVec_t m_bitmaps;

    int m_index;
    wxStyledTextCtrl* m_stc;
    wxFont m_ccFont;

protected:
    // Event handlers
    void OnUpdateList(wxStyledTextEvent& event);
    void OnStcKey(wxKeyEvent& event);
    void OnStcLeftDown(wxMouseEvent& event);
    void OnLeftDClick(wxMouseEvent& event);
    virtual void OnEraseBackground(wxEraseEvent& event);
    virtual void OnPaint(wxPaintEvent& event);

public:
    wxCodeCompletionBox(wxWindow* parent);
    virtual ~wxCodeCompletionBox();

    /**
     * @brief show the completion box, filter
     * @param word
     */
    void ShowCompletionBox(wxStyledTextCtrl* ctrl, const wxCodeCompletionBoxEntry::Vec_t& entries);

    void SetBitmaps(const wxCodeCompletionBox::BmpVec_t& bitmaps) { this->m_bitmaps = bitmaps; }
    const wxCodeCompletionBox::BmpVec_t& GetBitmaps() const { return m_bitmaps; }

protected:
    int GetSingleLineHeight() const;
    void FilterResults();
    void InsertSelection();
};
#endif // WXCODECOMPLETIONBOX_H
