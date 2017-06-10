#include <xc.h>
#include "wordsmith.h"
#include "ILI9163C.h"

void write_letter(char x, char y, char letter) {
    int target = letter - 0x20;
    int i = 0;
    int j = 0;
    unsigned short color; 
    for(i;i<=4;i++){
        int line = ASCII[target][i];
        j = 0;
        for(j;j<=7;j++){
            int dot = ((line>>j) & 0x01);
            if(dot == 1) {
                color = YELLOW;
            }
            else {
                color = CYAN;
            }
            LCD_drawPixel(x+i,y+j,color);
        }
    }
    
}

void make_line(char x, char y, unsigned short color) {
    int j = 0;
    for(j;j<=7;j++) {
        LCD_drawPixel(x,y+j,color);
    }
}

void write_word(char x, char y, char* phrase) {
    int i = 0;
    while(phrase[i]) {
        write_letter(x,y,phrase[i]);
        x = x+5;
        make_line(x,y,CYAN);
        x=x+1;
        i=i+1;
        
    }
}