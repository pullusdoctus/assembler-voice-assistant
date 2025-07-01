// main.cpp
// Interfaz principal y lógica de la aplicación

#include <iostream>
#include "audio.h"
#include <pocketsphinx.h>  // still needed for ps_args etc.
#include <portaudio.h>     // required if PortAudio functions are called directly

#define PREGUNTA "¿La UCR tiene becas?"
#define QUESTION "Does UCR offer scholarships?"

extern "C" void buscar_respuesta(const char* pregunta, char* respuesta, int idioma);

// Reconocimiento de voz desde archivo WAV (no micrófono)
void reconocer_voz_archivo(const char* archivo_wav) {
    FILE *fh;
    char const *hyp;
    int16 buf[512];
    int32 score;

    ps_config_t *config = ps_config_init(NULL);

    ps_config_set_str(config, "hmm", "/usr/local/share/pocketsphinx/model/en-us/en-us");
    ps_config_set_str(config, "lm", "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin");
    ps_config_set_str(config, "dict", "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict");

    if (!config) {
        std::cerr << "Error al crear configuración de PocketSphinx" << std::endl;
        return;
    }

    ps_decoder_t *ps = ps_init(config);
    if (!ps) {
        std::cerr << "Error al inicializar PocketSphinx" << std::endl;
        ps_config_free(config);
        return;
    }

    fh = fopen(archivo_wav, "rb");
    if (!fh) {
        std::cerr << "No se pudo abrir el archivo: " << archivo_wav << std::endl;
        ps_free(ps);
        ps_config_free(config);
        return;
    }

    fseek(fh, 44, SEEK_SET);  // Saltar encabezado WAV

    ps_start_utt(ps);
    while (!feof(fh)) {
        size_t nsamp = fread(buf, sizeof(int16), 512, fh);
        ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
    }
    ps_end_utt(ps);

    hyp = ps_get_hyp(ps, &score);
    if (hyp) std::cout << "Reconocido: " << hyp << std::endl;
    else std::cout << "No se reconoció ninguna frase." << std::endl;

    fclose(fh);
    ps_free(ps);
    ps_config_free(config);
}

int main() {
    std::cout << "Bienvenido al Assembler Voice Assistant" << std::endl;

    char respuesta[1024];
    buscar_respuesta(PREGUNTA, respuesta, 0);
    std::cout << PREGUNTA << "\n" << respuesta << std::endl;

    buscar_respuesta(QUESTION, respuesta, 1);
    std::cout << QUESTION << "\n" << respuesta << std::endl;

    // Activar si se desea probar reconocimiento desde archivo
    /*
    std::cout << "\nReconociendo voz desde ejemplo.wav..." << std::endl;
    reconocer_voz_archivo("ejemplo.wav");
    */

    std::cout << "\nReconociendo voz desde micrófono..." << std::endl;
    char buffer[1024];
    audio_a_texto(buffer, sizeof(buffer));
    std::cout << "Texto reconocido: " << buffer << std::endl;

    return 0;
}
