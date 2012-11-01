#include "4DPluginAPI.h"
#include "4DPlugin.h"

void ICU_GET_ENCODING_LIST(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Get_good_encodings(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Get_best_encoding(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Normalize_text(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Transform_text(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Get_error_text(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Convert_to_text(sLONG_PTR *pResult, PackagePtr pParams);
void ICU_Convert_from_text(sLONG_PTR *pResult, PackagePtr pParams);