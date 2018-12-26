/**
 * @file WS2812.c
 * @author LClancey ()
 * @brief 
 * @version 0.1
 * @date 2018-12-21
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#include "WS2812.h"
#include <string.h>
#include <stddef.h>

/**
 * @brief core data init
 * 
 * Do this before other functions.
 * Each LED String should init a suitble core data struct,
 * which saves bytes and bits for temp send datas depends on bead num.
 *  
 * @param BeadNum 
 * @return StructCore* . Do check this pointer (if NULL)
 * before other operations.  
 */
StructCore *WS2812_Init(uint8_t BeadNum)
{
	/// fail
	if (BeadNum == 0)
		return 0UL;

	/// success
	StructCore *pRetCore = (StructCore *)calloc(1, sizeof(StructCore));
	pRetCore->n = BeadNum;
	pRetCore->beadColor = (StructBeadColor *)calloc(BeadNum, sizeof(StructBeadColor));
	pRetCore->grainColor = (StructGrainColor *)calloc(BeadNum + 1, sizeof(StructGrainColor));
	pRetCore->pData = (uint8_t *)(pRetCore->grainColor);
	pRetCore->dataLen = sizeof(StructGrainColor) * (BeadNum + 1);
	return pRetCore;
}
/**
 * @brief 
 * 
 * @param pCore 
 * @return uint32_t 
 */
void WS2812_DeInit(StructCore *pCore)
{
	if (pCore == 0UL)
		return;
	else
	{
		free(pCore->beadColor);
		free(pCore->grainColor);
		free(pCore);
		pCore = 0UL;
		return;
	}
}

/**
 * @brief This function only work in 24LED String now.
 * Will extend soon.
 * 
 * @param pCore 
 * @return uint32_t 
 */
void WS2812_HueCircle(StructCore *pCore, uint8_t pos)
{
	/// number of all beads
	uint8_t n = pCore->n;

	/// get first bead's position
	StructBeadColor *pStr = pCore->beadColor;

	/// create six original colors
	for (int c1 = 0; c1 < 6; c1++)
	{
		memcpy(pStr + (pos + c1 * 4) % n, ColorValue + CC_Red + c1, sizeof(StructBeadColor));
	}

	/// insert one color in each two original colors ( 6 -> 12 )
	/// and insert again ( 12 - >24 )
	for (int c2 = 0; c2 < 6; c2++)
	{
		/// init original colors' location
		uint8_t cn0 = (pos + c2 * 4) % n;
		uint8_t cn100 = (pos + c2 * 4 + 4) % n;

		/// position new colors' location
		uint8_t cn50 = (cn0 + 2) % n;
		uint8_t cn25 = (cn0 + 1) % n;
		uint8_t cn75 = (cn0 + 3) % n;

		/// fuse original colors' into new colors
		ColorFusion(pStr + cn0, pStr + cn100, pStr + cn50);
		ColorFusion(pStr + cn0, pStr + cn50, pStr + cn25);
		ColorFusion(pStr + cn50, pStr + cn100, pStr + cn75);
	}
	Bead2Grain(pCore);
	return;
}

/**
 * @brief 
 * 
 * @param pCore 
 * @param color 
 */
void WS2812_FullColor(StructCore *pCore, ColorCode color)
{
	color %= CC_MAX;
	uint8_t n = pCore->n;
	StructBeadColor fullColor = ColorValue[color];
	for (int i = 0; i < n; i++)
		*((pCore->beadColor) + i) = fullColor;
	Bead2Grain(pCore);
	return;
}

/**
 * @brief 
 * 
 * @param pCore 
 * @param color 
 */
void WS2812_HueSingle(StructCore *pCore, ColorCode color, uint8_t pos)
{
	color %= CC_MAX;
	uint8_t n = pCore->n;
	StructBeadColor fullColor = ColorValue[color];
	for (int i = 0; i < n; i++)
	{
		((pCore->beadColor) + (i + pos) % n)->bead_G = (uint16_t)(fullColor.bead_G * i) / n;
		((pCore->beadColor) + (i + pos) % n)->bead_R = (uint16_t)(fullColor.bead_R * i) / n;
		((pCore->beadColor) + (i + pos) % n)->bead_B = (uint16_t)(fullColor.bead_B * i) / n;
	}
	Bead2Grain(pCore);
	return;
}

