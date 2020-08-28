#include <openacc.h>
#include <stdio.h>

int main(){
    if (_OPENACC == 201111){
		printf("1.0");
	}
	else if (_OPENACC == 201306){
		printf("2.0");
	}
	else if (_OPENACC == 201510){
		printf("2.5");
	}
	else if (_OPENACC == 201711){
		printf("2.6");
	}
	else if (_OPENACC == 201811){
		printf("2.7");
	}
	else if (_OPENACC == 201911){
		printf("3.0");
	}
	else{
		return 1;
	}
    return 0;
}
