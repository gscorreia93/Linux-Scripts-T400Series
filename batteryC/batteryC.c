#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MAX(a,b) (a > b ? a : b)

char** getRemainingTime(){

	FILE *f1 = fopen("/sys/class/power_supply/BAT0/power_now", "rb");
	FILE *f2 = fopen("/sys/class/power_supply/BAT1/power_now", "rb");

	int pNow0, pNow1;

	fscanf(f1, "%d", &pNow0);
	fscanf(f2, "%d", &pNow1);

	int totalpNow = pNow0 + pNow1;

	fclose(f1);
	fclose(f2);

	f1 = fopen("/sys/class/power_supply/BAT0/energy_now", "r");
	f2 = fopen("/sys/class/power_supply/BAT1/energy_now", "r");

	int bat0, bat1;

	fscanf(f1, "%d", &bat0);
	fscanf(f2, "%d", &bat1);

	int totalBat = bat0 + bat1;

	fclose(f1);
	fclose(f2);

	float remTime = totalBat / (float) totalpNow;	
	
	int hours = (int) remTime;
	int minutes = (remTime - hours) * 60; 

	struct tm *tm = (struct tm*) malloc(sizeof(struct tm));

	char *finalTime = (char *) malloc(6);

	tm->tm_min = MAX(minutes, 0);
	tm->tm_hour = MAX(hours, 0);

	strftime(finalTime, 6, "%H:%M", tm);

	free(tm);

	char **ret = (char **)malloc((sizeof (char*)) * 2);
	ret[0] = finalTime;
	ret[1] = (char *) malloc (8);
	sprintf(ret[1], "%d", totalpNow);

    	return ret;
} 

int getRemainingCapacity(){

	FILE *f1 = fopen("/sys/class/power_supply/BAT0/capacity", "r");
	FILE *f2 = fopen("/sys/class/power_supply/BAT1/capacity", "r");

	int bat0, bat1;

	fscanf(f1, "%d", &bat0);
	fscanf(f2, "%d", &bat1);

	fclose(f1);
	fclose(f2);

	return (bat0 + bat1) / 2.0;
}

int main(){

	FILE *f1 = fopen("/sys/class/power_supply/BAT0/status", "r");
	FILE *f2 = fopen("/sys/class/power_supply/BAT1/status", "r");

	char *stateBat0 = (char *) malloc(12);
	fgets(stateBat0, 12, f1);
	strtok(stateBat0, "\n");

	char *stateBat1 = (char *) malloc(12);
	fgets(stateBat1, 12, f2);
	strtok(stateBat1, "\n");

	fclose(f1);
	fclose(f2);

	if(strcmp(stateBat0, "Discharging") == 0 || strcmp(stateBat1, "Discharging") == 0){

		char **rems = getRemainingTime();
		int remCap = getRemainingCapacity();

		u_int64_t consumption = atoi(rems[1]) * (pow(10, -6));
		if(strcmp(rems[0], "00:00") == 0){
			free(rems[0]);
			rems[0] = strndup("%EST%", 5);
		}


		if(remCap <= 25){
			printf("<span font_desc='FontAwesome'>&#xf243;</span> %d%% %s %dW\n", remCap, rems[0], consumption);
		}else if(remCap > 25 && remCap < 50){
			printf("<span font_desc='FontAwesome'>&#xf242;</span> %d%% %s %dW\n", remCap, rems[0], consumption);
		}else if(remCap >= 50 && remCap < 75){
			printf("<span font_desc='FontAwesome'>&#xf241;</span> %d%% %s %dW\n", remCap, rems[0], consumption);
		}else{
			printf("<span font_desc='FontAwesome'>&#xf240;</span> %d%% %s %dW\n", remCap, rems[0], consumption);
		}
	
		free(rems[0]);
		free(rems[1]);
		free(rems);
	}else if (strcmp(stateBat0, "Charging") == 0 || strcmp(stateBat1, "Charging") == 0){
		int remCap = getRemainingCapacity();
		printf("<span font_desc='FontAwesome'>&#xf0e7;</span> %d%%\n", remCap);
	}else if (strcmp(stateBat0, "Full") == 0 || strcmp(stateBat1, "Full") == 0){
		printf("<span font_desc='FontAwesome'>&#xf1e6;</span>");
	}else{
		int remCap = getRemainingCapacity();
		printf("<span font_desc='FontAwesome'>&#xf362;</span> %d%%\n", remCap);
	}

	free(stateBat0);
	free(stateBat1);
	
	return 0;
}
