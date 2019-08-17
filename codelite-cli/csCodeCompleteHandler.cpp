#include "csCodeCompleteHandler.h"
#include "csCodeCompletePhpHandler.h"

csCodeCompleteHandler::csCodeCompleteHandler(csManager* manager)
    : csCommandHandlerBase(manager)
{
    m_codeCompleteHandlers.Register("code-complete-php",
                                    csCommandHandlerBase::Ptr_t(new csCodeCompletePhpHandler(manager)));
}

csCodeCompleteHandler::~csCodeCompleteHandler() {}

void csCodeCompleteHandler::DoProcessCommand(const JSONItem& options)
{
    CHECK_STR_PARAM("lang", m_lang);
    
    wxString handlerName;
    handlerName << "code-complete-" << m_lang;
    csCommandHandlerBase::Ptr_t handler = m_codeCompleteHandlers.FindHandler(handlerName);
    if(!handler) {
        clERROR() << "I have no handler for:" << handlerName;
        return;
    }
    handler->DoProcessCommand(options);
}
