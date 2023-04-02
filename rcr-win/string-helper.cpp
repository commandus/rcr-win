//
// Created by andrei on 14.03.23.
//
#include "string-helper.h"
#include <unicode/unistr.h>
#include "unicode/utypes.h"
#include "unicode/putil.h"
#include "unicode/uiter.h"

std::string toUpperCase(const std::string &value)
{
    std::string r;
    icu::UnicodeString::fromUTF8(value).toUpper().toUTF8String(r);
    return r;
}

std::wstring utf82wstring(const std::string& value)
{
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(icu::StringPiece(value.c_str()));
    // obtain the size of string we need
    std::wstring ws;
    ws.reserve(ustr.length());
    for (int i = 0; i < ustr.length(); ++i)
        ws += static_cast<wchar_t>(ustr[i]);
    return ws;
}
