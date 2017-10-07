#ifndef MSWPRIVATE_H
#define MSWPRIVATE_H

#if defined(_WIN32) && !defined(_WIN64)
#define YY_NEVER_INTERACTIVE 1
#define strdup _strdup
#define fileno(fp) 0
#endif
#endif // MSWPRIVATE_H
