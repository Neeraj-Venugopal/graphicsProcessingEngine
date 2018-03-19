/*
===============================================================================
 Name        : DrawLine.c
 Author      : $RJ
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#include <cr_section_macros.h>
#include <NXP/crp.h>
#include "LPC17xx.h"                        /* LPC17xx definitions */
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>



/* Be careful with the port number and location number, because

some of the location may not exist in that port. */

#define PORT_NUM            0


uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];


#define ST7735_TFTWIDTH 127
#define ST7735_TFTHEIGHT 159

#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_SLPOUT 0x11
#define ST7735_DISPON 0x29



#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

// defining color values

#define LIGHTBLUE 0x00FFE0
#define GREEN 0x00FF00
#define DARKBLUE 0x000033
#define BLACK 0x000000
#define BLUE 0x0007FF
#define RED 0xFF0000
#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF
#define PURPLE 0xCC33FF
#define ORANGE 0xFFA500
#define INDIGO 0x4B0082
#define PINK 0xFF69B4
#define IR 0xCD5C5C
#define CHOC 0xD2691E
#define TGREEN 0x55AE3A



int _height = ST7735_TFTHEIGHT;
int _width = ST7735_TFTWIDTH;


void spiwrite(uint8_t c)

{

 int pnum = 0;

 src_addr[0] = c;

 SSP_SSELToggle( pnum, 0 );

 SSPSend( pnum, (uint8_t *)src_addr, 1 );

 SSP_SSELToggle( pnum, 1 );

}



void writecommand(uint8_t c)

{

 LPC_GPIO0->FIOCLR |= (0x1<<21);

 spiwrite(c);

}



void writedata(uint8_t c)

{

 LPC_GPIO0->FIOSET |= (0x1<<21);

 spiwrite(c);

}



void writeword(uint16_t c)

{

 uint8_t d;

 d = c >> 8;

 writedata(d);

 d = c & 0xFF;

 writedata(d);

}



void write888(uint32_t color, uint32_t repeat)

{

 uint8_t red, green, blue;

 int i;

 red = (color >> 16);

 green = (color >> 8) & 0xFF;

 blue = color & 0xFF;

 for (i = 0; i< repeat; i++) {

  writedata(red);

  writedata(green);

  writedata(blue);

 }

}



void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)

{

 writecommand(ST7735_CASET);

 writeword(x0);

 writeword(x1);

 writecommand(ST7735_RASET);

 writeword(y0);

 writeword(y1);

}


void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 int16_t i;

 int16_t width, height;

 width = x1-x0+1;

 height = y1-y0+1;

 setAddrWindow(x0,y0,x1,y1);

 writecommand(ST7735_RAMWR);

 write888(color,width*height);

}



void lcddelay(int ms)

{

 int count = 24000;

 int i;

 for ( i = count*ms; i--; i > 0);

}



void lcd_init()

{

 int i;
 printf("LCD Demo Begins!!!\n");
 // Set pins P0.16, P0.21, P0.22 as output
 LPC_GPIO0->FIODIR |= (0x1<<16);

 LPC_GPIO0->FIODIR |= (0x1<<21);

 LPC_GPIO0->FIODIR |= (0x1<<22);

 // Hardware Reset Sequence
 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOCLR |= (0x1<<22);
 lcddelay(500);

 LPC_GPIO0->FIOSET |= (0x1<<22);
 lcddelay(500);

 // initialize buffers
 for ( i = 0; i < SSP_BUFSIZE; i++ )
 {

   src_addr[i] = 0;
   dest_addr[i] = 0;
 }

 // Take LCD display out of sleep mode
 writecommand(ST7735_SLPOUT);
 lcddelay(200);

 // Turn LCD display on
 writecommand(ST7735_DISPON);
 lcddelay(200);

}




void drawPixel(int16_t x, int16_t y, uint32_t color)

{

 if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))

 return;

 setAddrWindow(x, y, x + 1, y + 1);

 writecommand(ST7735_RAMWR);

 write888(color, 1);

}



/*****************************************************************************


** Descriptions:        Draw line function

**

** parameters:           Starting point (x0,y0), Ending point(x1,y1) and color

** Returned value:        None

**

*****************************************************************************/


void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)

{

 int16_t slope = abs(y1 - y0) > abs(x1 - x0);

 if (slope) {

  swap(x0, y0);

  swap(x1, y1);

 }

 if (x0 > x1) {

  swap(x0, x1);

  swap(y0, y1);

 }

 int16_t dx, dy;

 dx = x1 - x0;

 dy = abs(y1 - y0);

 int16_t err = dx / 2;

 int16_t ystep;

 if (y0 < y1) {

  ystep = 1;

 }

 else {

  ystep = -1;

 }

 for (; x0 <= x1; x0++) {

  if (slope) {

   drawPixel(y0, x0, color);

  }

  else {

   drawPixel(x0, y0, color);

  }

  err -= dy;

  if (err < 0) {

   y0 += ystep;

   err += dx;

  }

 }

}

