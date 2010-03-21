#include <stdio.h>
#include <stdlib.h>

main()
{
	//int a[2000][2000];

	int sz = 20000*20000;

	int *a = (int*) malloc(sizeof(int)*sz);

	printf("%d\n",*(a+4));
	printf("%d\n",a[4]);
	printf("%d\n",a[0][0]);

	printf("here\n");

	//a[0][1] = 2;

	//printf("%d",a[0][1]);
}
