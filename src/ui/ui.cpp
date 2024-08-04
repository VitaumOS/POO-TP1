/*	<src/UI.cpp>

	Where the UI components are defined. */


#include <iostream>
#include "ui.hpp"


Screen::Screen(void) : 
	width(100), height(100), 
	bg((rgb) { 10, 10, 10 }),
	fg((rgb) { 175, 175, 175 })
{

}

Screen::Screen(int width, int height, rgb bg, rgb fg) : 
	width(width), height(height), bg(bg), fg(fg)
{
	
}

Screen::~Screen(void)
{

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


#define in_range(a, b, x)   (((x) >= (a)) && ((b) >= (x)))

static inline int64_t to_numeral(const std::string _StringBuffer) {
	size_t iterator = 0;
	int64_t sum = 0;

	const char * cstring_buffer = _StringBuffer.c_str();
	while (cstring_buffer[iterator] && (in_range(48, 57, cstring_buffer[iterator])))
	{
		sum *= 10LL;
		sum += (cstring_buffer[iterator ++] - 48);
	}

	if (cstring_buffer[iterator])
		return -1;

	return sum;
}

bool input_verification(void)
{
	std::cout << "[s/n] ";
	char c = 0;
	while ((std::cin >> c) && (c != 's') && (c != 'n'));
	return c == 's';
}

int64_t input_numeral(void) {
	std::string string_buffer;
	std::cin >> string_buffer;
	return to_numeral(string_buffer);
}

