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
    icu::UnicodeString ustr;
    ustr.fromUTF8(value);
    // obtain the size of string we need
    int32_t requiredSize;
    UErrorCode error = U_ZERO_ERROR;

    u_strToWCS(nullptr, 0, &requiredSize, ustr.getBuffer(), ustr.length(), &error);

    std::wstring wstr;
    // resize accordingly (this will not include any terminating null character, but it also doesn't need to either)
    wstr.resize(requiredSize);

    // copy the UnicodeString buffer to the std::wstring.
    u_strToWCS(wstr.data(), wstr.size(), nullptr, ustr.getBuffer(), ustr.length(), &error);
    return wstr;
}
