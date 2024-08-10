/*	<src/UI.cpp>

	Where the UI screen is defined. */


#include <iostream>
#include "ui.hpp"


MenuScreen::MenuScreen(void) : 
	width(50), height(25), 
	bg((rgb) { 10, 10, 10 }),
	fg((rgb) { 175, 175, 175 }) {

}

MenuScreen::MenuScreen(int width, int height) : 
	width(width), height(height),
	bg((rgb) { 10, 10, 10 }),
	fg((rgb) { 175, 175, 175 }){

}

MenuScreen::MenuScreen(int width, int height, rgb bg, rgb fg) : 
	width(width), height(height), bg(bg), fg(fg) {
	
}

MenuScreen::~MenuScreen(void) {

}

/*	Graphical methods *
 *	----------------- */

void MenuScreen::fill_char(char c) const {
	for (int i = 0; i < height; i ++)
	{
		for (int j = 0; j < width; j ++)
			putchar(c);
		putchar('\n');
	}
}


/*	Standard Interaction Methods *
 *	---------------------------- */

void MenuScreen::clean_stdin(void) {
	std::cin.clear();
	std::cin.ignore(INT64_T_MAX, '\n');
}

bool MenuScreen::input_verification(void) {
	char c = '\0';

	std::cout << "[s/n] ";
	fflush(stdout);

	/*	keeps scanning until either [s] or [n] are encountered */
	while ((std::cin >> c) && (c != 's') && (c != 'n'));

	return c == 's';
}

void MenuScreen::press_anything_to_continue(void) {
	char c = '\0';

	std::cout << "[Entre com qualquer tecla para continuar...] ";
	fflush(stdout);

	std::cin >> c;
	
	MenuScreen::clean_stdin();
}

