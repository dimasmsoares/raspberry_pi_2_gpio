#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("%s\n", argv[0]);
	struct gpiod_chip *chip;
	struct gpiod_line_settings *led_line_settings;
	struct gpiod_line_settings *button_line_settings;
	struct gpiod_line_config *line_config;
	struct gpiod_request_config *request_config;
	struct gpiod_line_request *request;
	

	unsigned int leds[7] = {2, 3, 4, 17, 27, 22, 10};	//3, 5, 7, 11, 13, 15, 19
	unsigned int buttons[1] = {9};				//21
	enum gpiod_line_value buttons_value[1];
	const enum gpiod_line_value leds_value_on[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_0[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_1[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_2[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_3[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_4[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_5[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};

	const enum gpiod_line_value leds_value_6[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};

	const enum gpiod_line_value leds_value_7[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_8[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_9[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_A[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_B[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_C[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};

	const enum gpiod_line_value leds_value_D[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};

	const enum gpiod_line_value leds_value_E[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};

	const enum gpiod_line_value leds_value_F[7] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};

	const enum gpiod_line_value leds_value_off[7] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};


	// CHIP
	chip = gpiod_chip_open("/dev/gpiochip0");
	if(chip == NULL){
		perror("gpiod_chip_open return NULL");
		return 1;
	}
	printf("CHIP OPEN\n");

	// LED_LINE_SETTINGS
	led_line_settings = gpiod_line_settings_new();
	if(led_line_settings == NULL){
		perror("LED gpiod_line_settings_new return NULL");
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_settings_set_direction(led_line_settings, GPIOD_LINE_DIRECTION_OUTPUT) < 0){
		perror("LED gpiod_line_settings_set_direction FAILURE");	
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	gpiod_line_settings_set_output_value(led_line_settings, GPIOD_LINE_VALUE_INACTIVE);
	printf("led_line_settings OK\n");

	// BUTTON_LINE_SETTINGS
	button_line_settings = gpiod_line_settings_new();
	if(button_line_settings == NULL){
		perror("BUTTON gpiod_line_settings_new return NULL");
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_settings_set_direction(button_line_settings, GPIOD_LINE_DIRECTION_INPUT) < 0){
		perror("BUTTON gpiod_line_settings_set_direction FAILURE");	
		gpiod_line_settings_free(led_line_settings);
		gpiod_line_settings_free(button_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	gpiod_line_settings_set_bias(button_line_settings, GPIOD_LINE_BIAS_PULL_UP);
	
	// LINE_CONFIG
	line_config = gpiod_line_config_new();
	if(line_config == NULL){
		perror("gpiod_line_config_new return NULL");
		gpiod_line_settings_free(led_line_settings);
		gpiod_line_settings_free(button_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_config_add_line_settings(line_config, leds, 7, led_line_settings) < 0){
		perror("LED gpiod_line_config_add_line_settings return NULL");
		gpiod_line_config_free(line_config);
		gpiod_line_settings_free(button_line_settings);
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	printf("Leds ready\n");
	if(gpiod_line_config_add_line_settings(line_config, buttons, 1, button_line_settings) < 0){
		perror("LED gpiod_line_config_add_line_settings return NULL");
		gpiod_line_config_free(line_config);
		gpiod_line_settings_free(button_line_settings);
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	printf("Buttons ready\n");
	
	// REQUEST
	request_config = gpiod_request_config_new();
	if(request_config == NULL){
		perror("gpiod_request_config_new return NULL");
		gpiod_line_config_free(line_config);
		gpiod_line_settings_free(button_line_settings);
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	gpiod_request_config_set_consumer(request_config, "seven_segments");

	request = gpiod_chip_request_lines(chip, request_config, line_config);
	if(request == NULL){
		perror("gpiod_chip_request_lines return NULL");
		gpiod_request_config_free(request_config);
		gpiod_line_config_free(line_config);
		gpiod_line_settings_free(button_line_settings);
		gpiod_line_settings_free(led_line_settings);
		gpiod_chip_close(chip);
		return 1;
	}
	printf("Request ready\n");
	
	// LOOP
	while(1){
		if(gpiod_line_request_get_values_subset(request, 1, buttons, buttons_value) < 0){
			perror("Read buttons FAILURE");
			break;
		}
		if(buttons_value[0] == GPIOD_LINE_VALUE_INACTIVE){
			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_0);
			sleep(1);
			
			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_1);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_2);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_3);			
			sleep(1);
			
			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_4);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_5);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_6);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_7);
			sleep(1);			
			
			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_8);
			sleep(1);

			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_9);
			sleep(1);

		}	
		else {
			gpiod_line_request_set_values_subset(request, 7, leds, leds_value_0);	
		}
	}


	gpiod_line_request_release(request);
	gpiod_request_config_free(request_config);
	gpiod_line_config_free(line_config);
	gpiod_line_settings_free(button_line_settings);
	gpiod_line_settings_free(led_line_settings);
	gpiod_chip_close(chip);
	printf("FREE led_line_settings | chip\n");
	
	return 0;
}
