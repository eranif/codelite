/*
 *   $Id: main.c,v 1.24 2006/05/30 04:37:12 darren Exp $
 *
 *   Copyright (c) 1996-2003, Darren Hiebert
 *
 *   Author: Darren Hiebert <dhiebert@users.sourceforge.net>
 *           http://ctags.sourceforge.net
 *
 *   This source code is released for free distribution under the terms of the
 *   GNU General Public License. It is provided on an as-is basis and no
 *   responsibility is accepted for its failure to perform as expected.
 *
 *   This is a reimplementation of the ctags (1) program. It is an attempt to
 *   provide a fully featured ctags program which is free of the limitations
 *   which most (all?) others are subject to.
 *
 *   This module contains the start-up code and routines to determine the list
 *   of files to parsed for tags.
 */

/*
 *   INCLUDE FILES
 */
#include "clist.h"
#include "general.h" /* must always come first */
#include "libctags.h"
#include <errno.h>
#include <string.h>

/*  To provide timings features if available.
 */
#ifdef HAVE_CLOCK
#ifdef HAVE_TIME_H
#include <time.h>
#endif
#else
#ifdef HAVE_TIMES
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif
#endif
#endif

/*  To provide directory searching for recursion feature.
 */
#ifdef AMIGA
#include <clib/dos_protos.h> /* function prototypes */
#include <dos/dosasl.h>      /* for struct AnchorPath */
#define ANCHOR_BUF_SIZE 512
#define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)
#ifdef __SASC
extern struct DosLibrary* DOSBase;
#include <pragmas/dos_pragmas.h>
#endif
#endif

#ifdef HAVE_DIRENT_H
#ifdef __BORLANDC__
#define boolean BORLAND_boolean
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h> /* required by dirent.h */
#endif
#include <dirent.h> /* to declare opendir() */
#undef boolean
#endif
#ifdef HAVE_DIRECT_H
#include <direct.h> /* to _getcwd() */
#endif
#ifdef HAVE_DOS_H
#include <dos.h> /* to declare FA_DIREC */
#endif
#ifdef HAVE_DIR_H
#include <dir.h> /* to declare findfirst() and findnext */
#endif
#ifdef HAVE_IO_H
#include <io.h> /* to declare _findfirst() */
#endif

#include "debug.h"
#include "keyword.h"
#include "main.h"
#include "options.h"
#include "read.h"
#include "routines.h"
/**
 * Some globals
 */
static list_t* gList = NULL;
static char** gArgv = NULL;
static char** tmp_gArgv = NULL;
static int gArgc = 0;

/*
 *   MACROS
 */
#define plural(value) (((unsigned long)(value) == 1L) ? "" : "s")

/*
 *   DATA DEFINITIONS
 */
static struct {
    long files, lines, bytes;
} Totals = { 0, 0, 0 };

#ifdef AMIGA
#include "ctags.h"
static const char* VERsion = "$VER: " PROGRAM_NAME " " PROGRAM_VERSION " "
#ifdef __SASC
    __AMIGADATE__
#else
    __DATE__
#endif
                             " " AUTHOR_NAME " $";
#endif

/*
 *   FUNCTION PROTOTYPES
 */
static boolean createTagsForEntry(const char* const entryName);

/*
 *   FUNCTION DEFINITIONS
 */

extern void addTotals(const unsigned int files, const long unsigned int lines, const long unsigned int bytes)
{
    Totals.files += files;
    Totals.lines += lines;
    Totals.bytes += bytes;
}

extern boolean isDestinationStdout(void)
{
    boolean toStdout = FALSE;

    if(Option.xref || Option.filter ||
       (Option.tagFileName != NULL && (strcmp(Option.tagFileName, "-") == 0
#if defined(VMS)
                                       || strcmp(Option.tagFileName, "sys$output") == 0
#else
                                       || strcmp(Option.tagFileName, "/dev/stdout") == 0
#endif
                                       )))
        toStdout = TRUE;
    return toStdout;
}

