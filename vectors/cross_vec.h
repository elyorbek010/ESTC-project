#include <stdio.h>

void cross_vec(int *x, int *y, int *z, int n){
	printf("\nAfter cross product:\n");
	z[0] = x[1] * y[2] - x[2] * y[1];
	z[1] = x[2] * y[0] - x[0] * y[2];
	z[2] = x[0] * y[1] - x[1] * y[0];
	
	int i;
	for(i = 0; i < n; i++)
		printf("z[%d] = %d\n", i, *(z+i));
	return;
}
