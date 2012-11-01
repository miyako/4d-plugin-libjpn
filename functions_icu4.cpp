#include "functions_icu4.h"

#include "unicode/ucnv.h"
#include "unicode/utrans.h"
#include "unicode/ucsdet.h"
#include "unicode/unorm2.h"

void ICU_GET_ENCODING_LIST(sLONG_PTR *pResult, PackagePtr pParams)
{
	ARRAY_TEXT encodings;
	
	encodings.setSize(1);
	
	int32_t count = ucnv_countAvailable();
	int32_t i;
	C_TEXT n;
	
	for(i = 0; i < count; i++){
		
		CUTF8String name = CUTF8String((const uint8_t *)ucnv_getAvailableName(i));
		n.setUTF8String(&name);
		encodings.appendUTF16String((const PA_Unichar *)n.getUTF16StringPtr());
		
	}
	
	encodings.toParamAtIndex(pParams, 1);
}

void ICU_Convert_from_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT sourceText;
	C_TEXT encodingName;
	C_BLOB targetData;
	C_LONGINT returnValue;
	
	sourceText.fromParamAtIndex(pParams, 1);
	encodingName.fromParamAtIndex(pParams, 2);
	
	UErrorCode error = U_ZERO_ERROR;
	
	CUTF8String targetEncoding;		
	encodingName.copyUTF8String(&targetEncoding);	
	
	UConverter* converter = ucnv_open((const char *)targetEncoding.c_str(), &error);	
	
	if(!error){
        
		int32_t  sourceLength = sourceText.getUTF16Length();
		int32_t  targetLength = ((sourceLength + 1) * ucnv_getMaxCharSize(converter));
		
		std::vector<uint8_t> buf(targetLength);
        
		char *target = (char *)&buf[0];			
		const UChar *source = (UChar *)sourceText.getUTF16StringPtr(); 
		
		int32_t len = ucnv_fromUChars(converter, target, targetLength, source, sourceLength, &error);
		
		switch (error) {
			case U_ZERO_ERROR:
				targetData.setBytes((const uint8_t *)&buf[0], len);
				break;
			case U_STRING_NOT_TERMINATED_WARNING:
				targetData.setBytes((const uint8_t *)&buf[0], len);
				break;				
			default:
				returnValue.setIntValue(error);	
				break;
		}
		
		ucnv_close(converter);		
		
	}else{
		returnValue.setIntValue(error);	
	}
	
	targetData.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

void ICU_Convert_to_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB sourceData;
	C_TEXT encodingName;
	C_TEXT targetText;
	C_LONGINT returnValue;
	
	sourceData.fromParamAtIndex(pParams, 1);
	encodingName.fromParamAtIndex(pParams, 2);
	
	UErrorCode error = U_ZERO_ERROR;
	
	CUTF8String sourceEncoding;		
	encodingName.copyUTF8String(&sourceEncoding);	
	
	UConverter* converter = ucnv_open((const char *)sourceEncoding.c_str(), &error);
	
	if (!error) {
		
		int32_t sourceLength = sourceData.getBytesLength();
		int32_t targetLength = (sourceLength / ucnv_getMinCharSize(converter)) + 1;
		
		std::vector<UChar> buf(targetLength);
		
		UChar *target = (UChar *)&buf[0];	
		const char *source = (char *)sourceData.getBytesPtr(); 
		
		int32_t len = ucnv_toUChars(converter, target, targetLength, source, sourceLength, &error);
		
		if(!error){	
			targetText.setUTF16String((const PA_Unichar*)&buf[0], len);
		}else{
			returnValue.setIntValue(error);		
		}
        
        ucnv_close(converter);
		
	}else{
		returnValue.setIntValue(error);
	}
	
	targetText.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

// ------------------------------------- Error ------------------------------------


void ICU_Get_error_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_LONGINT errorCode;
	C_TEXT returnValue;
	
	errorCode.fromParamAtIndex(pParams, 1);
    
	CUTF8String errorString = CUTF8String((const uint8_t *)u_errorName((UErrorCode)errorCode.getIntValue()));
	returnValue.setUTF8String(&errorString);
	
	returnValue.setReturn(pResult);
}

// -------------------------------- Transformation --------------------------------


