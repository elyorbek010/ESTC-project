#include <stdio.h>

void add_vec(int *x, int*y, int *z, int n){
	printf("\nAfter addition:\n");
	int i;
	for(i = 0; i < n; i++){
		*(z+i) = *(x+i) + *(y+i);
		printf("z[%d] = %d\n", i, *(z+i));
	}
	return;
}
