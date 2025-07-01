// audio.h
#ifndef AUDIO_H
#define AUDIO_H

void iniciar_audio();
void detener_audio();
void audio_a_texto(char* buffer, int buffer_size);
extern "C" void iniciar_grabacion_microfono();
extern "C" void detener_y_reconocer_microfono(char* buffer, int buffer_size);

#endif
