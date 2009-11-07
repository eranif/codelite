#ifndef __LIBCTAGS_H__
#define __LIBCTAGS_H__

#ifdef __cplusplus
extern "C" {
#endif

/** allow loading the libctags using dlsymbol & dlopen **/
typedef char* (*CTAGS_MAKE_TAGS_FUNC)(const char*, const char*);
typedef void  (*CTAGS_SHUDOWN_FUNC)();
typedef void (*CTAGS_FREE_FUNC)(char *);

/** standard incudes **/
char *ctags_make_tags (const char *cmd, const char *infile);
void ctags_shutdown();
void ctags_free(char *ptr);
void ctags_batch_parse(const char* filelist, const char * outputfile);
#ifdef __cplusplus
}
#endif

#endif // __LIBCTAGS_H__
