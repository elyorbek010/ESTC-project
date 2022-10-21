#include <stdio.h>
#include <stdbool.h>
#include "add_vec.h"
#include "sub_vec.h"
#include "dot_vec.h"
#include "cross_vec.h"

int main(){
	int n = 3, i;
        int x[n], y[n], z[n];

	printf("Enter vector X: ");
	for(i = 0; i < n; i++);
		scanf("%d", &x[i]);
	
	printf("Enter vector Y: ");
	for(i = 0; i < n; i++)
		scanf("%d", &y[i]);
	
	add_vec(x, y, z, n);
	sub_vec(x, y, z, n);
	dot_vec(x, y, z, n);
	cross_vec(x, y, z, n);

	return 0;
}
