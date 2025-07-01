// audio.cpp
// Procesamiento de audio y reconocimiento de voz con PocketSphinx

#include "audio.h"
#include <pocketsphinx.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/cmd_ln.h>
#include <sphinxbase/err.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cstring>
#include <iostream>

// Variables estáticas para la grabación
static ad_rec_t* ad = nullptr;
static std::vector<int16_t> audio_buffer;
static std::atomic<bool> grabando(false);
static std::thread grabacion_thread;
static std::mutex audio_mutex;
static std::condition_variable cv;

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

extern "C" void iniciar_grabacion_microfono() {
    if (grabando) return;
    ad = ad_open_dev(NULL, 16000);
    if (!ad) {
        std::cerr << "No se pudo abrir el micrófono" << std::endl;
        return;
    }
    if (ad_start_rec(ad) < 0) {
        std::cerr << "No se pudo iniciar la grabación" << std::endl;
        ad_close(ad);
        ad = nullptr;
        return;
    }
    grabando = true;
    audio_buffer.clear();
    grabacion_thread = std::thread([]() {
        int16_t buf[2048];
        while (grabando) {
            int32_t k = ad_read(ad, buf, 2048);
            if (k > 0) {
                std::lock_guard<std::mutex> lock(audio_mutex);
                audio_buffer.insert(audio_buffer.end(), buf, buf + k);
            }
        }
        cv.notify_one();
    });
}

extern "C" void detener_y_reconocer_microfono(char* buffer, int buffer_size) {
    if (!grabando) {
        strncpy(buffer, "No se estaba grabando", buffer_size-1);
        buffer[buffer_size-1] = '\0';
        return;
    }
    grabando = false;
    if (ad) ad_stop_rec(ad);
    if (grabacion_thread.joinable()) grabacion_thread.join();
    // Reconocimiento
    cmd_ln_t* config = cmd_ln_init(NULL, (const arg_t*) ps_args(), TRUE,
        "-hmm", "/usr/share/pocketsphinx/model/en-us/en-us",
        "-lm", "/usr/share/pocketsphinx/model/en-us/en-us.lm.bin",
        "-dict", "/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict",
        NULL);
    if (!config) {
        strncpy(buffer, "Error de configuración", buffer_size-1);
        buffer[buffer_size-1] = '\0';
        if (ad) { ad_close(ad); ad = nullptr; }
        return;
    }
    ps_decoder_t* ps = ps_init(config);
    if (!ps) {
        strncpy(buffer, "Error al inicializar PocketSphinx", buffer_size-1);
        buffer[buffer_size-1] = '\0';
        cmd_ln_free_r(config);
        if (ad) { ad_close(ad); ad = nullptr; }
        return;
    }
    ps_start_utt(ps);
    {
        std::lock_guard<std::mutex> lock(audio_mutex);
        if (!audio_buffer.empty())
            ps_process_raw(ps, audio_buffer.data(), audio_buffer.size(), FALSE, TRUE);
    }
    ps_end_utt(ps);
    const char* hyp = ps_get_hyp(ps, NULL);
    if (hyp) {
        strncpy(buffer, hyp, buffer_size-1);
        buffer[buffer_size-1] = '\0';
    } else {
        strncpy(buffer, "No se reconoció ninguna frase", buffer_size-1);
        buffer[buffer_size-1] = '\0';
    }
    ps_free(ps);
    cmd_ln_free_r(config);
    if (ad) { ad_close(ad); ad = nullptr; }
    audio_buffer.clear();
}
