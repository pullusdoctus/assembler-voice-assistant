# assembler-voice-assistant
A Voice Assistant powered by x86 and an LLM

## Requisitos y dependencias

- Linux x86_64
- NASM (ensamblador)
- g++ (compilador C++)
- SDL2
- SDL2_ttf
- SDL2_image
- pocketsphinx
- pocketsphinx-en-us

## Instalación de dependencias en Ubuntu/Debian

```bash
sudo apt update
sudo apt install build-essential nasm libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev pocketsphinx pocketsphinx-en-us
```

## Compilación

```bash
make
```

## Ejecución

```bash
./ui_demo
```

## Notas
- El archivo `db.txt` debe estar en la carpeta `src/`.
- El ejecutable principal es la interfaz gráfica (`ui_demo`).
