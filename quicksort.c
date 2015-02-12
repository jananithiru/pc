#include "myHeaders.h"
#define SIZE; 
int main() {

	int numbers = {11,123,45,167,89,2,8,1};
	int nnumbers = SIZE; 
	partition(numbers,0,nnumbers-1);
}

int partition (int* numbers, int left, int right) {
	int pivot = getPivotElement();
	if (left < right ){
		partition(numbers,left,pivot); 
		partition(numbers,pivot+1,right);
		sort(numbers, left, pivot, right);
	}	
}

int sort ( int* numbers, int left, int pi