void delayLogic()
{

	 int k = 1239434;
	 int i = 10;

	 // Delay Logic // Sleep Not Working //
	 while( i != 0)
	 {
		 while ( k != 0)
		 {
			 k--;
		 }
		 k = 1239434;
	 i--;
	 }

}

void drawSquare()
{
		 int X0, X1, X2, X3, Y0, Y1, Y2, Y3;
		 int x0, x1, x2, x3, y0, y1, y2, y3;
		 int i;
		 int j;

		 X0 = 2;
		 Y0 = 2;

		 X1 = 122;
		 Y1 = 2;

		 X2 = 122;
		 Y2 = 122;

		 X3 = 2;
		 Y3 = 122;

		 x0 = X0;
		 x1 = X1;
		 x2 = X2;
		 x3 = X3;
		 y0 = Y0;
		 y1 = Y1;
		 y2 = Y2;
		 y3 = Y3;
		 int prev = x0;

		 for ( i = 0; i < 10; i++)
					 {
						if (i == 1)
							x3 = prev;
							// prev = x0;

			 	 	 	 	 drawLine(x0, y0, x1, y1, WHITE);
							 drawLine(x1, y1, x2, y2, WHITE);
							 drawLine(x2, y2, x3, y3, WHITE);
							 drawLine(x3, y3, x0, y0, WHITE);


						 float lambda = 0.2;


						 x0 =  x0 + (lambda * ( x1 - x0));
						 y0 = y0 + (lambda * ( y1 - y0));
						 x1 = x1 + (lambda * ( x2 - x1));
						 y1 = y1 + (lambda * ( y2 - y1));
						 x2 = x2 + (lambda * ( x3 - x2));
						 y2 = y2 + (lambda * ( y3 - y2));
						 x3 = x3 + (lambda * ( x0 - x3));
						 y3 = y3 + (lambda * (y0 - y3));
						 delayLogic();
					 }

		 delayLogic();
		 delayLogic();

		 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

		 X0 = 0;
				 Y0 = 0;

				 X1 = 0;
				 Y1 = 35;

				 X2 = 35;
				 Y2 = 35;

				 X3 = 35;
				 Y3 = 0;

		for( j = 0; j < 4; j++)
		{
			x0 = X0;
			x1 = X1;
			x2 = X2;
			x3 = X3;
			y0 = Y0;
			y1 = Y1;
			y2 = Y2;
			y3 = Y3;

			 for ( i = 0; i < 10; i++)
			 {
				 if(j == 0)
				 {
					 drawLine(x0, y0, x1, y1, LIGHTBLUE);
					 drawLine(x1, y1, x2, y2, LIGHTBLUE);
					 drawLine(x2, y2, x3, y3, LIGHTBLUE);
					 drawLine(x3, y3, x0, y0, LIGHTBLUE);
				 }
				 else if ( j == 1)
				 {
					 drawLine(x0, y0, x1, y1, IR);
					 drawLine(x1, y1, x2, y2, IR);
					 drawLine(x2, y2, x3, y3, IR);
					 drawLine(x3, y3, x0, y0, IR);

				 }
				 else if ( j == 2 )
				 {
					 drawLine(x0, y0, x1, y1, CHOC);
					 drawLine(x1, y1, x2, y2, CHOC);
					 drawLine(x2, y2, x3, y3, CHOC);
					 drawLine(x3, y3, x0, y0, CHOC);
				 }
				 else if ( j == 3)
				 {
					 drawLine(x0, y0, x1, y1, GREEN);
					 drawLine(x1, y1, x2, y2, GREEN);
					 drawLine(x2, y2, x3, y3, GREEN);
					 drawLine(x3, y3, x0, y0, GREEN);
				 }


				 float lambda = 0.2;
				 float temp = 0;

				 temp =  x0 + (lambda * ( x1 - x0));
						 x0 = ceil(temp);

						 temp = y0 + (lambda * ( y1 - y0));
						 y0 = ceil(temp);

						 temp = x1 + (lambda * ( x2 - x1));
						 x1 = floor(temp);

						 temp = y1 + (lambda * ( y2 - y1));
						 y1 = floor(temp);

						 temp = x2 + (lambda * ( x3 - x2));
						 x2 = floor(temp);

						 temp = y2 + (lambda * ( y3 - y2));
						 y2 = floor(temp);

						 temp = x3 + (lambda * ( x0 - x3));
						 x3 = floor(temp);

						 temp = y3 + (lambda * (y0 - y3));
						 y3 = floor(temp);

			 }
			 delayLogic();

			 int k = 38;

			 X0 = X1;
			 X3 = X2;

			 Y0 += k;
			 Y1 += k;
			 Y2 += k;
			 Y3 += k;
		}


		 X0 = 38;
		 Y0 = 0;

		 X1 = 73;
		 Y1 = 0;

		 X2 = 73;
		 Y2 = 35;

		 X3 = 38;
		 Y3 = 35;

			for( j = 0; j < 2; j++)
			{
				x0 = X0;
				x1 = X1;
				x2 = X2;
				x3 = X3;
				y0 = Y0;
				y1 = Y1;
				y2 = Y2;
				y3 = Y3;

				 for ( i = 0; i < 10; i++)
				 {
					 if(j == 0)
					 {
						 drawLine(x0, y0, x1, y1, ORANGE);
						 drawLine(x1, y1, x2, y2, ORANGE);
						 drawLine(x2, y2, x3, y3, ORANGE);
						 drawLine(x3, y3, x0, y0, ORANGE);
					 }
					 else if ( j == 1)
					 {
						 drawLine(x0, y0, x1, y1, RED);
						 drawLine(x1, y1, x2, y2, RED);
						 drawLine(x2, y2, x3, y3, RED);
						 drawLine(x3, y3, x0, y0, RED);

					 }

					 float lambda = 0.2;
					 float temp = 0;

					 temp =  x0 + (lambda * ( x1 - x0));
							 x0 = ceil(temp);

							 temp = y0 + (lambda * ( y1 - y0));
							 y0 = ceil(temp);

							 temp = x1 + (lambda * ( x2 - x1));
							 x1 = floor(temp);

							 temp = y1 + (lambda * ( y2 - y1));
							 y1 = floor(temp);

							 temp = x2 + (lambda * ( x3 - x2));
							 x2 = floor(temp);

							 temp = y2 + (lambda * ( y3 - y2));
							 y2 = floor(temp);

							 temp = x3 + (lambda * ( x0 - x3));
							 x3 = floor(temp);

							 temp = y3 + (lambda * (y0 - y3));
							 y3 = floor(temp);

				 }
				 delayLogic();
				 int k = 38;

				 Y0 = Y1;
				 Y3 = Y2;

				 X0 += k;
				 X1 += k;
				 X2 += k;
				 X3 += k;
			}

			 X0 = 38;
			 Y0 = 38;

			 X1 = 73;
			 Y1 = 38;

			 X2 = 73;
			 Y2 = 73;

			 X3 = 38;
			 Y3 = 73;

				for( j = 0; j < 2; j++)
				{
					x0 = X0;
					x1 = X1;
					x2 = X2;
					x3 = X3;
					y0 = Y0;
					y1 = Y1;
					y2 = Y2;
					y3 = Y3;

					 for ( i = 0; i < 10; i++)
					 {
						 if(j == 0)
						 {
							 drawLine(x0, y0, x1, y1, WHITE);
							 drawLine(x1, y1, x2, y2, WHITE);
							 drawLine(x2, y2, x3, y3, WHITE);
							 drawLine(x3, y3, x0, y0, WHITE);
						 }
						 else if ( j == 1)
						 {
							 drawLine(x0, y0, x1, y1, MAGENTA);
							 drawLine(x1, y1, x2, y2, MAGENTA);
							 drawLine(x2, y2, x3, y3, MAGENTA);
							 drawLine(x3, y3, x0, y0, MAGENTA);

						 }

						 float lambda = 0.2;
						 float temp = 0;

						 temp =  x0 + (lambda * ( x1 - x0));
								 x0 = ceil(temp);

								 temp = y0 + (lambda * ( y1 - y0));
								 y0 = ceil(temp);

								 temp = x1 + (lambda * ( x2 - x1));
								 x1 = floor(temp);

								 temp = y1 + (lambda * ( y2 - y1));
								 y1 = floor(temp);

								 temp = x2 + (lambda * ( x3 - x2));
								 x2 = floor(temp);

								 temp = y2 + (lambda * ( y3 - y2));
								 y2 = floor(temp);

								 temp = x3 + (lambda * ( x0 - x3));
								 x3 = floor(temp);

								 temp = y3 + (lambda * (y0 - y3));
								 y3 = floor(temp);

					 }
					 delayLogic();
					 int k = 38;

					 Y0 = Y1;
					 Y3 = Y2;

					 X0 += k;
					 X1 += k;
					 X2 += k;
					 X3 += k;
				}

				 X0 = 38;
				 Y0 = 76;

				 X1 = 73;
				 Y1 = 76;

				 X2 = 73;
				 Y2 = 111;

				 X3 = 38;
				 Y3 = 111;

					for( j = 0; j < 2; j++)
					{
						x0 = X0;
						x1 = X1;
						x2 = X2;
						x3 = X3;
						y0 = Y0;
						y1 = Y1;
						y2 = Y2;
						y3 = Y3;

						 for ( i = 0; i < 10; i++)
						 {
							 if(j == 0)
							 {
								 drawLine(x0, y0, x1, y1, PURPLE);
								 drawLine(x1, y1, x2, y2, PURPLE);
								 drawLine(x2, y2, x3, y3, PURPLE);
								 drawLine(x3, y3, x0, y0, PURPLE);
							 }
							 else if ( j == 1)
							 {
								 drawLine(x0, y0, x1, y1, BLUE);
								 drawLine(x1, y1, x2, y2, BLUE);
								 drawLine(x2, y2, x3, y3, BLUE);
								 drawLine(x3, y3, x0, y0, BLUE);

							 }

							 float lambda = 0.2;
							 float temp = 0;

							 temp =  x0 + (lambda * ( x1 - x0));
									 x0 = ceil(temp);

									 temp = y0 + (lambda * ( y1 - y0));
									 y0 = ceil(temp);

									 temp = x1 + (lambda * ( x2 - x1));
									 x1 = floor(temp);

									 temp = y1 + (lambda * ( y2 - y1));
									 y1 = floor(temp);

									 temp = x2 + (lambda * ( x3 - x2));
									 x2 = floor(temp);

									 temp = y2 + (lambda * ( y3 - y2));
									 y2 = floor(temp);

									 temp = x3 + (lambda * ( x0 - x3));
									 x3 = floor(temp);

									 temp = y3 + (lambda * (y0 - y3));
									 y3 = floor(temp);

						 }
						 delayLogic();
						 int k = 38;

						 Y0 = Y1;
						 Y3 = Y2;

						 X0 += k;
						 X1 += k;
						 X2 += k;
						 X3 += k;
					}
}

