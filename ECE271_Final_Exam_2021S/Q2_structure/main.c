#include "stm32l476xx.h"

struct Student_T {
 char c1;
 int score;
 char c2;
} students[10];

extern int find_avg_score(struct Student_T *p, int num_of_students);

void init_structure(){
	uint32_t i;
	for (i = 0; i < 10; i++){
		students[i].score = i * 10;
	}
}

	
int main(void){

	volatile int avg;

	init_structure();
	
	avg = find_avg_score(&students[0], 10);

	while(1);
}

