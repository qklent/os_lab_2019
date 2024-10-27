#include "revert_string.h"
#include <string.h>
#include <stdio.h>

void RevertString(char *str)
{
	// two pointers on start and end, go untill len // 2 and change left and right values
	int l = 0;
	int r = strlen(str) - 1;
	int mid_pointer = r / 2;

    while (l < r) { 
        // Swap characters
        char tmp = str[l];
        str[l] = str[r];
        str[r] = tmp;

        // Move pointers
        l++;
        r--;
    }
}

