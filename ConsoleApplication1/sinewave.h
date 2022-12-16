#pragma once
#ifndef SINEWAVE_H_INCLUDED 
#define SINEWAVE_H_INCLUDED 
#include "audio.h"
class SineWave : public AudioAbstract {
protected:
	void MinMaxChannels(int* min, int* max, int* preferred) {
		*min = *max = *preferred = 1;
	};
	size_t GetSamples(AudioSample* buff, size_t bytesWanted);
private:
	AudioSample* sine; // Таблица моментальных значений синусоиды.
	int length; // Длина вышеупомянутой таблицы.
	int pos; // Текущее положение в таблице,
	int frequency; // Требуемая частота выходного сигнала,
	void Init(void) {
		sine = (AudioSample*)0;
		length = 0; pos = 0; frequency = 1;
	}
	void BuildTable(long rate);
public:
	void Frequency(int f) { frequency = f; }
	SineWave(void) :AudioAbstract() { Init(); }
	SineWave(int f) :AudioAbstract() {
		Init();
		Frequency(f);
	};
	~SineWave(void) {
		if (sine) delete[] sine;
	}
	void SamplingRate(long rate) {
		AudioAbstract::SamplingRate(rate);
		BuildTable(rate);
	}
};
#endif