#ifndef AU_H_INCLUDED
#define AU_H_INCLUDED
#include "audio.h"
#include "compress.h"
#include "aplayer.h"
#include "g711.h"
#include <iostream>

bool IsAuFile(std::istream& file);

class AuRead : public AudioAbstract {
private:
	std::istream& _stream;
	AbstractDecompressor* _decoder;
	size_t _dataLength;
	bool _headerRead; // Истина, если заголовок уже прочтен.
	int _headerChannels; // Каналы из заголовка.
	int _headerRate; // Частота дискретизации из заголовка.
	void ReadHeader(void);

protected:
	void MinMaxSamplingRate(long* min, long* max, long* preferred)
	{
		ReadHeader();
		*min = *max = *preferred = _headerRate;
	}
	void MinMaxChannels(int* min, int* max, int* preferred) {
		ReadHeader();
		*min = *max = *preferred = _headerChannels;
	}

public:
	AuRead(std::istream& input = std::cin) :AudioAbstract(), _stream(input) {
		// Формат файла: Sun AU
		// (также известен как NeXT SND).
		std::cerr << "File Format: Sun AU (also known as NeXT SND)\n";
		_headerRead = false; // Заголовок еще не читали.
		_decoder = 0;
	}
	~AuRead() {
		if (_decoder) delete _decoder;
	}
	size_t GetSamples(AudioSample* buffer, size_t numSamples) {
		return _decoder->GetSamples(buffer, numSamples);
	}
	size_t ReadBytes(AudioByte* buffer, size_t length);
};

class AuWrite : public AbstractPlayer {
private:
	std::ostream& _stream;
public:
	AuWrite(AudioAbstract* audio, std::ostream& output = std::cout)
		:AbstractPlayer(audio), _stream(output) {
	};
	void Play(void);
};

#endif