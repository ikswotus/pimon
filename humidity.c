#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <pcf8574.h>
#include <lcd.h>

#define BASE 64
#define pcf8574_address 0x27
#define MAXTIMINGS	85
#define DHTPIN 7

#define RS BASE+0
#define RW BASE+1
#define EN BASE+2
#define LED BASE+3
#define D4 BASE+4
#define D5 BASE+5
#define D6 BASE +6
#define D7 BASE +7

int lcdhd; // lcd



int dht11_dat[5] = { 0, 0, 0, 0, 0 };
 
void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;
 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 18 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 40 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		lcdPosition(lcdhd, 0, 0);
		lcdPrintf(lcdhd, "Humidity = %d.%d %%\n", dht11_dat[0], dht11_dat[1]);
		lcdPosition(lcdhd, 0, 1);
		lcdPrintf(lcdhd, "Temp %.1f F\n", f);
	}else  {
		printf( "Data not good, skip\n" );
	}
}
 
int main( void )
{
	printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );
 
	if ( wiringPiSetup() == -1 )
		exit( 1 );
	int lcd;

	pcf8574Setup(BASE, pcf8574_address);

	for (int i =0; i < 8;i++)
	{
		pinMode(BASE+i, OUTPUT);
	}

	digitalWrite(LED, HIGH);
	digitalWrite(RW, LOW);

	lcdhd = lcdInit(2, 16, 4, RS, EN, D4, D5, D6, D7, 0, 0, 0, 0);

	if(lcdhd == -1)
{
	printf("lcdInit failed!");
	return 1;
} 

	while ( 1 )
	{
		read_dht11_dat();
		delay( 1000 );
	}
 
	return(0);
}
