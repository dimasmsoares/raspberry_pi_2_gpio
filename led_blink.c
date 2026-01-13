/*This program aims to blink an LED using a Raspberry Pi*/

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

#define CHIP_PATH "/dev/gpiochip0"
#define LED_PIN 17  // GPIO 17 (pino físico 11)

int main(int argc, char *argv[]){
    printf("Blinking a LED...\n");

    int aux, count;

    /***STRUCTS***/
    struct gpiod_chip *chip;
    struct gpiod_request_config *request_config;
    struct gpiod_line_settings *line_led_settings;
    struct gpiod_line_config *lines_config;
    struct gpiod_line_request *lines_request;

    /***OFFSETS***/
    unsigned int offset_led[1] = {LED_PIN};
    enum gpiod_line_value initial_state_offset_led[1] = {GPIOD_LINE_VALUE_INACTIVE};

    /***CHIP***/    
    chip = gpiod_chip_open(CHIP_PATH);  // Note 1
    if(chip == NULL){
        perror("An error occurred: function 'gpiod_chip_open' return NULL");
        return 1;
    }

    /***REQUEST***/
    request_config = gpiod_request_config_new();
    if(request_config == NULL){
        perror("An error occurred: function 'gpiod_request_config_new' return NULL");
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_request_config_set_consumer(request_config, "blink-led");

    /***LINE SETTINGS***/
    // LED -> OUTPUT -> LOW
    line_led_settings = gpiod_line_settings_new();  // Note 2
    if(line_led_settings == NULL){
        perror("An error occurred: function 'gpiod_line_settings_new' return NULL");
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }
    // Defining as OUTPUT
    aux = gpiod_line_settings_set_direction(line_led_settings, GPIOD_LINE_DIRECTION_OUTPUT);  // Note 3
    if(aux < 0){
        perror("An error occurred: function 'gpiod_line_settings_set_direction' return -1");
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }
    // Initial state LOW
    aux = gpiod_line_settings_set_output_value(line_led_settings, GPIOD_LINE_VALUE_INACTIVE);   // Note 4
    if(aux < 0){
        perror("An error occurred: function 'gpiod_line_settings_set_output_value' return -1");
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }

    /***LINES CONFIG***/
    lines_config = gpiod_line_config_new();
    if(lines_config == NULL){
        perror("An error occurred: function 'gpiod_line_config_new' return NULL");
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }
    // Adding line_led_settings to line_config
    aux = gpiod_line_config_add_line_settings(lines_config, offset_led, 1, line_led_settings);
    if(aux < 0){
        perror("An error occurred: function 'gpiod_line_config_add_line_settings' return -1");
        gpiod_line_config_free(lines_config);
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }
    aux = gpiod_line_config_set_output_values(lines_config, initial_state_offset_led, 1);
    if(aux < 0){
        perror("An error occurred: function 'gpiod_line_config_set_output_values' return -1");
        gpiod_line_config_free(lines_config);
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }

    /***LINES REQUEST***/
    lines_request = gpiod_chip_request_lines(chip, request_config, lines_config);
    if(lines_request == NULL){
        perror("An error occurred: function 'gpiod_chip_request_lines' return NULL");
        gpiod_line_config_free(lines_config);
        gpiod_line_settings_free(line_led_settings);
        gpiod_request_config_free(request_config);
        gpiod_chip_close(chip);
        return 1;
    }

    /***LOOP***/
    count = 0;
    while(count < 10){
        aux = gpiod_line_request_set_value(lines_request, offset_led[0], GPIOD_LINE_VALUE_ACTIVE);
        if(aux < 0 ){
            perror("An error occurred: function 'gpiod_line_request_set_value' return -1");
            gpiod_line_request_release(lines_request);
            gpiod_line_config_free(lines_config);
            gpiod_line_settings_free(line_led_settings);
            gpiod_request_config_free(request_config);
            gpiod_chip_close(chip);
            return 1;
        }
        usleep(1000000); // 500 ms
        aux = gpiod_line_request_set_value(lines_request, offset_led[0], GPIOD_LINE_VALUE_INACTIVE);
        if(aux < 0 ){
            perror("An error occurred: function 'gpiod_line_request_set_value' return -1");
            gpiod_line_request_release(lines_request);
            gpiod_line_config_free(lines_config);
            gpiod_line_settings_free(line_led_settings);
            gpiod_request_config_free(request_config);
            gpiod_chip_close(chip);
            return 1;
        }
        usleep(1000000); // 500 ms
        count ++;
    }
    

    /****FREE***/
    gpiod_line_request_release(lines_request);
    gpiod_line_config_free(lines_config);
    gpiod_line_settings_free(line_led_settings);
    gpiod_request_config_free(request_config);
    gpiod_chip_close(chip);

    return 0;
}


/*NOTES
1) Function 'gpiod_chip_open' opens a chip by path.

struct gpiod_chip *gpiod_chip_open(const char *path)
Parameters:
 - path – Path to the gpiochip device file.
Returns:
 - GPIO chip object or NULL if an error occurred. 

 OBS: The returned object must be closed by the caller using 'gpiod_chip_close'.

2) Function 'gpiod_line_settings_new' creates a new line settings object.

struct gpiod_line_settings *gpiod_line_settings_new(void)
Returns:
 - New line settings object or NULL on error. 
 
 OBS: The returned object must be freed by the caller using 'gpiod_line_settings_free'.

3) Function 'gpiod_line_settings_set_direction' sets direction.

int gpiod_line_settings_set_direction(struct gpiod_line_settings *settings, enum gpiod_line_direction direction)
Parameters:
 - settings – Line settings object.
 - direction – New direction (enum gpiod_line_direction).
Returns:
 - 0 on success, -1 on error.

enum gpiod_line_direction
 - GPIOD_LINE_DIRECTION_AS_IS: Request the line(s), but don’t change direction.
 - GPIOD_LINE_DIRECTION_INPUT: Direction is input - for reading the value of an externally driven GPIO line.
 - GPIOD_LINE_DIRECTION_OUTPUT: Direction is output - for driving the GPIO line.

4) Function 'gpiod_line_settings_set_output_value' sets the initial output value.

int gpiod_line_settings_set_output_value(struct gpiod_line_settings *settings, enum gpiod_line_value value)
Parameters:
 - settings – Line settings object.
 - value – Initial output value (1 - HIGH | 0 - LOW).
Returns:
 - 0 on success, -1 on failure.

5) Function 'gpiod_line_config_add_line_settings' adds line settings for a set of offsets.

int gpiod_line_config_add_line_settings(struct gpiod_line_config *config, const unsigned int *offsets, size_t num_offsets, struct gpiod_line_settings *settings)
Parameters:
 - config – Line config object.
 - offsets – Array of offsets for which to apply the settings.
 - num_offsets – Number of offsets stored in the offsets array.
 - settings – Line settings to apply.
Returns:
 - 0 on success, -1 on failure.

6) Function 'gpiod_chip_request_lines' requests a set of lines for exclusive usage.

struct gpiod_line_request *gpiod_chip_request_lines(struct gpiod_chip *chip, struct gpiod_request_config *req_cfg, struct gpiod_line_config *line_cfg)
Parameters:
 - chip – GPIO chip object.
 - req_cfg – Request config object. Can be NULL for default settings.
 - line_cfg – Line config object.
Returns:
 - New line request object or NULL if an error occurred. 

OBS: The request must be released by the caller using gpiod_line_request_release.

*/
