#include "assembler.hpp"
#include "audio.hpp"
#include "ui.hpp"
#include <iostream>
#include <cstring>

// Static callback functions
void Ui::on_window_destroy(GtkWidget* widget, gpointer data) { gtk_main_quit(); }
gboolean Ui::on_window_delete_event(GtkWidget* widget, GdkEvent* event, gpointer data) { return FALSE; }

// Constructor
Ui::Ui(const char* title, int width, int height) : w(width), h(height) {
  // Initialize state variables
  boldTextEnabled = false;
  highContrastEnabled = false;
  recording = false;
  currentFontSize = 12;
  currentLanguage = "Español";
  // Create main window
  this->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(this->window), title);
  gtk_window_set_default_size(GTK_WINDOW(this->window), this->w, this->h);
  gtk_window_set_position(GTK_WINDOW(this->window), GTK_WIN_POS_CENTER);
  // Create main box
  this->mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_add(GTK_CONTAINER(this->window), this->mainBox);
  gtk_container_set_border_width(GTK_CONTAINER(this->window), 15);
  // Connect signals
  g_signal_connect(this->window, "destroy", G_CALLBACK(Ui::on_window_destroy), nullptr);
  g_signal_connect(this->window, "delete-event", G_CALLBACK(Ui::on_window_delete_event), nullptr);
  // Create all UI components
  createHeader();
  createLanguageSelection();
  createAccessibilityOptions();
  createInputSection();
  createResponseSection();
  createFooter();
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

// Create header section
void Ui::createHeader() {
  // Header container
  headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_box_pack_start(GTK_BOX(mainBox), headerBox, FALSE, FALSE, 0);
  // Logo/Title
  logoLabel = gtk_label_new("🌐 Assembler Voice Assistant");
  gtk_label_set_markup(GTK_LABEL(logoLabel), "<span size='large' weight='bold'>🌐 Assembler Voice Assistant</span>");
  gtk_box_pack_start(GTK_BOX(headerBox), logoLabel, TRUE, TRUE, 0);
  // Configuration controls box
  configBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_end(GTK_BOX(headerBox), configBox, FALSE, FALSE, 0);
  // Separator
  GtkWidget* separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(mainBox), separator1, FALSE, FALSE, 5);
}

// Create language selection section
void Ui::createLanguageSelection() {
  languageFrame = gtk_frame_new("📌 Seleccione su idioma:");
  gtk_box_pack_start(GTK_BOX(mainBox), languageFrame, FALSE, FALSE, 0);
  languageButtonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_container_add(GTK_CONTAINER(languageFrame), languageButtonBox);
  gtk_container_set_border_width(GTK_CONTAINER(languageButtonBox), 10);
  // Language buttons
  spanishButton = gtk_button_new_with_label("Español");
  englishButton = gtk_button_new_with_label("English");
  frenchButton = gtk_button_new_with_label("Français");
  gtk_box_pack_start(GTK_BOX(languageButtonBox), spanishButton, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(languageButtonBox), englishButton, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(languageButtonBox), frenchButton, TRUE, TRUE, 0);
  // Connect signals
  g_signal_connect(spanishButton, "clicked", G_CALLBACK(on_language_button_clicked), this);
  g_signal_connect(englishButton, "clicked", G_CALLBACK(on_language_button_clicked), this);
  g_signal_connect(frenchButton, "clicked", G_CALLBACK(on_language_button_clicked), this);
}

// Create accessibility options section
void Ui::createAccessibilityOptions() {
  accessibilityFrame = gtk_frame_new("📌 Configuración de Accesibilidad:");
  gtk_box_pack_start(GTK_BOX(mainBox), accessibilityFrame, FALSE, FALSE, 0);
  accessibilityBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_container_add(GTK_CONTAINER(accessibilityFrame), accessibilityBox);
  gtk_container_set_border_width(GTK_CONTAINER(accessibilityBox), 10);
  // Accessibility buttons
  increaseFontButton = gtk_button_new_with_label("+ Aumentar Tamaño");
  decreaseFontButton = gtk_button_new_with_label("- Disminuir Tamaño");
  boldTextButton = gtk_button_new_with_label(boldTextEnabled ? "🅱 Texto Normal" : "🅱 Texto en Negrita");
  contrastButton = gtk_button_new_with_label("🌗 Alto Contraste");
  gtk_box_pack_start(GTK_BOX(accessibilityBox), increaseFontButton, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(accessibilityBox), decreaseFontButton, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(accessibilityBox), boldTextButton, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(accessibilityBox), contrastButton, TRUE, TRUE, 0);
  // Connect signals
  g_signal_connect(increaseFontButton, "clicked", G_CALLBACK(on_increase_font_clicked), this);
  g_signal_connect(decreaseFontButton, "clicked", G_CALLBACK(on_decrease_font_clicked), this);
  g_signal_connect(boldTextButton, "clicked", G_CALLBACK(on_bold_text_clicked), this);
  g_signal_connect(contrastButton, "clicked", G_CALLBACK(on_contrast_clicked), this);
  // Separator
  GtkWidget* separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(mainBox), separator2, FALSE, FALSE, 5);
}

