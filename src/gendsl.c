/*
 * gendsl.c
 *
 *  Created on: Mar 17, 2012
 *      Author: shlomi
 */

#include  "gendsl.h"



void deferLabelAddrResolution(list* l, int* into, char* label) {
	node* n = find(*l, findLabelText, label);
	if (n==NULL) {
		printf("label %s not defined!", label);
	} else {
		*into = LABEL(n)->offset;
	}
}

void deferLabelDistanceResolution(list* l, int* into, char* label) {
	*into = 5;
}