void ICU_Transform_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT tId;
	C_TEXT tRules;
	C_LONGINT tDirection;
	C_TEXT sourceText;
	C_TEXT targetText;	
	C_LONGINT returnValue;
	
	tId.fromParamAtIndex(pParams, 1);
	tRules.fromParamAtIndex(pParams, 2);
	tDirection.fromParamAtIndex(pParams, 3);
	sourceText.fromParamAtIndex(pParams, 4);
	
	UErrorCode error = U_ZERO_ERROR;
	
	const UChar *transId = (UChar *)tId.getUTF16StringPtr();
	int32_t idLength = tId.getUTF16Length();
    
	const UChar *transRules = NULL;
	int32_t rulesLength = tRules.getUTF16Length();
    
	if(rulesLength)
		transRules = (UChar *)tRules.getUTF16StringPtr();
    
	UTransDirection direction = (UTransDirection)tDirection.getIntValue();
	
	UTransliterator *transliterator = utrans_openU(transId,
                                                   idLength,
                                                   direction,
                                                   transRules,
                                                   rulesLength,
                                                   NULL,
                                                   &error);
	
	if (!error) {
        
		int32_t textLength = sourceText.getUTF16Length();
		int32_t textCapacity = textLength + 1;
		int32_t limit = textLength;
		
		std::vector<UChar> text(textCapacity);	
		
		memcpy(&text[0], (const void *)sourceText.getUTF16StringPtr(), (sourceText.getUTF16Length())*sizeof(PA_Unichar)); 
        
		utrans_transUChars(transliterator,
						   (UChar *)&text[0],
						   &textLength,
						   textCapacity,
						   0,
						   &limit,
						   &error
						   );
		
		if(textLength >= textCapacity){
			
			//the original text is lost sometimes?
			memcpy(&text[0], (const void *)sourceText.getUTF16StringPtr(), (sourceText.getUTF16Length())*sizeof(PA_Unichar)); 
			
			textCapacity = textLength + 1;
			text.resize(textCapacity);
			textLength = sourceText.getUTF16Length();
			limit = textLength;
			error = U_ZERO_ERROR;//need to reset error 
			
			utrans_transUChars(transliterator,
							   (UChar *)&text[0],
							   &textLength,
							   textCapacity,
							   0,
							   &limit,
							   &error
							   );			
			
		}
		
		if(!error){	
			targetText.setUTF16String((const PA_Unichar*)&text[0], textLength);
		}else{
			returnValue.setIntValue(error);		
		}
        
		utrans_close(transliterator);
		
	}else{
		returnValue.setIntValue(error);		
	}	
	
	targetText.toParamAtIndex(pParams, 5);
	returnValue.setReturn(pResult);
}

// --------------------------------- Normalization --------------------------------


typedef enum
{
	_NFC = 0,
	_NFD = 1,
	_NFKC = 2,
	_NFKD = 3,
	_NFKC_CF = 4
} NormalizationForm;

void ICU_Normalize_text(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT sourceText;
	C_LONGINT normalizationForm;
	C_TEXT targetText;
	C_LONGINT returnValue;
	
	sourceText.fromParamAtIndex(pParams, 1);
	normalizationForm.fromParamAtIndex(pParams, 2);
	
	UErrorCode error = U_ZERO_ERROR;
	
	const char *name;
	UNormalization2Mode mode;
	
	switch ((NormalizationForm)normalizationForm.getIntValue()) {
		case _NFC:
			name = "nfc";
			mode = UNORM2_COMPOSE;
			break;
		case _NFD:
			name = "nfc";
			mode = UNORM2_DECOMPOSE;
			break;	
		case _NFKC:
			name = "nfkc";
			mode = UNORM2_COMPOSE;
			break;
		case _NFKD:
			name = "nfkc";
			mode = UNORM2_DECOMPOSE;
			break;			
		default:
			name = "nfkc_cf";
			mode = UNORM2_COMPOSE;			
			break;
	}
	
	const UNormalizer2 *normalizer = unorm2_getInstance(NULL, name, mode, &error);
	
	if(!error){
        
		int32_t sourceLength = sourceText.getUTF16Length();
		const UChar *source = (UChar *)sourceText.getUTF16StringPtr(); 
		
		int32_t textCapacity = sourceLength + 1;	
		std::vector<UChar> text(textCapacity);
		
		int32_t size = unorm2_normalize(normalizer,
										source,
										sourceLength,
										(UChar *)&text[0],
										textCapacity,
										&error);
		
		if(size >= textCapacity){
			
			error = U_ZERO_ERROR;
			
			textCapacity = size + 1;			
			text.resize(textCapacity);
			
			size = unorm2_normalize(normalizer,
									source,
									sourceLength,
									(UChar *)&text[0],
									textCapacity,
									&error);
            
		}
		
		if(!error){	
			targetText.setUTF16String((const PA_Unichar*)&text[0], size);
		}else{
			returnValue.setIntValue(error);		
		}		
		
	}else{
		returnValue.setIntValue(error);			
	}
	
	
	
	targetText.toParamAtIndex(pParams, 3);
	returnValue.setReturn(pResult);
}

