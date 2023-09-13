#include <portaudio.h>
#include <stdio.h>
#define SAMPLE_RATE 44100

float wave = 0.0f;

int paCallback(const void* input, float* output,
	unsigned long frame_count,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags statusFlags,
	float* user_data) {

	float wave = *user_data;

	for (int i=0; i < frame_count; i++) {
		wave += 0.023f;
		*(output++) = wave;
		if (wave >= 1.0f) {
			wave -= 2.0f;
		}
	}

	return 0;
}

int main(int argc, char* argv) {
	PaStream* stream;
	PaError err;

	err = Pa_Initialize();
	if (err != paNoError) {
		
	}

	err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 256, paCallback, &wave);
	if( err != paNoError ) goto error;

	err = Pa_StartStream(stream);
	if( err != paNoError ) goto error;

	Pa_Sleep(1000);

	err = Pa_StopStream(stream);
	if( err != paNoError ) goto error;

	err = Pa_CloseStream(stream);
	if( err != paNoError ) goto error;

	err = Pa_Terminate();
	if( err != paNoError ) goto error;

	return 0;

error:
	Pa_Terminate();
	fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(err));
}
