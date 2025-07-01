// main.cpp
// Interfaz principal y lógica de la aplicación

#include <iostream>
#include "audio.h"
#include <pocketsphinx.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/err.h>
#include <sphinxbase/cmd_ln.h>

#define PREGUNTA "¿La UCR tiene becas?"
#define QUESTION "Does UCR offer scholarships?"

extern "C" void buscar_respuesta(const char* pregunta, char* respuesta, int idioma);

// Función de ejemplo para reconocimiento de voz desde un archivo WAV
void reconocer_voz_archivo(const char* archivo_wav) {
    ps_decoder_t *ps;
    cmd_ln_t *config;
    FILE *fh;
    char const *hyp, *uttid;
    int16 buf[512];
    int rv;
    int32 score;

    // Configuración básica (usa el modelo por defecto en inglés)
    config = cmd_ln_init(NULL, (const arg_t*) ps_args(), TRUE,
    "-hmm", "/usr/share/pocketsphinx/model/en-us/en-us",
    "-lm", "/usr/share/pocketsphinx/model/en-us/en-us.lm.bin",
    "-dict", "/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict",
    NULL);
    if (config == NULL) {
        std::cerr << "Error al crear configuración de PocketSphinx" << std::endl;
        return;
    }
    ps = ps_init(config);
    if (ps == NULL) {
        std::cerr << "Error al inicializar PocketSphinx" << std::endl;
        return;
    }

    fh = fopen(archivo_wav, "rb");
    if (fh == NULL) {
        std::cerr << "No se pudo abrir el archivo de audio: " << archivo_wav << std::endl;
        ps_free(ps);
        return;
    }
    // Saltar cabecera WAV (44 bytes)
    fseek(fh, 44, SEEK_SET);
    ps_start_utt(ps);
    while (!feof(fh)) {
        size_t nsamp = fread(buf, 2, 512, fh);
        rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
    }
    ps_end_utt(ps);
    hyp = ps_get_hyp(ps, &score);
    if (hyp != NULL) {
        std::cout << "Reconocido: " << hyp << std::endl;
    } else {
        std::cout << "No se reconoció ninguna frase." << std::endl;
    }
    fclose(fh);
    ps_free(ps);
    cmd_ln_free_r(config);
}

int main() {
    std::cout << "Bienvenido al Assembler Voice Assistant" << std::endl;
    char respuesta[1024];
    buscar_respuesta(PREGUNTA, respuesta, 0);
    std::cout << PREGUNTA << '\n' << respuesta << std::endl;
    buscar_respuesta(QUESTION, respuesta, 1);
    std::cout << QUESTION << '\n' << respuesta << std::endl;

    /*
    // Ejemplo de uso de reconocimiento de voz desde archivo
    std::cout << "\nReconociendo voz desde ejemplo.wav..." << std::endl;
    reconocer_voz_archivo("ejemplo.wav"); // Cambia por el nombre de tu archivo WAV
    */
   
    // Ejemplo de reconocimiento de voz desde micrófono
    char buffer[1024];
    std::cout << "\nReconociendo voz desde micrófono..." << std::endl;
    audio_a_texto(buffer, sizeof(buffer));
    std::cout << "Texto reconocido: " << buffer << std::endl;

    return 0;
}