#if defined(HAVE_OPENDIR)
static boolean recurseUsingOpendir(const char* const dirName)
{
    boolean resize = FALSE;
    DIR* const dir = opendir(dirName);
    if(dir == NULL)
        error(WARNING | PERROR, "cannot recurse into directory \"%s\"", dirName);
    else {
        struct dirent* entry;
        while((entry = readdir(dir)) != NULL) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                vString* filePath;
                if(strcmp(dirName, ".") == 0)
                    filePath = vStringNewInit(entry->d_name);
                else
                    filePath = combinePathAndFile(dirName, entry->d_name);
                resize |= createTagsForEntry(vStringValue(filePath));
                vStringDelete(filePath);
            }
        }
        closedir(dir);
    }
    return resize;
}

#elif defined(HAVE_FINDFIRST) || defined(HAVE__FINDFIRST)

static boolean createTagsForWildcardEntry(const char* const pattern, const size_t dirLength,
                                          const char* const entryName)
{
    boolean resize = FALSE;
    /* we must not recurse into the directories "." or ".." */
    if(strcmp(entryName, ".") != 0 && strcmp(entryName, "..") != 0) {
        vString* const filePath = vStringNew();
        vStringNCopyS(filePath, pattern, dirLength);
        vStringCatS(filePath, entryName);
        resize = createTagsForEntry(vStringValue(filePath));
        vStringDelete(filePath);
    }
    return resize;
}

static boolean createTagsForWildcardUsingFindfirst(const char* const pattern)
{
    boolean resize = FALSE;
    const size_t dirLength = baseFilename(pattern) - pattern;
#if defined(HAVE_FINDFIRST)
    struct ffblk fileInfo;
    int result = _findfirst(pattern, &fileInfo, FA_DIREC);
    while(result == 0) {
        const char* const entry = (const char*)fileInfo.ff_name;
        resize |= createTagsForWildcardEntry(pattern, dirLength, entry);
        result = _findnext(&fileInfo);
    }
#elif defined(HAVE__FINDFIRST)
    struct _finddata_t fileInfo;
    intptr_t /*findfirst_t*/ hFile = _findfirst(pattern, &fileInfo);
    if(hFile != -1L) {
        do {
            const char* const entry = (const char*)fileInfo.name;
            resize |= createTagsForWildcardEntry(pattern, dirLength, entry);
        } while(_findnext(hFile, &fileInfo) == 0);
        _findclose(hFile);
    }
#endif
    return resize;
}

#elif defined(AMIGA)

static boolean createTagsForAmigaWildcard(const char* const pattern)
{
    boolean resize = FALSE;
    struct AnchorPath* const anchor = (struct AnchorPath*)eMalloc((size_t)ANCHOR_SIZE);
    LONG result;

    memset(anchor, 0, (size_t)ANCHOR_SIZE);
    anchor->ap_Strlen = ANCHOR_BUF_SIZE;
    /* Allow '.' for current directory */
#ifdef APF_DODOT
    anchor->ap_Flags = APF_DODOT | APF_DOWILD;
#else
    anchor->ap_Flags = APF_DoDot | APF_DoWild;
#endif
    result = MatchFirst((UBYTE*)pattern, anchor);
    while(result == 0) {
        resize |= createTagsForEntry((char*)anchor->ap_Buf);
        result = MatchNext(anchor);
    }
    MatchEnd(anchor);
    eFree(anchor);
    return resize;
}
#endif

static boolean recurseIntoDirectory(const char* const dirName)
{
    boolean resize = FALSE;
    if(isRecursiveLink(dirName))
        verbose("ignoring \"%s\" (recursive link)\n", dirName);
    else if(!Option.recurse)
        verbose("ignoring \"%s\" (directory)\n", dirName);
    else {
        verbose("RECURSING into directory \"%s\"\n", dirName);
#if defined(HAVE_OPENDIR)
        resize = recurseUsingOpendir(dirName);
#elif defined(HAVE_FINDFIRST) || defined(HAVE__FINDFIRST)
        {
            vString* const pattern = vStringNew();
            vStringCopyS(pattern, dirName);
            vStringPut(pattern, OUTPUT_PATH_SEPARATOR);
            vStringCatS(pattern, "*.*");
            resize = createTagsForWildcardUsingFindfirst(vStringValue(pattern));
            vStringDelete(pattern);
        }
#elif defined(AMIGA)
        {
            vString* const pattern = vStringNew();
            if(*dirName != '\0' && strcmp(dirName, ".") != 0) {
                vStringCopyS(pattern, dirName);
                if(dirName[strlen(dirName) - 1] != '/')
                    vStringPut(pattern, '/');
            }
            vStringCatS(pattern, "#?");
            resize = createTagsForAmigaWildcard(vStringValue(pattern));
            vStringDelete(pattern);
        }
#endif
    }
    return resize;
}

