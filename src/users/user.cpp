/*	<src/users/user.cpp>

	(...) */


#include "user.hpp"


UserScreen::UserScreen(class SO_Manager * const so_manager, class UsersDatabase * const users_db, 
	const struct MinimalUserData & user_data) :
	user_data(user_data), so_manager(so_manager), users_db(users_db)
{
	char string_buffer[64];

	if (so_manager == nullptr) {
		sprintf(string_buffer, "; passando SO_Manager inválido para o usuário (id:%llu) - nullptr.", user_data.id.id);

		throw std::runtime_error(string_buffer);
	}

	if (users_db == nullptr) {

	}
}

/*	* All UserScreen will essentially use the same structure on their loop. */
int UserScreen::interact(void)
{
	get_date(interaction_begin);

	UserScreen::main_loop = true;
	while (UserScreen::main_loop) {
		/*	rendering the screen */
		render();

		/*	processing the interactions */
		process();
	}

	if (! users_db->logout(user_data.id))
	{
		std::cerr << "Algo deu errado enquanto tentando deslogar no sistema...\n";
		press_anything_to_continue();
		return 1;
	}

	return 0;
}

void UserScreen::render_menu_header(void) const
{
	print_n_char('=', width);
	printf("\n\t\t%s\n", UserScreen::menu_title);
	print_n_char('=', width);
	printf("\n");

	struct _Date date_of_now;
	get_date(date_of_now);

	printf("Agora: ");
	std::cout << date_of_now;

	aec_fg_rgb(150, 150, 200);
	std::cout << "\t\tTempo de uso: ";
	struct _Date time_active = date_of_now - interaction_begin;
	printf("%02hhu:%02hhu:%02hhu [h:m:s]\n", time_active.hour, time_active.minutes, time_active.seconds);
	aec_reset();
		
	/*	user information */
	printf("\n\n\nUsuário <%s>:\t[id - %019llu] ", user_data.username, user_data.id.id);
	aec_bg_rgb(75, 75, 122);
	aec_reset();

	printf("\n\t\t");
	print_n_char(' ', 21);
	print_n_char('-', strlen(user_data.username));
	print_n_char('\n', 3);
}
