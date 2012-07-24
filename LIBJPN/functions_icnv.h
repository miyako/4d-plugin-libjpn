#include "4DPluginAPI.h"
#include "4DPlugin.h"

#ifdef _WIN32
#define _LIBICONV __stdcall
#else
#define _LIBICONV
#endif

void _LIBICONV _iconv(sLONG_PTR *pResult, PackagePtr pParams);