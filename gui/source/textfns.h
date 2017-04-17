// Text functions.
#ifndef TEXTFNS_H
#define TEXTFNS_H

#include <3ds/types.h>
#include <string>
#include <vector>

/**
 * Convert a UTF-16 string to wstring.
 * @param in_u16str UTF-16 string.
 * @return wstring. (UTF-32)
 */
std::wstring utf16_to_wstring(const u16 *in_u16str);

/**
 * Convert a UTF-16 string to wchar_t*.
 * @param in_u16str UTF-16 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_u16str is nullptr, this returns nullptr.)
 */
wchar_t *utf16_to_wchar(const u16 *in_u16str);

/**
 * Convert a UTF-16 string with newlines to a vector of wstrings.
 * @param in_u16str UTF-16 string with newlines. (not necessarily NULL-terminated)
 * @param len Length of in_u16str.
 * @return vector<wstring>, split on newline boundaries.
 */
std::vector<std::wstring> utf16_nl_to_vwstring(const u16 *in_u16str, int len);

/**
 * Convert a UTF-8 string to wstring.
 * @param in_u8str UTF-8 string.
 * @return wstring. (UTF-32)
 */
std::wstring utf8_to_wstring(const char *in_u8str);

/**
 * Convert a UTF-8 string to wchar_t*.
 * @param in_u8str UTF-8 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_u8str is nullptr, this returns nullptr.)
 */
wchar_t *utf8_to_wchar(const char *in_u8str);

/**
 * Convert a Latin-1 string to wstring.
 * @param in_str Latin-1 string.
 * @return wstring. (UTF-32)
 */
std::wstring latin1_to_wstring(const char *in_str);

/**
 * Convert a Latin1- string to wchar_t*.
 * @param in_str Latin-1 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_str is nullptr, this returns nullptr.)
 */
wchar_t *latin1_to_wchar(const char *in_str);

#endif /* TEXTFNS_H */
