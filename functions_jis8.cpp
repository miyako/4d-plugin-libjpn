#include "functions_jis8.h"

#define JIS7_SO_SI 0
#define JIS7_ESC 1
#define JIS8 2

/*
 there are 3 unofficial ways to encode half width katakana in iso-2022-jp;
 shift out/shift in
 0x0E to start h/w katakana, 0x0F to terminate
 escape sequence
 0x1B 0x28 0x49 to switch on h/w/ katakana
 jis8
 use 8 byte codes (0xA1 - 0xDF) for katakana
 */

#if VERSIONWIN
#include "mlang.h"
#endif

void JIS_TO_ISO(C_BLOB *data, int type)
{
	std::string str;
	size_t pos = 0;
	size_t found = 0;
	size_t i = 0;
	std::string result;
	std::string escape;
	
	switch (type) 
	{
		case JIS7_SO_SI:
			str = std::string((const char *)data->getBytesPtr(), (size_t)data->getBytesLength());
			
			for(pos = str.find("\x0E"); pos != std::string::npos; pos = str.find("\x0E", found))
			{
				result.append(str.substr(found, pos-found));				
				result.append("\x1B\x28\x49");		
				found = pos + 1;
				
				pos = str.find("\x0F", found);
				
				if(pos != std::string::npos)
				{	
					result.append(str.substr(found, pos-found));	
					found = pos + 1;					
				}
			}
			
			result.append(str.substr(found, str.length()-found));
			data->setBytes((const uint8_t *)result.c_str(), result.length());	   
			break;
			
		case JIS8:
			str = std::string((const char *)data->getBytesPtr(), (size_t)data->getBytesLength());
			
			for(pos = str.find("\x1B\x28\x4A"); pos != std::string::npos; pos = str.find("\x1B\x28\x4A", found))
			{
				result.append(str.substr(found, pos-found));				
				result.append("\x1B\x28\x49");
				found = pos + 3;
				
				pos = str.find("\x1B", found);
				
				if(pos != std::string::npos)
				{	
					for(i = found;i < pos; i++){
						unsigned char kana = str.at(i);
						result.append(1, kana - 0x80);
					}
					found = pos;
				}
			}			
			
			result.append(str.substr(found, str.length()-found));
			data->setBytes((const uint8_t *)result.c_str(), result.length());
			break;	
			
		default:
			break;
	}	
}

typedef enum
{
	kG0Undefined	= 0,
	kG0ASCII		= 1,
	kG0JIS			= 2
}eG0Mode;

void ISO_TO_JIS(C_BLOB *data, int type)
{
	std::string str;
	size_t pos = 0;
	size_t found = 0;
	size_t i = 0;
	std::string result;
	
	switch (type) 
	{
		case JIS7_SO_SI:
			str = std::string((const char *)data->getBytesPtr(), (size_t)data->getBytesLength());
			
			for(pos = str.find("\x1B\x28\x49"); pos != std::string::npos; pos = str.find("\x1B\x28\x49", found))
			{
				result.append(str.substr(found, pos-found));				
				result.append("\x0E");		
				found = pos + 3;
				
				pos = str.find("\x1B", found);
				
				if(pos != std::string::npos)
				{	
					result.append(str.substr(found, pos-found));
					result.append("\x0F");
					result.append(str.substr(found+1, 3));
					found = pos + 3;					
				}else
				{
					result.append("\x0F");
				}
			}
			
			result.append(str.substr(found, str.length()-found));
			data->setBytes((const uint8_t *)result.c_str(), result.length());	   
			break;
			
		case JIS8:
			str = std::string((const char *)data->getBytesPtr(), (size_t)data->getBytesLength());
			
			for(pos = str.find("\x1B\x28\x49"); pos != std::string::npos; pos = str.find("\x1B\x28\x49", found))
			{
				result.append(str.substr(found, pos-found));
				result.append("\x1B\x28\x4A");
				found = pos + 3;
				
				pos = str.find("\x1B", found);
				
				if(pos != std::string::npos)
				{	
					for(i = found;i < pos; i++)
					{
						unsigned char kana = str.at(i);
						result.append(1, kana + 0x80);
					}
					found = pos;
				}
				
			}
			
			result.append(str.substr(found, str.length()-found));
			data->setBytes((const uint8_t *)result.c_str(), result.length());
			break;	
			
		default:
			break;
	}	
}

