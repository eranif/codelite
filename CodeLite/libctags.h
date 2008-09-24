#ifndef __LIBCTAGS_H__
#define __LIBCTAGS_H__

typedef char* (*CTAGS_MAKE_TAGS_FUNC)(const char*, const char*);
typedef void  (*CTAGS_SHUDOWN_FUNC)();
typedef void (*CTAGS_FREE_FUNC)(char *);

#endif // __LIBCTAGS_H__
