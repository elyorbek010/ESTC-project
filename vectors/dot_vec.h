#include <stdio.h>

void dot_vec(int *x, int *y, int *z, int n){
	printf("\nAfter dot product:\n");
	int i;
	for(i = 0; i < n; i++){
		*(z+i) = (*(x+i)) * (*(y+i));
		printf("z[%d] = %d\n", i, *(z+i));
	}
	return;
}
