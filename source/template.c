

#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <stdio.h>
#include <stdlib.h>


// saves typing each time...
typedef unsigned int uint32;
typedef unsigned short uint16;


//IMP 0 IS PRESSED, 1 IS RELEASED
#define BUTTON_A	(1 << 0)  //bit 0 = A
#define BUTTON_B	(1 << 1)  //bit 1 = B
#define BUTTON_L	(1 << 9)  //bit 9 = L
#define BUTTON_R	(1 << 8)  //bit 8 = R
#define DPAD_UP     (1 << 6)  //bit 6 = up
#define DPAD_DOWN	(1 << 7)  //bit 7 = down
#define DPAD_LEFT	(1 << 5)  //bit 5 = left
#define DPAD_RIGHT	(1 << 4)  //bit 4 = right

extern void someFunction(int *x);
extern const unsigned int charData[];
extern const unsigned int charData2[];

enum Notes { note_a = 1750, note_A = 1767, note_b = 1783, note_c = 1797, note_C = 1812, note_d = 1823, note_D = 1837, note_e = 1849, note_f = 1860, note_F = 1871, note_g = 1880, note_G = 1890};

uint16 song[32] = { note_b, note_b, note_F, note_b, note_d, note_b, note_A, note_A, note_A, note_F, note_g, note_F, note_e, note_d, note_C, note_b,               //16 notes
					note_A, note_b, note_b, note_b, note_F, note_b, note_d, note_b, note_A, note_b, note_d, note_g, note_F, note_A, note_b,      0};                   //32 notes

