#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL_image.h>
#include <time.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>



void convertIntToCharWithSpaces(int intArray[], char charArray[], int length, int spacesBetweenChars) {
    int charIndex = 0;

    for (int i = 0; i < length; i++) {
        charArray[charIndex] = intArray[i] + '0';
        charIndex++;

        if (i < length - 1) {
            for (int space = 0; space < spacesBetweenChars; space++) {
                charArray[charIndex] = ' ';
                charIndex++;
            }
        }
    }
    charArray[charIndex] = '\0';
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}



