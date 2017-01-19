#ifndef NS_X_H
#define NS_X_H

// ns:s functions not available in ctrulib.

#ifdef __cplusplus
extern "C" {
#endif

Result nsxInit(void);

void nsxExit(void);

Result NSSX_SetTWLBannerHMAC(const u8 *sha1_hmac);

#ifdef __cplusplus
}
#endif

#endif /* PTMU_X_H */
