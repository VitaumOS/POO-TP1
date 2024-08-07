/*	<src/ui/seller-ui.cpp

	(...) */

#include "seller-ui.hpp"
#include <stdexcept>


SO_MenuScreen::SO_MenuScreen(class SO_Manager * const so_manager) : MenuScreen(), so_manager(so_manager) {
    if (so_manager == nullptr)
        throw std::runtime_error("VTMNC\n");
}

SO_MenuScreen::~SO_MenuScreen(void)
{

}

SO_ApprovalMenu::SO_ApprovalMenu(class SO_Manager * const so_manager) : SO_MenuScreen(so_manager) {

}

SO_ApprovalMenu::~SO_ApprovalMenu(void)
{

}

int SO_ApprovalMenu::render(void)
{
    clean_screen();
    
    constexpr const char * menu_base_title = "Menu de aprovações de ordens";
    constexpr size_t title_length = literal_string_length(menu_base_title);

    print_n_char('=', 50); putchar('\n');
    printf("%s\n", menu_base_title);
    print_n_char('*', title_length - 1); putchar('\n');
    print_n_char('\n', 5);

    std::cout << "*: " << output_buffer;
    print_n_char('\n', 5);

    return 0;
}

void SO_ApprovalMenu::interact_so(void)
{
    int64_t input_so_id;
    bool menu_loop = true;

    while (menu_loop)
    {
        /*  Asking the SO id. */
        input_so_id = input_numeral();

        if (input_so_id < 0)
        {
            std::cout << "Entrada inválida. Deseja inserir novamente? ";
            if (! input_verification()) {
                menu_loop = false; // <- for correctness...
                break;
            }

            std::cerr << "VTMNC VIADO" << std::endl;
            continue;
        }
        
        SO_ApprovalMenu::id = static_cast<so_id_t> (input_so_id);

        if (! SO_ApprovalMenu::so_manager->get_order(SO_ApprovalMenu::id, &so_buffer))
        {
            std::cout << "Entrada inválida. Não existe nenhuma SO com o id \"" << 
                static_cast<unsigned long long> (SO_ApprovalMenu::id) << "\". Tente novamente ou dê o bumbum." << std::endl;
        }
        
        std::cout << "After get_order..." << std::endl;

        // SO_BUDGET
        if (SO_ApprovalMenu::so_buffer.stage != SO_BUDGET)
        {
            SO_ApprovalMenu::output_buffer +=
                "VTMNC DIGITA O TREM CERTO VÉI NÓOOOOO (a ordem não está sob o estágio de orçamento).\n";
            break;
        }

        /*  Asking for verification on the approving the order. */
        std::cout << "Deseja aprovar ordem de id \"" << static_cast<unsigned long long> (SO_ApprovalMenu::id) << "\"? ";
        if (! input_verification())
        {
            std::cout << "Deseja cancelá-la? ";
            if (input_verification())
            {
                if (SO_ApprovalMenu::so_manager->close_order(SO_ApprovalMenu::id, &(SO_ApprovalMenu::so_buffer))) {
                    SO_ApprovalMenu::output_buffer +=
                        "Ordem NSQLA foi cancelada...\n";
                }
                else {
                    SO_ApprovalMenu::output_buffer += 
                        "Ordem não pôde ser cancelada...\n";
                }
            }

            menu_loop = false;
            break;
        }
        
        if (SO_ApprovalMenu::so_manager->operate_order(SO_ApprovalMenu::id, &(SO_ApprovalMenu::so_buffer))) {
            SO_ApprovalMenu::output_buffer +=
                "Ordem SLAOQ aprovada com sucesso...";
        }
        else {
            SO_ApprovalMenu::output_buffer +=
                "A ordem SLAOQ não pôde ser aprovada...";
        }
        menu_loop = false;
    }
}

int SO_ApprovalMenu::interact(void)
{
    char command_buffer;

    bool menu_loop = true;
    while (menu_loop) {
        /*  Rendering the screen... */
        SO_ApprovalMenu::render();
        output_buffer.clear();
        
        /*  Inputting the command. */
        std::cout << "[i] para entrar com a ordem [q] para sair\n";
        std::cin >> command_buffer;
        std::cin.clear();
        std::cin.ignore(INT64_MAX, '\n');

        switch (command_buffer)
        {
        case 'i':   
            SO_ApprovalMenu::interact_so();
            break;

        case 'q':
            menu_loop = false;
            break;

        default: continue;
        }
    }

    return 0;
}

SO_ClosingMenu::SO_ClosingMenu(class SO_Manager * const so_manager) : SO_MenuScreen(so_manager)
{


}

SO_ClosingMenu::~SO_ClosingMenu(void)
{

}

int SO_ClosingMenu::render(void)
{
    clean_screen();

    constexpr const char * menu_base_title = "Menu de conclusão de ordens";
    constexpr size_t title_length = literal_string_length(menu_base_title);

    print_n_char('=', 50); putchar('\n');
    printf("%s\n", menu_base_title);
    print_n_char('*', title_length - 1); putchar('\n');
    print_n_char('\n', 5);


    std::cout << "*: " << output_buffer;
    print_n_char('\n', 5);

    return 0;
}

int SO_ClosingMenu::interact(void)
{
    char command_buffer;

    char temp_bf;

    bool menu_loop = true;
    while (menu_loop) {
        /*  Rendering the screen... */
        SO_ClosingMenu::render();
        output_buffer.clear();

        /*  Inputting the command. */
        std::cout << "[i] para entrar com a ordem [q] para sair\n";
        std::cin >> command_buffer;
        std::cin.clear();
        std::cin.ignore(INT64_MAX, '\n');

        switch (command_buffer)
        {
        case 'i':
            std::cout << "temporarily blank; not yet implemented... (PRESS ANYTHING TO CONTINUE)" << std::endl;
            std::cin >> temp_bf;
            break;

        case 'q':
            menu_loop = false;
            break;

        default: continue;
        }
    }

    return 0;
}

