/*	<src/headers/UI.h>

	Where the UI screen is declared. */
 

#ifndef _UI_HEADER_
#define _UI_HEADER_

/*	toggling ANSI-ESC for graphical handling... */
#define ANSI_ESCAPE_CODE
#include "ansi-esc.h"

#include "commons.h"

/*	mixed C/C++ IO handling */
#include <stdio.h>
#include <iostream>

#include <string.h>	// for strcpy


// Default screen values

#if ! defined(SWIDTH)
#   define SWIDTH 100
#endif

#if ! defined(SHEIGHT)
#   define SHEIGHT  30
#endif


// Cleaning screen
// ===============

#if ! defined(CLEAN_SCREEN)
	/*	Defines whether the clean_screen macro will try to clean the stdout output screen.
		The functionality is toggled on by default. */
	#define CLEAN_SCREEN	true
#endif // defined(CLEAN_SCREEN)


#if CLEAN_SCREEN
	#define clean_screen	aec_clean
#else
	#define clean_screen
#endif


// Coloring
// ========

typedef struct rgb { uint8_t r, g, b; } rgb;


// Positioning
// ===========

typedef enum {
	TOP_LEFT,
	MID_LEFT,
	BOT_LEFT,

	TOP_CENTER,
	CENTER,
	BOT_CENTER,

	TOP_RIGHT,
	MID_RIGHT,
	BOT_RIGHT,
} SCREEN_POSITIONING;


#define print_c_string(_String)	(fputs(_String, stdout) > 0)


/*	Represents a UI menu screen. 
	It's looping is given by its virtual method <interact>. */
class MenuScreen {
protected:
	/*	Graphical attributes and methods
		(not yet used) */

	int width, height;
	rgb bg, fg;
	
	void set_bg(void)	const { aec_bg_rgb(bg.r, bg.g, bg.b); }
	void set_fg(void)	const { aec_fg_rgb(fg.r, fg.g, fg.b); }

	void fill_char(char c) const;
	
	void to_beggining(void)	const { 
		aec_beginning(); 
	}

	void to_ending(void) const { 
		aec_beginning(); 
		aec_crs_down(height); 
	}

	// Moves the cursor to the absolute position (x, y) on the screen.
	bool go_to_abs(int x, int y) const {
		if ((x < 0) || (y < 0) || (x > width) || (y > height))
			return false;

		aec_beginning();
		
		if (y > 0)	aec_crs_down(y);
		if (x > 0)	aec_crs_right(x);

		return (x >= 0) && (y >= 0);
	}

	// Moves the cursor to the relative screen pre-defined positions.
	bool go_to_rel(SCREEN_POSITIONING pos) const {
		if ((pos > 8) || (pos < 0))
			return false;
		
		aec_beginning();
		
		const int x_index = pos / 3;
		const int y_index = pos % 3;
		
		const int y = (y_index == 0) ? 0 : (
			(y_index == 1) ? (height >> 1) : height
		);

		const int x = (x_index == 0) ? 0 : (
			(x_index == 1) ? (width >> 1) : width
			);
		
		return go_to_abs(x, y);
	}

	bool pos_char(int x, int y, char c) const {
		if (go_to_abs(x, y)) {
			putchar(c);
			return true;
		}
		return false;
	}

	bool pos_string(int x, int y, const char * _String) const {
		return go_to_abs(x, y) 
			&& print_c_string(_String);
	}

	bool centralize_string_at(int x, int y, const char * _String) const
	{
		const size_t string_length = strlen(_String);

		return go_to_abs(x - (string_length >> 1), y)
			&& print_c_string(_String);
	}

	/*	Standard interaction methods */

	/*	Cleans the stdin buffer. 
		Internally, calls std::cin::clean, and std::cin::ignore
		with a reasonably large limit with '\n'. */
	void clean_stdin(void);

	/*	Interacts with the user, asking whether to proceed; returns that response.
		Filters to only let pass if either the keys [s] or [n] are entered. */
	bool input_verification(void);

	/*	Interacts with the user, asking any key to proceed. 
		A single character is consumed, as an effect. */
	void press_anything_to_continue(void);

	/*	Virtual inner methods */

	/*	Renders the screen. 
		By standard, the return-code is 0 in case of success.
		Shall be overwritten hierarchically. */
	virtual int render(void) const = 0;

	/*	Processes the input on stdout. 
		By standard, the return-code is 0 in case of success.
		Shall be overwritten hierarchically. */
	virtual int process(void) = 0;

public:
	MenuScreen(void);
	MenuScreen(int width, int height);
	MenuScreen(int width, int height, rgb bg, rgb fg);
	virtual ~MenuScreen(void);

	/*	Interacts with the screen. Represents the driver call for menu loop.
		By standard, the return-code is 0 in case of success.
		Shall be overwritten hierarchically. */
	virtual int interact(void) = 0;
};


/*	Standard UI and graphical tools 
 *	
 
 
 ---------------- */

constexpr size_t literal_string_length(const char * _String)
{
	size_t iterator = 0;
	while (_String[iterator ++]);
	return iterator;
}

inline void print_n_char(char c, size_t n) {	while (n --) putchar(c); }

bool input_verification(void);
void press_anything_to_continue(void);

int64_t input_numeral(void);

#endif // _UI_HEADER_
