// main.cpp
// Interfaz principal y lógica de la aplicación

#include <iostream>
#include "audio.h"

extern "C" void buscar_respuesta(const char* pregunta, char* respuesta, int idioma);

int main() {
    std::cout << "Bienvenido al Assembler Voice Assistant" << std::endl;
    return 0;
}
