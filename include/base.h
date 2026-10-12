#ifndef BASE_H
#define BASE_H

#define log(level, tmp, ...) printf("[%s] - `%s`@%d: " tmp "\n", level, __FILE__, __LINE__, __VA_ARGS__)

#define info(tmp, ...) log("info", tmp, __VA_ARGS__)
#define warn(tmp, ...) log("warn", tmp, __VA_ARGS__)
#define error(tmp, ...) log("error", tmp, __VA_ARGS__)

#define assertr(case, explanation) if (!(case)) { error("%s", explanation); return; }
#define asserte(case, explanation, expr) if (!(case)) { error("%s", explanation); expr; }
#define assertj(case, explanation, postjmp) if (!(case)) { error("%s", explanation); goto postjmp; }
#define assertp(case, explanation, postjmp) if (!(case)) { perror(explanation); goto postjmp; }
#define assertev(case, explanation, retval) if (!(case)) { error("%s", explanation); exit(retval); }
#define assertrv(case, explanation, retval) if (!(case)) { error("%s", explanation); return retval; }

#if __WORDSIZE == 64
    #define fmt64 "%ld"
#else
    #define fmt64 "%lld"
#endif

#endif