#include "mechanic.hpp"
#include "../headers/ui.hpp"
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
using namespace std;

void Mechanic::interact(void){
    size_t menu_buffer = 0;
    bool repeater = true;

    while (repeater){
        cout << "[::Menu do Mec�nico::] \n\n\n" << endl;
        cout << "O que deseja fazer a seguir?: " << endl;
        cout << "0\t->\tSair " << endl;        
        cout << "1\t->\tVisualisar todas as Ordens de Servi�o " << endl;
        cout << "2\t->\tRegistar um novo produto/servi�o " << endl;
        cout << "3\t->\t Outras coisas ai mano sla viado" << endl;

        cin >> menu_buffer;
        
        switch (menu_buffer){
        case 0:
            repeater = false;   // Finaliza a opera��o
            break;
        case 1:
            // so_manager->so_vizualizer()
            // codar as intera��es pqp
            break;
        case 2:
            // Executar as parada l� meu tlg
            break;
        default:
            clean_screen();
            cout << "Algo deu errado, seu espertinho!!! \n\n" << endl;
            break;
        }
    }

}