int main(void) {
	
	// required to enable vBlank interrupts (i will explain in the lesson)
	irqInit();
	irqEnable(IRQ_VBLANK);

	// pointer to the memory that controls the display options
	uint16* DISPLAYCONTROL = (uint16*)0x4000000;
	DISPLAYCONTROL[0] = ((1 << 8) | (1 << 9) | (1 << 12)); // turn BG layer 0 on

	// pointer to BG palette info
	uint16* BGPALETTE = (uint16*)0x5000000;
	BGPALETTE[0] = ((0 << 0) | (0 << 5) | (0 << 10));	// RGB, values 0-31: palette 0, colour 0 is the BG colour

	uint16* OBJPALETTE = (uint16*)0x5000200;

	//OBJPALETTE[(7 * 16) + 1] = ((28 << 0) | (28 << 5) | (28 << 10));	// RGB
	//OBJPALETTE[(7 * 16) + 2] = ((24 << 0) | (24 << 5) | (24 << 10));	// RGB
	//OBJPALETTE[(7 * 16) + 3] = ((19 << 0) | (19 << 5) | (19 << 10));	// RGB
	//OBJPALETTE[(7 * 16) + 4] = ((3 << 0) | (3 << 5) | (3 << 10));	// RGB
	//OBJPALETTE[(7 * 16) + 5] = ((0 << 0) | (29 << 5) | (0 << 10));	// RGB
	//OBJPALETTE[(7 * 16) + 6] = ((0 << 0) | (19 << 5) | (0 << 10));	// RGB

	volatile uint16* INPUT = (volatile uint16*)0x4000130;

	//BGPALETTE[(palno * 16) + colno]
	BGPALETTE[(7 * 16) + 1] = ((28 << 0) | (28 << 5) | (28 << 10));	// RGB
	BGPALETTE[(7 * 16) + 2] = ((24 << 0) | (24 << 5) | (24 << 10));	// RGB
	BGPALETTE[(7 * 16) + 3] = ((19 << 0) | (19 << 5) | (19 << 10));	// RGB
	BGPALETTE[(7 * 16) + 4] = ((3 << 0) | (3 << 5) | (3 << 10));	// RGB
	BGPALETTE[(7 * 16) + 5] = ((0 << 0) | (29 << 5) | (0 << 10));	// RGB
	BGPALETTE[(7 * 16) + 6] = ((0 << 0) | (19 << 5) | (0 << 10));	// RGB

	int i;
	time_t t;
	srand((unsigned)time(&t));

	uint32* BGTILES = (uint32*)0x6000000; //bg graphics
	uint32* OBJTILES = (uint32*)0x6010000; //sprites

	BGPALETTE[(0 * 16) + 1] = (31 << 0) | (31 << 5) | (31 << 10);
	BGPALETTE[(0 * 16) + 2] = (23 << 0) | (23 << 5) | (23 << 10);
	BGPALETTE[(0 * 16) + 3] = (15 << 0) | (15 << 5) | (15 << 10);
	BGPALETTE[(0 * 16) + 4] = (31 << 0) | (0 << 5) | (0 << 10);
	BGPALETTE[(0 * 16) + 5] = (0 << 0) | (31 << 5) | (0 << 10);
	BGPALETTE[(0 * 16) + 6] = (31 << 0) | (15 << 5) | (15 << 10);

	BGPALETTE[(1 * 16) + 1] = (20 << 0) | (20 << 5) | (20 << 10);
	BGPALETTE[(1 * 16) + 2] = (17 << 0) | (17 << 5) | (17 << 10);
	BGPALETTE[(1 * 16) + 3] = (9 << 0) | (9 << 5) | (9 << 10);

	BGPALETTE[(2 * 16) + 1] = (31 << 0) | (31 << 5) | (0 << 10);
	BGPALETTE[(2 * 16) + 2] = (31 << 0) | (15 << 5) | (0 << 10);

	BGPALETTE[(3 * 16) + 1] = (0 << 0) | (0 << 5) | (15 << 10);
	BGPALETTE[(3 * 16) + 2] = (31 << 0) | (0 << 5) | (31 << 10);

	for (int i = 0; i <= 2048; i++)
	{
		BGTILES[i] = charData[i];
	}
	
	for (int i = 0; i <= 7; i++)
	{
		BGTILES[(16 * 8) + i] = charData2[(16 * 8) + i];       //le bullet from chardata2
		BGTILES[(17 * 8) + i] = charData2[(17 * 8) + i];       //enemy from chardata2
	}

	for (i = 0; i <= 256; i++)
	{
		OBJPALETTE[i] = BGPALETTE[i];
	}

	for (int i = 0; i <= 2048; i++)
	{
		OBJTILES[i] = BGTILES[i];
	}



	 
	OBJTILES[(12 * 8) + 0] = ((1 << 0) | (1 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	OBJTILES[(12 * 8) + 1] = ((5 << 0) | (1 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	OBJTILES[(12 * 8) + 2] = ((6 << 0) | (6 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	OBJTILES[(12 * 8) + 3] = ((3 << 0) | (3 << 4) | (3 << 8) | (3 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28));
	OBJTILES[(12 * 8) + 4] = ((1 << 0) | (2 << 4) | (4 << 8) | (1 << 12) | (1 << 16) | (2 << 20) | (4 << 24) | (1 << 28));
	OBJTILES[(12 * 8) + 5] = ((1 << 0) | (2 << 4) | (4 << 8) | (5 << 12) | (1 << 16) | (2 << 20) | (4 << 24) | (5 << 28));
	OBJTILES[(12 * 8) + 6] = ((6 << 0) | (2 << 4) | (4 << 8) | (6 << 12) | (2 << 16) | (2 << 20) | (4 << 24) | (6 << 28));
	OBJTILES[(12 * 8) + 7] = ((6 << 0) | (3 << 4) | (3 << 8) | (6 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28));
	
	//BGTILES[(tileNum * 8) + rowNum]
	BGTILES[(12 * 8) + 0] = ((1 << 0) | (1 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	BGTILES[(12 * 8) + 1] = ((5 << 0) | (1 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	BGTILES[(12 * 8) + 2] = ((6 << 0) | (6 << 4) | (2 << 8) | (4 << 12) | (1 << 16) | (1 << 20) | (2 << 24) | (4 << 28));
	BGTILES[(12 * 8) + 3] = ((3 << 0) | (3 << 4) | (3 << 8) | (3 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28));
	BGTILES[(12 * 8) + 4] = ((1 << 0) | (2 << 4) | (4 << 8) | (1 << 12) | (1 << 16) | (2 << 20) | (4 << 24) | (1 << 28));
	BGTILES[(12 * 8) + 5] = ((1 << 0) | (2 << 4) | (4 << 8) | (5 << 12) | (1 << 16) | (2 << 20) | (4 << 24) | (5 << 28));
	BGTILES[(12 * 8) + 6] = ((6 << 0) | (2 << 4) | (4 << 8) | (6 << 12) | (2 << 16) | (2 << 20) | (4 << 24) | (6 << 28));
	BGTILES[(12 * 8) + 7] = ((6 << 0) | (3 << 4) | (3 << 8) | (6 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (3 << 28));

	uint16* OAM = (uint16*)0x7000000;
	// OAM[(objno * 4) + attno]

	int y = 63;
	int x = 35;

	OBJTILES[165] = OBJTILES[7];
	OBJTILES[166] = OBJTILES[8];
	OBJTILES[167] = OBJTILES[9];
	OBJTILES[134] = OBJTILES[10];

	int lhs, rhs = 7;

	for (lhs = 165; lhs <= 167; lhs++)
	{
		if (rhs <= 9)
		{		
			for (i = 0; i <= 7; i++)
			{                                                                              //placing the ship elsewhere hence 2d mapping the sprite
				OBJTILES[(lhs * 8) + i] = OBJTILES[(rhs * 8) + i];
			}
		}
		rhs++;
	}

	for (i = 0; i <= 7; i++)
	{
		OBJTILES[(134 * 8) + i] = OBJTILES[(10 * 8) + i];								  //placing the ship elsewhere hence 2d mapping the sprite
	}
	
	OBJTILES[(165 * 8) + 7] = (3 << 0) | (0 << 4) | (0 << 8) | (6 << 12) | (6 << 16) | (6 << 20) | (6 << 24) | (6 << 28);
	OBJTILES[(166 * 8) + 7] = (6 << 0) | (6 << 4) | (6 << 8) | (6 << 12) | (6 << 16) | (6 << 20) | (6 << 24) | (6 << 28);
	OBJTILES[(167 * 8) + 7] = (6 << 0) | (6 << 4) | (6 << 8) | (6 << 12) | (6 << 16) | (6 << 20) | (6 << 24) | (6 << 28);

	OBJTILES[(167 * 8) + 0] = (6 << 0) | (4 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28);
	OBJTILES[(167 * 8) + 1] = (4 << 0) | (6 << 4) | (4 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28);
	OBJTILES[(167 * 8) + 2] = (4 << 0) | (4 << 4) | (6 << 8) | (4 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28);

	OAM[(25 * 4) + 0] = (y << 0) | (1 << 14) ; //y coord, 14 = obj type
	OAM[(25 * 4) + 1] = (x << 0) | (0 << 12) | (0 << 13) | (2 << 14);  //x coord, 12 = h flip, 13 = v flip, 14 = obj size
	OAM[(25 * 4) + 2] = (133 << 0) | (0 << 12); // tile << 0 and pal << 12

	uint16* MAPMEM = (uint16*)0x6004000;
	//tile 12, pal 7
	//(tile << 0) | (pal << 12)

	//someFunction(&location, &place, &finalplace);

	//MAPMEM[finalplace] = (12 << 0) | (7 << 12);
	//MAPMEM[1024 + 101] = (12 << 0) | (7 << 12);

	for (i = 0; i <= 1024; i++)                                                               // for the stars
	{
		int randHFlip = rand() % 2;
		int randVFlip = rand() % 2;
		int randStarTile = rand() % 15;

		if (randStarTile == 0)
		{
			MAPMEM[i] = (6 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);     //10 = hflip, 11 = vflip
		}
		else if(randStarTile == 1)
		{
			MAPMEM[i] = (0 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);
		}
		else if (randStarTile == 2)
		{
			MAPMEM[i] = (4 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);
		}
		else if (randStarTile == 3)
		{
			MAPMEM[i] = (5 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);
		}
		else if (randStarTile == 4)
		{
			MAPMEM[i] = (6 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);
		}
		else
		{
			MAPMEM[i] = (0 << 0) | (randHFlip << 10) | (randVFlip << 11) | (0 << 12);
		}	
	}

	for (i = 1025; i <= 2048; i++)                                                               // for the stars
	{
		int randHFlip = rand() % 2;
		int randVFlip = rand() % 2;
		int randStarTile = rand() % 15;

		if (randStarTile == 0)
		{
			MAPMEM[i] = (6 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);     //10 = hflip, 11 = vflip
		}
		else if (randStarTile == 1)
		{
			MAPMEM[i] = (0 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);
		}
		else if (randStarTile == 2)
		{
			MAPMEM[i] = (4 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);
		}
		else if (randStarTile == 3)
		{
			MAPMEM[i] = (5 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);
		}
		else if (randStarTile == 4)
		{
			MAPMEM[i] = (6 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);
		}
		else
		{
			MAPMEM[i] = (0 << 0) | (randHFlip << 10) | (randVFlip << 11) | (1 << 12);
		}
	}

	MAPMEM[1024 + 116 + 64] = (2 << 0) | (0 << 10) | (0 << 11) | (1 << 12);
	MAPMEM[1024 + 117 + 64] = (3 << 0) | (0 << 10) | (0 << 11) | (1 << 12);
	MAPMEM[1024 + 118 + 64] = (2 << 0) | (1 << 10) | (0 << 11) | (1 << 12);

	MAPMEM[1024 + 116 + 32 + 64] = (3 << 0) | (1 << 10) | (0 << 11) | (1 << 12);
	MAPMEM[1024 + 117 + 32 + 64] = (3 << 0) | (0 << 10) | (1 << 11) | (1 << 12);                                   //le planet
	MAPMEM[1024 + 118 + 32 + 64] = (3 << 0) | (1 << 10) | (1 << 11) | (1 << 12);

	MAPMEM[1024 + 116 + 32 + 32 + 64] = (2 << 0) | (0 << 10) | (1 << 11) | (1 << 12);
	MAPMEM[1024 + 117 + 32 + 32 + 64] = (3 << 0) | (0 << 10) | (0 << 11) | (1 << 12);
	MAPMEM[1024 + 118 + 32 + 32 + 64] = (2 << 0) | (1 << 10) | (1 << 11) | (1 << 12);

	//screen << 8 
	//tileset << 2
	//priority << 0

	uint16* BG0 = (uint16*)0x4000008; //BG0 screen 8 tile 0 priority 1
	BG0[0] = (8 << 8) | (0 << 2) | (0 << 0);

	uint16* BG1 = (uint16*)0x400000A; //BG0 screen 9 tile 0 priority 0 (highest)
	BG1[0] = (9 << 8) | (0 << 2) | (1 << 0);

	uint16* BG0SCROLLX = (uint16*)0x4000010;
	int BG0xScroll = 0;

	uint16* BG1SCROLLX = (uint16*)0x4000014;
	int BG1xScroll = 0;

	uint16* SOUND_MASTER = (uint16*)0x4000084;
	SOUND_MASTER[0] = (1 << 7); //turns on sound and enables access to all sound registers
	
	uint16* SOUND_MIX = (uint16*)0x4000082;
	SOUND_MIX[0] = (2 << 0); //Sound channels = 100%, DMA = 0%

	uint16* SOUND_VOLUMES = (uint16*)0x4000080;
	SOUND_VOLUMES[0] = (7 << 0) | (7 << 4) | (1 << 9) | (1 << 13);
	// right_vol << 0 | left_vol << 4 | sound2_playright << 9 | sound2_playleft << 13

	//uint16* SOUND1_SWEEP = (uint16*)0x4000060;   //TONE & SWEEP
	//SOUND1_SWEEP[0] = (5 << 0) | (0 << 3) | (0 << 4);

	//uint16* SOUND1_DUTYANDENV = (uint16*)0x4000062;
	//SOUND1_DUTYANDENV[0] = (0 << 0) | (2 << 6) | (0 << 8) | (15 << 12);

	//uint16* SOUND1_FREQ = (uint16*)0x4000064;
	//SOUND1_FREQ[0] = (856 << 0) | (1 << 14) | (1 << 15);

	uint16* SOUND2_SETTINGS = (uint16*)0x4000068;   //TONE
	// length << 0 | duty << 6 | env_speed << 8 | start_vol << 12
	SOUND2_SETTINGS[0] = (0 << 0) | (2 << 6) | (0 << 8) | (7 << 12);
	// length 0 = 0.25s, duty = 2 (normal), env off, vol 7

	uint16* SOUND2_FREQ = (uint16*)0x400006C;
	// freq << 0 | use_length << 14 | reset << 15 
	//SOUND2_FREQ[0] = (1750 << 0) | (1 << 14) | (1 << 15);
	// freq here = 2048 - (2 ^ 17 / freq val)

	int bulletx = 250, bullety = 250;

	int bulletno = 26;

	int frames = 0;

	int bulletpal = 2;

	int enemy1X = 240;
	int enemy1Y = 70;

	int enemy2X = 275;
	int enemy2Y = 55;

	int enemy1Tile = 17;
	int enemy2Tile = 17;

	int enemy1pal = 2;
	int enemy2pal = 3;

	uint16 songspeed = 12;
	uint16 curNote = 0;
	uint16 curFrame = 0;
	// GBA docs are here:	https://mgba-emu.github.io/gbatek/

	// game goes here

	//if not much code in while loop, wait a few minutes before clicking, it will run each code 2-3 timnes each frame
	while (1)
	{
		uint16 buttonsnotpressed = *INPUT;

		uint16 buttonspressed = ~(buttonsnotpressed);

		if (curFrame == 0)
		{
			uint16 theNote = song[curNote];
			if (theNote > 0)
			{
				SOUND2_FREQ[0] = (theNote << 0) | (1 << 14) | (1 << 15);
			}
			curNote++;
			curNote %= 32;
		}

		curFrame++;
		curFrame %= songspeed;

			frames++;

			BG0SCROLLX[0] = BG0xScroll;
			BG1SCROLLX[0] = BG1xScroll;
			BG0xScroll++;
			if (frames % 3 == 0)
			{
				BG1xScroll++;
			}
			if (BG0xScroll > 255) { BG0xScroll = 0; }
			if (BG1xScroll > 255) { BG1xScroll = 0; }
			
			someFunction(&bulletx);
			OAM[(bulletno * 4) + 0] = ((bullety + 10) << 0) | (0 << 14); //y coord, 14 = obj type
			OAM[(bulletno * 4) + 1] = ((bulletx + 20) << 0) | (0 << 12) | (0 << 13) | (0 << 14);  //x coord, 12 = h flip, 13 = v flip, 14 = obj size
			OAM[(bulletno * 4) + 2] = (16 << 0) | (bulletpal << 12); // tile << 0 and pal << 12

			OAM[(35 * 4) + 0] = (enemy1Y << 0) | (0 << 14); //y coord, 14 = obj type
			OAM[(35 * 4) + 1] = (enemy1X << 0) | (0 << 12) | (0 << 13) | (0 << 14);  //x coord, 12 = h flip, 13 = v flip, 14 = obj size
			OAM[(35 * 4) + 2] = (enemy1Tile << 0) | (enemy1pal << 12); // tile << 0 and pal << 12

			OAM[(36 * 4) + 0] = (enemy2Y << 0) | (0 << 14); //y coord, 14 = obj type
			OAM[(36 * 4) + 1] = (enemy2X << 0) | (0 << 12) | (0 << 13) | (0 << 14);  //x coord, 12 = h flip, 13 = v flip, 14 = obj size
			OAM[(36 * 4) + 2] = (enemy2Tile << 0) | (enemy2pal << 12); // tile << 0 and pal << 12

			if (bulletx >= 250)
			{
				bulletx = 250;
			}

			if ((bullety >= (enemy1Y - 5)) && (bullety <= (enemy1Y + 5)) && (bulletx >= (enemy1X - 20)) && (bulletx <= (enemy1X + 20)) && (bulletpal == enemy1pal))
			{
				enemy1Tile = 0;
			}

			if ((bullety >= (enemy2Y - 5)) && (bullety <= (enemy2Y + 5)) && (bulletx >= (enemy2X - 20)) && (bulletx <= (enemy2X + 20)) && (bulletpal == enemy2pal))
			{
				enemy2Tile = 0;
			}

			if (frames % 5 == 0)
			{
				enemy1X--;
				enemy2X--;
			}

		if ((buttonspressed & BUTTON_A) && (frames % 40 == 0))
		{
			bullety = y;
			bulletx = x;
			bulletno++;
			if (bulletno >= 31) { bulletno = 26; }

			// freq << 0 | use_length << 14 | reset << 15 
			SOUND2_FREQ[0] = (856 << 0) | (1 << 14) | (1 << 15);
			// freq here = 2048 - (2 ^ 17 / freq val)

		}

		//if (buttonsnotpressed & BUTTON_A)
		//{
		//	OAM[(bulletno * 4) + 0] = ((bullety + 10) << 0) | (0 << 14); //y coord, 14 = obj type
		//	OAM[(bulletno * 4) + 1] = ((bulletx + 20) << 0) | (0 << 12) | (0 << 13) | (0 << 14);  //x coord, 12 = h flip, 13 = v flip, 14 = obj size
		//	OAM[(bulletno * 4) + 2] = (16 << 0) | (2 << 12); // tile << 0 and pal << 12
		//	bulletx += 5;

		//	if (bulletx >= 250)
		//	{
		//		bulletx = 250;
		//	}
		//}

		if (buttonspressed & BUTTON_L)
		{
			bulletpal = 3;
		}

		if (buttonspressed & BUTTON_R)
		{
			bulletpal = 2;
		}

		if (buttonspressed & DPAD_DOWN)         // y-axis is flipped
		{
			y += 2;
			if (y > 140)
			{ 
				y = 140;
			}
			OAM[(25 * 4) + 0] = (y << 0) | (1 << 14);
		}

		if (buttonspressed & DPAD_UP)          // y-axis is flipped
		{
			y -= 2;
			if (y < 0)
			{
				y = 0;
			}
			OAM[(25 * 4) + 0] = (y << 0) | (1 << 14);
		}

		if (buttonspressed & DPAD_RIGHT)
		{
			x += 2;
			if (x > 140)
			{
				x = 140;
			}
			OAM[(25 * 4) + 1] = (x << 0) | (0 << 12) | (0 << 13) | (2 << 14);
		}

		if (buttonspressed & DPAD_LEFT)
		{
			x -= 2;
			if (x < 0)
			{
				x = 0;
			}
			OAM[(25 * 4) + 1] = (x << 0) | (0 << 12) | (0 << 13) | (2 << 14);
		}

		VBlankIntrWait();
		// crucial gfx update code goes as close to here as possible

	}


}

