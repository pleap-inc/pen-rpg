#include <assert.h>
#include <stdio.h>

#define SIZE_OF_ARRAY(array) (sizeof(array)/sizeof(array[0]))

int maxElement(const int* array, size_t size) {
	assert(array != NULL);
	assert(size >= 1);
	
	int max = array[0];
	for(size_t i=1; i<size; ++i) {
		if(max < array[i]) {
			max = array[i];	
		}
	}

	return max;
}

int minElement(const int* array, size_t size) {
	assert(array != NULL);
	assert(size >= 1);
	
	int min = array[0];
	for(size_t i=1; i<size; ++i) {
		if(min > array[i]) {
			min = array[i];	
	}
	return min;
	}
}

int main(void) {
	const int array[] = {9, -4, 6, -2, 10, 7, -1};
	
	printf("max: %d\n", maxElement(array, SIZE_OF_ARRAY(array)));
	printf("max: %d\n", minElement(array, SIZE_OF_ARRAY(array)));
	
	return 0;
}
