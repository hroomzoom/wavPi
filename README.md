# wavPi

**wavPi** is a C and C++ based audio processing project designed for the Raspberry Pi, Orange Pi and other Embedded Linux Device, capable of real-time audio playback, frequency shifting, FIR filtering, and modulation effects using Low-Frequency Oscillators (LFOs). This project is ideal for experimentation with audio signal processing, music production, or educational purposes.

## Features
- **Real-Time Audio Processing**: Stream audio in real time with minimal latency.
- **Frequency Shifting**: Shift the frequency of audio signals to create unique effects.
- **FIR Filtering**: Apply Finite Impulse Response (FIR) filtering for precise audio signal modification.
- **Modulation Effects**: Use Low-Frequency Oscillators (LFOs) for effects like vibrato or tremolo.
- **Cross-Platform Compatibility**: While optimized for the Raspberry Pi, wavPi can run on other Linux systems.

## Hardware Requirements
- Raspberry Pi Zero 2 W (or compatible Raspberry Pi model)
- USB microphone or audio input device
- USB audio output or speakers
- Optional: Amplifier or additional audio accessories

## Software Requirements
- GCC or any C/C++ compiler compatible with Linux
- Raspbian (or any Debian-based Linux OS for Raspberry Pi)
- Required libraries (see the Installation section below)

## Installation
1. **Clone the Repository**
   ```bash
   git clone https://github.com/hroomzoom/wavPi.git
   cd wavPi
   ```

2. **Build the Project**
   Use `make` to build the project:
   ```bash
   sudo make
   ```

3. **Set Up Audio Devices**
   Ensure your Raspberry Pi audio devices are properly configured.
   - Use `alsamixer` to adjust volume levels.
   - Confirm audio input and output devices with `arecord -l` and `aplay -l`.

4. **Run the Program**
   Start the audio processing application:
   ```bash
   ./wavpi <path/to/wav>
   ```

## Usage
- **FIR Filtering**: Configure FIR filter coefficients in the provided code.
- **Modulation Effects**: Enable and customize LFO parameters within the application.

## Inspiration
This project was inspired by experimenting with the Spotify Developer API. Those initial explorations ignited a passion for deeper development in music and audio processing, leading to the creation of wavPi.

## Acknowledgments
Special thanks to Spotify for providing the Developer API, which inspired the creation of wavPi, and to the Raspberry Pi Foundation for offering the tools and inspiration for makers worldwide!