// Create input section
void Ui::createInputSection() {
  inputFrame = gtk_frame_new("💬 Ingrese su pregunta:");
  gtk_box_pack_start(GTK_BOX(mainBox), inputFrame, FALSE, FALSE, 0);
  inputBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(inputFrame), inputBox);
  gtk_container_set_border_width(GTK_CONTAINER(inputBox), 10);
  // Text view with scrolled window
  inputScrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(inputScrolled), 
                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_widget_set_size_request(inputScrolled, -1, 80);
  inputTextView = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(inputTextView), GTK_WRAP_WORD);
  gtk_container_add(GTK_CONTAINER(inputScrolled), inputTextView);
  gtk_box_pack_start(GTK_BOX(inputBox), inputScrolled, TRUE, TRUE, 0);
  // Button box for Record and Send
  GtkWidget* sendBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(inputBox), sendBox, FALSE, FALSE, 0);
  // Record button
  recordButton = gtk_button_new_with_label("🎤 Grabar");
  gtk_box_pack_start(GTK_BOX(sendBox), recordButton, TRUE, TRUE, 0);
  g_signal_connect(recordButton, "clicked", G_CALLBACK(on_record_button_clicked), this);
  // Send button
  sendButton = gtk_button_new_with_label("Enviar Pregunta");
  gtk_box_pack_start(GTK_BOX(sendBox), sendButton, TRUE, TRUE, 0);
  g_signal_connect(sendButton, "clicked", G_CALLBACK(on_send_button_clicked), this);
  // Separator
  GtkWidget* separator3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(mainBox), separator3, FALSE, FALSE, 5);
}

// Create response section
void Ui::createResponseSection() {
  responseFrame = gtk_frame_new("📝 Respuesta:");
  gtk_box_pack_start(GTK_BOX(mainBox), responseFrame, TRUE, TRUE, 0);
  responseBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(responseFrame), responseBox);
  gtk_container_set_border_width(GTK_CONTAINER(responseBox), 10);
  // Text view with scrolled window
  responseScrolled = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(responseScrolled), 
                                GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  responseTextView = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(responseTextView), GTK_WRAP_WORD);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(responseTextView), FALSE);
  gtk_container_add(GTK_CONTAINER(responseScrolled), responseTextView);
  gtk_box_pack_start(GTK_BOX(responseBox), responseScrolled, TRUE, TRUE, 0);
  // Separator
  GtkWidget* separator4 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(mainBox), separator4, FALSE, FALSE, 5);
}

// Create footer
void Ui::createFooter() {
  footerLabel = gtk_label_new("© 2025 Assembler Voice Assistant");
  gtk_label_set_markup(GTK_LABEL(footerLabel), 
                      "<span size='small' style='italic'>© 2025 Assembler Voice Assistant</span>");
  gtk_widget_set_halign(footerLabel, GTK_ALIGN_CENTER);
  gtk_box_pack_end(GTK_BOX(mainBox), footerLabel, FALSE, FALSE, 0);
}

// Legacy methods for compatibility
void Ui::addLabel(const char* text) {
  GtkWidget* label = gtk_label_new(text);
  gtk_box_pack_start(GTK_BOX(this->mainBox), label, FALSE, FALSE, 0);
  gtk_widget_show(label);
}

void Ui::addButton(const char* text, GCallback callback, gpointer data) {
  GtkWidget* button = gtk_button_new_with_label(text);
  gtk_box_pack_start(GTK_BOX(this->mainBox), button, FALSE, FALSE, 0);
  if (callback) g_signal_connect(button, "clicked", callback, data);
  gtk_widget_show(button);
}

