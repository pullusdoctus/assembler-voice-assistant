#pragma once

#include <gtk/gtk.h>

class Ui {
 private:
  // propiedades de ventana
  GtkWidget* window;
  GtkWidget* mainBox;
  int w, h;

  // UI Components
  GtkWidget* headerBox;
  GtkWidget* logoLabel;
  GtkWidget* configBox;
  GtkWidget* languageCombo;
  GtkWidget* fontSizeCombo;
  GtkWidget* accessibilityButton;
  // Language selection
  GtkWidget* languageFrame;
  GtkWidget* languageButtonBox;
  GtkWidget* spanishButton;
  GtkWidget* englishButton;
  GtkWidget* frenchButton;
  // Accessibility options
  GtkWidget* accessibilityFrame;
  GtkWidget* accessibilityBox;
  GtkWidget* increaseFontButton;
  GtkWidget* decreaseFontButton;
  GtkWidget* boldTextButton;
  GtkWidget* contrastButton;
  // Input section
  GtkWidget* inputFrame;
  GtkWidget* inputBox;
  GtkWidget* inputLabel;
  GtkWidget* inputTextView;
  GtkWidget* inputScrolled;
  GtkWidget* sendButton;
  // Response section
  GtkWidget* responseFrame;
  GtkWidget* responseBox;
  GtkWidget* responseLabel;
  GtkWidget* responseTextView;
  GtkWidget* responseScrolled;
  // Footer
  GtkWidget* footerLabel;
  // State variables
  bool boldTextEnabled;
  bool highContrastEnabled;
  int currentFontSize;
  const char* currentLanguage;

 public:
  // constructor
  Ui(const char* title = "app", int width = 800, int height = 600);
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

  // === WIDGETS ===
  void createHeader();
  void createLanguageSelection();
  void createAccessibilityOptions();
  void createInputSection();
  void createResponseSection();
  void createFooter();
  void setupLayout();
  void addLabel(const char* text);
  void addButton(const char* text, GCallback callback = nullptr, gpointer data = nullptr);
  void addButtonToWindow(const char* text, GCallback callback = nullptr, gpointer data = nullptr);

  // === GETTERS & SETTERS ===
  GtkWidget* getWindow() const;
  GtkWidget* getMainBox() const;
  GtkWidget* getInputTextView() const;
  GtkWidget* getResponseTextView() const;

  // === TEXTO ===
  void setInputText(const char* text);
  void setResponseText(const char* text);
  void appendResponseText(const char* text);
  const char* getInputText();

  // === ACCESIBLIDAD ===
  void increaseFontSize();
  void decreaseFontSize();
  void applyFontSize();
  void updateAllWidgetsFont();
  void toggleBoldText();
  void toggleHighContrast();
  void changeLanguage(const char* language);

  // === CALLBACKS ===
  static void on_window_destroy(GtkWidget* widget, gpointer data);
  static gboolean on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data);
  static void on_button_clicked(GtkWidget* widget, gpointer data);
  static void on_language_changed(GtkWidget* widget, gpointer data);
  static void on_font_size_changed(GtkWidget* widget, gpointer data);
  static void on_accessibility_clicked(GtkWidget* widget, gpointer data);
  static void on_send_button_clicked(GtkWidget* widget, gpointer data);
  static void on_increase_font_clicked(GtkWidget* widget, gpointer data);
  static void on_decrease_font_clicked(GtkWidget* widget, gpointer data);
  static void on_bold_text_clicked(GtkWidget* widget, gpointer data);
  static void on_contrast_clicked(GtkWidget* widget, gpointer data);
  static void on_language_button_clicked(GtkWidget* widget, gpointer data);
};
