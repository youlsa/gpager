/*
 * board.h
 *
 *  Created on: 2017. 9. 21.
 *      Author: baekjg
 */

#ifndef BOARD_H_
#define BOARD_H_
#ifdef __cplusplus
 extern "C" {
#endif

void _RESET(void);

/*!
* \brief Get the current battery level
*
* \retval value  battery level ( 0: very low, 254: fully charged )
*/
uint8_t BoardGetBatteryLevel(void);

/*!
* Returns a pseudo random seed generated using the MCU Unique ID
*
* \retval seed Generated pseudo random seed
*/
uint32_t BoardGetRandomSeed(void);

/*!
* \brief Gets the board 64 bits unique ID
*
* \param [IN] id Pointer to an array that will contain the Unique ID
*/
void BoardGetUniqueId(uint8_t *id);

#ifdef __cplusplus
}
#endif
#endif /* BOARD_H_ */
