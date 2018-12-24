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
	pRetCore->grainColor = (StructGrainColor *)calloc(BeadNum, sizeof(StructGrainColor));
	pRetCore->pData = (uint8_t *)(pRetCore->grainColor);
	pRetCore->dataLen = sizeof(StructGrainColor) * BeadNum;
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
void WS2812_HueCircle(StructCore *pCore)
{
	StructBeadColor *pStr = pCore->beadColor;
	memcpy(pStr + 0, &CV_Red, sizeof(StructBeadColor));
	memcpy(pStr + 4, &CV_Yellow, sizeof(StructBeadColor));
	memcpy(pStr + 8, &CV_Green, sizeof(StructBeadColor));
	memcpy(pStr + 12, &CV_Cyan, sizeof(StructBeadColor));
	memcpy(pStr + 16, &CV_Blue, sizeof(StructBeadColor));
	memcpy(pStr + 20, &CV_Purple, sizeof(StructBeadColor));
	for (int fi = 0; fi < 6; fi++)
	{
		uint8_t cn0 = fi * 4;
		uint8_t cn100 = (fi * 4 + 4) % 24;
		uint8_t cn25 = cn0 + 1;
		uint8_t cn50 = cn0 + 2;
		uint8_t cn75 = cn0 + 3;
		ColorFusion(pStr + cn0, pStr + cn100, pStr + cn50);
		ColorFusion(pStr + cn0, pStr + cn50, pStr + cn25);
		ColorFusion(pStr + cn50, pStr + cn100, pStr + cn75);
	}
	Bead2Grain(pCore);
	return;
}
void WS2812_FullColor(StructCore *pCore, ColorCode color)
{
	if (color > 0x07)
		return;
	StructBeadColor fullColor = ColorValue[color];
	for (int i = 0; i < (pCore->n); i++)
		//memcpy((pCore->beadColor+i), &fullColor, sizeof(fullColor));
		*((pCore->beadColor)+i) = fullColor;
	Bead2Grain(pCore);
	return;
}

void WS2812_HueSingle(StructCore *pCore, ColorCode color)
{
	if (color > 0x07)
		return;
	StructBeadColor fullColor = ColorValue[color];
	for (int i = 0; i < (pCore->n); i++)
	{

		((pCore->beadColor) + i)->bead_G = (fullColor.bead_G / (pCore->n)) * i;
		((pCore->beadColor) + i)->bead_R = (fullColor.bead_R / (pCore->n)) * i;
		((pCore->beadColor) + i)->bead_B = (fullColor.bead_B / (pCore->n)) * i;
	}
	Bead2Grain(pCore);
	return;
}

//----------------------------middle layer function-------------------------------
/**
 * @brief Set pCore's beadvalue to grainvalue.
 * 
 * @param pCore 
 */
void Bead2Grain(StructCore *pCore)
{
	uint8_t g, r, b;
	memset(pCore->grainColor, 0, sizeof(StructGrainColor) * (pCore->n));
	for (int i = 0; i < pCore->n; i++)
	{
		g = ((pCore->beadColor) + i)->bead_G;
		r = ((pCore->beadColor) + i)->bead_R;
		b = ((pCore->beadColor) + i)->bead_B;
		for (int j = 0; j < 8; j++)
		{
			if ((g >> j) & 0x1)
			{
				(((pCore->grainColor) + i)->grain_G) |= (GRAIN_H << (j * 4));
			}
			else
			{
				(((pCore->grainColor) + i)->grain_G) |= (GRAIN_L << (j * 4));
			}
			if ((r >> j) & 0x1)
			{
				(((pCore->grainColor) + i)->grain_R) |= (GRAIN_H << (j * 4));
			}
			else
			{
				(((pCore->grainColor) + i)->grain_R) |= (GRAIN_L << (j * 4));
			}
			if ((b >> j) & 0x1)
			{
				(((pCore->grainColor) + i)->grain_B) |= (GRAIN_H << (j * 4));
			}
			else
			{
				(((pCore->grainColor) + i)->grain_B) |= (GRAIN_L << (j * 4));
			}
		}
	}
	return;
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
