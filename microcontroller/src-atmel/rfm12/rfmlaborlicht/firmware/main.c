#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "rfm12.h"
#include <string.h>
#include "../common/protocol.h"

#define PW(a) pgm_read_word(&(a))

#define MASK_RED (1<<PC0)
#define MASK_GREEN (1<<PC1)
#define MASK_BLUE (1<<PC2)
#define PORTFL PORTC
#define DDRFL DDRC

/* number of seconds before rfmlight starts autonomous fading */
#define CONF_TIMEOUT 2

#if F_CPU == 16000000
uint16_t PROGMEM timeTable [] = { 0, 0, 0, 0, 0, 0, 0, 0, 210, 56, 62, 69, 75, 82, 89, 95, 102, 108, 115, 121, 128, 
 								  135, 141, 148, 154, 161, 167, 174, 180, 187, 194, 200, 207, 
								  213, 220, 226, 233, 239, 246, 253, 259, 266, 272, 279, 285, 
								  292, 299, 305, 312, 318, 325, 331, 338, 344, 351, 358, 364, 
								  371, 377, 384, 390, 397, 404, 410, 417, 423, 430, 436, 443, 
								  449, 456, 463, 469, 476, 482, 489, 495, 502, 509, 515, 522, 
								  528, 535, 541, 548, 554, 561, 568, 574, 581, 587, 594, 600, 
								  607, 614, 620, 627, 633, 640, 646, 653, 659, 666, 673, 679, 
								  686, 692, 699, 705, 712, 718, 725, 732, 738, 745, 751, 758, 
								  764, 771, 778, 784, 791, 797, 804, 810, 817, 823, 830, 837, 
								  843, 850, 856, 863, 869, 876, 883, 889, 896, 902, 909, 915, 
								  922, 928, 935, 942, 948, 955, 961, 968, 974, 981, 988, 994, 
								  1001, 1007, 1014, 1020, 1027, 1033, 1040, 1047, 1053, 1060, 
								  1066, 1073, 1079, 1086, 1093, 1099, 1106, 1112, 1119, 1125, 
								  1132, 1138, 1145, 1152, 1158, 1165, 1171, 1178, 1184, 1191, 
								  1197, 1204, 1211, 1217, 1224, 1230, 1237, 1243, 1250, 1257, 
								  1263, 1270, 1276, 1283, 1289, 1296, 1302, 1309, 1316, 1322, 
								  1329, 1335, 1342, 1348, 1355, 1362, 1368, 1375, 1381, 1388, 
								  1394, 1401, 1407, 1414, 1421, 1427, 1434, 1440, 1447, 1453, 
								  1460, 1467, 1473, 1480, 1486, 1493, 1499, 1506, 1512, 1519, 
								  1526, 1532, 1539, 1545, 1552, 1558, 1565, 1572, 1578, 1585, 
								  1591, 1598, 1604, 1611, 1617, 1624, 1631, 1637, 1644, 1650, 
								  1657, 1663, 1670
};
#elif F_CPU==8000000
uint16_t PROGMEM timeTable [] = { 0, 0, 0, 0, 0, 0, 0, 0, 105, 28, 31, 35, 75, 38, 45, 48, 51, 54, 57, 60, 64,  								  	       67, 70,74,77,81,84,87,90,94,97,100,104,107,110,113,117,120,123,127,130,133,136,140,143,146,150,153,156,159,163,166,169,172,176,179,182,186,189,192,195,199,202,205,209,212,215,218,222,225,228,232,235,238,241,245,248,251,255,258,261,264,268,271,274,277,281,284,287,291,294,297,300,304,307,310,314,317,320,323,327,330,333,337,340,343,346,350,353,356,359,363,366,369,373,376,379,382,386,389,392,396,399,402,405,409,412,415,419,422,425,428,432,435,438,442,445,448,451,455,458,461,464,468,471,474,478,481,484,487,491,494,497,501,504,507,510,514,517,520,524,527,530,533,537,540,543,547,550,553,556,560,563,566,569,573,576,579,583,586,589,592,596,599,602,606,609,612,615,619,622,625,629,632,635,638,642,645,648,651,655,658,661,665,668,671,674,678,681,684,688,691,694,697,701,704,707,711,714,717,720,724,727,730,734,737,740,743,747,750,753,756,760,763,766,770,773,776,779,783,786,789,793,796,799,802,806,809,812,816,819,822,825,829,832,835};
#endif

uint8_t color[3] = {0, 0, 0}; 
volatile uint8_t seconds = 0, timeoutticks = 0;


