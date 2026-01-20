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

    /*ENUM*/

    /*CHIP*/
    chip = gpiod_chip_open("/dev/chipgpio0");
    if(chip == NULL){
        return 1;
    }

    /*REQUEST CONFIG*/
    r_config = gpiod_request_config_new();
    if(r_config == NULL){
        gpiod_chip_close(chip);
        return 1;
    }

    /*LINES SETTINGS*/
    l_settings_out_bcd = gpiod_line_settings_new();
    if(l_settings_out_bcd == NULL){
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }

    l_settings_out_mult = gpiod_line_settings_new();
    if(l_settings_out_mult == NULL){
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }

    l_settings_in_reset = gpiod_line_settings_new();
    if(l_settings_in_reset == NULL){
        gpiod_line_settings_free(l_settings_out_mult);
        gpiod_line_settings_free(l_settings_out_bcd);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }


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

    /*FREE*/
    gpiod_line_config_free(l_config);
    gpiod_line_settings_free(l_settings_in_reset);
    gpiod_line_settings_free(l_settings_out_mult);
    gpiod_line_settings_free(l_settings_out_bcd);
    gpiod_request_config_free(r_config);
    gpiod_chip_close(chip);

    return 0;
}