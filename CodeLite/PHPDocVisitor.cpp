#include "PHPDocVisitor.h"
#include "PHPEntityBase.h"
#include "PHPDocComment.h"
#include "PHPEntityFunction.h"
#include "PHPEntityVariable.h"
#include <algorithm>

PHPDocVisitor::PHPDocVisitor(PHPSourceFile& sourceFile, const std::vector<phpLexerToken>& comments)
    : m_sourceFile(sourceFile)
{
    for(size_t i = 0; i < comments.size(); ++i) {
        m_comments.insert(std::make_pair(comments.at(i).endLineNumber, comments.at(i)));
    }
}

PHPDocVisitor::~PHPDocVisitor() {}

void PHPDocVisitor::OnEntity(PHPEntityBase::Ptr_t entity)
{
    // Locate a comment for this entity
    entity->SetFilename(m_sourceFile.GetFilename());

    if(!entity->GetDocComment().IsEmpty()) {
        // PHPDoc was already assigned to this entity during the parsing phase
        if(entity->Is(kEntityTypeClass)) {
            // Process @property tags here
            PHPDocComment docComment(m_sourceFile, entity->GetDocComment());
            if(!docComment.GetProperties().empty()) {
                // Got some @properties
                std::for_each(docComment.GetProperties().begin(), docComment.GetProperties().end(),
                    [&](PHPDocComment::Property::Map_t::value_type& p) {
                        PHPEntityBase::Ptr_t child = entity->FindChild(p.second.name);
                        if(!child) {
                            // No child of this type, create a new property and add it
                            child.Reset(new PHPEntityVariable());
                            child->SetFilename(m_sourceFile.GetFilename());
                            child->SetLine(entity->GetLine());
                            child->Cast<PHPEntityVariable>()->SetTypeHint(p.second.type);
                            child->Cast<PHPEntityVariable>()->SetFlag(kVar_Member); // Member variable
                            child->Cast<PHPEntityVariable>()->SetFlag(kVar_Public); // Public access
                            child->SetShortName(p.second.name);
                            child->SetFullName(p.second.name);
                            entity->AddChild(child);
                        }
                    });
            } else if(!docComment.GetMethods().empty()) {
                std::for_each(docComment.GetMethods().begin(), docComment.GetMethods().end(),
                    [&](PHPEntityBase::Ptr_t method) { entity->AddChild(method); });
            }
        }
    } else {
        // search for the comment placed at the top of the variable
        // this is why we use here -1
        int lineNum = (entity->GetLine() - 1);

        // for debugging purposes
        wxString entityName = entity->GetShortName();
        wxUnusedVar(entityName);

        std::map<int, phpLexerToken>::iterator iter = m_comments.find(lineNum);
        if(iter == m_comments.end()) {
            // try to locate a comment on the same line
            ++lineNum;
            iter = m_comments.find(lineNum);
        }

        if(iter != m_comments.end()) {

            // we got a match
            entity->SetDocComment(iter->second.Text());
            m_comments.erase(iter);

            PHPDocComment docComment(m_sourceFile, entity->GetDocComment());
            if(entity->Is(kEntityTypeFunction) && !docComment.GetReturn().IsEmpty()) {
                entity->Cast<PHPEntityFunction>()->SetReturnValue(docComment.GetReturn());
                if (docComment.IsReturnNullable()) {
                    entity->Cast<PHPEntityFunction>()->SetFlag(kFunc_ReturnNullable);
                }
            } else if(entity->Is(kEntityTypeVariable) && !entity->Cast<PHPEntityVariable>()->IsFunctionArg()) {
                // A global variable, const or a member
                entity->Cast<PHPEntityVariable>()->SetTypeHint(docComment.GetVar());
            }

        } else if(entity->Is(kEntityTypeVariable) && entity->Parent() && entity->Parent()->Is(kEntityTypeFunction) &&
            entity->Cast<PHPEntityVariable>()->IsFunctionArg()) {
            // A function argument
            PHPDocComment docComment(m_sourceFile, entity->Parent()->GetDocComment());
            wxString typeHint = docComment.GetParam(entity->GetFullName());
            const wxString& currentTypeHint = entity->Cast<PHPEntityVariable>()->GetTypeHint();
            if(!typeHint.IsEmpty() && currentTypeHint.IsEmpty()) {
                // The typehint of a functin argument should have more value than the one provided
                // in the documentation
                entity->Cast<PHPEntityVariable>()->SetTypeHint(typeHint);
            }
        }
    }
}
