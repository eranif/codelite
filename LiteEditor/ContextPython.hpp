#ifndef CONTEXTPYTHON_HPP
#define CONTEXTPYTHON_HPP

#include "generic_context.h"

class ContextPython : public ContextGeneric
{
protected:
    bool m_eventsBound = false;

protected:
    bool IsComment(int pos) const;
    bool IsCommentBlock(int pos) const;

protected:
    void OnCommentSelection(wxCommandEvent& event);
    void OnCommentLine(wxCommandEvent& event);
    void AutoIndent(const wxChar& ch) override;

public:
    ContextPython();
    explicit ContextPython(clEditor* container);
    ~ContextPython() override;
    void ApplySettings() override;
    std::shared_ptr<ContextBase> NewInstance(clEditor* container) override;
    bool IsAtBlockComment() const override;
    bool IsAtLineComment() const override;
};

#endif // CONTEXTPYTHON_HPP
