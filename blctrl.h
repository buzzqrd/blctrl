#ifndef BRIGHTNESS_CONTROL_H
#define BRIGHTNESS_CONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEFAULT_BACKLIGHT_PATH "/sys/class/backlight/"
#define DEFAULT_BACKLIGHT_MAX "max_brightness"
#define DEFAULT_BACKLIGHT_VALUE "brightness"

#define MAX_PERCENT 100

#define MAX_STR_SIZE 512


typedef struct {
	char path[MAX_STR_SIZE];
	char device[MAX_STR_SIZE];
	char value[MAX_STR_SIZE];
	int brightness;
	int maximum;
} Backlight;


void backlight_constrain(int *value, int min, int max);
int backlight_setup(Backlight *backlight, char *path, char *device, char *max, char *value);
void backlight_print(Backlight backlight);
int backlight_set_value(Backlight *backlight, int value);
int backlight_set_percent(Backlight *backlight, int percent);
int backlight_get_value(Backlight *backlight);
int backlight_get_percent(Backlight *backlight);



void backlight_constrain(int *value, int min, int max){
	if(min > max){
		fprintf(stderr, "Error: Minimum value is greater than maximum value when attempting to constrain.\n");
		return;
	}
	if(*value < min){
		*value = min;
	}
	if(*value > max){
		*value = max;
	}
	return;
}


int backlight_setup(Backlight *backlight, char *path, char *device, char *max, char *value){
	memset(backlight, 0, sizeof(Backlight));

	strcpy(backlight->device, device);
	strcpy(backlight->path, (path == NULL) ? DEFAULT_BACKLIGHT_PATH : path);

	strcat(backlight->path, backlight->device);
	
	strcpy(backlight->value, backlight->path);
	strcat(backlight->value, "/");
	strcat(backlight->value, (value == NULL) ? DEFAULT_BACKLIGHT_VALUE : value);

	char max_bright_filename[MAX_STR_SIZE];
	memset(max_bright_filename, 0, 1);
	strcpy(max_bright_filename, backlight->path);
	strcat(max_bright_filename, "/"); 
	strcat(max_bright_filename, (max == NULL) ? DEFAULT_BACKLIGHT_MAX : max);
	
	FILE *fmax = fopen(max_bright_filename, "r");
	char read_bright_str[MAX_STR_SIZE];
	memset(read_bright_str, 0, MAX_STR_SIZE);
	if(fmax == NULL){
		fprintf(stderr, "Error: backlight_setup could not open the file for the maximum brightness. (%s)\n", max_bright_filename);
		return(1);
	}

	fgets(read_bright_str, MAX_STR_SIZE, fmax);
	fclose(fmax);	

	backlight->maximum = atoi(read_bright_str);
	
	FILE *fcur = fopen(backlight->value, "r");
	if(fcur == NULL){
		fprintf(stderr, "Error: backlight_setup could not open the file for reading or modifying the brightness. (%s)\n", backlight->path);
	}

	memset(read_bright_str, 0, MAX_STR_SIZE);
	fgets(read_bright_str, MAX_STR_SIZE, fcur);
	fclose(fcur);
	
	backlight->brightness = atoi(read_bright_str);
	
	return(0);
}

void backlight_print(Backlight backlight){
	printf("Path: \t\t%s\nDevice: \t%s\nValue file: \t%s\nBrightness: \t%d\nMaximum: \t%d\n", backlight.path, backlight.device, backlight.value, backlight.brightness, backlight.maximum);
	return;
}

int backlight_set_value(Backlight *backlight, int value){
	backlight_constrain(&value, 0, backlight->maximum);
	FILE *vf = fopen(backlight->value, "w+");
	if(vf == NULL){
		fprintf(stderr, "Error: backlight_set_value(): could not open the file for the backlight brightness. (%s)\n", backlight->value);
		return(1);
	}	

	char value_str[MAX_STR_SIZE];
	sprintf(value_str, "%d", value);
	fputs(value_str, vf);
	backlight->brightness = value;
	fclose(vf);
	return(0);
}

int backlight_set_percent(Backlight *backlight, int percent){
	backlight_constrain(&percent, 0, MAX_PERCENT);
	double percent_ratio = (double)percent / (double)MAX_PERCENT;
	int new_value = (int)ceil((double)backlight->maximum * percent_ratio);
	return(backlight_set_value(backlight, new_value));
}

int backlight_get_value(Backlight *backlight){
	FILE *vf = fopen(backlight->value, "r");
	if(vf == NULL){
		fprintf(stderr, "Error: backlight_get_value(): could not open the file for reading backlight brightness. (%s)\n", backlight->value);
		return(-1);
	}
	
	char brightness_str[MAX_STR_SIZE];
	fgets(brightness_str, MAX_STR_SIZE, vf);
	fclose(vf);
	backlight->brightness = (int)atoi(brightness_str);
	return(backlight->brightness);
}

int backlight_get_percent(Backlight *backlight){
	int brightness_value = backlight_get_value(backlight);
	if(brightness_value == -1){
		return(-1);
	}
	double ratio = (double)brightness_value / (double)backlight->maximum;
	int percent = (int)(ratio * MAX_PERCENT);
	return(percent);
}


#endif
