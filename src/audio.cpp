// audio.cpp
// Procesamiento de audio y reconocimiento de voz con PocketSphinx
/*
#include "audio.h"
#include <pocketsphinx.h>
#include <stdio.h>
#include <string.h>
#include <sphinxbase/cmd_ln.h>

static ps_decoder_t *ps = NULL;
static cmd_ln_t *config = NULL;

void iniciar_audio() {
    // Simulación de inicialización de PocketSphinx
    config = cmd_ln_init(NULL, NULL, TRUE,
        "-hmm", "model/en-us/en-us",
        "-lm", "model/en-us/en-us.lm.bin",
        "-dict", "model/en-us/cmudict-en-us.dict",
        NULL);
    ps = ps_init(config);
    printf("[PocketSphinx] Inicializado.\n");
}

void detener_audio() {
    // Simulación de limpieza de recursos
    if (ps) ps_free(ps);
    if (config) cmd_ln_free_r(config);
    printf("[PocketSphinx] Recursos liberados.\n");
}

void audio_a_texto(char* buffer, int buffer_size) {
    // Simulación de reconocimiento de voz
    strncpy(buffer, "[Reconocimiento simulado]", buffer_size);
    buffer[buffer_size-1] = '\0';
    printf("[PocketSphinx] Audio procesado (simulado).\n");
}*/
