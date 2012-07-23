#include "functions_icnv.h"

#include <errno.h>
#include "iconv.h"

void _iconv(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT Param2;
	C_BLOB Param3;
	C_BLOB Param4;
	C_LONGINT returnValue;
    
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	Param3.fromParamAtIndex(pParams, 3);
    
	CUTF8String _io;
	CUTF8String _eo;	
	
	Param1.copyUTF8String(&_io);
	Param2.copyUTF8String(&_eo);
	
	const char *inEncoding = (const char *)_io.c_str();
	const char *outEncoding = (const char *)_eo.c_str();
    
	iconv_t conv_desc = iconv_open (outEncoding, inEncoding);
	
	if ((size_t)conv_desc == (size_t) -1){
		returnValue.setIntValue(errno);	
	}else{
        
		const char *inData = (const char *)Param3.getBytesPtr();		
		size_t inDataLen = (size_t)Param3.getBytesLength();
		size_t outDataLen = (size_t)(inDataLen * 4) + 4;	
		size_t outDataSize = outDataLen;
		
		char *outData = (char *)calloc(outDataLen, 1);
		char *outDataPtr = outData;
		
		size_t iconv_value;
		
		iconv_value = iconv (conv_desc, &inData, &inDataLen, &outData, &outDataLen);
		
		if (iconv_value){
			returnValue.setIntValue(errno);
		}else{
			Param4.setBytes((const uint8_t *)outDataPtr, outDataSize-outDataLen);
		}
		free(outDataPtr);		
		iconv_close (conv_desc);
	}
	
	Param4.toParamAtIndex(pParams, 4);
	returnValue.setReturn(pResult);
}