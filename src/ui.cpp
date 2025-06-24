// ui.cpp
// Interfaz gráfica básica con SDL2

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>

#define FONT_PATH "./font/static/OpenSans-Regular.ttf"

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Configuración de UI
struct UIState {
    std::string pregunta;
    std::string respuesta;
    int idioma = 0; // 0: Español, 1: Inglés, 2: Francés
    bool preguntaActiva = false;
};

const char* idiomas[] = {"Español", "Inglés", "Francés"};

// Renderiza texto centrado
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int tw, th;
    SDL_QueryTexture(texture, NULL, NULL, &tw, &th);
    SDL_Rect dst = {x - tw/2, y, tw, th};
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Renderiza un rectángulo con esquinas redondeadas (simulado)
void renderRoundedRect(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color, int radius = 10) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    // Esquinas: puedes mejorar esto con una librería, aquí solo se simula el efecto
    // (No hay soporte nativo en SDL2, pero el fondo claro lo disimula)
}

// Renderiza una línea separadora
void renderSeparator(SDL_Renderer* renderer, int x, int y, int w, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x, y, x + w, y);
}

extern "C" void buscar_respuesta(const char* pregunta, char* respuesta, int idioma);

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("No se pudo inicializar SDL: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() == -1) {
        SDL_Log("No se pudo inicializar SDL_ttf: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Assembler Voice Assistant", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Log("No se pudo crear la ventana: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("No se pudo crear el renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    UIState ui;
    TTF_Font* font = TTF_OpenFont(FONT_PATH, 22);
    if (!font) {
        SDL_Log("No se pudo cargar la fuente");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
    SDL_Color fg = {30,30,30,255};
    SDL_Color bg = {245,247,250,255};
    SDL_Color accent = {0, 120, 215, 255};
    SDL_Color btn = {230,235,245,255};
    SDL_Color btnText = {40,40,40,255};
    SDL_Color border = {200,200,200,255};
    SDL_Color respBg = {252,252,255,255};
    // Cargar sprite de micrófono
    SDL_Surface* micSurface = IMG_Load("src/resources/sprite/3800798.png");
    SDL_Texture* micTexture = nullptr;
    if (micSurface) {
        micTexture = SDL_CreateTextureFromSurface(renderer, micSurface);
        SDL_FreeSurface(micSurface);
    }
    char respuesta_buffer[512];
    // Tabla de códigos de idioma de 3 letras como enteros
    const int idioma_codes[] = {
        ('e' << 16) | ('s' << 8) | 'p', // esp
        ('e' << 16) | ('n' << 8) | 'g', // eng
        ('f' << 16) | ('r' << 8) | 'a'  // fra
    };
    // Loop principal
    bool running = true;
    SDL_Event e;
    while (running) {
        // Render
        SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, 255);
        SDL_RenderClear(renderer);
        // Título
        renderText(renderer, font, "Assembler Voice Assistant", 400, 24, accent);
        renderSeparator(renderer, 120, 54, 580, border);
        // Campo de texto pregunta
        SDL_Rect preguntaRect = {140, 70, 420, 44};
        renderRoundedRect(renderer, preguntaRect, {255,255,255,255});
        SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, 255);
        SDL_RenderDrawRect(renderer, &preguntaRect);
        if (ui.preguntaActiva)
            SDL_RenderDrawRect(renderer, &preguntaRect); // Doble borde si activo
        renderText(renderer, font, ui.pregunta.empty() ? "Escribe tu pregunta aquí..." : ui.pregunta, 350, 82, fg);
        // Botón enviar
        SDL_Rect btnEnviar = {580, 70, 120, 44};
        renderRoundedRect(renderer, btnEnviar, accent);
        renderText(renderer, font, "Enviar", 640, 82, {255,255,255,255});
        // Botón grabar audio
        SDL_Rect btnMic = {520, 70, 44, 44};
        renderRoundedRect(renderer, btnMic, accent);
        if (micTexture) {
            SDL_Rect micDst = {btnMic.x+6, btnMic.y+6, 32, 32};
            SDL_RenderCopy(renderer, micTexture, NULL, &micDst);
        } else {
            renderText(renderer, font, "🎤", btnMic.x+22, btnMic.y+8, {255,255,255,255});
        }
        // Selector de idioma
        for (int i = 0; i < 3; ++i) {
            SDL_Rect idiomaBtn = {140 + i*140, 130, 120, 38};
            renderRoundedRect(renderer, idiomaBtn, i == ui.idioma ? accent : btn);
            renderText(renderer, font, idiomas[i], 140 + i*140 + 60, 140, i == ui.idioma ? SDL_Color{255,255,255,255} : btnText);
        }
        // Área de respuesta
        SDL_Rect respRect = {140, 200, 560, 260};
        renderRoundedRect(renderer, respRect, respBg);
        SDL_SetRenderDrawColor(renderer, border.r, border.g, border.b, 255);
        SDL_RenderDrawRect(renderer, &respRect);
        renderSeparator(renderer, 140, 195, 560, border);
        renderText(renderer, font, ui.respuesta.empty() ? "Respuesta aquí..." : ui.respuesta, 420, 220, fg);
        SDL_RenderPresent(renderer);
        // Eventos
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x, my = e.button.y;
                // Campo pregunta
                if (mx > preguntaRect.x && mx < preguntaRect.x+preguntaRect.w && my > preguntaRect.y && my < preguntaRect.y+preguntaRect.h)
                    ui.preguntaActiva = true;
                else ui.preguntaActiva = false;
                // Botón enviar
                if (mx > btnEnviar.x && mx < btnEnviar.x+btnEnviar.w && my > btnEnviar.y && my < btnEnviar.y+btnEnviar.h) {
                    buscar_respuesta(ui.pregunta.c_str(), respuesta_buffer, idioma_codes[ui.idioma]);
                    ui.respuesta = respuesta_buffer;
                }
                // Botón micrófono
                if (mx > btnMic.x && mx < btnMic.x+btnMic.w && my > btnMic.y && my < btnMic.y+btnMic.h)
                    ui.respuesta = "[Grabando audio...]";
                // Selector idioma
                for (int i = 0; i < 3; ++i) {
                    SDL_Rect idiomaBtn = {140 + i*140, 130, 120, 38};
                    if (mx > idiomaBtn.x && mx < idiomaBtn.x+idiomaBtn.w && my > idiomaBtn.y && my < idiomaBtn.y+idiomaBtn.h)
                        ui.idioma = i;
                }
            }
            else if (e.type == SDL_TEXTINPUT && ui.preguntaActiva) {
                ui.pregunta += e.text.text;
            }
            else if (e.type == SDL_KEYDOWN && ui.preguntaActiva) {
                if (e.key.keysym.sym == SDLK_BACKSPACE && !ui.pregunta.empty())
                    ui.pregunta.pop_back();
            }
        }
        SDL_Delay(10);
    }
    if (micTexture) SDL_DestroyTexture(micTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
