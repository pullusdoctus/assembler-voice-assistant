// audio.cpp
// Procesamiento de audio y reconocimiento de voz con PocketSphinx
#include "audio.h"
#include <pocketsphinx.h>
#include <portaudio.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <vector>

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 512
#define RECORD_SECONDS 5

// Variables estáticas para la grabación
static PaStream* stream = nullptr;
static std::vector<int16_t> audio_buffer;
static std::atomic<bool> grabando(false);
static std::thread grabacion_thread;
static std::mutex audio_mutex;
static std::condition_variable cv;

void audio_a_texto(char* buffer, int buffer_size) {
    PaStream *stream;
    PaError err;

    err = Pa_Initialize();
    if (err != paNoError) {
        snprintf(buffer, buffer_size, "Error inicializando PortAudio");
        return;
    }

    err = Pa_OpenDefaultStream(&stream,
                               1, // input channels
                               0, // output channels
                               paInt16, // 16-bit int samples
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               nullptr,
                               nullptr);

    if (err != paNoError) {
        snprintf(buffer, buffer_size, "Error abriendo el micrófono");
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        snprintf(buffer, buffer_size, "No se pudo iniciar la grabación");
        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }

    std::cout << "Habla ahora (grabando durante " << RECORD_SECONDS << " segundos)..." << std::endl;

    size_t total_samples = SAMPLE_RATE * RECORD_SECONDS;
    std::vector<int16_t> audio_data(total_samples);
    size_t index = 0;

    while (index < total_samples) {
        int16_t temp[FRAMES_PER_BUFFER];
        err = Pa_ReadStream(stream, temp, FRAMES_PER_BUFFER);
        if (err != paNoError) {
            snprintf(buffer, buffer_size, "Error al leer del micrófono");
            Pa_StopStream(stream);
            Pa_CloseStream(stream);
            Pa_Terminate();
            return;
        }
        size_t to_copy = std::min<size_t>(FRAMES_PER_BUFFER, total_samples - index);
        std::memcpy(audio_data.data() + index, temp, to_copy * sizeof(int16_t));
        index += to_copy;
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    // === Reconocimiento con PocketSphinx ===
    ps_config_t *config = ps_config_init(NULL);

    ps_config_set_str(config, "hmm", "/usr/local/share/pocketsphinx/model/en-us/en-us");
    ps_config_set_str(config, "lm", "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin");
    ps_config_set_str(config, "dict", "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict");
    if (!config) {
        snprintf(buffer, buffer_size, "Error de configuración");
        return;
    }

    ps_decoder_t *ps = ps_init(config);
    if (!ps) {
        snprintf(buffer, buffer_size, "Error al inicializar PocketSphinx");
        ps_config_free(config);
        return;
    }

    ps_start_utt(ps);
    ps_process_raw(ps, audio_data.data(), audio_data.size(), FALSE, TRUE);
    ps_end_utt(ps);

    const char* hyp = ps_get_hyp(ps, NULL);
    if (hyp) {
        strncpy(buffer, hyp, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        snprintf(buffer, buffer_size, "No se reconoció ninguna frase");
    }

    ps_free(ps);
    ps_config_free(config);
}

extern "C" void iniciar_grabacion_microfono() {
    if (grabando) return;

    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "Error al inicializar PortAudio\n";
        return;
    }

    err = Pa_OpenDefaultStream(&stream,
                               1, 0,         // 1 input channel, 0 output
                               paInt16,
                               SAMPLE_RATE,
                               FRAMES_PER_BUFFER,
                               nullptr,
                               nullptr);
    if (err != paNoError) {
        std::cerr << "No se pudo abrir el micrófono\n";
        Pa_Terminate();
        return;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "No se pudo iniciar la grabación\n";
        Pa_CloseStream(stream);
        Pa_Terminate();
        return;
    }

    grabando = true;
    audio_buffer.clear();

    grabacion_thread = std::thread([]() {
        int16_t buf[FRAMES_PER_BUFFER];
        while (grabando) {
            PaError err = Pa_ReadStream(stream, buf, FRAMES_PER_BUFFER);
            if (err == paNoError) {
                std::lock_guard<std::mutex> lock(audio_mutex);
                audio_buffer.insert(audio_buffer.end(), buf, buf + FRAMES_PER_BUFFER);
            }
        }
        cv.notify_one();
    });
}

extern "C" void detener_y_reconocer_microfono(char* buffer, int buffer_size) {
    if (!grabando) {
        strncpy(buffer, "No se estaba grabando", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        return;
    }

    grabando = false;
    if (grabacion_thread.joinable()) grabacion_thread.join();

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    // PocketSphinx config and recognition
    ps_config_t* config = ps_config_init(NULL);
    ps_config_set_str(config, "hmm", "/usr/local/share/pocketsphinx/model/en-us/en-us");
    ps_config_set_str(config, "lm", "/usr/local/share/pocketsphinx/model/en-us/en-us.lm.bin");
    ps_config_set_str(config, "dict", "/usr/local/share/pocketsphinx/model/en-us/cmudict-en-us.dict");

    ps_decoder_t* ps = ps_init(config);
    if (!ps) {
        strncpy(buffer, "Error al inicializar PocketSphinx", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
        ps_config_free(config);
        return;
    }

    ps_start_utt(ps);
    {
        std::lock_guard<std::mutex> lock(audio_mutex);
        if (!audio_buffer.empty()) {
            ps_process_raw(ps, audio_buffer.data(), audio_buffer.size(), FALSE, TRUE);
        }
    }
    ps_end_utt(ps);

    const char* hyp = ps_get_hyp(ps, NULL);
    if (hyp) {
        strncpy(buffer, hyp, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    } else {
        strncpy(buffer, "No se reconoció ninguna frase", buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }

    ps_free(ps);
    ps_config_free(config);
    audio_buffer.clear();
}