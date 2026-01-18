#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>

int main(){
    printf("=== TESTE DE CONEXÃO DO TECLADO ===\n\n");

    struct gpiod_chip *chip;
    struct gpiod_line_request *request;
    struct gpiod_request_config *r_config;
    struct gpiod_line_settings *settings;
    struct gpiod_line_config *l_config;

    // Todos os 7 pinos que você está usando
    unsigned int all_pins[7] = {2, 3, 4, 17, 10, 9, 11};
    const char *pin_names[7] = {
        "GPIO 2  (pino 3) ",
        "GPIO 3  (pino 5) ",
        "GPIO 4  (pino 7) ",
        "GPIO 17 (pino 11)",
        "GPIO 10 (pino 19)",
        "GPIO 9  (pino 21)",
        "GPIO 11 (pino 23)"
    };

    chip = gpiod_chip_open("/dev/gpiochip0");
    if(!chip){
        perror("gpiod_chip_open");
        return 1;
    }

    r_config = gpiod_request_config_new();
    gpiod_request_config_set_consumer(r_config, "keypad_test");

    // Configurar todos como INPUT com PULL-UP
    settings = gpiod_line_settings_new();
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT);
    gpiod_line_settings_set_bias(settings, GPIOD_LINE_BIAS_PULL_UP);

    l_config = gpiod_line_config_new();
    gpiod_line_config_add_line_settings(l_config, all_pins, 7, settings);

    request = gpiod_chip_request_lines(chip, r_config, l_config);
    if(!request){
        perror("gpiod_chip_request_lines");
        gpiod_line_config_free(l_config);
        gpiod_line_settings_free(settings);
        gpiod_request_config_free(r_config);
        gpiod_chip_close(chip);
        return 1;
    }

    printf("Todos os pinos configurados como INPUT com PULL-UP.\n");
    printf("Pinos devem estar em HIGH (1).\n");
    printf("Quando você PRESSIONAR uma tecla, os pinos conectados vão para LOW (0).\n\n");

    printf("Pressione Ctrl+C para sair.\n");
    printf("═══════════════════════════════════════════════════════════\n\n");

    while(1){
        printf("\rEstado dos pinos: ");
        
        for(int i = 0; i < 7; i++){
            enum gpiod_line_value val = gpiod_line_request_get_value(request, all_pins[i]);
            
            if(val == GPIOD_LINE_VALUE_ACTIVE){
                printf("%d ", 1);  // HIGH
            } else {
                printf("\033[1;31m0\033[0m ");  // LOW em vermelho
            }
        }
        
        fflush(stdout);
        usleep(50000);  // 50ms
    }

    gpiod_line_request_release(request);
    gpiod_line_config_free(l_config);
    gpiod_line_settings_free(settings);
    gpiod_request_config_free(r_config);
    gpiod_chip_close(chip);
    
    return 0;
}
