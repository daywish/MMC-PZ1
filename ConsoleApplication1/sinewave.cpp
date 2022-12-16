#pragma once
#include "audio.h"
#include "sinewave.h"
#include <cmath>

size_t SineWave::GetSamples(AudioSample* buff, size_t samplesWanted)
{
	AudioSample* p = buff;
	long samplesCopied = 0;
	while ((samplesWanted > 1)) {
		*p = sine[pos];
		pos += frequency;
		if (pos > length) pos -= length;
		samplesWanted--;
		samplesCopied++;
	    p++;
	}
	return samplesCopied;
}

void SineWave::BuildTable(long rate) {
	if (sine) { delete[] sine; }
	length = rate;
	sine = new AudioSample[length];
	double scale = 2.0 * (3.14159265358) /
		static_cast<double>(length);
	for (int i = 0; i < length; i++) {
		sine[i] = int(32767 * sin(static_cast<double>(i) * scale));
	}
}
