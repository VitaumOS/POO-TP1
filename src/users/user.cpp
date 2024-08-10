/*	<src/users/user.cpp>

	(...) */


#include "user.hpp"


UserScreen::UserScreen(class SO_Manager * const so_manager, class UsersDatabase * const users_db, const struct MinimalUserData & user_data) :
	so_manager(so_manager), users_db(users_db), user_data(user_data)
{
	char string_buffer[64];

	if (so_manager == nullptr) {
		sprintf(string_buffer, ";Passando SO_Manager inválido para o usuário (id:%llu) - nullptr.", user_data.id.id);

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

	return 0;
}

void UserScreen::render_menu_header(void) const
{
	const size_t menu_title_length = strlen(UserScreen::menu_title);

	printf("%s\n", UserScreen::menu_title);
	print_n_char('-', menu_title_length);
	printf("\n");

	struct _Date date_of_now;
	get_date(date_of_now);

	printf("agora: ");
	std::cout << date_of_now;

	aec_fg_rgb(150, 150, 200);
	std::cout << "\t\ttempo de seção: ";
	struct _Date time_active = date_of_now - interaction_begin;
	printf("%02hhu:%02hhu:%02hhu [h:m:s]\n", time_active.hour, time_active.minutes, time_active.seconds);
	aec_reset();
		
	/*	user information */
	const size_t username_length = strlen(user_data.username);
	printf("\n\n\nUsuário:\t%019llu: ", user_data.id.id);
	aec_bg_rgb(75, 75, 122);
	printf("%s", user_data.username);
	aec_reset();

	printf("\n\t\t");
	print_n_char(' ', 21);
	print_n_char('-', username_length);
	print_n_char('\n', 3);
}
