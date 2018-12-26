/**
 * @file WS2812.h
 * @author LClancey ()
 * @brief 
 * @version 0.1
 * @date 2018-12-21
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef __WS2812_H__
#define __WS2812_H__
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
//-------------------------------lib struct define ------------------------------
typedef struct
{
	uint8_t bead_G;
	uint8_t bead_R;
	uint8_t bead_B;
} StructBeadColor;

typedef struct
{
	uint32_t grain_G;
	uint32_t grain_R;
	uint32_t grain_B;
} StructGrainColor;

typedef struct
{
	bool isInitialed;
	uint8_t n;
	StructBeadColor *beadColor;
	StructGrainColor *grainColor;
	uint8_t *pData;
	uint32_t dataLen;
} StructCore;
///---------------------------colors pre-define------------------------------
typedef enum
{
	CC_Black,
	CC_Red,
	CC_Yellow,
	CC_Green,
	CC_Cyan,
	CC_Blue,
	CC_Purple,
	CC_White
} ColorCode;
static StructBeadColor ColorValue[] = {
	{0x00, 0x00, 0x00},
	{0x00, 0xFF, 0x00},
	{0xFF, 0xFF, 0x00},
	{0xFF, 0x00, 0x00},
	{0xFF, 0x00, 0xFF},
	{0x00, 0x00, 0xFF},
	{0x00, 0xFF, 0xFF},
	{0xFF, 0xFF, 0xFF}
};
#define CC_MAX 8

///----------------------------lib pre defines-------------------------------
#define GRAIN_H ((uint32_t)0x7)
#define GRAIN_L ((uint32_t)0x4)
//#define BEAD_LEN (3)
//#define GRAIN_LEN (12)

//----------------------------lib function-------------------------------
StructCore *WS2812_Init(uint8_t BeadNum);
void WS2812_DeInit(StructCore *pCore);
void WS2812_HueCircle(StructCore *pCore, uint8_t pos);
void WS2812_FullColor(StructCore *pCore, ColorCode color);
void WS2812_HueSingle(StructCore *pCore, ColorCode color, uint8_t pos);
void WS2812_ReBright(StructCore *pCore, double brightness);
void WS2812_StaticGap(StructCore *pCore, StructBeadColor *firstBead, uint8_t gap);

//----------------------------middle layer function-------------------------------
void Bead2Grain(StructCore *pCore);
void Bead2GrainOfOne(StructBeadColor *pB, StructGrainColor *pG);
void ColorFusion(StructBeadColor *color1, StructBeadColor *color2, StructBeadColor *colorMix);

//---------------------------------------end-------------------------------
#endif