// ----------------------------------- Detection ----------------------------------


void ICU_Get_best_encoding(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB sourceData;
	C_TEXT name;
	C_TEXT language;
	C_LONGINT confidence;
	C_LONGINT returnValue;
	
	sourceData.fromParamAtIndex(pParams, 1);
	
	UErrorCode error = U_ZERO_ERROR;
	
	UCharsetDetector *detector = ucsdet_open(&error);
	
	if(!error){
		
		ucsdet_setText(detector, (const char *)sourceData.getBytesPtr(),
					   sourceData.getBytesLength(),
					   &error);
		
		if(!error){
			
			const UCharsetMatch *match = ucsdet_detect(detector, &error);	
			
			if(!error){
				
				CUTF8String n = CUTF8String((const uint8_t *)ucsdet_getName(match, &error));
				CUTF8String l = CUTF8String((const uint8_t *)ucsdet_getLanguage(match, &error));
				
				name.setUTF8String(&n);
				language.setUTF8String(&l);
				confidence.setIntValue(ucsdet_getConfidence(match, &error));
				
			}else{
				returnValue.setIntValue(error);			
			}		
		}else{
			returnValue.setIntValue(error);			
		}
		
		ucsdet_close(detector);
		
	}else{
		returnValue.setIntValue(error);		
	}
    
	name.toParamAtIndex(pParams, 2);
	language.toParamAtIndex(pParams, 3);
	confidence.toParamAtIndex(pParams, 4);
	returnValue.setReturn(pResult);
}

void ICU_Get_good_encodings(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_BLOB sourceData;
	ARRAY_TEXT names;
	ARRAY_TEXT languages;
	ARRAY_LONGINT confidences;
	C_LONGINT returnValue;
	C_TEXT name;
	C_TEXT language;
	
	sourceData.fromParamAtIndex(pParams, 1);
	names.setSize(1);
	languages.setSize(1);
	confidences.setSize(1);
	
	UErrorCode error = U_ZERO_ERROR;
	
	UCharsetDetector *detector = ucsdet_open(&error);
	
	if(!error){
		
		ucsdet_setText(detector, (const char *)sourceData.getBytesPtr(),
					   sourceData.getBytesLength(),
					   &error);
		
		if(!error){
			
			int32_t i, count;
			
			const UCharsetMatch **matches = ucsdet_detectAll(detector, &count, &error);	
			
			if(!error){
				
				for(i = 0; i < count; i++){
                    
					CUTF8String n = CUTF8String((const uint8_t *)ucsdet_getName(matches[i], &error));
					CUTF8String l = CUTF8String((const uint8_t *)ucsdet_getLanguage(matches[i], &error));					
					name.setUTF8String(&n);
					language.setUTF8String(&l);	
					names.appendUTF16String((const PA_Unichar *)name.getUTF16StringPtr());
					languages.appendUTF16String((const PA_Unichar *)language.getUTF16StringPtr());					
					confidences.appendIntValue(ucsdet_getConfidence(matches[i], &error));
					
				}
				
			}else{
				returnValue.setIntValue(error);			
			}		
		}else{
			returnValue.setIntValue(error);			
		}
		
		ucsdet_close(detector);
		
	}else{
		returnValue.setIntValue(error);		
	}
	
	names.toParamAtIndex(pParams, 2);
	languages.toParamAtIndex(pParams, 3);
	confidences.toParamAtIndex(pParams, 4);
	returnValue.setReturn(pResult);
}