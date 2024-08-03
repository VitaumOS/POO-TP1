/*	<src/headers/UI.h>

	Where the UI components are defined. */
 

#ifndef _UI_HEADER_
#define _UI_HEADER_


#define ANSI_ESCAPE_CODE
#include "ansi-esc.h"
#include "commons.h"
#include <iostream>
#include <stdio.h>
#include <string.h>


// Cleaning screen
// ===============

#if ! defined(CLEAN_SCREEN)
	#define CLEAN_SCREEN	true
#endif // defined(CLEAN_SCREEN)


#if CLEAN_SCREEN
	#define clean_screen	aec_clean
#else
	#define clean_screen
#endif


// Coloring
// ========

typedef struct { uint8_t r, g, b; } rgb;


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


/*
class Screen {
protected:
	int width, height;
	rgb bg, fg;

	void set_bg(void)	const	{ aec_bg_rgb(bg.r, bg.g, bg.b); }
	void set_fg(void)	const	{ aec_fg_rgb(fg.r, fg.g, fg.b); }

	void fill_char(char c);
	
	void to_beggining(void)	const { 
		aec_beginning(); 
	}

	void to_ending(void) const { 
		aec_beginning(); 
		aec_crs_down(height); 
	}

public:
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
		
			TOP_LEFT,
			MID_LEFT,
			BOT_LEFT,

			TOP_CENTER,
			CENTER,
			BOT_CENTER,

			TOP_RIGHT,
			MID_RIGHT,
			BOT_RIGHT,

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

	public:
	Screen(void);
	Screen(int width, int height, rgb bg, rgb fg);
	virtual void render() {
		aec_clean();
		printf("\n\t\tBLANK SCREEN\n\n\n");
	};
	
	virtual int interact(void) {
		printf("vtmnc VS\n");
		return -1;
	};
};

class Label {
private:

public:
	int x, y;
	std::string label_str;

	Label(void)
	{
		x = 0;  y = 0;
		label_str = "";
	}

	Label(int x, int y, std::string str) {
		this->x = x;
		this->y = y;
		this->label_str = str;
	}

	bool render_at_screen(const Screen * _Screen)
	{
		return _Screen->go_to_abs(x, y) && print_c_string(label_str.c_str());
	}
};
*/


bool input_verification(void);
int64_t input_numeral(void);

#endif // _UI_HEADER_