/**
 * @brief 
 * 
 * @param pCore 
 * @param brightness double value,should between 0~1 
 */
void WS2812_ReBright(StructCore *pCore, double brightness)
{
	uint8_t n = pCore->n;
	if (brightness < 0 || brightness > 1)
		return;
	for (int i = 0; i < n; i++)
	{
		((pCore->beadColor) + i)->bead_G *= brightness;
		((pCore->beadColor) + i)->bead_R *= brightness;
		((pCore->beadColor) + i)->bead_B *= brightness;
	}
	Bead2Grain(pCore);
	return;
}

/**
 * @brief 
 * 
 * @param pCore 
 * @param firstBead 
 * @param gap 
 */
void WS2812_StaticGap(StructCore *pCore, StructBeadColor *firstBead, uint8_t gap)
{
	uint8_t g_plus = (firstBead->bead_G) ? gap : 0;
	uint8_t r_plus = (firstBead->bead_R) ? gap : 0;
	uint8_t b_plus = (firstBead->bead_B) ? gap : 0;

	/// create the rest beads
	for (int i = 0; i < (pCore->n); i++)
	{
		(pCore->beadColor + i)->bead_G = ((firstBead->bead_G) + g_plus * i);
		(pCore->beadColor + i)->bead_R = ((firstBead->bead_R) + r_plus * i);
		(pCore->beadColor + i)->bead_B = ((firstBead->bead_B) + b_plus * i);
	}

	///
	Bead2Grain(pCore);
}
//----------------------------middle layer function-------------------------------
/**
 * @brief Set pCore's beadvalue to grainvalue.
 * 
 * @param pCore 
 */
void Bead2Grain(StructCore *pCore)
{
	//memset(pCore->grainColor, 0, sizeof(StructGrainColor) * (pCore->n));
	for (int i = 0; i < pCore->n; i++)
		Bead2GrainOfOne((pCore->beadColor) + i, (pCore->grainColor) + i);
	return;
}

void Bead2GrainOfOne(StructBeadColor *pB, StructGrainColor *pG)
{
	uint8_t g = pB->bead_G, r = pB->bead_R, b = pB->bead_B;
	memset(pG, 0, sizeof(StructGrainColor));
	for (int i = 0; i < 8; i++)
	{
		if ((g >> i) & 0x1)
		{
			(pG->grain_G) |= (GRAIN_H << ((7 - i) * 4));
		}
		else
		{
			(pG->grain_G) |= (GRAIN_L << ((7 - i) * 4));
		}
		if ((r >> i) & 0x1)
		{
			(pG->grain_R) |= (GRAIN_H << ((7 - i) * 4));
		}
		else
		{
			(pG->grain_R) |= (GRAIN_L << ((7 - i) * 4));
		}
		if ((b >> i) & 0x1)
		{
			(pG->grain_B) |= (GRAIN_H << ((7 - i) * 4));
		}
		else
		{
			(pG->grain_B) |= (GRAIN_L << ((7 - i) * 4));
		}
	}
}
/**
 * @brief Mix two colors into one.
 * 
 * @param color1 ,one color to be mixed
 * @param color2 , the other color to be mixed
 * @param colorMix ,mixed color of above
 */
void ColorFusion(
	StructBeadColor *color1,
	StructBeadColor *color2,
	StructBeadColor *colorMix)
{
	colorMix->bead_G = ((uint16_t)(color1->bead_G) + (uint16_t)(color2->bead_G)) / 2;
	colorMix->bead_R = ((uint16_t)(color1->bead_R) + (uint16_t)(color2->bead_R)) / 2;
	colorMix->bead_B = ((uint16_t)(color1->bead_B) + (uint16_t)(color2->bead_B)) / 2;
}