void JIS_Convert_from_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_LONGINT Param2;
	C_BLOB returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	// --- write the code of JIS_Convert_to_BLOB here...
	
#if VERSIONMAC	
	
	NSMutableString *str = [[NSMutableString alloc]initWithCharacters:Param1.getUTF16StringPtr() length:Param1.getUTF16Length()];
	
	if(str)
	{
		NSData *data = [str dataUsingEncoding:NSISO2022JPStringEncoding allowLossyConversion:NO];	
		returnValue.setBytes((const uint8_t *)[data bytes], (uint32_t)[data length]);
		ISO_TO_JIS(&returnValue, Param2.getIntValue());
		returnValue.setReturn(pResult);
		[str release];
	}
	
#else
	
	unsigned int len, mlen, ulen;
	DWORD mode = 0;
	IMultiLanguage2 *mlang = NULL;
	CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, (void **)&mlang);
	
	if(mlang)
	{
		ulen = (unsigned int)Param1.getUTF16Length();		
		
		if(!mlang->ConvertStringFromUnicode(&mode, 50221, (LPWSTR)Param1.getUTF16StringPtr(), (UINT *)&ulen, NULL, (UINT *)&mlen))
		{
			len = mlen+1;
			std::vector<uint8_t> buf(len);
			mlang->ConvertStringFromUnicode(&mode, 50221, (LPWSTR)Param1.getUTF16StringPtr(), (UINT *)&ulen, (CHAR *)&buf[0], (UINT *)&mlen);			
			returnValue.setBytes((const uint8_t *)&buf[0], (uint32_t)mlen);
			ISO_TO_JIS(&returnValue, Param2.getIntValue());
			returnValue.setReturn(pResult);
		}
		mlang->Release();
	}
	
#endif
	
}

void JIS_Convert_to_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB Param1;
	C_LONGINT Param2;
	C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param2.fromParamAtIndex(pParams, 2);
	
	// --- write the code of JIS_Convert_from_BLOB here...
	
	JIS_TO_ISO(&Param1, Param2.getIntValue());
	
#if VERSIONMAC	
	
	NSString *str = [[NSString alloc]initWithBytes:(const void *)Param1.getBytesPtr() length:(unsigned int)Param1.getBytesLength() encoding:NSISO2022JPStringEncoding];		
	
	if(str)
	{
		returnValue.setUTF16String(str);
		[str release];
	}
	
#else
	
	unsigned int len, mlen, ulen;
	DWORD mode = 0;
	IMultiLanguage2 *mlang = NULL;
	CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_INPROC_SERVER, IID_IMultiLanguage2, (void **)&mlang);
	
	if(mlang)
	{
		mlen = Param1.getBytesLength();
		if(!mlang->ConvertStringToUnicode(&mode, 50221, (LPSTR)Param1.getBytesPtr(), (UINT *)&mlen, NULL, (UINT *)&ulen))
		{
			len = ((ulen * sizeof(PA_Unichar)) + sizeof(PA_Unichar));
			std::vector<uint8_t> buf(len);
			mlang->ConvertStringToUnicode(&mode, 50221, (LPSTR)Param1.getBytesPtr(), (UINT *)&mlen, (WCHAR *)&buf[0], (UINT*)&ulen);
			returnValue.setUTF16String((const PA_Unichar *)&buf[0], (uint32_t)ulen);	
		}
		mlang->Release();
	}
#endif
	
	returnValue.setReturn(pResult);
	
}