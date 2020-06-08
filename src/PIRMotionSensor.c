#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>

#define LED_PIN   0
#define PIR_PIN   7


int main(void)
{
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi config failed!");
		exit(1);	
	}

	pinMode(LED_PIN, OUTPUT);
	pinMode(PIR_PIN, INPUT);

	digitalWrite(LED_PIN,LOW);

	while(1)
	{
		if(digitalRead(PIR_PIN) == 0)
		{
			digitalWrite(LED_PIN, LOW);
			printf("|       no motion detected      |\n");
		}
		else
		{
			printf("|       motion detected         |\n");
			digitalWrite(LED_PIN, HIGH);
		}
		delay(1000);
	}
	return 0;
}
