#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#define SAMPLE_RATE 44100
#define PAYLOAD "this is a secret message"
#define WPM 10 // Assuming "PARIS"
#define DIT_LENGTH 1.0f/(WPM*50.0f/60.0f) * 1000
#define DAH_LENGTH 3 * DIT_LENGTH
#define MAX_TAPE_SIZE 4096

float wave = 0.0f;

bool state = false;
int paCallback(const void* input, float* output,
	unsigned long frame_count,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags statusFlags,
	bool* state) {

	for (int i=0; i < frame_count; i++) {
		wave += 0.023f;
		*(output++) = *state ? wave : 0;
		if (wave >= 1.0f) {
			wave -= 2.0f;
		}
	}

	return 0;
}

const char* MORSE_SPACE = "       ";
const char* MORSE_GAP = "   ";

// 0-9, a-z
const char* MORSE_DICT[] = {
	"-----",
	".----",
	"..---",
	"...--",
	"....-",
	".....",
	"-....",
	"--...",
	"---..",
	"----.",
	".-",
	"-...",
	"-.-.",
	"-..",
	".",
	"..-.",
	"--.",
	"....",
	"..",
	".---",
	"-.-",
	".-..",
	"--",
	"-.",
	"---",
	".--.",
	"--.-",
	".-.",
	"...",
	"-",
	"..-",
	"...-",
	".--",
	"-..-",
	"-.--",
	"--..",
};

int main(int argc, char** argv) {
	PaStream* stream;
	PaError err;

	char tape[MAX_TAPE_SIZE] = {0};
	int tape_index = 0;

	// Build the tape
	for (int i=0; i < strlen(PAYLOAD); i++) {
		char c = PAYLOAD[i];
		// numbers or lowercase characters or space
		if (c >= 48 && c <= 57 || c >= 97 && c <= 122 || c == 32) {

			if (c == 32) {
				strcpy(tape + tape_index, MORSE_SPACE);
				tape_index += strlen(MORSE_SPACE);
				continue;
			}

			const char* signal;

			if (c >= 48 && c <= 57) {
				signal = MORSE_DICT[c-48];
			}

			if (c >= 97 && c <= 122) {
				signal = MORSE_DICT[c-87];
			}

			strcpy(tape + tape_index, signal);
			tape_index += strlen(signal);
			strcpy(tape + tape_index, MORSE_GAP);
			tape_index += strlen(MORSE_GAP);
		} else {
			fprintf(stderr, "Illegal char %c in payload", c);
			return 1;
		}
	}

	printf("Encoded payload:\n%s\n", tape);

	err = Pa_Initialize();
	if (err != paNoError) {
		
	}

	bool state = false;
	err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 128, (PaStreamCallback*) paCallback, &state);
	if (err != paNoError) goto error;

	int dit_length = DIT_LENGTH;

	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	for (int i=0; i < MAX_TAPE_SIZE; i++) {
		char c = tape[i];
		switch(c) {
			case '-':
				state = true;
				Pa_Sleep(DAH_LENGTH);
				break;
			case '.':
				state = true;
				Pa_Sleep(DIT_LENGTH);
				break;
			case ' ':
				state = false;
				Pa_Sleep(DIT_LENGTH);
				break;
		}

		if (c != ' ') {
			state = false;
			Pa_Sleep(DIT_LENGTH);
		}
	}

	err = Pa_StopStream(stream);
	if (err != paNoError) goto error;

	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	err = Pa_Terminate();
	if (err != paNoError) goto error;

	return 0;

error:
	Pa_Terminate();
	fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(err));
	return 1;
}