void Ui::addButtonToWindow(const char* text, GCallback callback, gpointer data) {
  addButton(text, callback, data);
}

// Getters
GtkWidget* Ui::getWindow() const { return this->window; }
GtkWidget* Ui::getMainBox() const { return this->mainBox; }
GtkWidget* Ui::getInputTextView() const { return this->inputTextView; }
GtkWidget* Ui::getResponseTextView() const { return this->responseTextView; }

// Text manipulation methods
void Ui::setInputText(const char* text) {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(inputTextView));
  gtk_text_buffer_set_text(buffer, text, -1);
}

void Ui::setResponseText(const char* text) {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(responseTextView));
  gtk_text_buffer_set_text(buffer, text, -1);
}

void Ui::appendResponseText(const char* text) {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(responseTextView));
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(buffer, &end);
  gtk_text_buffer_insert(buffer, &end, text, -1);
}

const char* Ui::getInputText() {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(inputTextView));
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);
  return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}

// Accessibility methods
void Ui::increaseFontSize() {
  int newSize = increase_font_size(&currentFontSize);
  applyFontSize();
  std::cout << "Font size increased to: " << currentFontSize << std::endl;
}

void Ui::decreaseFontSize() {
  int oldSize = currentFontSize;
  int newSize = decrease_font_size(&currentFontSize);
  if (newSize != oldSize) {
    applyFontSize();
    std::cout << "Font size decreased to: " << currentFontSize << std::endl;
  }
}

void Ui::applyFontSize() {
  updateAllWidgetsFont();
  // Also update the font size combo box to reflect the current size
  int comboIndex;
  if (currentFontSize <= 10) {
    comboIndex = 0; // Small
  } else if (currentFontSize <= 12) {
    comboIndex = 1; // Normal
  } else {
    comboIndex = 2; // Large
  }
}

