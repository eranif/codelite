#ifndef CLUNREDO_H
#define CLUNREDO_H

#include "unredobase.h"

class LEditor;


class CLTextCommand : public CLCommand
{
public:
    CLTextCommand(CLC_types type, const wxString& name="") : CLCommand(type, name)
    {}
    virtual ~CLTextCommand() {}

    virtual bool GetIsAppendable() const { // We can append to a text command
        return true;
    }

protected:

};

class CLInsertTextCommand : public CLTextCommand
{
public:
    CLInsertTextCommand(const wxString& name="insertion:") : CLTextCommand(CLC_insert, name) {}
    virtual ~CLInsertTextCommand() {}

protected:

};

class CLDeleteTextCommand : public CLTextCommand
{
public:
    CLDeleteTextCommand(const wxString& name="deletion:") : CLTextCommand(CLC_delete, name) {}
    virtual ~CLDeleteTextCommand() {}

protected:

};

class CLCommandProcessor : public CommandProcessorBase
{
public:
    CLCommandProcessor();
    virtual ~CLCommandProcessor() {}

    void StartNewTextCommand(CLC_types type, const wxString& text = "");

    void AppendToTextCommand(const wxString& text, int position);
    
    virtual void ProcessOpenCommand();

    LEditor* GetParent() const {
        return m_parent;
    }

    void SetParent(LEditor* parent) {
        m_parent = parent;
    }

    void Reset() { // Like Clear() but retain m_initialCommand. Used when an editor is reloaded
       m_commands.clear();
    }

    virtual bool DoUndo();

    virtual bool DoRedo();

protected:
    LEditor* m_parent;

};

#endif // CLUNREDO_H
