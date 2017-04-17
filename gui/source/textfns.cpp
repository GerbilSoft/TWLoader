// Text functions.
#include "textfns.h"

#include <cstring>
#include <malloc.h>

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::wstring;

/** UTF-16 **/

/**
 * Convert a UTF-16 string to wchar_t. (internal function)
 * @param out_wstr	[out] UTF-32 buffer.
 * @param in_u16str	[in] UTF-16 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int utf16_to_wchar_internal(wchar_t *out_wstr, const u16 *in_u16str)
{
	if (!in_u16str) {
		// No string.
		*out_wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *in_u16str != 0; in_u16str++, out_wstr++, size++) {
		// Convert the UTF-16 character to UTF-32.
		// Special handling is needed only for surrogate pairs.
		// (TODO: Test surrogate pair handling.)
		if ((*in_u16str & 0xFC00) == 0xD800) {
			// High surrogate. (0xD800-0xDBFF)
			if ((in_u16str[1] & 0xFC00) == 0xDC00) {
				// Low surrogate. (0xDC00-0xDFFF)
				// Recombine to get the actual character.
				wchar_t wchr = 0x10000;
				wchr += ((in_u16str[0] & 0x3FF) << 10);
				wchr +=  (in_u16str[1] & 0x3FF);
				*out_wstr = wchr;
				// Make sure we don't process the low surrogate
				// on the next iteration.
				in_u16str++;
			} else {
				// Unpaired high surrogate.
				*out_wstr = (wchar_t)(0xFFFD);
			}
		} else if ((*in_u16str & 0xFC00) == 0xDC00) {
			// Unpaired low surrogate.
			*out_wstr = (wchar_t)(0xFFFD);
		} else {
			// Standard UTF-16 character.
			*out_wstr = (wchar_t)*in_u16str;
		}
	}

	// Add a NULL terminator.
	*out_wstr = 0;
	return size;
}

/**
 * Convert a UTF-16 string to wstring.
 * @param str UTF-16 string.
 * @return wstring. (UTF-32)
 */
