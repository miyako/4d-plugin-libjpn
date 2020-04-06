4d-plugin-libjpn
================

Collection of useful Unicode/Japanese functions based on the iconv, ICU and MeCab libraries.

![obsolete-word-black-frame-word-obsolete-word-black-frame-d-rendering-123942590](https://user-images.githubusercontent.com/1725068/78463940-29122280-771e-11ea-8be8-a7830725403e.jpg)

Use [d-plugin-text-convert](https://github.com/miyako/4d-plugin-text-convert) or [4d-plugin-mecab-v2](https://github.com/miyako/4d-plugin-mecab-v2)

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />||<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" />|

### Examples

* iconv

```
$U3042:=Char(0x3042)
$U3044:=Char(0x3044)
$U3046:=Char(0x3046)
$U3048:=Char(0x3048)
$U304A:=Char(0x304A)
$unicode_text_t:=$U3042+$U3044+$U3046+$U3048+$U304A

CONVERT FROM TEXT($unicode_text_t;"utf-8";$inData)

C_BLOB($outData)
$error:=iconv ("utf-8";"utf-32";$inData;$outData)
```

* ICU

```
$src:="株式会社４Ｄジャパン渋谷区道玄坂１ー１０ー２"
$id:="[:^Katakana:];Fullwidth-Halfwidth"
$rule:=""

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

  //http://userguide.icu-project.org/transforms/general/rules
  //You either can specify properties with the POSIX-style syntax, 
  //such as [:LowercaseLetter:], or with the Perl-style syntax, such as \p{LowercaseLetter}.
  //http://userguide.icu-project.org/strings/unicodeset

$id:="[[:Nd:][:Lu:][:Ll:][ー]];Fullwidth-Halfwidth"
$id:="[[:Nd:][:Lu:][:Ll:][\\u30FC]];Fullwidth-Halfwidth"

$src:="ティー"
$id:="[:Katakana:];Katakana-Hiragana"
$rule:=""

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

$src:="それは12345ですか"
$id:=""
$rule:="[:^Number:] > ; # 数字だけを抜き取るぞ"

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

$src:="abcde"
$id:=""
$rule:="a > A; # aをAに変換するだけだ"

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

$src:="トランスフォームデスネ"
$id:="-Latin"
$rule:=""

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

$error:=ICU Convert from text ("あいうえお";"utf-32";$data)
$text:=ICU Get error text ($error)

$error:=ICU Convert to text ($data;"utf-32";$text)

$error:=ICU Convert from text ("あいうえお";"shift_jis";$data)
$error:=ICU Convert to text ($data;"shift_jis";$text)

$error:=ICU Convert from text ("あいうえお";"iso-2022-jp";$data)
$error:=ICU Convert to text ($data;"iso-2022-jp";$text)

$error:=ICU Convert from text ("あいうえお";"euc-jp";$data)
$error:=ICU Convert to text ($data;"euc-jp";$text)

$error:=ICU Convert from text ("あいうえお";"utf-32le";$data)
$error:=ICU Convert to text ($data;"utf-32le";$text)

$error:=ICU Convert from text ("あいうえお";"utf-32be";$data)
$error:=ICU Convert to text ($data;"utf-32be";$text)

$error:=ICU Convert from text ("あいうえお";"utf-16le";$data)
$error:=ICU Convert to text ($data;"utf-16le";$text)

$error:=ICU Convert from text ("あいうえお";"utf-16be";$data)
$error:=ICU Convert to text ($data;"utf-16be";$text)

$error:=ICU Normalize text ("ががが";ICU Normalize NFD;$normalized)

CONVERT FROM TEXT("日本語";"shift_jis";$data)

C_TEXT($encoding;$language)

$error:=ICU Get best encoding ($data;$encoding;$language;$confidence)

ARRAY TEXT($encodings;0)
ARRAY TEXT($languages;0)
ARRAY LONGINT($confidences;0)

$error:=ICU Get good encodings ($data;$encodings;$languages;$confidences)

$src:="カキクケコ"
$id:="Katakana-Hiragana"
$rule:=""

$error:=ICU Transform text ($id;$rule;ICU Transform Forward;$src;$dst)

$errorText:=ICU Get error text (4)

ARRAY TEXT($encodings;0)
ICU GET ENCODING LIST ($encodings)
```

* JIS
 
```
$TEST1:=JIS Convert from text ("ｱ";JIS7_ESC)
$RESULT:=JIS Convert to text ($TEST1;JIS7_ESC)
ALERT($RESULT)

$TEST2:=JIS Convert from text ("ｱ";JIS7_SO_SI)
$RESULT:=JIS Convert to text ($TEST1;JIS7_SO_SI)
ALERT($RESULT)

$TEST3:=JIS Convert from text ("ｱ";JIS8)
$RESULT:=JIS Convert to text ($TEST1;JIS8)
ALERT($RESULT)

$TEST4:=JIS Convert from text ("漢字ｶﾀｶﾅABC漢字ABCｶﾀｶﾅ";JIS8)
$RESULT:=JIS Convert to text ($TEST4;JIS8)
ALERT($RESULT)
```

* MeCab
 
```
$errStr:=mecab ("「ちょっと待ってくださいよ～～～～！」！";$arr1;$arr2)
$errStr:=mecab ("太郎はこの本を二郎を見た女性に渡した。";$arr1;$arr2;"-l2 -a")
```
