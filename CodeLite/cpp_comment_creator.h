#ifndef CPP_COMMENT_CREATOR_H
#define CPP_COMMENT_CREATOR_H

#include "wx/string.h"
#include "entry.h"
#include "comment_creator.h"

class CppCommentCreator : public CommentCreator
{
	TagEntryPtr m_tag;
public:
	CppCommentCreator(TagEntryPtr tag);
	virtual ~CppCommentCreator();
	virtual wxString CreateComment();

private:
	wxString FunctionComment();
	wxString ClassComment();
};

#endif //CPP_COMMENT_CREATOR_H
