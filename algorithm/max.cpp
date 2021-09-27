#include <assert.h>
#include <stdio.h>

int maxElement(const int* array, size_t size) {
	assert(array != NULL);
	assert(size >= 1);
	
	int max = aaray[0];
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

int main(void) {
	const int array[]


}