static boolean createTagsForEntry(const char* const entryName)
{
    boolean resize = FALSE;
    fileStatus* status = eStat(entryName);

    Assert(entryName != NULL);
    if(isExcludedFile(entryName))
        verbose("excluding \"%s\"\n", entryName);
    else if(status->isSymbolicLink && !Option.followLinks)
        verbose("ignoring \"%s\" (symbolic link)\n", entryName);
    else if(!status->exists)
        error(WARNING | PERROR, "cannot open source file \"%s\"", entryName);
    else if(status->isDirectory)
        resize = recurseIntoDirectory(entryName);
    else if(!status->isNormalFile)
        verbose("ignoring \"%s\" (special file)\n", entryName);
    else
        resize = parseFile(entryName);

    return resize;
}

#ifdef MANUAL_GLOBBING

static boolean createTagsForWildcardArg(const char* const arg)
{
    boolean resize = FALSE;
    vString* const pattern = vStringNewInit(arg);
    char* patternS = vStringValue(pattern);

#if defined(HAVE_FINDFIRST) || defined(HAVE__FINDFIRST)
    /*  We must transform the "." and ".." forms into something that can
     *  be expanded by the findfirst/_findfirst functions.
     */
    if(Option.recurse && (strcmp(patternS, ".") == 0 || strcmp(patternS, "..") == 0)) {
        vStringPut(pattern, OUTPUT_PATH_SEPARATOR);
        vStringCatS(pattern, "*.*");
    }
    resize |= createTagsForWildcardUsingFindfirst(patternS);
#endif
    vStringDelete(pattern);
    return resize;
}

#endif

static boolean createTagsForArgs(cookedArgs* const args)
{
    boolean resize = FALSE;

    /*  Generate tags for each argument on the command line.
     */
    while(!cArgOff(args)) {
        const char* const arg = cArgItem(args);

#ifdef MANUAL_GLOBBING
        resize |= createTagsForWildcardArg(arg);
#else
        resize |= createTagsForEntry(arg);
#endif
        cArgForth(args);
        parseOptions(args);
    }
    return resize;
}

/*  Read from an opened file a list of file names for which to generate tags.
 */
static boolean createTagsFromFileInput(FILE* const fp, const boolean filter)
{
    boolean resize = FALSE;
    if(fp != NULL) {
        cookedArgs* args = cArgNewFromLineFile(fp);
        parseOptions(args);
        while(!cArgOff(args)) {
            resize |= createTagsForEntry(cArgItem(args));
            if(filter) {
                if(Option.filterTerminator != NULL) {
                    fputs(Option.filterTerminator, stdout);
                    /** Eran Ifrah **/
                    fputs("\n", stdout);
                }
                fflush(stdout);
            }
            cArgForth(args);
            parseOptions(args);
        }
        cArgDelete(args);
    }
    return resize;
}

/*  Read from a named file a list of file names for which to generate tags.
 */
static boolean createTagsFromListFile(const char* const fileName)
{
    boolean resize;
    Assert(fileName != NULL);
    if(strcmp(fileName, "-") == 0)
        resize = createTagsFromFileInput(stdin, FALSE);
    else {
        FILE* const fp = fopen(fileName, "r");
        if(fp == NULL)
            error(FATAL | PERROR, "cannot open list file \"%s\"", fileName);
        resize = createTagsFromFileInput(fp, FALSE);
        fclose(fp);
    }
    return resize;
}

#if defined(HAVE_CLOCK)
#define CLOCK_AVAILABLE
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000000
#endif
#elif defined(HAVE_TIMES)
#define CLOCK_AVAILABLE
#define CLOCKS_PER_SEC 60
static clock_t clock(void)
{
    struct tms buf;

    times(&buf);
    return (buf.tms_utime + buf.tms_stime);
}
#else
#define clock() (clock_t)0
#endif