void Ui::updateAllWidgetsFont() {
  PangoFontDescription* font = pango_font_description_new();
  pango_font_description_set_family(font, "Sans");
  pango_font_description_set_size(font, currentFontSize * PANGO_SCALE);  // must be in Pango units
  pango_font_description_set_weight(font, boldTextEnabled ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
  GtkWidget* widgets[] = {
    inputTextView,
    responseTextView,
    logoLabel,
    recordButton,
    sendButton,
    spanishButton,
    englishButton,
    frenchButton,
    increaseFontButton,
    decreaseFontButton,
    boldTextButton,
    contrastButton,
    footerLabel
  };
  for (GtkWidget* w : widgets) {
    gtk_widget_override_font(w, font);
  }
  // Frame label widgets (special handling)
  GtkWidget* frameLabels[] = {
    gtk_frame_get_label_widget(GTK_FRAME(languageFrame)),
    gtk_frame_get_label_widget(GTK_FRAME(accessibilityFrame)),
    gtk_frame_get_label_widget(GTK_FRAME(inputFrame)),
    gtk_frame_get_label_widget(GTK_FRAME(responseFrame))
  };
  for (GtkWidget* label : frameLabels) {
    if (label) gtk_widget_override_font(label, font);
  }
  pango_font_description_free(font);
}

void Ui::toggleBoldText() {
  toggle(reinterpret_cast<uint8_t*>(&boldTextEnabled));
  updateAllWidgetsFont();
  std::cout << "Bold text " << (boldTextEnabled ? "enabled" : "disabled") << std::endl;
}

void Ui::toggleHighContrast() {
  toggle(reinterpret_cast<uint8_t*>(&highContrastEnabled));
  updateAllWidgetsColors();
  std::cout << "High contrast " << (highContrastEnabled ? "enabled" : "disabled") << std::endl;
}

void Ui::updateAllWidgetsColors() {
  GtkWidget* widgets[] = {
    inputTextView,
    responseTextView,
    logoLabel,
    recordButton,
    sendButton,
    spanishButton,
    englishButton,
    frenchButton,
    increaseFontButton,
    decreaseFontButton,
    boldTextButton,
    contrastButton,
    footerLabel
  };
  for (GtkWidget* w : widgets) {
    GtkStyleContext* ctx = gtk_widget_get_style_context(w);
    if (highContrastEnabled) {
      // Get current fg and bg colors for NORMAL state
      GdkRGBA fg, bg;
      gtk_style_context_get_color(ctx, GTK_STATE_FLAG_NORMAL, &fg);
      gtk_style_context_get_background_color(ctx, GTK_STATE_FLAG_NORMAL, &bg);
      // invert colors
      GdkRGBA inv_fg = invert_color(fg);
      GdkRGBA inv_bg = invert_color(bg);
      gtk_widget_override_background_color(w, GTK_STATE_FLAG_NORMAL, &inv_bg);
      gtk_widget_override_color(w, GTK_STATE_FLAG_NORMAL, &inv_fg);
    } else {
      // Passing nullptr resets to default theme colors
      gtk_widget_override_background_color(w, GTK_STATE_FLAG_NORMAL, nullptr);
      gtk_widget_override_color(w, GTK_STATE_FLAG_NORMAL, nullptr);
    }
  }
}

GdkRGBA Ui::invert_color(const GdkRGBA& c) {
  GdkRGBA inverted;
  _invert_color(&c, &inverted);
  return inverted;
}

void Ui::changeLanguage(const char* language) {
  currentLanguage = language;
  std::cout << "Language changed to: " << language << std::endl;
}

// Callback implementations
void Ui::on_language_changed(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  int active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  const char* languages[] = {"Español", "English", "Français"};
  ui->changeLanguage(languages[active]);
}

void Ui::on_font_size_changed(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  int active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  // Map combo box selection to font sizes
  int newFontSize;
  switch(active) {
    case 0: newFontSize = 12; break; // Small
    case 1: newFontSize = 16; break; // Normal
    case 2: newFontSize = 20; break; // Large
    default: newFontSize = 12; break;
  }
  ui->currentFontSize = newFontSize;
  ui->updateAllWidgetsFont();
  std::cout << "Font size changed via combo: " << ui->currentFontSize << std::endl;
}

void Ui::on_accessibility_clicked(GtkWidget* widget, gpointer data) {
  std::cout << "Accessibility menu clicked" << std::endl;
}

void Ui::on_record_button_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  if (!ui->recording) {
    // Start recording
    std::cout << "🎤 Starting recording..." << std::endl;
    // Update button appearance
    gtk_button_set_label(GTK_BUTTON(ui->recordButton), "⏹️ Detener");
    // Call the recording start function
    iniciar_grabacion_microfono();
    // Update state
    ui->recording = true;
    std::cout << "Recording started. Click button again to stop." << std::endl;
  } else {
    // Stop recording and get result
    std::cout << "⏹️ Stopping recording..." << std::endl;
    // Update button appearance
    gtk_button_set_label(GTK_BUTTON(ui->recordButton), "🎤 Grabar");
    // Buffer to receive the recognized text
    char recognized_text[1024];
    // Call the recording stop function
    detener_y_reconocer_microfono(recognized_text, sizeof(recognized_text));
    // Update state
    ui->recording = false;
    // Display the recognized text in the input field
    ui->setInputText(recognized_text);
    std::cout << "Recording stopped. Recognized text: " << recognized_text << std::endl;
  }

  std::cout << "🎤 Record button clicked" << std::endl;
}

void Ui::on_send_button_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  const char* inputText = ui->getInputText();
  std::cout << "Sending question: " << inputText << std::endl;
  // Example response
  ui->setResponseText("Esta es una respuesta de ejemplo al asistente de voz inclusivo.\n\n"
                     "El sistema ha procesado su consulta y está preparado para "
                     "proporcionar asistencia en múltiples idiomas con opciones "
                     "de accesibilidad completas.");
}

void Ui::on_increase_font_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  ui->increaseFontSize();
}

void Ui::on_decrease_font_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  ui->decreaseFontSize();
}

void Ui::on_bold_text_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  ui->toggleBoldText();
}

void Ui::on_contrast_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  ui->toggleHighContrast();
}

void Ui::on_language_button_clicked(GtkWidget* widget, gpointer data) {
  Ui* ui = static_cast<Ui*>(data);
  const char* label = gtk_button_get_label(GTK_BUTTON(widget));
  ui->changeLanguage(label);
}

// Helper callback
void Ui::on_button_clicked(GtkWidget* widget, gpointer data) { 
  std::cout << "button clicked" << std::endl; 
}
