#ifndef DATE_H
#define DATE_H

#include <3ds/types.h>
#include <string>
#include <stddef.h>

typedef enum {
	FORMAT_YDM	= 0,
	FORMAT_YMD	= 1,
	FORMAT_DM	= 2,
	FORMAT_MD	= 3,
	FORMAT_M_D	= 4,
} DateFormat;

/**
 * Get the current date as a C string.
 * @param format Date format.
 * @param buf Output buffer.
 * @param size Size of the output buffer.
 * @return Number of bytes written, excluding the NULL terminator.
 * @return Current date. (Caller must free() this string.)
 */
size_t GetDate(int format, char *buf, size_t size);

/**
 * Get the current time formatted for the top bar.
 * This includes the blinking ':'.
 * @return std::string containing the time.
 */
std::string RetTime(void);

/**
 * Draw the date using the specified color.
 * Date format depends on language setting.
 * @param color Text color.
 */
void DrawDate(u32 color);

#endif // DATE_H
