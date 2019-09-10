#ifndef CSCOMMANDHANDLERBASE_H
#define CSCOMMANDHANDLERBASE_H

#include "file_logger.h"
#include "JSON.h"
#include <cl_command_event.h>
#include <wx/event.h>
#include <wx/sharedptr.h>
#include <wx/string.h>

class csManager;
wxDECLARE_EVENT(wxEVT_COMMAND_PROCESSED, clCommandEvent);

#define CHECK_STR_PARAM(str_option, sVal)                       \
    if(!options.hasNamedObject(str_option)) {                   \
        clERROR() << "Command is missing field:" << str_option; \
        NotifyCompletion();                                     \
        return;                                                 \
    }                                                           \
    sVal = options.namedObject(str_option).toString();

#define CHECK_INT_PARAM(str_option, iVal)                       \
    if(!options.hasNamedObject(str_option)) {                   \
        clERROR() << "Command is missing field:" << str_option; \
        NotifyCompletion();                                     \
        return;                                                 \
    }                                                           \
    iVal = options.namedObject(str_option).toInt();

#define CHECK_BOOL_PARAM(str_option, bVal)                      \
    if(!options.hasNamedObject(str_option)) {                   \
        clERROR() << "Command is missing field:" << str_option; \
        NotifyCompletion();                                     \
        return;                                                 \
    }                                                           \
    bVal = options.namedObject(str_option).toBool();

#define CHECK_ARRSTR_PARAM(str_option, arrVal)                  \
    if(!options.hasNamedObject(str_option)) {                   \
        clERROR() << "Command is missing field:" << str_option; \
        NotifyCompletion();                                     \
        return;                                                 \
    }                                                           \
    arrVal = options.namedObject(str_option).toArrayString();

#define CHECK_STR_PARAM_OPTIONAL(str_option, sVal) \
    if(options.hasNamedObject(str_option)) { sVal = options.namedObject(str_option).toString(); }

#define CHECK_INT_PARAM_OPTIONAL(str_option, iVal) \
    if(options.hasNamedObject(str_option)) { iVal = options.namedObject(str_option).toInt(); }

#define CHECK_BOOL_PARAM_OPTIONAL(str_option, bVal) \
    if(options.hasNamedObject(str_option)) { bVal = options.namedObject(str_option).toBool(); }

#define CHECK_ARRSTR_PARAM_OPTIONAL(str_option, arrVal) \
    if(options.hasNamedObject(str_option)) { arrVal = options.namedObject(str_option).toArrayString(); }

class csCommandHandlerBase
{
protected:
    csManager* m_manager;
    bool m_notifyOnExit;

public:
    typedef wxSharedPtr<csCommandHandlerBase> Ptr_t;

protected:
    void NotifyCompletion();
    void SetNotifyCompletion(bool b) { m_notifyOnExit = b; }

public:
    /**
     * @brief process a request from the command line and print the result to the stdout
     * @param the handler options
     */
    virtual void DoProcessCommand(const JSONItem& options) = 0;

public:
    csCommandHandlerBase(csManager* manager);
    virtual ~csCommandHandlerBase();

    csManager* GetSink() { return m_manager; }

    /**
     * @brief process a request from the command line and print the result to the stdout
     * @param the handler options
     */
    void Process(const JSONItem& options);
};

#endif // CSCOMMANDHANDLERBASE_H
