#ifndef __LIBCTAGS_H__
#define __LIBCTAGS_H__

/** allow loading the libctags using dlsymbol & dlopen **/
typedef char* (*CTAGS_MAKE_TAGS_FUNC)(const char*, const char*);
typedef void  (*CTAGS_SHUDOWN_FUNC)();
typedef void (*CTAGS_FREE_FUNC)(char *);

/** standard incudes **/
extern char *ctags_make_tags (const char *cmd, const char *infile);
extern void ctags_shutdown();
extern void ctags_free(char *ptr);
#endif // __LIBCTAGS_H__
