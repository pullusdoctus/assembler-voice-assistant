#pragma once

#include <gtk/gtk.h>

class Ui {
 private:
  GtkWidget* window;
  GtkWidget* mainBox;
  int w;
  int h;

  static void on_window_destroy(GtkWidget* widget, gpointer data);
  static gboolean on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data);

 public:
  // constructor
  Ui(const char* title = "Assembler Voice Assistant", int width = 800, int height = 600);
  // destructor
  ~Ui();
  // init GTK
  static void initGTK(int argc, char* argv[]);
  // main loop
  static void run();

  // === METODOS DE VENTANA ===
  void show();
  void hide();
  void setWindowTitle(const char* title);
  void setWindowSize(int width, int height);
  void addLabel(const char* text);
  void addButton(const char* text, GCallback callback = nullptr, gpointer data = nullptr);
  void addButtonToWindow(const char* text, GCallback callback = nullptr, gpointer data = nullptr);

  // === GETTERS & SETTERS ===
  GtkWidget* getWindow() const;
  GtkWidget* getMainBox() const;

  // === GTK HELPERS ===
  static void on_button_clicked(GtkWidget* widget, gpointer data);
};
