#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    	printf("Running %s\n", argv[0]);
	
	/*STRUCTS*/
	struct gpiod_chip *chip;
	struct gpiod_chip_info *chip_info;

	chip = gpiod_chip_open("/dev/gpiochip0");
	if(chip == NULL){
		perror("gpiod_chip_open return NULL");
		return 1;
	}
	printf("chip is open\n");

	chip_info = gpiod_chip_get_info(chip);
	if(chip_info == NULL){
		perror("gpiod_chip_get_info");
		gpiod_chip_close(chip);
		return 1;
	}
	printf("chip_info\n");

	printf("\tName: %s\n", gpiod_chip_info_get_name(chip_info));
	printf("\tLabel: %s\n", gpiod_chip_info_get_label(chip_info));
	printf("\tLines: %u\n", gpiod_chip_info_get_num_lines(chip_info));
	
	gpiod_chip_info_free(chip_info);
	printf("chip_info is free\n");

	gpiod_chip_close(chip);
	printf("chip is close\n");
	return 0;
}