void grow_mytree(int x0, int y0, float angle, int length, int level, int color){
//	printf("inside mytree\n");

	int x1, y1, length1;
	float angle1;


		if(level>0){

//		if(level == 6)
//		{
//			drawLine(64, y0, 64, 130,color);
//		}
//		//x-y coordinates of branch
//		else
		{
		x1 = x0+length*cos(angle);
//		printf("%f\n",x1);
	    y1 = y0+length*sin(angle);
//	    printf("%f\n",y1);

	    drawLine(x0,y0,x1,y1,color); //tree branch
		}
		x1 = x0+length*cos(angle);
		//		printf("%f\n",x1);
	    y1 = y0+length*sin(angle);

	   // if ( level == 6)
	    	angle1 = angle + 0.52; //deviate right->0.52 rad/30 deg
	  //  else
	    //	angle1 = angle + 0.52;
	    length1 = 0.8 * length; //reduction of length by 20% of previous length
	    grow_mytree(x1,y1,angle1,length1,level-1,color);

	    angle1 = angle - 0.52; //deviate left->0.52 rad/30 deg
	    length1 = 0.8 * length;
	    grow_mytree(x1,y1,angle1,length1,level-1,color);

	    angle1 = angle; //center->0 deg
	    length1 = 0.8 * length;
	    grow_mytree(x1,y1,angle1,length1,level-1,color);
	}
	//	printf("exiting mytree\n");
}
/*

 Main Function main()

*/



