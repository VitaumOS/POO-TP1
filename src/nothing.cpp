
#include <iostream>


static int
UI(void) 
{
    std::cout << "There we'll go...\n(Enter anything to exit)\n";

    char buffer;
    fflush(stdin); scanf("%c", & buffer);
    return 0;
}

int main(int argc, char ** argsv)
{
    /*  State initialization */
    std::cout << "one of the greatest initialization this world has ever seen...\n\n";
    
    /*  UI initialization stuff
        (...) */

    /*  returning direct from UI call... */
    return UI();
}
