// audio.cpp
// Procesamiento de audio y reconocimiento de voz con PocketSphinx

#include "audio.h"
#include <pocketsphinx.h>
#include <stdio.h>
#include <string.h>
#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/err.h>
#include <iostream>

//static ps_decoder_t *ps = NULL;
//static cmd_ln_t *config = NULL;

void iniciar_audio() {
    // Simulación de inicialización de PocketSphinx
}

void detener_audio() {
    // Simulación de limpieza de recursos
}

void audio_a_texto(char* buffer, int buffer_size) {
    // Inicialización de configuración y decodificador
    cmd_ln_t *config = cmd_ln_init(NULL, (const arg_t*) ps_args(), TRUE,
        "-hmm", "/usr/share/pocketsphinx/model/en-us/en-us",
        "-lm", "/usr/share/pocketsphinx/model/en-us/en-us.lm.bin",
        "-dict", "/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict",
        NULL);
    if (!config) {
        snprintf(buffer, buffer_size, "Error de configuración");
        return;
    }
    ps_decoder_t *ps = ps_init(config);
    if (!ps) {
        snprintf(buffer, buffer_size, "Error al inicializar PocketSphinx");
        cmd_ln_free_r(config);
        return;
    }
    ad_rec_t *ad = ad_open_dev(NULL, 16000);
    if (ad == NULL) {
        snprintf(buffer, buffer_size, "No se pudo abrir el micrófono");
        ps_free(ps);
        cmd_ln_free_r(config);
        return;
    }
    if (ad_start_rec(ad) < 0) {
        snprintf(buffer, buffer_size, "No se pudo iniciar la grabación");
        ad_close(ad);
        ps_free(ps);
        cmd_ln_free_r(config);
        return;
    }
    if (ps_start_utt(ps) < 0) {
        snprintf(buffer, buffer_size, "No se pudo iniciar el reconocimiento");
        ad_stop_rec(ad);
        ad_close(ad);
        ps_free(ps);
        cmd_ln_free_r(config);
        return;
    }
    int16 adbuf[2048];
    int32 k;
    bool in_speech = false;
    std::cout << "Habla ahora (esperando silencio para terminar)..." << std::endl;
    while (1) {
        k = ad_read(ad, adbuf, 2048);
        if (k < 0) break;
        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        in_speech = ps_get_in_speech(ps);
        if (in_speech) {
            // Espera a que termine de hablar
            while (ps_get_in_speech(ps)) {
                k = ad_read(ad, adbuf, 2048);
                if (k < 0) break;
                ps_process_raw(ps, adbuf, k, FALSE, FALSE);
            }
            break;
        }
    }
    ps_end_utt(ps);
    const char* hyp = ps_get_hyp(ps, NULL);
    if (hyp) {
        strncpy(buffer, hyp, buffer_size-1);
        buffer[buffer_size-1] = '\0';
    } else {
        snprintf(buffer, buffer_size, "No se reconoció ninguna frase");
    }
    ad_stop_rec(ad);
    ad_close(ad);
    ps_free(ps);
    cmd_ln_free_r(config);
}
