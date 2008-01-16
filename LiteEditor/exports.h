#ifndef EXPORTS_H
#define EXPORTS_H

#ifdef WXDLLIMPEXP_LE
#undef WXDLLIMPEXP_LE
#endif 

#ifdef WXMAKINGDLL_LE
#    define WXDLLIMPEXP_LE WXEXPORT
#elif defined(WXUSINGDLL_LE)
#    define WXDLLIMPEXP_LE WXIMPORT
#else 
#    define WXDLLIMPEXP_LE
#endif // WXDLLIMPEXP_LE


#endif //EXPORTS_H

