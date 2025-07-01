#include "ui.hpp"
#include <iostream>

// Static callback functions
void Ui::on_window_destroy(GtkWidget* widget, gpointer data) { 
  gtk_main_quit();
}

gboolean Ui::on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data) { 
  return FALSE;
}

// Constructor
Ui::Ui(const char* title, int width, int height) : w(width), h(height) {
  // ventana principal
  this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(this->window), title);
  gtk_window_set_default_size(GTK_WINDOW(this->window), this->w, this->h);
  gtk_window_set_position(GTK_WINDOW(this->window), GTK_WIN_POS_CENTER);
  // caja principal
  this->mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(this->window), this->mainBox);
  gtk_container_set_border_width(GTK_CONTAINER(this->window), 10);
  g_signal_connect(this->window, "destroy", G_CALLBACK(Ui::on_window_destroy), nullptr);
  g_signal_connect(this->window, "delete-event", G_CALLBACK(Ui::on_window_delete_event), nullptr);
}
// Destructor
Ui::~Ui() { if (this->window) gtk_widget_destroy(this->window); }

// Static methods
void Ui::initGTK(int argc, char* argv[]) { gtk_init(&argc, &argv); }
void Ui::run() { gtk_main(); }

// Window methods
void Ui::show() { gtk_widget_show_all(this->window); }
void Ui::hide() { gtk_widget_hide(this->window); }
void Ui::setWindowTitle(const char* title) {
  gtk_window_set_title(GTK_WINDOW(this->window), title); 
}
void Ui::setWindowSize(int width, int height) {
  this->w = width;
  this->h = height;
  gtk_window_resize(GTK_WINDOW(this->window), this->w, this->h);
}

void Ui::addLabel(const char* text) {
  GtkWidget* label = gtk_label_new(text);
  gtk_box_pack_start(GTK_BOX(this->mainBox), label, FALSE, FALSE, 0);
  gtk_widget_show(label);
}
void Ui::addButton(const char* text, GCallback callback, gpointer data) {
  GtkWidget* button = gtk_button_new_with_label(text);
  gtk_box_pack_start(GTK_BOX(this->mainBox), button, FALSE, FALSE, 0);
  if (callback) 
    g_signal_connect(button, "clicked", callback, data);
  gtk_widget_show(button);
}
void Ui::addButtonToWindow(const char* text, GCallback callback, gpointer data) {
  addButton(text, callback, data);
}

// Getters
GtkWidget* Ui::getWindow() const { return this->window; }
GtkWidget* Ui::getMainBox() const { return this->mainBox; }

// Helper callback
void Ui::on_button_clicked(GtkWidget* widget, gpointer data) { 
  std::cout << "button clicked" << std::endl; 
}
