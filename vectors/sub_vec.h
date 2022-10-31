#include <stdio.h>

void sub_vec(int *x, int *y, int *z, int n){
	printf("\nAfter subtraction:\n");
	for(int i = 0; i < n; i++){
		*(z+i) = *(x+i) - *(y+i);
		printf("z[%d] = %d\n", i, *(z+i));
	}
	return;
}
