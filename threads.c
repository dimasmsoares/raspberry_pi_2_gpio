#include <stdio.h>
#include <gpiod.h>
#include <unistd.h>
#include <pthread.h>

void *thread_function(void *arg);

int main(int argc, char *argv[]){
    printf("INICIO\n");

    int aux;

    pthread_t thread_id;

    aux = pthread_create(&thread_id, NULL, thread_function, NULL);
    if(aux != 0){
        fprintf(stderr, "pthread_create failed: %d\n", aux);
        return 1;
    }

    for(int i = 0; i < 10; i++){
        printf("%d\n", i);
        usleep(500000);
    }

    pthread_join(thread_id, NULL);

    return 0;
}

void *thread_function(void *arg){
    for(char c = 'A'; c <= 'Z': c++){
        printf("%c\n", c);
        usleep(500000);
    }
    return NULL;
}