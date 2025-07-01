#include <iostream>
#include "ui.hpp"

// Button callback function (legacy - kept for compatibility)
void on_button_clicked(GtkWidget* widget, gpointer data) {
  std::cout << "¡Botón presionado!" << std::endl;
}

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
