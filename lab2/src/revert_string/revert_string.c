#include "revert_string.h"
#include <string.h>
#include <stdio.h>

void RevertString(char *str)
{
	int l = 0;
	int r = strlen(str) - 1;
	int mid_pointer = r / 2;

    while (l < r) { 
        char tmp = str[l];
        str[l] = str[r];
        str[r] = tmp;
        l++;
        r--;
        // printf("It's old thing\n\n");
    }
}