int main (void)

{
	 uint32_t pnum = PORT_NUM;

	 pnum = 0 ;

	 if ( pnum == 0 )
		 SSP0Init();

	 else
		 puts("Port number is not correct");

	 lcd_init();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

	 drawSquare();
	 delayLogic();
	 delayLogic();
	 delayLogic();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, WHITE);

	 int x0, y0;

	 x0 = 60;
	 y0 = 160;

	 // void grow_mytree(int x0, int y0, float angle, int length, int level, int color)
	 // One Big Tree //
	 grow_mytree(x0,  y0, 30, 30, 7, GREEN);
	 fillrect(0, 0, ST7735_TFTWIDTH, 18, LIGHTBLUE);

	 delayLogic();
	 delayLogic();
	 delayLogic();

	 fillrect(0, 0, ST7735_TFTWIDTH, ST7735_TFTHEIGHT, BLACK);

	 grow_mytree(0,  y0, 30, 38, 7, TGREEN);
	 grow_mytree(25,  y0, 30, 20, 5, TGREEN);
	 grow_mytree(50,  y0, 30, 25, 6, TGREEN);
	 grow_mytree(80,  y0, 30, 15, 5, TGREEN);
	 grow_mytree(130, y0, 30, 20, 7, TGREEN);

	 return 0;
}
