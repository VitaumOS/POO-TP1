/*	<src/UI.cpp>

	Where the UI components are defined. */


#include <iostream>
#include "headers/UI.h"


Screen::Screen(void) : 
	width(100), height(100), 
	bg((rgb) { 10, 10, 10 }),
	fg((rgb) { 10, 10, 10 })
{

}

Screen::Screen(int width, int height, rgb bg, rgb fg) : 
	width(width), height(height),
	bg(bg), fg(fg)
{
	
	std::cout << "[Screen constructor]\n";
}

void Screen::fill_char(char c)
{
	for (int i = 0; i < height; i ++)
	{
		for (int j = 0; j < width; j ++)
			putchar(c);
		putchar('\n');
	}
}
