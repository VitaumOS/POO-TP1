/*  <src/workshop.cpp>

    The entry point for the system. */



#include <stdio.h>
#include <iostream>
#include <login.h>
#include <databases.h>


	


/*
	printf("%d:%d:%d|%d:%d:%d\n",
		(int) the_date.day,
		(int) the_date.month + 1,
		(int) the_date.year + 1900,
		(int) the_date.hour,
		(int) the_date.minutes,
		(int) the_date.seconds);


*/


//
/*  (Program specs.) */
int main(void) {
    /*  Poor login screen testing... */
#   define LOGIN_SCREEN_TEST   false

#   if LOGIN_SCREEN_TEST
    LoginScreen login_screen(50, 25, (rgb) { 43, 43, 43 }, (rgb) { 200, 200, 225 });
    login_screen.render();

    login_screen.go_to_abs((50 >> 3) + 12, (25 >> 1) - 2);
    login_screen.capture_username();
    login_screen.capture_password();
#   endif // LOGIN_SCREEN_TEST

    SO_Manager so;
	
	


    return 0;
}