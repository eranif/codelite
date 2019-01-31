#ifndef SIZERWRAPPERBASE_H
#define SIZERWRAPPERBASE_H

#include "wxc_widget.h" // Base class: WrapperBase

class SizerWrapperBase : public wxcWidget
{

public:
    SizerWrapperBase();
    virtual ~SizerWrapperBase();

protected:
    wxString GenerateMinSizeCode() const;
    wxString GenerateMinSizeXRC() const;

public:
    virtual void GetIncludeFile(wxArrayString& headers) const;

    /**
     * @brief can this wrapper be an actual Window parent?
     */
    bool IsValidParent() const { return false; }

    /**
     * @brief Is this a 'real' control, or something else e.g. a sizer or a toolbar separator, that doesn't do events
     */
    virtual bool IsEventHandler() const { return false; }

    /**
     * @brief return true indicating that this class wraps a wxSizer
     */
    virtual bool IsSizer() const { return true; }

    /**
     * @brief return true if this sizer's parent is
     * an actual window / control and *not* sizer item
     */
    bool IsMainSizer() const;
};

#endif // SIZERWRAPPERBASE_H
