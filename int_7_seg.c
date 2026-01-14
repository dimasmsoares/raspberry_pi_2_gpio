#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	printf("%s\n", argv[0]);
	
	/***STRUCTS***/
	struct gpiod_chip *chip;
	struct gpiod_line_settings *line_sts_out;
	struct gpiod_line_settings *line_sts_in;
	struct gpiod_line_config *line_cfg;
	struct gpiod_line_request *line_rqt;
	struct gpiod_request_config *rqt_cfg;
	struct gpiod_edge_event_buffer *evt_buf;

	/***PINS***/
	unsigned int display[8] = {2, 3, 4, 17, 27, 22, 10, 9};	// OUTPUT
	unsigned int button[1] = {26};	// INPUT

	/***CHIP***/
	chip = gpiod_chip_open("/dev/gpiochip0");
	if(chip == NULL){
		perror("gpiod_chip_open return NULL");
		return 1;
	}

	/***LINE SETTINGS***/
	line_sts_out = gpiod_line_settings_new();
	if(line_sts_out == NULL){
		perror("gpiod_line_settings_new return NULL");
		gpiod_chip_close(chip);
		return 1;
	}
	// OUTPUT - ACTIVE LOW
	if(gpiod_line_settings_set_direction(line_sts_out, GPIOD_LINE_DIRECTION_OUTPUT) < 0){
		perror("gpiod_line_settings_set_direction ERROR");
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}
	gpiod_line_settings_set_output_value(line_sts_out, GPIOD_LINE_VALUE_INACTIVE);
	
	// INPUT - PULL_UP - EDGE_FALLING
	line_sts_in = gpiod_line_settings_new();
	if(line_sts_in == NULL){
		perror("gpiod_line_settings_new return NULL");
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}
	
	if(gpiod_line_settings_set_direction(line_sts_in, GPIOD_LINE_DIRECTION_INPUT) < 0){
		perror("gpiod_line_settings_set_direction ERROR");
		gpiod_line_settings_free(line_sts_out);	
		gpiod_line_settings_free(line_sts_in);
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_settings_set_bias(line_sts_in, GPIOD_LINE_BIAS_PULL_UP) < 0){
		perror("gpiod_line_settings_set_bias ERROR");
		gpiod_line_settings_free(line_sts_in);
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;	
	}
	if(gpiod_line_settings_set_edge_detection(line_sts_in, GPIOD_LINE_EDGE_FALLING) < 0){
		perror("gpiod_line_settings_set_edge_detection ERROR");
		gpiod_line_settings_free(line_sts_in);
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;	
	}

	//LINE CONFIGURATION
	line_cfg = gpiod_line_config_new();
	if(line_cfg == NULL){
		perror("gpiod_line_config_new return NULL");
		gpiod_line_settings_free(line_sts_in);	
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_config_add_line_settings(line_cfg, display, 8, line_sts_out) < 0){
		perror("gpiod_line_config_add_line_settings ERROR");
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(line_sts_in);	
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}
	if(gpiod_line_config_add_line_settings(line_cfg, button, 1, line_sts_in) < 0){
		perror("gpiod_line_config_add_line_settings ERROR");
		gpiod_line_config_free(line_cfg);
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(line_sts_in);	
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}

	//REQUEST CONFIGURATION
	rqt_cfg = gpiod_request_config_new();
	if(rqt_cfg == NULL){
		perror("gpiod_request_config_new return NULL");
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(line_sts_in);	
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}
	gpiod_request_config_set_consumer(rqt_cfg, "int_7seg");

	line_rqt = gpiod_chip_request_lines(chip, rqt_cfg, line_cfg);
	if(line_rqt < 0){
		perror("gpiod_chip_request_lines return NULL");
		gpiod_request_config_free(rqt_cfg);
		gpiod_line_config_free(line_cfg);
		gpiod_line_settings_free(line_sts_in);	
		gpiod_line_settings_free(line_sts_out);
		gpiod_chip_close(chip);
		return 1;
	}

	//EVENT
	evt_buf = gpiod_edge_event_buffer_new(1);

	// LOOP
	while(1){
		int ret = gpiod_line_request_wait_edge_events(line_rqt, -1);
		if(ret < 0){
			perror("Erro ao esperar eventos");
			break;
		}
		else if(ret > 0){
			ret = gpiod_line_request_read_edge_events(line_rqt, evt_buf, 1);
			
			if(ret > 0){
				printf("Botão pressionado\n");
			}
			usleep(200000);
		}

	}	

	//FREE
	gpiod_edge_event_buffer_free(evt_buf);
	gpiod_line_request_release(line_rqt);
	gpiod_request_config_free(rqt_cfg);
	gpiod_line_config_free(line_cfg);
	gpiod_line_settings_free(line_sts_in);	
	gpiod_line_settings_free(line_sts_out);
	gpiod_chip_close(chip);
	return 0;
}
