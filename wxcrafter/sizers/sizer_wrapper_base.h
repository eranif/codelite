#ifndef SIZERWRAPPERBASE_H
#define SIZERWRAPPERBASE_H

#include "wxc_widget.h" // Base class: WrapperBase

class SizerWrapperBase : public wxcWidget
{

public:
    SizerWrapperBase();
    ~SizerWrapperBase() override = default;

protected:
    wxString GenerateMinSizeCode() const;
    wxString GenerateMinSizeXRC() const;

public:
    void GetIncludeFile(wxArrayString& headers) const override;

    /**
     * @brief can this wrapper be an actual Window parent?
     */
    bool IsValidParent() const override { return false; }

    /**
     * @brief Is this a 'real' control, or something else e.g. a sizer or a toolbar separator, that doesn't do events
     */
    bool IsEventHandler() const override { return false; }

    /**
     * @brief return true indicating that this class wraps a wxSizer
     */
    bool IsSizer() const override { return true; }

    /**
     * @brief return true if this sizer has to be kept as a class member
     */
    bool KeepAsClassMember() const override;

    /**
     * @brief return true if this sizer's parent is
     * an actual window / control and *not* sizer item
     */
    bool IsMainSizer() const;
};

#endif // SIZERWRAPPERBASE_H
