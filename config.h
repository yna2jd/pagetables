#define _XOPEN_SOURCE 700

/** @brief LEVELS = number of PTEs used to translate one address.
 *  @details Max: LEVELS at 44, POBITS at 4.
 *  Min: See POBITS max.*/
#define LEVELS  44

/** @brief POBITS = number of bits used for the page offset.
 *  @details Max: POBITS at 25, LEVELS at 1.
 *  Min: See LEVELS max.*/
#define POBITS  4
