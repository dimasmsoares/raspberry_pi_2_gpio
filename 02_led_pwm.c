/*DESAFIO 2 — LED com brilho controlado por PWM*/
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t running = 1;  // Ver exlicação ao final sobre o SIGINT

void handle_sigint(int sig)
{
    (void)sig;   // evita warning
    running = 0;
}

int main(){

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
    struct timespec ts_on, ts_off;
    ts_on.tv_sec = 0;
    ts_off.tv_sec = 0;
    unsigned int period_ns = 10000000;  // 10ms

    int count_status = 0;

    int aux = 1;
    int btn_prev = 1; 
    int btn_now = 1;

    while(running){ 
        // Ler o valor do botão
        btn_now = gpiod_line_request_get_value(line_req, btn_offset[0]);
        if(btn_prev == 1 && btn_now == 0){   // Botão pressionado
            if(count_status<4) count_status++;
            else count_status = 0;
        }

        // PWM
        ts_on.tv_nsec = count_status*2500000;
        ts_off.tv_nsec = period_ns - ts_on.tv_nsec;
        if(ts_on.tv_nsec > 0){
            gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_ACTIVE);
            nanosleep(&ts_on, NULL);
        }
        gpiod_line_request_set_value(line_req, led_offset[0], GPIOD_LINE_VALUE_INACTIVE);
        nanosleep(&ts_off, NULL);

        btn_prev = btn_now;
    }


    /*FREE*/
    gpiod_line_request_release(line_req);
    gpiod_line_config_free(line_cfg);
    gpiod_line_settings_free(btn_line_sets);
    gpiod_line_settings_free(led_line_sets);
    gpiod_request_config_free(req_cfg);
    gpiod_chip_close(chip);
    return 0;
}

/* 
SIGINT --> Signal Interrupt
    É um sinal (signal) enviado pelo kernel para o processo em execução para interrompe-lo de forma controlada.

    Para usa-lo se registra um handler. Algo como dizer: “Quando chegar um SIGINT, execute esta função.”
    A função a ser executada muda o estado da flag (sinal).

    SIGINT é o sinal que o Linux envia ao seu programa quando você pressiona Ctrl+C, permitindo que ele encerre de forma limpa.

*/

/*
The nanosleep() function is a POSIX standard system call used in C/C++ programming to suspend the execution of the calling thread with nanosecond precision. 
Synopsis
To use nanosleep(), you must include the <time.h> header file. 
c
#include <time.h>
int nanosleep(const struct timespec *req, struct timespec *rem);
req: A pointer to a struct timespec that specifies the duration of the sleep interval.
rem: A pointer to a struct timespec where the remaining unslept time will be stored if the function is interrupted by a signal. If you don't need this information, you can pass NULL. 
The struct timespec is defined as:
c
struct timespec {
    time_t tv_sec;  //seconds
    long   tv_nsec; // nanoseconds (0 to 999999999)
};
The nanosecond value (tv_nsec) must be in the range of 0 to 999,999,999. 
Description
The function suspends the calling thread until either: 
The time interval specified by req has elapsed.
A signal is delivered to the thread, and its action is to invoke a signal-catching function or terminate the process. 
Compared to sleep() and usleep(), nanosleep() offers higher resolution and is explicitly specified by POSIX.1 to not interact with signals, making it easier to resume a sleep after an interruption. 
Return Value
0: The function returns 0 if the requested time has elapsed successfully.
-1: The function returns -1 if it is interrupted by a signal or encounters an error, and the global variable errno is set accordingly. 
Errors
Common errors include:
EINTR: The function was interrupted by a signal.
EINVAL: The tv_nsec field in req was outside the valid range, or tv_sec was negative. 
*/