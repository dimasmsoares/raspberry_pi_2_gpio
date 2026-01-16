/* Este código tem como objetivo uma aplicação em que um display de 7 segmentos é 
controlado por botões.
Botão 1: incrementa o valor
Botão 2: decrementa o valor
Botão 3: reseta o valor

Para o acionamento do display é usado o CI 4511, que recebe um valor de 4 bits e o 
converte para o display. Além disso outras 3 entradas do CI são utilizadas, 
totalizando 7 saídas.
*/

/***Bibliotecas***/
#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    printf("INÍCIO\n");

    int aux;
    const unsigned int btns_offset[3] = {2, 3, 4}; // Equivalente aos pinos 3, 5 e 7
    const unsigned int bcd_offset[4] = {17, 27, 22, 10}; // 11, 13, 15, 19

    /*STRUCTS*/
    struct gpiod_chip *chip;
    struct gpiod_request_config *r_config;
    struct gpiod_line_settings *btns_line_settings;
    struct gpiod_line_settings *bcd_line_settings;
    struct gpiod_line_config *l_config;
    struct gpiod_line_request *l_request;
    struct gpiod_edge_event_buffer *buffer;
    struct gpiod_edge_event *event;

    /*DISPLAY*/
    const enum gpiod_line_value bcd0[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    const enum gpiod_line_value bcd1[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};
    const enum gpiod_line_value bcd2[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    const enum gpiod_line_value bcd3[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};
    const enum gpiod_line_value bcd4[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    const enum gpiod_line_value bcd5[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};
    const enum gpiod_line_value bcd6[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    const enum gpiod_line_value bcd7[4] = {GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_ACTIVE};
    const enum gpiod_line_value bcd8[4] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE};
    const enum gpiod_line_value bcd9[4] = {GPIOD_LINE_VALUE_ACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_INACTIVE, GPIOD_LINE_VALUE_ACTIVE};

    const enum gpiod_line_value *bcd[10] = {bcd0, bcd1, bcd2, bcd3, bcd4, bcd5, bcd6, bcd7, bcd8, bcd9};
    

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
        perror("Function'gpiod_request_config_new' return NULL");
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_request_config_set_consumer(r_config, "event_display");
    printf("REQUEST CONFIG ok\n");

    /*LINES SETTINGS*/
    // 1) GPIOs como entrada com pull-up e interrupção por borda de descida (botões)
    btns_line_settings = gpiod_line_settings_new();
    if(btns_line_settings == NULL){
        perror("Function'gpiod_line_settings_new' return NULL");
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("BTNS_LINE_SETTINGS ok\n");

    aux = gpiod_line_settings_set_direction(btns_line_settings, GPIOD_LINE_DIRECTION_INPUT);
    if(aux < 0){
        perror("Function'gpiod_line_settings_set_direction' failed");
        gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BTNS_LINE_SETTINGS --> INPUT ok\n");

    aux = gpiod_line_settings_set_bias(btns_line_settings, GPIOD_LINE_BIAS_PULL_UP);
    if(aux < 0){
        perror("Function'gpiod_line_settings_set_bias' failed");
        gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BTNS_LINE_SETTINGS --> PULL_UP ok\n");

    aux = gpiod_line_settings_set_edge_detection(btns_line_settings, GPIOD_LINE_EDGE_FALLING);
    if(aux < 0){
        perror("Function'gpiod_line_settings_set_edge_detection' failed");
        gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BTNS_LINE_SETTINGS --> EDGE_FALLING ok\n");

    // 2) GPIOs como saída (BCD para o CI 4511)
    bcd_line_settings = gpiod_line_settings_new();
    if(bcd_line_settings == NULL){
        perror("Function'gpiod_line_settings_new' return NULL");
        gpiod_line_settings_free(btns_line_settings);
	gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BCD_LINE_SETTINGS ok\n");

    aux = gpiod_line_settings_set_direction(bcd_line_settings, GPIOD_LINE_DIRECTION_OUTPUT);
    if(aux < 0){
        perror("Function'gpiod_line_settings_set_direction' failed");
        gpiod_line_settings_free(bcd_line_settings);
	gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BCD_LINE_SETTINGS --> OUTPUT ok\n");
    
    /*LINE CONFIG*/
    l_config = gpiod_line_config_new();
    if(l_config == NULL){
        perror("Function'gpiod_line_config_new' return NULL");
        gpiod_line_settings_free(bcd_line_settings);
	gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("L_CONFIG ok\n");

    aux = gpiod_line_config_add_line_settings(l_config, btns_offset, 3, btns_line_settings);
    if(aux < 0){
        perror("Function'gpiod_line_config_add_line_settings' failed");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(bcd_line_settings);
	gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- L_CONFIG added BTNS_LINE_SETTINGS ok\n");

    aux = gpiod_line_config_add_line_settings(l_config, bcd_offset, 4, bcd_line_settings);
    if(aux < 0){
        perror("Function'gpiod_line_config_add_line_settings' failed");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(bcd_line_settings);
	gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- L_CONFIG added BCD_LINE_SETTINGS ok\n");

    aux = gpiod_line_config_set_output_values(l_config, bcd0, 4);
    if(aux < 0){
        perror("Function'gpiod_line_config_set_output_values' failed");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(bcd_line_settings);
	    gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BCD_LINE set to bcd0 ok\n");

    /*LINE REQUEST*/
    l_request = gpiod_chip_request_lines(chip, r_config, l_config);
    if(l_request == NULL){
        perror("Function'gpiod_chip_request_lines' return NULL");
        gpiod_line_config_free(l_config);
	gpiod_line_settings_free(bcd_line_settings);
        gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- LINE_REQUEST ok\n");

    /*EVENT*/
    buffer = gpiod_edge_event_buffer_new(1);
    if(buffer == NULL){
        perror("Function'gpiod_edge_event_buffer_new' return NULL");
        gpiod_line_config_free(l_config);
	gpiod_line_settings_free(bcd_line_settings);
        gpiod_line_settings_free(btns_line_settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("\t- BUFFER ok\n");

    int display_value = 0;
    /*LOOP*/
    while(1){
        printf("display_value = %d\n", display_value);
        // Espera até um novo evento acontecer no kernel
	    aux = gpiod_line_request_wait_edge_events(l_request, -1);
        if(aux < 0){
            perror("Function'gpiod_line_request_wait_edge_events' failed");
            gpiod_edge_event_buffer_free(buffer);
            gpiod_line_config_free(l_config);
	        gpiod_line_settings_free(bcd_line_settings);
            gpiod_line_settings_free(btns_line_settings);
            gpiod_request_config_free(r_config);
            gpiod_chip_close(chip);
            return 1;
        }
        // Transfere o evento do kernel para o buffer
        aux = gpiod_line_request_read_edge_events(l_request, buffer, 1);
        if(aux < 0){
            perror("Function'gpiod_line_request_read_edge_events' failed");
            gpiod_edge_event_buffer_free(buffer);
            gpiod_line_config_free(l_config);
	        gpiod_line_settings_free(bcd_line_settings);
            gpiod_line_settings_free(btns_line_settings);
            gpiod_request_config_free(r_config);
            gpiod_chip_close(chip);
            return 1;
        }
        // Processando o evento
        event = gpiod_edge_event_buffer_get_event(buffer, 0);   // Acessa o evento que está no buffer e o coloca em 'event'
        unsigned int offset = gpiod_edge_event_get_line_offset(event);  // Ver de qual line ele se origina
        enum gpiod_edge_event_type tipo = gpiod_edge_event_get_event_type(event);   // Ver seu tipo

        if (tipo == GPIOD_EDGE_EVENT_FALLING_EDGE) {
            // Incrementando
            if (offset == btns_offset[0] && display_value <= 8){
                display_value++;
            }
            else if (offset == btns_offset[0] && display_value == 9){
                display_value = 0;
            }
            // Decrementando
            else if (offset == btns_offset[1] && display_value >= 1){
                display_value--;
            }
            else if (offset == btns_offset[1] && display_value == 0){
                display_value = 9;
            }

            // Reset
            else if (offset == btns_offset[2]){
                display_value = 0;
            }
        }

        aux = gpiod_line_request_set_values_subset(l_request, 4, bcd_offset, bcd[display_value]);
        if(aux < 0){
            perror("Function'gpiod_line_request_set_values_subset' failed");
            gpiod_edge_event_buffer_free(buffer);
            gpiod_line_config_free(l_config);
	        gpiod_line_settings_free(bcd_line_settings);
            gpiod_line_settings_free(btns_line_settings);
            gpiod_request_config_free(r_config);
            gpiod_chip_close(chip);
            return 1;
        }

        usleep(50000);  // Debounce 50ms
	
        // Limpar eventuais eventos acumulados
        while(gpiod_line_request_wait_edge_events(l_request, 0) > 0){
            gpiod_line_request_read_edge_events(l_request, buffer, 1);
        }
    }

    /*FREE*/
    gpiod_edge_event_buffer_free(buffer);
    gpiod_line_request_release(l_request);
    gpiod_line_config_free(l_config);
    gpiod_line_settings_free(bcd_line_settings);
    gpiod_line_settings_free(btns_line_settings);
    gpiod_request_config_free(r_config);
    gpiod_chip_close(chip);
    printf("FREE ok\n");
    
    return 0;
}