static void printTotals(const clock_t* const timeStamps)
{
    const unsigned long totalTags = TagFile.numTags.added + TagFile.numTags.prev;

    fprintf(errout, "%ld file%s, %ld line%s (%ld kB) scanned", Totals.files, plural(Totals.files), Totals.lines,
            plural(Totals.lines), Totals.bytes / 1024L);
#ifdef CLOCK_AVAILABLE
    {
        const double interval = ((double)(timeStamps[1] - timeStamps[0])) / CLOCKS_PER_SEC;

        fprintf(errout, " in %.01f seconds", interval);
        if(interval != (double)0.0)
            fprintf(errout, " (%lu kB/s)", (unsigned long)(Totals.bytes / interval) / 1024L);
    }
#endif
    fputc('\n', errout);

    fprintf(errout, "%lu tag%s added to tag file", TagFile.numTags.added, plural(TagFile.numTags.added));
    if(Option.append)
        fprintf(errout, " (now %lu tags)", totalTags);
    fputc('\n', errout);

    if(totalTags > 0 && Option.sorted != SO_UNSORTED) {
        fprintf(errout, "%lu tag%s sorted", totalTags, plural(totalTags));
#ifdef CLOCK_AVAILABLE
        fprintf(errout, " in %.02f seconds", ((double)(timeStamps[2] - timeStamps[1])) / CLOCKS_PER_SEC);
#endif
        fputc('\n', errout);
    }

#ifdef DEBUG
    fprintf(errout, "longest tag line = %lu\n", (unsigned long)TagFile.max.line);
#endif
}

static boolean etagsInclude(void) { return (boolean)(Option.etags && Option.etagsInclude != NULL); }

static void makeTags(cookedArgs* args)
{
    clock_t timeStamps[3];
    boolean resize = FALSE;
    boolean files = (boolean)(!cArgOff(args) || Option.fileList != NULL || Option.filter);

    if(!files) {
        if(filesRequired())
            error(FATAL, "No files specified. Try \"%s --help\".", getExecutableName());
        else if(!Option.recurse && !etagsInclude())
            return;
    }

#define timeStamp(n) timeStamps[(n)] = (Option.printTotals ? clock() : (clock_t)0)
    if(!Option.filter)
        openTagFile();

    timeStamp(0);

    if(!cArgOff(args)) {
        verbose("Reading command line arguments\n");
        resize = createTagsForArgs(args);
    }
    if(Option.fileList != NULL) {
        verbose("Reading list file\n");
        resize = (boolean)(createTagsFromListFile(Option.fileList) || resize);
    }
    if(Option.filter) {
        verbose("Reading filter input\n");
        resize = (boolean)(createTagsFromFileInput(stdin, TRUE) || resize);
    }
    if(!files && Option.recurse)
        resize = recurseIntoDirectory(".");

    timeStamp(1);

    if(!Option.filter)
        closeTagFile(resize);

    timeStamp(2);

    if(Option.printTotals)
        printTotals(timeStamps);
#undef timeStamp
}

/*------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------*/

/** Internal method to this file **/
static void ctags_make_argv(const char* str, const char* filename);
static void ctags_init(const char* options, const char* filename);
static char* load_file(const char* fileName);

char* load_file(const char* fileName)
{
    FILE* fp;
    long len;
    char* buf = NULL;
    long bytes;

    fp = fopen(fileName, "rb");
    if(!fp) {
        return NULL;
    }

    // read the whole file
    fseek(fp, 0, SEEK_END);       // go to end
    len = ftell(fp);              // get position at end (length)
    fseek(fp, 0, SEEK_SET);       // go to begining
    buf = (char*)malloc(len + 1); // malloc buffer

    // read into buffer
    bytes = fread(buf, sizeof(char), len, fp);
    if(bytes != len) {
        fclose(fp);
        free(buf);
        printf("failed to read from file 'test.h': %s\n", strerror(errno));
        return NULL;
    }

    buf[len] = 0; // make it null terminated string
    fclose(fp);
    return buf;
}

/** Create tags from argv **/
extern char* ctags_make_tags(const char* cmd, const char* infile)
{
    char* tags = NULL;
    char* file_name = NULL;

    ctags_init(cmd, infile);

    cookedArgs* args = cArgNewFromArgv(gArgv);

    previewFirstOption(args);
    parseOptions(args);
    checkOptions();

    makeTags(args);
    cArgDelete(args);

    if(Option.ignore) {
        stringListClear(Option.ignore);
    }

    /* open the tags file, read it and convert it into char* */
    file_name = (char*)malloc(strlen(TagFile.directory) + 6);
    memset(file_name, 0, strlen(TagFile.directory) + 6);
    strcpy(file_name, TagFile.directory);
    strcat(file_name, "/tags");

    tags = load_file(file_name);
    free(file_name);
    return tags;
}

