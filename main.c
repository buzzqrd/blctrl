/*
blctrl
main.c
buzzqrd

An example C file for using blctrl.h functions in another program.

This code is licenced in the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include "bright_ctrl.h"


int main(int argc, const char *argv[]){

	if(argc < 2){
		fprintf(stderr, "Error: Not enough arguments.\n");
		return(-1);
	}

	Backlight light;
	if(backlight_setup(&light, NULL, "amdgpu_bl0", NULL, NULL)){
		fprintf(stderr, "Could not open backlight properly.\n");
		return(-1);
	}

	backlight_print(light);


	int new_brightness = atoi(argv[1]);
	printf("Setting new brightness to: %d\n", new_brightness);
	backlight_set_percent(&light, new_brightness);

	int current_light = backlight_get_value(&light);
	printf("Current Backlight value: %d\n", current_light);
	int percent = backlight_get_percent(&light);
	printf("Current Backlight percent: %d\n", percent);

	return(0);
}


