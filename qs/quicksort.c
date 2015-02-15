#include "my_headers.h"
#define SIZE 8

#define min(x,y) ((x) < (y) ? (x) :(y))
#
void swap2 (float* x, float* y) {
	float temp; 
	temp = *x; 
	*x = *y ; 
	*y = temp;
}

int choosePivot(float* numbers, int leftIndex, int rightIndex) {
	return leftIndex;
}

float parition2(float* numbers, int leftIndex, int rightIndex,size_t nnumbers) {

	int leftIndexEnd = rightIndex-1; 
	int rightIndexEnd = nnumbers;

	int pivotIndex = choosePivot(numbers, leftIndex, rightIndex); 
	float pivotValue = numbers[pivotIndex];

	while (leftIndex <= rightIndex) {

	 while (numbers[leftIndex] < pivotValue) 
		leftIndex++; 

	 while (numbers[rightIndex] > pivotValue) 
		rightIndex++; 

	 if (leftIndex <= rightIndex) {
		swap(numbers+leftIndex,numbers+rightIndex); 
		leftIndex++; 
		rightIndex--;
	}
	}
	return leftIndex; 
}

int partition2( float* a, int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;

   while(1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }
   t = a[l];
   a[l] = a[j]; a[j] = t;
   return j;
}

void quicksort2(float* numbers, int leftIndex, int rightIndex) {

	if ( leftIndex >= rightIndex )
		return ;

	int pivotIndex = partition(numbers, leftIndex,rightIndex);

	if (leftIndex < pivotIndex)
		quicksort(numbers,leftIndex,pivotIndex-1);
	 if (pivotIndex + 1 < rightIndex)
		quicksort(numbers,pivotIndex+1,rightIndex);

}



int main6() {

	int i = 0; 
	float numbers[] = {11,123,45,167,89,2,8,1};
	float nnumbers = SIZE; 
	quicksort(numbers,0,nnumbers-1);
	for (i=0; i < nnumbers; i++)
		printf ("%f",numbers[i]);
	printf("\n\nSorted array is:  ");
		for(i = 0; i < nnumbers; ++i)
			printf(" %f ", numbers[i]);
	return 0;

}

