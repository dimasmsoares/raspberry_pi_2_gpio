/*DESAFIO 1 — LED controlado por botão (Polling)*/
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t running = 1;  // Ver exlicação ao final sobre o SIGINT

void handle_sigint(int sig)
{
    (void)sig;   // evita warning
    running = 0;
}

int main(){

    int led_state = 0;
    int aux;

    signal(SIGINT, handle_sigint);

    /*STRUCT*/
    struct gpiod_chip *chip;
    struct gpiod_request_config *req_cfg;
    struct gpiod_line_settings *led_line_sets;
    struct gpiod_line_settings *btn_line_sets;
    struct gpiod_line_config *line_cfg;
    struct gpiod_line_request *line_req;

    /*OFFSET*/
    const unsigned int led_offset[1] = {27};
    const unsigned int btn_offset[1] = {17};

    /*CHIP*/
    chip = gpiod_chip_open("/dev/gpiochip0");
    if(chip == NULL){
        perror("FUNCTION 'gpiod_chip_open' RETURN NULL");
        return 1;
    }
    printf("CHIP ✅\n");
    
    /*REQUEST CONFIG*/
    req_cfg = gpiod_request_config_new();
    if(req_cfg == NULL){
        perror("FUNCTION 'gpiod_request_config_new' RETURN NULL");
        gpiod_chip_close(chip);    
        return 1;
    }
    gpiod_request_config_set_consumer(req_cfg, "led_ctrl_btn");
    printf("REQUEST CONFIG ✅\n");

    /*LED -> LINE SETTINGS*/
    led_line_sets = gpiod_line_settings_new();
    if(led_line_sets == NULL){
        perror("FUNCTION 'gpiod_line_settings_new' RETURN NULL (LED)");
        gpiod_request_config_free(req_cfg);    
        gpiod_chip_close(chip);    
        return 1;
    }
    gpiod_line_settings_set_direction(led_line_sets, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(led_line_sets, GPIOD_LINE_VALUE_INACTIVE);
    printf("LED -> LINE SETTINGS ✅\n");

    /*BTN -> LINE SETTINGS*/
    btn_line_sets = gpiod_line_settings_new();
    if(btn_line_sets == NULL){
        perror("FUNCTION 'gpiod_line_settings_new' RETURN NULL (BTN)");
        gpiod_line_settings_free(led_line_sets);
        gpiod_request_config_free(req_cfg);    
        gpiod_chip_close(chip);    
        return 1;
    }
    gpiod_line_settings_set_direction(btn_line_sets, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(btn_line_sets, GPIOD_LINE_BIAS_PULL_UP);
    printf("BTN -> LINE SETTINGS ✅\n");

    /*LINE CONFIG*/
    line_cfg = gpiod_line_config_new();
    if(line_cfg == NULL){
        perror("FUNCTION 'gpiod_line_config_new' RETURN NULL");
        gpiod_line_settings_free(btn_line_sets);    
        gpiod_line_settings_free(led_line_sets);
        gpiod_request_config_free(req_cfg);    
        gpiod_chip_close(chip);    
        return 1;
    }
    gpiod_line_config_add_line_settings(line_cfg, led_offset, 1, led_line_sets);
    gpiod_line_config_add_line_settings(line_cfg, btn_offset, 1, btn_line_sets);
    printf("LINE CONFIG ✅\n");

    /*LINE REQUEST*/
    line_req = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if(line_req == NULL){
        perror("FUNCTION 'gpiod_chip_request_lines' RETURN NULL");
        gpiod_line_config_free(line_cfg);
        gpiod_line_settings_free(btn_line_sets);
        gpiod_line_settings_free(led_line_sets);
        gpiod_request_config_free(req_cfg);
        gpiod_chip_close(chip);
        return 1;
    }
    printf("LINE REQUEST ✅\n");

    printf("WHILE 🔄\n");
    printf("\tPressione o botão para ligar o LED\n\tCtrl+C para encerrar...\n");
    while(running){
        // Inicialmente o led começa desligado (0) e o botão em 1 (pull up).
        aux = gpiod_line_request_get_value(line_req, btn_offset[0]);
        if(aux < 0){
            perror("FUNCTION 'gpiod_line_request_get_value' RETURN -1");
            gpiod_line_request_release(line_req);
            gpiod_line_config_free(line_cfg);
            gpiod_line_settings_free(btn_line_sets);
            gpiod_line_settings_free(led_line_sets);
            gpiod_request_config_free(req_cfg);
            gpiod_chip_close(chip);
            return 1;
        }
        else if(aux == 0){
            if(led_state == 0) gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_ACTIVE);
            led_state = 1;
        }
        else{
            if(led_state == 1) gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_INACTIVE);
            led_state = 0;
        }
        usleep(10000);  // 10ms
    }
    
    /*FREE*/
    gpiod_line_request_release(line_req);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(btn_line_sets);
    gpiod_line_settings_free(led_line_sets);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);
    printf("END\n");
    return 0;
}

/* 
SIGINT --> Signal Interrupt
    É um sinal (signal) enviado pelo kernel para o processo em execução para interrompe-lo de forma controlada.

    Para usa-lo se registra um handler. Algo como dizer: “Quando chegar um SIGINT, execute esta função.”
    A função a ser executada muda o estado da flag (sinal).

    SIGINT é o sinal que o Linux envia ao seu programa quando você pressiona Ctrl+C, permitindo que ele encerre de forma limpa.

*/
