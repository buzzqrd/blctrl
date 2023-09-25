
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <signal.h>

#include "blctrl.h"

/* change for your specific backlight */
#define BACKLIGHT_DEVICE "amdgpu_bl1"
#define BACKLIGHT_PATH NULL
#define MIN_BRIGHTNESS_PERCENT 5
#define MAX_BRIGHTNESS_PERCENT 100

/* change to modify the brightness bar */
#define BAR_INCREMENTS 5.0
#define MAX_BARS (100.0 / BAR_INCREMENTS)
#define STEP_SIZE 5

#define ESCAPE_CODE 27
#define SECOND_CODE 91

#define KEYCODE_0 48
#define KEYCODE_9 57
#define SELECT_MULT 10

#define KEYCODE_UP    65
#define KEYCODE_DOWN  66
#define KEYCODE_RIGHT 67
#define KEYCODE_LEFT  68

#define KEYCODE_QUIT 'q'


struct termios term;


void kill_sig(int err){
	term.c_lflag |= (ICANON | ECHO);
	tcsetattr(fileno(stdin), 0, &term);
	printf("\n");
	exit(0);
}


void percent_bar(Backlight *backlight){
	int percent = backlight_get_percent(backlight);
	int bars = (int)((double)percent / BAR_INCREMENTS);
	printf("\r%s: [", backlight->device);
	int i;
	for(i=0; i<bars; i++){
		printf("=");
	}
	for(i=0; i<(MAX_BARS-bars); i++){
		printf(" ");
	}	
	printf("] %3d%%", percent);
	fflush(stdout);
	return;
}

int main(int argc, const char *argv[]){
	signal(SIGINT, kill_sig);
	
	tcgetattr(fileno(stdin), &term);
	term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(stdin), 0, &term);

	Backlight backlight;
	backlight_setup(&backlight, BACKLIGHT_PATH, BACKLIGHT_DEVICE, NULL, NULL);

	int percent = backlight_get_percent(&backlight);

	percent_bar(&backlight);
	
	char kb = 0;
	int  changed = 0;
	while(1){
		kb = getchar();
	
		/* arrow keys */
		if(kb == ESCAPE_CODE){
			kb = getchar();
			if(kb == SECOND_CODE){
				kb = getchar();
				if(kb == KEYCODE_LEFT || kb == KEYCODE_DOWN){
					percent -= STEP_SIZE;
					changed = 1;
				}
				if(kb == KEYCODE_RIGHT || kb == KEYCODE_UP){
					percent += STEP_SIZE;
					changed = 1;
				}
			}
		}	

		/* percent levels */
		if(kb >= KEYCODE_0 && kb <= KEYCODE_9){
			percent = ((kb - KEYCODE_0) * SELECT_MULT);
			changed = 1;
		}

		if(kb == KEYCODE_QUIT){
			break;
		}

		if(changed){
			backlight_constrain(&percent, MIN_BRIGHTNESS_PERCENT, MAX_BRIGHTNESS_PERCENT);
			backlight_set_percent(&backlight, percent);
			percent_bar(&backlight);
			changed = 0;
		}
	}

	term.c_lflag |= (ICANON | ECHO);
	tcsetattr(fileno(stdin), 0, &term);
	printf("\n");

	return(0);
}

