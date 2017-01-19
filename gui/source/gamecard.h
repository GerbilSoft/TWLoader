// Game card functions.
#ifndef TWLOADER_GAMECARD_H
#define TWLOADER_GAMECARD_H

#include <sf2d.h>

#include <vector>
#include <string>

/**
 * Poll for a game card change.
 * @param force If true, force a re-cache. (Use if polling hasn't been done in a while.)
 * @return True if the card has changed; false if not.
 */
bool gamecardPoll(bool force);

/**
 * Clear the cached icon and text.
 */
void gamecardClearCache(void);

enum GameCardType {
	CARD_TYPE_UNKNOWN	= 0,	// Unknown type.
	CARD_TYPE_NTR		= 1,	// DS
	CARD_TYPE_TWL_ENH	= 2,	// DSi-enhanced
	CARD_TYPE_TWL_ONLY	= 3,	// DSi only
	CARD_TYPE_CTR		= 4,	// 3DS
};

/**
 * Get the game card's type.
 * @return Game card type.
 */
GameCardType gamecardGetType(void);

/**
 * Get the game card's game ID.
 * @return Game ID, or NULL if not a TWL card.
 */
const char *gamecardGetGameID(void);

/**
 * Get the game card's product code.
 * @return Product code, or NULL if not a TWL card.
 */
const char *gamecardGetProductCode(void);

/**
 * Get the game card's revision..
 * @return Game card revision. (0xFF if unknown.)
 */
u8 gamecardGetRevision(void);

/**
 * Get the game card's icon.
 * @return Game card icon, or NULL if not a TWL card.
 */
sf2d_texture *gamecardGetIcon(void);

/**
 * Get the game card's banner text.
 * @return Game card banner text, or empty vector if not a TWL card.
 */
std::vector<std::wstring> gamecardGetText(void);

/**
 * Get the SHA1 HMAC of the icon/banner area.
 * This is present in DSi and later DS ROMs.
 * @return Pointer to the SHA1 HMAC (20 bytes), or nullptr if not present.
 */
const u8 *gamecardGetTWLBannerHMAC(void);

#endif /* TWLOADER_GAMECARD_H */
