#include <iostream>
#include "ui.hpp"  // Include the UI header

// Button callback function
void on_button_clicked(GtkWidget* widget, gpointer data) {
  std::cout << "¡Botón presionado!" << std::endl;
  // You can cast data back to your window if needed
  // Ui* window = static_cast<Ui*>(data);
}

int main(int argc, char* argv[]) {
  // Initialize GTK
  Ui::initGTK(argc, argv);
  // Create window
  Ui window("Assembler Voice Assistant", 600, 400);
  // Add widgets
  window.addLabel("ASISTENTE DE VOZ");
  window.addButton("boton", G_CALLBACK(on_button_clicked), &window);
  window.addLabel("etiqueta 2");
  // Show window
  window.show();
  // Run main loop
  Ui::run();
  return 0;
}
