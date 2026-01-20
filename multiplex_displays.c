#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char *argv[]){
    printf("INICIO -> %s\n", argv[0]);
    
    /*STRUCTS*/
    struct gpiod_chip *chip;
    struct gpiod_request_config *r_config;
    struct gpiod_line_settings *l_settings_out_bcd;
    struct gpiod_line_settings *l_settings_out_mult;
    struct gpiod_line_settings *l_settings_in_reset;
    struct gpiod_line_config *l_config;
    struct gpiod_line_request *l_request;

    /*OFFSETS*/
    unsigned int offset_out_bcd = {};
    unsigned int offset_out_mult = {};
    unsigned int offset_in_reset = {};

    /*ENUM*/
    const enum gpiod_line_value bcd_table[10][4] = {
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // 0
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE}, // 1
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // 2
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE}, // 3
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // 4
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE}, // 5
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // 6
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE}, // 7
        {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // 8
        {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE} //9
    };

    const enum gpiod_line_value display[5][4] = {
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // Todos desativados
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE}, // Display 1 ativado
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // Display 2 ativado
        {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE}, // Display 3 ativado
        {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE} // Display 4 ativado
    };

    /*CHIP*/
    chip = gpiod_chip_open("/dev/gpiochip0");
    if(chip == NULL){
        return 1;
    }
    printf("CHIP\n");

    /*REQUEST CONFIG*/
    r_config = gpiod_request_config_new();
    if(r_config == NULL){
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_request_config_set_consumer(r_config, "Mult_display");
    printf("REQUEST CONFIG\n");

    /*LINES SETTINGS*/
    l_settings_out_bcd = gpiod_line_settings_new();
    if(l_settings_out_bcd == NULL){
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_line_settings_set_direction(l_settings_out_bcd, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(l_settings_out_bcd, GPIOD_LINE_VALUE_INACTIVE);

    l_settings_out_mult = gpiod_line_settings_new();
    if(l_settings_out_mult == NULL){
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_line_settings_set_direction(l_settings_out_mult, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(l_settings_out_mult, GPIOD_LINE_VALUE_INACTIVE);


    l_settings_in_reset = gpiod_line_settings_new();
    if(l_settings_in_reset == NULL){
        gpiod_line_settings_free(l_settings_out_mult);
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_line_settings_set_direction(l_settings_in_reset, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(l_settings_in_reset, GPIOD_LINE_BIAS_PULL_UP);
    gpiod_line_settings_set_edge_detection(l_settings_in_reset, GPIOD_LINE_EDGE_FALLING);

    /*LINE CONFIG*/
    l_config = gpiod_line_config_new();
    if(l_config == NULL){
        gpiod_line_settings_free(l_settings_in_reset);
        gpiod_line_settings_free(l_settings_out_mult);
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_line_config_add_line_settings(l_config, offset_out_bcd, 4, l_settings_out_bcd);
    gpiod_line_config_add_line_settings(l_config, offset_out_mult, 4, l_settings_out_mult);
    gpiod_line_config_add_line_settings(l_config, offset_in_reset, 1, l_settings_in_reset);

    /*LINE REQUEST*/
    l_request = gpiod_chip_request_lines(chip, r_config, l_config);
    if(l_request == NULL){
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(l_settings_in_reset);
        gpiod_line_settings_free(l_settings_out_mult);
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }

    /*LOOP*/
    while(1){
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[0]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_bcd, bcd_table[0]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[1]);
        usleep(1000); // 1ms
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[0]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_bcd, bcd_table[1]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[2]);
        usleep(1000); // 1ms
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[0]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_bcd, bcd_table[2]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[3]);
        usleep(1000); // 1ms
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[0]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_bcd, bcd_table[3]);
        gpiod_line_request_set_values_subset(l_request, 4, offset_out_mult, display[4]);
        usleep(1000); // 1ms
    }

    /*FREE*/
    gpiod_line_config_free(l_config);
    gpiod_line_settings_free(l_settings_in_reset);
    gpiod_line_settings_free(l_settings_out_mult);
    gpiod_line_settings_free(l_settings_out_bcd);
    gpiod_request_config_free(r_config);
    gpiod_chip_close(chip);

    return 0;
}