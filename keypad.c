#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    printf("INICIO %s\n", argv[0]);

    int aux;

    /*STRUCTS*/
    struct gpiod_chip *chip;
    struct gpiod_request_config *r_config;
    struct gpiod_line_settings *row_l_settings;
    struct gpiod_line_settings *col_l_settings;
    struct gpiod_line_config *l_config;
    struct gpiod_line_request *l_request;
    struct gpiod_edge_event_buffer *event_buffer;

    /*OFFSET*/
    unsigned int row_offset[4] = {};
    unsigned int col_offset[3] = {}; 

    /*ENUM*/
    const enum gpiod_line_value row_initial_values[4] = {GPIO_LINE_VALUE_INACTIVE, GPIO_LINE_VALUE_INACTIVE, GPIO_LINE_VALUE_INACTIVE, GPIO_LINE_VALUE_INACTIVE};

    /*CHIP*/
    chip = gpiod_chip_open("/dev/gpiochip0");
    if(chip == NULL){
        perror("Function 'gpiod_chip_open' return NULL");
        return 1;
    }
    printf("CHIP ok\n");

    /*REQUEST CONFIG*/
    r_config = gpiod_request_config_new();
    if(r_config == NULL){
        perror("Function 'gpiod_request_config_new' return NULL");
        gpiod_chip_close(chip);    
        return 1;
    }
    gpio_request_config_set_consumer(r_config, "keypad");
    printf("REQUEST CONFIG ok\n");

    /*LINE SETTINGS*/
    // ROW --> OUTPUT
    row_l_settings = gpiod_line_settings_new();
    if(row_l_settings == NULL){
        perror("Function 'gpiod_line_settings_new' return NULL");
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("LINE SETTINGS row_l_settings ...\n");

    aux = gpiod_line_settings_set_direction(row_l_settings, GPIOD_LINE_DIRECTION_OUTPUT);
    if(aux < 0){
        perror("Function 'gpiod_line_settings_set_direction' failure");
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tOUTPUT ok\n");

    aux = gpiod_line_settings_set_output_values(row_l_settings, GPIO_LINE_VALUE_INACTIVE);
    if(aux < 0){
        perror("Function 'gpiod_line_settings_set_output_values' failure");
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tINITIAL STATE INACTIVE (LOW) ok\n");
    
    // COL --> INPUT | PULL_UP | RISING
    col_l_settings = gpiod_line_settings_new();
    if(col_l_settings == NULL){
        perror("Function 'gpiod_line_settings_new' return NULL");
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("LINE SETTINGS col_l_settings ...\n");

    aux = gpiod_line_settings_set_direction(col_l_settings, GPIOD_LINE_DIRECTION_INPUT);
    if(aux < 0){
        perror("Function 'gpiod_line_settings_set_direction' failure");
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tINPUT ok\n");

    aux = gpiod_line_settings_set_bias(col_l_settings, GPIOD_LINE_BIAS_PULL_UP);
    if(aux < 0){
        perror("Function 'gpiod_line_settings_set_bias' failure");
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tPULL_UP ok\n");

    aux = gpiod_line_settings_set_edge_detection(col_l_settings, GPIOD_LINE_EDGE_RISING);
    if(aux < 0){
        perror("Function 'gpiod_line_settings_set_edge_detection' failure");
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tEDGE_RISING ok\n");

    /*LINE CONFIG*/
    l_config = gpiod_line_config_new();
    if(l_config == NULL){
        perror("Function 'gpiod_line_settings_new' return NULL");
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("LINE CONFIG ...\n");

    aux = gpiod_line_config_add_line_settings(l_config, row_offset, 4, row_l_settings);
    if(aux < 0){
        perror("Function 'gpiod_line_config_add_line_settings' failure");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tADD row_l_settings\n");

    aux = gpiod_line_config_add_line_settings(l_config, col_offset, 3, col_l_settings);
    if(aux < 0){
        perror("Function 'gpiod_line_config_add_line_settings' failure");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tADD col_l_settings\n");

    aux = gpiod_line_config_set_output_values(l_config, row_initial_values, 4);
    if(aux < 0){
        perror("Function 'gpiod_line_config_set_output_values' failure");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("\tINITIAL VALUES (LOW) ok\n");

    /*LINE REQUEST*/
    l_request = gpiod_chip_request_lines(chip, r_config, l_config);
    if(l_request == NULL){
        perror("Function 'gpiod_chip_request_lines' return NULL");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("LINE CONFIG ...\n");

    /*EVENT BUFFER*/
    event_buffer = gpiod_edge_event_buffer_new(1);
    if(event_buffer == NULL){
        perror("Function 'gpiod_edge_event_buffer_new' return NULL");
        gpiod_line_request_release(l_request);
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(col_l_settings);
        gpiod_line_settings_free(row_l_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);    
        return 1;
    }
    printf("EVENT BUFFER ok\n");

    /*LOOP*/
    

    /*FREE*/
    gpiod_edge_event_buffer_free(event_buffer);
    gpiod_line_request_release(l_request);
    gpiod_line_config_free(l_config);
    gpiod_line_settings_free(col_l_settings);
    gpiod_line_settings_free(row_l_settings);
    gpiod_request_config_free(r_config);
    gpiod_chip_close(chip);
    return 0;
}