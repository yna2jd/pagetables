#define _XOPEN_SOURCE 700

/** @brief LEVELS = number of PTEs used to translate one address.
 *  @details Max: LEVELS at 44, POBITS at 4.
 *  Min: LEVELS at 1, POBITS at 25.*/
#define LEVELS  4

/** @brief POBITS = number of bits used for the page offset.
 *  @details Max: POBITS at 25, LEVELS at 1.
 *  Min: POBITS at 4, LEVELS at 44.*/
#define POBITS  12

#define DEBUG 0