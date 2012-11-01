#include "4DPluginAPI.h"
#include "4DPlugin.h"

#ifdef _WIN32
#define _LIBMECAB __stdcall
#else
#define _LIBMECAB
#endif

void _LIBMECAB _mecab(sLONG_PTR *pResult, PackagePtr pParams);
void _LIBMECAB InitPlugin_mcab();