wstring utf16_to_wstring(const u16 *in_u16str)
{
	wstring wstr;
	if (!in_u16str) {
		// No string.
		return wstr;
	}

	// Allocate at least as many UTF-32 units
	// as there are characters in the string,
	// plus one for the NULL terminator.
	int len = 0;
	for (const u16 *p = in_u16str; *p != 0; p++, len++) { }
	wstr.resize(len+1);

	// Convert the string.
	int size = utf16_to_wchar_internal(&wstr[0], in_u16str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a UTF-16 string to wchar_t*.
 * @param in_u16str UTF-16 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_u16str is nullptr, this returns nullptr.)
 */
wchar_t *utf16_to_wchar(const u16 *in_u16str)
{
	if (!in_u16str) {
		// No string.
		return nullptr;
	}

	// Allocate at least as many UTF-32 units
	// as there are characters in the string,
	// plus one for the NULL terminator.
	int len = 0;
	for (const u16 *p = in_u16str; *p != 0; p++, len++) { }
	wchar_t *wstr = (wchar_t*)malloc((len+1) * sizeof(wchar_t));

	// Convert the string.
	utf16_to_wchar_internal(wstr, in_u16str);
	return wstr;
}

/**
 * Convert a UTF-16 string with newlines to a vector of wstrings.
 * @param in_u16str UTF-16 string with newlines.
 * @param len Length of in_u16str.
 * @return vector<wstring>, split on newline boundaries.
 */
vector<wstring> utf16_nl_to_vwstring(const u16 *in_u16str, int len)
{
	// Buffers for the strings.
	// Assuming wchar_t is 32-bit.
	static_assert(sizeof(wchar_t) == 4, "wchar_t is not 32-bit.");
	vector<wstring> vec_wstr;
	vec_wstr.reserve(3);
	wstring wstr;
	wstr.reserve(64);

	for (; *in_u16str != 0 && len > 0; in_u16str++, len--) {
		// Convert the UTF-16 character to UTF-32.
		// Special handling is needed only for surrogate pairs.
		// (TODO: Test surrogate pair handling.)
		if ((*in_u16str & 0xFC00) == 0xD800) {
			// High surrogate. (0xD800-0xDBFF)
			if (len > 2 && (in_u16str[1] & 0xFC00) == 0xDC00) {
				// Low surrogate. (0xDC00-0xDFFF)
				// Recombine to get the actual character.
				wchar_t wchr = 0x10000;
				wchr += ((in_u16str[0] & 0x3FF) << 10);
				wchr +=  (in_u16str[1] & 0x3FF);
				wstr += wchr;
				// Make sure we don't process the low surrogate
				// on the next iteration.
				in_u16str++;
				len--;
			} else {
				// Unpaired high surrogate.
				wstr += (wchar_t)(0xFFFD);
			}
		} else if ((*in_u16str & 0xFC00) == 0xDC00) {
			// Unpaired low surrogate.
			wstr += (wchar_t)(0xFFFD);
		} else {
			// Standard UTF-16 character.
			switch (*in_u16str) {
				case L'\r':
					// Skip carriage returns.
					break;
				case L'\n':
					// Newline.
					vec_wstr.push_back(wstr);
					wstr.clear();
					break;
				default:
					// Add the character.
					wstr += *in_u16str;
					break;
			}
		}
	}

	// Add the last line if it's not empty.
	if (!wstr.empty()) {
		vec_wstr.push_back(wstr);
	}

	return vec_wstr;
}

/**
 * Convert a UTF-16 string with newlines to a vector of strings.
 * @param in_u16str UTF-16 string with newlines.
 * @param len Length of in_str.
 * @return vector<string>, split on newline boundaries.
 */
vector<string> utf16_nl_to_vstring(const u16 *in_u16str, int len)
{
	// Buffers for the strings.
	// Assuming wchar_t is 32-bit.
	static_assert(sizeof(wchar_t) == 4, "wchar_t is not 32-bit.");
	vector<string> vec_u8str;
	vec_u8str.reserve(3);
	string u8str;
	u8str.reserve(64);

	for (; *in_u16str != 0 && len > 0; in_u16str++, len--) {
		// Convert the UTF-16 character to UTF-32.
		// Special handling is needed only for surrogate pairs.
		// (TODO: Test surrogate pair handling.)
		wchar_t wchr;
		bool has_chr = true;
		if ((*in_u16str & 0xFC00) == 0xD800) {
			// High surrogate. (0xD800-0xDBFF)
			if (len > 2 && (in_u16str[1] & 0xFC00) == 0xDC00) {
				// Low surrogate. (0xDC00-0xDFFF)
				// Recombine to get the actual character.
				wchr = 0x10000;
				wchr += ((in_u16str[0] & 0x3FF) << 10);
				wchr +=  (in_u16str[1] & 0x3FF);
				// Make sure we don't process the low surrogate
				// on the next iteration.
				in_u16str++;
				len--;
			} else {
				// Unpaired high surrogate.
				// Use U+FFFD.
				u8str += "\xEF\xBF\xBD";
				has_chr = false;
			}
		} else if ((*in_u16str & 0xFC00) == 0xDC00) {
			// Unpaired low surrogate.
			// Use U+FFFD.
			u8str += "\xEF\xBF\xBD";
			has_chr = false;
		} else {
			// Standard UTF-16 character.
			switch (*in_u16str) {
				case L'\r':
					// Skip carriage returns.
					has_chr = false;
					break;
				case L'\n':
					// Newline.
					vec_u8str.push_back(u8str);
					u8str.clear();
					has_chr = false;
					break;
				default:
					// Add the character.
					wchr = *in_u16str;
					break;
			}
		}

		if (has_chr) {
			// Convert to UTF-8;
			if (wchr <= 0x007F) {
				// Single byte.
				u8str += (char)wchr;
			} else if (wchr <= 0x07FF) {
				// Two bytes.
				u8str += (0xC0 | (wchr >> 6));
				u8str += (0x80 | (wchr & 0x3F));
			} else if (wchr <= 0xFFFF) {
				// Three bytes.
				u8str += (0xE0 | (wchr >> 12));
				u8str += (0x80 | ((wchr >> 6) & 0x3F));
				u8str += (0x80 | (wchr & 0x3F));
			} else if (wchr < 0x10FFFF) {
				// Four bytes.
				u8str += (0xF0 | (wchr >> 18));
				u8str += (0x80 | ((wchr >> 12) & 0x3F));
				u8str += (0x80 | ((wchr >> 6) & 0x3F));
				u8str += (0x80 | (wchr & 0x3F));
			} else {
				// Invalid. (U+FFFD)
				u8str += "\xEF\xBF\xBD";
			}
		}
	}

	// Add the last line if it's not empty.
	if (!u8str.empty()) {
		vec_u8str.push_back(u8str);
	}

	return vec_u8str;
}

/** UTF-8 **/

/**
 * Convert a UTF-8 string to wchar_t. (internal function
 * @param out_wstr	[out] UTF-32 buffer.
 * @param in_u8str	[in] UTF-8 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int utf8_to_wchar_internal(wchar_t *out_wstr, const char *in_u8str)
{
	if (!in_u8str) {
		// No string.
		*out_wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *in_u8str != 0; in_u8str++, out_wstr++, size++) {
		if (!(*in_u8str & 0x80)) {
			// ASCII. (U+0000-U+007F)
			*out_wstr = *in_u8str;
		} else if ((*in_u8str & 0xE0) == 0xC0) {
			// Two-byte UTF-8.
			if ((in_u8str[1] & 0xC0) != 0x80) {
				// Invalid sequence.
				*out_wstr = (wchar_t)0xFFFD;
			} else {
				*out_wstr = (wchar_t)(
					((in_u8str[0] & 0x1F) << 6) |
					((in_u8str[1] & 0x3F)));
				in_u8str++;
			}
		} else if ((*in_u8str & 0xF0) == 0xE0) {
			// Three-byte UTF-8.
			if ((in_u8str[1] & 0xC0) != 0x80 ||
			    (in_u8str[2] & 0xC0) != 0x80)
			{
				// Invalid sequence.
				*out_wstr = (wchar_t)0xFFFD;
			} else {
				*out_wstr = (wchar_t)(
					((in_u8str[0] & 0x0F) << 12) |
					((in_u8str[1] & 0x3F) << 6)  |
					((in_u8str[2] & 0x3F)));
				in_u8str += 2;
			}
		} else if ((*in_u8str & 0xF8) == 0xF0) {
			// Four-byte UTF-8.
			if ((in_u8str[1] & 0xC0) != 0x80 ||
			    (in_u8str[2] & 0xC0) != 0x80 ||
			    (in_u8str[3] & 0xC0) != 0x80)
			{
				// Invalid sequence.
				*out_wstr = (wchar_t)0xFFFD;
			} else {
				*out_wstr = (wchar_t)(
					((in_u8str[0] & 0x07) << 18) |
					((in_u8str[1] & 0x3F) << 12) |
					((in_u8str[2] & 0x3F) << 6)  |
					((in_u8str[3] & 0x3F)));
				in_u8str += 3;
			}
		} else {
			// Invalid UTF-8 sequence.
			*out_wstr = (wchar_t)0xFFFD;
		}
	}

	// Add a NULL terminator.
	*out_wstr = 0;
	return size;
}

/**
 * Convert a UTF-8 string to wstring.
 * @param in_u8str UTF-8 string.
 * @return wstring. (UTF-32)
 */
wstring utf8_to_wstring(const char *in_u8str)
{
	wstring wstr;
	if (!in_u8str) {
		// No string.
		return wstr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wstr.resize(strlen(in_u8str)+1);

	// Convert the string.
	int size = utf8_to_wchar_internal(&wstr[0], in_u8str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a UTF-8 string to wchar_t*.
 * @param in_u8str UTF-8 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_u8str is nullptr, this returns nullptr.)
 */
wchar_t *utf8_to_wchar(const char *in_u8str)
{
	if (!in_u8str) {
		// No string.
		return nullptr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wchar_t *wstr = (wchar_t*)malloc((strlen(in_u8str)+1) * sizeof(wchar_t));

	// Convert the string.
	utf8_to_wchar_internal(wstr, in_u8str);
	return wstr;
}

/** Latin-1 **/

/**
 * Convert a Latin-1 string to wchar_t. (internal function
 * @param out_wstr	[out] UTF-32 buffer.
 * @param in_str	[in] Latin-1 string.
 * @return Number of characters used in wstr, not including the NULL terminator.
 */
static int latin1_to_wchar_internal(wchar_t *out_wstr, const char *in_str)
{
	if (!in_str) {
		// No string.
		*out_wstr = 0;
		return 0;
	}

	int size = 0;
	for (; *in_str != 0; in_str++, out_wstr++, size++) {
		*out_wstr = *in_str;
	}

	// Add a NULL terminator.
	*out_wstr = 0;
	return size;
}

/**
 * Convert a Latin-1 string to wstring.
 * @param in_str Latin-1 string.
 * @return wstring. (UTF-32)
 */
wstring latin1_to_wstring(const char *in_str)
{
	wstring wstr;
	if (!in_str) {
		// No string.
		return wstr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wstr.resize(strlen(in_str)+1);

	// Convert the string.
	int size = latin1_to_wchar_internal(&wstr[0], in_str);

	// Resize the string to trim extra spaces.
	wstr.resize(size);
	return wstr;
}

/**
 * Convert a Latin-1 string to wchar_t*.
 * @param in_str Latin-1 string.
 * @return malloc()'d wchar_t*. (UTF-32) (NOTE: If in_str is nullptr, this returns nullptr.)
 */
wchar_t *latin1_to_wchar(const char *in_str)
{
	if (!in_str) {
		// No string.
		return nullptr;
	}

	// Allocate at least as many UTF-32 units
	// as there are bytes in the string, plus
	// one for the NULL terminator.
	wchar_t *wstr = (wchar_t*)malloc((strlen(in_str)+1) * sizeof(wchar_t));

	// Convert the string.
	latin1_to_wchar_internal(wstr, in_str);
	return wstr;
}
