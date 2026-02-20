#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t running = 1;

void handle_sigint(int sig){
    (void)sig;
    running = 0;
}

int main(){

    signal(SIGINT, handle_sigint);

    struct gpiod_chip *chip;
    struct gpiod_request_config *req_cfg;
    struct gpiod_line_settings *led_line_stgs;
    struct gpiod_line_settings *btn_line_stgs;
    struct gpiod_line_config *line_cfg;
    struct gpiod_line_request *line_req;
    struct gpiod_edge_event_buffer *buffer;
    struct gpiod_edge_event *event;

    unsigned int led_offset[1] = {27};
    unsigned int btn_offset[1] = {17};

    // ABERTURA DO CHIP
    chip = gpiod_chip_open("/dev/gpiochip0");
    if(chip == NULL){
        perror("FUNÇÃO gpiod_chip_open RETURN NULL");
        return 1;
    }
    printf("CHIP OK\n");

    // REQUEST CONFIG
    req_cfg = gpiod_request_config_new();
    if(req_cfg == NULL){
        perror("FUNÇÃO gpiod_request_config_new RETURN NULL");
        gpiod_chip_close(chip);
        return 1;
    }
    gpiod_request_config_set_consumer(req_cfg, "led_event");
    printf("REQUEST CONFIG OK\n");


    // LINE SETTINGS
    //LED
    led_line_stgs = gpiod_line_settings_new();
    if(led_line_stgs == NULL){
        perror("(LED) FUNÇÃO gpiod_line_settings_new RETURN NULL");
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
    }
    gpiod_line_settings_set_direction(led_line_stgs, GPIOD_LINE_DIRECTION_OUTPUT);
    printf("LED LINE SETTINGS OK\n");


    //BUTTON
    btn_line_stgs = gpiod_line_settings_new();
    if(btn_line_stgs == NULL){
        perror("(BUTTON) FUNÇÃO gpiod_line_settings_new RETURN NULL");
        gpiod_line_settings_free(led_line_stgs);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
    }
    gpiod_line_settings_set_direction(btn_line_stgs, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(btn_line_stgs, GPIOD_LINE_BIAS_PULL_UP);
    gpiod_line_settings_set_edge_detection(btn_line_stgs, GPIOD_LINE_EDGE_FALLING);
    printf("BUTTON LINE SETTINGS OK\n");

    // LINE CONFIG
    line_cfg = gpiod_line_config_new();
    if(line_cfg == NULL){
        perror("FUNÇÃO gpiod_line_config_new RETURN NULL");
        gpiod_line_settings_free(btn_line_stgs);
        gpiod_line_settings_free(led_line_stgs);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
    }
    gpiod_line_config_add_line_settings(line_cfg, led_offset, 1, led_line_stgs);
    gpiod_line_config_add_line_settings(line_cfg, btn_offset, 1, btn_line_stgs);
    printf("LINE CONFIG OK\n");

    // LINE REQUEST
    line_req = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if(line_req == NULL){
        perror("FUNÇÃO gpiod_chip_request_lines RETURN NULL");
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(btn_line_stgs);
        gpiod_line_settings_free(led_line_stgs);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
    }
    printf("LINE REQUEST OK\n");


    // EDGE EVENT BUFFER
    buffer = gpiod_edge_event_buffer_new(10);
    if(buffer == NULL){
        perror("FUNÇÃO gpiod_edge_event_buffer_new RETURN NULL");
        gpiod_line_request_release(line_req);
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(btn_line_stgs);
        gpiod_line_settings_free(led_line_stgs);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
    }
    printf("EDGE EVENT BUFFER OK\n");

    printf("LOOP\n");

    unsigned int aux = 0;
    unsigned int led_status = 0;

    while(running){
        printf("Esperando ocorrer um evento ...");        
        // Espera ocorrer um evento
        aux = gpiod_line_request_wait_edge_events(line_req, -1);
        if(aux < 0){
            perror("FUNÇÃO gpiod_line_request_wait_edge_events RETURN %d", aux);
            gpiod_edge_event_buffer_free(buffer);
            gpiod_line_request_release(line_req);
            gpiod_line_config_free(line_cfg);
            gpiod_line_settings_free(btn_line_stgs);
            gpiod_line_settings_free(led_line_stgs);
            gpiod_request_config_free(req_cfg);
            gpiod_chip_close(chip);
        }
        printf("Evento detectado\n");

        // Transfere o evento para kernel para o buffer
        aux = gpiod_line_request_read_edge_events(line_req, buffer, 10);
        if(aux < 0){
            perror("FUNÇÃO gpiod_line_request_read_edge_events RETURN %d", aux);
            gpiod_edge_event_buffer_free(buffer);
            gpiod_line_request_release(line_req);
            gpiod_line_config_free(line_cfg);
            gpiod_line_settings_free(btn_line_stgs);
            gpiod_line_settings_free(led_line_stgs);
            gpiod_request_config_free(req_cfg);
            gpiod_chip_close(chip);
        }
        printf("%d eventos lidos\n", aux);

        // Pega o evento do buffer
        event = gpiod_edge_event_buffer_get_event(buffer, 0);
        enum gpiod_edge_event_type tipo = gpiod_edge_event_get_type(event);
        unsigned int offset = gpiod_edge_event_get_line_offset(event);

        if(tipo == GPIOD_EDGE_FALLING_EDGE && offset == btn_offset[0]){
            if(led_status){
                gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_INACTIVE);
                led_status = 0;
            }
            else{
                gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_ACTIVE);
                led_status = 1;
            }
        }




    }

    // FREE
    gpiod_edge_event_buffer_free(buffer);
    gpiod_line_request_release(line_req);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(btn_line_stgs);
    gpiod_line_settings_free(led_line_stgs);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);

    return 0;
}
