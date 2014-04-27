#include "WProgram.h"

#include "Config.h"

#include "LedGrid.h"

const uint16_t ledGridToIndexMapping[GRID_WIDTH * GRID_HEIGHT] = {

		// FRONT

		  0,   0,   0,  62,  63,  88,   0,   0,   0,
		  0,   0, 252,  61,  64,  87, 208,   0,   0,
		  0, 230, 251,  60,  65,  86, 207, 186,   0,
		  0, 231, 250,  59,  66,  85, 206, 187,   0,
		229, 232, 249,  58,  67,  84, 205, 188, 185,
		228, 233, 248,  57,  68,  83, 204, 189, 184,
		227, 234, 247,  56,  69,  82, 203, 190, 183,
		226, 235, 246,  55,  70,  81, 202, 191, 182,
		225, 236, 245,  54,  71,  80, 201, 192, 181,
		224, 237, 244,  53,  72,  79, 200, 193, 180,
		223, 238, 243,  52,  73,  78, 199, 194, 179,
		222, 239, 242,  51,  74,  77, 198, 195, 178,
		221, 240, 241,  50,  75,  76, 197, 196, 177,

		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,

		265,   0,   0,   0,   0,   0,   0,   0, 309,
		266,   0,   0,   0,   0,   0,   0,   0, 310,
		267,   0,   0,   0,   0,   0,   0,   0, 311,
		268,   0,   0,   0,   0,   0,   0,   0, 312,
		269, 288, 289, 308,  49, 352, 333, 332, 313,
		270, 287, 290, 307,  48, 351, 334, 331, 314,
		271, 286, 291, 306,  47, 350, 335, 330, 315,
		272, 285, 292, 305,  46, 349, 336, 329, 316,
		273, 284, 293, 304,  45, 348, 337, 328, 317,
		274, 283, 294, 303,   1, 347, 338, 327, 318,
		275, 282, 295, 302,   2, 346, 339, 326, 319,
		276, 281, 296, 301,   3, 345, 340, 325, 320,
		277, 280, 297, 300,   4, 344, 341, 324, 321,
		278, 279, 298, 299,   5, 343, 342, 323, 322,

		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,
		  0,   0,   0,   0,   0,   0,   0,   0,   0,

	     89, 108, 109,   6,  31,  32, 153, 152, 133,
		 90, 107, 110,   7,  30,  33, 154, 151, 134,
		 91, 106, 111,   8,  29,  34, 155, 150, 135,
		 92, 105, 112,   9,  28,  35, 156, 149, 136,
		 93, 104, 113,  10,  27,  36, 157, 148, 137,
		 94, 103, 114,  11,  26,  37, 158, 147, 138,
		 95, 102, 115,  12,  25,  38, 159, 146, 139,
		 96, 101, 116,  13,  24,  39, 160, 145, 140,
		 97, 100, 117,  14,  23,  40, 161, 144, 141,
		  0,  99, 118,  15,  22,  41, 162, 143,   0,
		  0,  98, 119,  16,  21,  42, 163, 142,   0,
		  0,   0, 120,  17,  20,  43, 164,   0,   0,
		  0,   0,   0,  18,  19,  44,   0,   0,   0

		// REAR
};

LedGrid::LedGrid(uint8_t width, uint8_t height, const uint16_t *mapping) :
	width(width),
	height(height),
	mapping(mapping) {
}

uint16_t LedGrid::retrieveLedNumberByPosition(uint8_t x, uint8_t y) {
	uint16_t index = width * y + x;
	return mapping[index];
}