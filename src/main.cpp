// main.cpp
// Interfaz principal y lógica de la aplicación

#include <iostream>
#include "audio.h"

#define PREGUNTA "¿La UCR tiene becas?"
#define QUESTION "Does UCR offer scholarships?"

extern "C" void buscar_respuesta(const char* pregunta, char* respuesta, int idioma);

int main() {
    std::cout << "Bienvenido al Assembler Voice Assistant" << std::endl;
    char respuesta[1024];
    buscar_respuesta(PREGUNTA, respuesta, 0);
    std::cout << PREGUNTA << '\n' << respuesta << std::endl;
    buscar_respuesta(QUESTION, respuesta, 1);
    std::cout << QUESTION << '\n' << respuesta << std::endl;
    return 0;
}