void ctags_init(const char* options, const char* filename)
{
    /* convert options to **argv */
    ctags_make_argv(options, filename);

#ifdef AMIGA
    /* This program doesn't work when started from the Workbench */
    if(gArgc == 0)
        exit(1);
#endif

#ifdef __EMX__
    _wildcard(&gArgc, &gArgv); /* expand wildcards in argument list */
#endif

#if defined(macintosh) && BUILD_MPW_TOOL == 0
    argc = ccommand(&argv);
#endif

    setCurrentDirectory();
    setExecutableName(*gArgv++);
    checkRegex();

    /*testEtagsInvocation ();*/
    initializeParsing();
    initOptions();
    /*readOptionConfiguration ();*/
    verbose("Reading initial options from command line\n");
}

extern void ctags_shutdown()
{
    freeKeywordTable();
    freeRoutineResources();
    freeSourceFileResources();
    freeTagFileResources();
    freeOptionResources();
    freeParserResources();
    freeRegexResources();

    gArgc = 0;
    gArgv = NULL;
    gList = NULL;
    tmp_gArgv = NULL;

    list_node_t* node = NULL;
    if(gList) {
        node = gList->head;
        while(node) {
            free(node->data);
            node = node->next;
        }
        list_destroy(gList);
        free(gList);
    }
}

void ctags_make_argv(const char* str, const char* filename)
{
    size_t i = 0;
    list_node_t* node = NULL;
    char* tmp = NULL;

    tmp = strdup(str);

    /** free the list **/
    if(tmp_gArgv) {
        free((char**)tmp_gArgv);
        gArgv = NULL;
        gArgc = 0;
        tmp_gArgv = NULL;
    }

    if(gList) {
        node = gList->head;
        while(node) {
            free(node->data);
            node = node->next;
        }
        list_destroy(gList);
        free(gList);
    }

    list_init(&gList);
    list_append(gList, strdup("ctags"));

    char* token = strtok(tmp, " ");
    while(token) {
        list_append(gList, strdup(token));
        token = strtok(NULL, " ");
    }

    list_append(gList, strdup(filename));
    list_append(gList, NULL);

    gArgv = (char**)malloc(gList->size * sizeof(char*));
    tmp_gArgv = gArgv;
    node = gList->head;

    while(node) {
        gArgv[i] = (char*)node->data;
        node = node->next;
        i++;
    }

    free(tmp);
    gArgc = (int)gList->size - 1;
}

extern void ctags_free(char* ptr)
{
    if(ptr) {
        free(ptr);
    }
}

extern void ctags_batch_parse(const char* filelist, const char* outputfile)
{
    /* ctags command line to use */
    const char* ctags_cmd = getenv("CTAGS_BATCH_CMD");
    if(!ctags_cmd)
        ctags_cmd = "--excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --C++-kinds=+p ";

    /* Open the filelist and load list of files */
    char* line = (char*)0;
    FILE* fp = NULL;
    list_t* l = NULL;
    int counter = 0;

    char* file_content = load_file(filelist);

    if(!file_content)
        return;

    /* Open output file */
    fp = fopen(outputfile, "w+a");
    if(!fp)
        return;

    /* read the string as line by line */
    line = strtok(file_content, "\r\n");

    list_init(&l);

    while(line) {
        if(strlen(line)) {
            list_append(l, (void*)strdup(line));
        }
        line = strtok((char*)0, "\r\n");
    }

    list_node_t* n = l->head;
    int count = l->size;
    while(n) {
        counter++;
        char* tags = ctags_make_tags(ctags_cmd, (char*)n->data);
        free(n->data);
        if(tags) {
            size_t tags_size = strlen(tags);
            if(tags[tags_size - 1] == '\n') {
                tags[tags_size - 1] = 0;
            }
            fprintf(fp, "%s\n", tags);
            free(tags);
        }
        n = n->next;
    }
    fclose(fp);
    list_destroy(l);
    free(file_content);
    ctags_shutdown();
}

/* vi:set tabstop=4 shiftwidth=4: */