SIGNAL(SIG_OUTPUT_COMPARE1A) {
	static uint8_t bright = 7;
	static uint16_t secticks = 0;
	TCNT1 = 0;
	OCR1A  = PW(timeTable[++bright]);

	if (bright < color[0]) { // red
		PORTFL |= MASK_RED;
	} else {
	    PORTFL &= ~MASK_RED;
	}
	if (bright < color[1]) { // green
		PORTFL |= MASK_GREEN;
	} else {
		PORTFL &= ~MASK_GREEN;
	} 
	if (bright < color[2]) { // blue
		PORTFL |= MASK_BLUE;
	} else {
		PORTFL &= ~MASK_BLUE;
	}
	if (bright == 255)
		bright = 7;
	
	secticks++;
	if (secticks != 64000) return;
	
	timeoutticks++;
	seconds++;
	secticks = 0;
}


void timer1_init()
{
	TCCR1A = 0; 			
	TCCR1B = 1;		// Counter clk (once per clockcycle)
	OCR1A  = 250;
	TCNT1  = 0;
	TIFR   = (1 << OCF1A);
	TIMSK  = (1 << OCIE1A);
}


void fadeToColor(uint8_t *fadeColor, uint8_t steps)
{
	static uint16_t i;
	static int16_t helpColor[3], addColor[3];
	
	if (!steps)
		return;
	
	helpColor[0] = color[0]*128;
	helpColor[1] = color[1]*128;
	helpColor[2] = color[2]*128;
	
	addColor[0] = ((fadeColor[0]*128)-helpColor[0])/(int16_t)steps;
	addColor[1] = ((fadeColor[1]*128)-helpColor[1])/(int16_t)steps;
	addColor[2] = ((fadeColor[2]*128)-helpColor[2])/(int16_t)steps;
	
//	for (i = 0; i < steps; ++i)
	++i;
	{
		helpColor[0] += addColor[0];
		helpColor[1] += addColor[1];
		helpColor[2] += addColor[2];
		color[0] = (helpColor[0]+64)/128;
		color[1] = (helpColor[1]+64)/128;
		color[2] = (helpColor[2]+64)/128;
		_delay_ms (10);
	}

	if (i >= steps) i = 0;
	color[0] = fadeColor[0];
	color[1] = fadeColor[1];
	color[2] = fadeColor[2];
}

#define BIT_S(var,b) ((var&(1<<b))?1:0)

void rlight_process_msg (uint8_t *in_buf)
{
	static uint8_t mycolor[3] = { 0x00, 0x00, 0x00 };
	uint8_t i = 0;
	uint8_t tmp = 0;

	switch (in_buf[0])
	{
		case RLIGHT_SETCOLOR:
			if (rfm12_rx_len() < 4) return;
			for (i=1;i<4;i++)
				mycolor[i-1] = in_buf[i];
			
			fadeToColor (mycolor, 1);
			seconds = 0;
		break;
		case RLIGHT_FADETO:
			if (rfm12_rx_len() < 4) return;
			tmp = 128;
			for (i=1;i<4;i++)
				mycolor[i-1] = in_buf[i];
			fadeToColor (mycolor, tmp);	
			seconds = 0;
		break;
	}
}

volatile uint8_t fadecolors[6][3] = 
{
	{0xff, 0x00, 0x00},
	{0xff, 0xff, 0x00},
	{0x00, 0xff, 0x00},
	{0x00, 0xff, 0xff},
	{0x00, 0x00, 0xff},
	{0xff, 0x00, 0xff}
};

int main(void)
{
	const uint8_t PROGMEM strhello[] = "LABORLICHT ALIVE!\r\n";
	uint8_t *rxbuf;
	uint8_t currentcolor = 0; 
	uint8_t csteps;
	// Initialiesierung
	DDRFL = 0xff;
	PORTFL = 0x00;

	_delay_ms(250);
	_delay_ms(250);
	timer1_init();
	rfm12_init();
	_delay_ms(250);
	_delay_ms(250);

	sei ();

	rfm12_tx (10, 0xE2, strhello);
	
	fadeToColor((uint8_t[]){0,0, 255}, 128);

	while (42)
	{
		rfm12_tick();

		if (rfm12_rx_status() == STATUS_COMPLETE)
		{
			rxbuf = rfm12_rx_buffer();
			if (rfm12_rx_type() != 0xE2)
			{
				rfm12_rx_clear();
				continue;
			}

			rlight_process_msg (rxbuf);
			rfm12_rx_clear();
		}
		continue;

		if (seconds > CONF_TIMEOUT)
		{
			seconds = CONF_TIMEOUT+1;
			
			if (timeoutticks != 3) continue;
			csteps = 250;
			timeoutticks = 0;
			currentcolor++;
			if (currentcolor == 6) currentcolor = 0;

		}

		if (csteps)
		{
			fadeToColor (fadecolors[currentcolor], csteps);
			csteps = 0;
		}
	}
	  
}
