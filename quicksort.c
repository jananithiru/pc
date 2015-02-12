#include "myHeaders.h"
#define SIZE 8

#define min(x,y) ((x) < (y) ? (x) :(y))
#
void swap (float* x, float* y) {
	float temp; 
	temp = *x; 
	*x = *y ; 
	*y = temp;
}

int choosePivot(float* numbers, int leftIndex, int rightIndex) {
	return leftIndex;
}

float parition(float* numbers, int leftIndex, int rightIndex,size_t nnumbers) {

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

int quicksort (float* numbers, int leftIndex, int rightIndex) {
	if ( leftIndex >= rightIndex ) 
		return; 

	if (leftIndex < rightIndex){
		int pivotIndex = parition(numbers, leftIndex,rightIndex,rightIndex+1);
		quicksort(numbers,leftIndex,pivotIndex-1); // -1 
		quicksort(numbers,pivotIndex+1,rightIndex); // +1 leave one element space for index 
	}	
}


int main() {

	int i = 0; 
	float numbers[] = {11,123,45,167,89,2,8,1};
	float nnumbers = SIZE; 
	quicksort(numbers,0,nnumbers-1);
	for (i=0; i < nnumbers; i++)
		printf ("%f",numbers[i]);
}

