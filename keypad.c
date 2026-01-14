#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>

#define CHIP_PATH "/dev/gpiochip0"

int main(){

    /*STRUCTS*/
    struct gpiod_chip *chip;
    struct gpiod_request_config *req_cfg;
    struct gpiod_line_settings *in_line_sts, *out_line_sts;
    struct gpiod_line_config *lines_cfg;
    struct gpiod_line_request *lines_req;

    /*OFFSETS*/
    unsigned int row_keypad_pins[4] = {2, 3, 4, 17 };   // Phisical -> 3, 5, 7, 11 (INPUT)
    unsigned int column_keypad_pins[3] = {27, 22, 10};  // Phisical -> 13, 15, 19 (OUTPUT)
    enum gpiod_line_value initial_line_value_column[3] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    enum gpiod_line_value line_value_column_1[3] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    enum gpiod_line_value line_value_column_2[3] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    enum gpiod_line_value line_value_column_3[3] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

    /*CHIP*/
    chip = gpiod_chip_open(CHIP_PATH);

    /*REQUEST CONFIG*/
    req_cfg = gpiod_request_config_new();
    gpiod_request_config_set_consumer(req_cfg, "keypad");

    /*LINE SETTINGS*/
    //INPUT - ROW
    in_line_sts = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(in_line_sts, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(in_line_sts, GPIOD_LINE_BIAS_PULL_UP);
    //OUTPUT - COLUMN
    out_line_sts = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(in_line_sts, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(in_line_sts, GPIOD_LINE_VALUE_INACTIVE);

    /*LINE CONFIG*/
    lines_cfg = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(lines_cfg, row_keypad_pins, 4, in_line_sts);
    gpiod_line_config_add_line_settings(lines_cfg, column_keypad_pins, 3, out_line_sts);
    gpiod_line_config_set_output_values(lines_cfg, initial_line_value_column, 3);

    /*LINE REQUEST*/
    lines_req = gpiod_chip_request_lines(chip, req_cfg, lines_cfg);

    /*LOOP*/
    while(1){
	
    }

    return 0;
}
