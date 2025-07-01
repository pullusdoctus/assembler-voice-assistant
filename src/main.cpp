#include <iostream>
#include "ui.hpp"

int main(int argc, char* argv[]) {
  // Initialize GTK
  Ui::initGTK(argc, argv);
  // Create window with appropriate size for the inclusive voice assistant
  Ui window("InclusivaApp - Asistente de Voz Inclusivo", 800, 700);
  // Show window
  window.show();
  // Run main loop
  Ui::run();
  return 0;
}
