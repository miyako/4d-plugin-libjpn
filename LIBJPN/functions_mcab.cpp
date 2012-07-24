#include "functions_mcab.h"

#include "mecab.h"

#if VERSIONWIN
#include <iostream>
#else
#include <Cocoa/Cocoa.h>
#endif

void _LIBMECAB InitPlugin_mcab()
{
#if VERSIONWIN	
	// write initialisation code here...
	
	wchar_t	libmecabPath[ _MAX_PATH ] = {0};
	wchar_t	mecabrcPath[ _MAX_PATH ] = {0};
	wchar_t	fDrive[ _MAX_DRIVE ], fDir[ _MAX_DIR ], fName[ _MAX_FNAME ], fExt[ _MAX_EXT ];
	
	HMODULE libmecabModule = GetModuleHandleW(L"libmecab.dll");
	GetModuleFileNameW(libmecabModule, libmecabPath, _MAX_PATH);
	
	_wsplitpath_s( libmecabPath, fDrive, fDir, fName, fExt );
	_wmakepath_s( mecabrcPath, fDrive, fDir, L"mecabrc", NULL );
	
	_wputenv_s(L"MECABRC", mecabrcPath);
#else
	NSBundle *b = [NSBundle bundleWithIdentifier:@"com.4D.4DPlugin.miyako.LIBJPN"];
	if(b){
#if PA_SMALLENDIAN		
		NSString *mecabrcPath = [[[b executablePath]stringByDeletingLastPathComponent]stringByAppendingPathComponent:@"mecabrc"];
#else
		NSString *mecabrcPath = [[[b executablePath]stringByDeletingLastPathComponent]stringByAppendingPathComponent:@"mecabrc-ppc"];
#endif		
		setenv("MECABRC",(const char *)[mecabrcPath UTF8String], 1);
	}
	
#endif
}

void _LIBMECAB _mecab(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	ARRAY_TEXT Param2;
	ARRAY_TEXT Param3;
	C_TEXT Param4;
    C_TEXT returnValue;
	
	Param1.fromParamAtIndex(pParams, 1);
	Param4.fromParamAtIndex(pParams, 4);
	
	CUTF8String input;
	Param1.copyUTF8String(&input);

    CUTF8String options;
	Param4.copyUTF8String(&options);
    
	MeCab::Tagger *tagger = MeCab::createTagger((const char *)options.c_str());
	
    if(tagger){
    
        const MeCab::Node* node = tagger->parseToNode((const char *)input.c_str());
        
        Param2.setSize(1);
        Param3.setSize(1);
        
        C_TEXT str;
        
        while(node){
            
            switch(node->stat)
            {
                case MECAB_BOS_NODE:
                    break;
                case MECAB_EOS_NODE:
                    break;
                default:
                    str.setUTF8String((const uint8_t *)node->surface, node->length);
                    Param2.appendUTF16String(str.getUTF16StringPtr());	
                    str.setUTF8String((const uint8_t *)node->feature, strlen(node->feature));
                    Param3.appendUTF16String(str.getUTF16StringPtr());					
                    break;
            }
            node = node->next;
        }
                
        delete tagger;
    }else{
        CUTF8String errorString = (const uint8_t *)MeCab::getTaggerError();	
        returnValue.setUTF8String(&errorString);
    }
    
    Param2.toParamAtIndex(pParams, 2);
    Param3.toParamAtIndex(pParams, 3);	
	returnValue.setReturn(pResult);    
}
