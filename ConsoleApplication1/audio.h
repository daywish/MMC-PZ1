#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
#include <typeinfo>
#include <iostream>
#include <cstddef>

typedef short AudioSample; // Отдельная аудиовыборка.
typedef unsigned char AudioByte; // 8-битный беззнаковый байт.
long ReadIntMsb(std::istream& in, int bytes);
long BytesToIntMsb(void* buff, int bytes);
long ReadIntLsb(std::istream& in, int bytes);
long BytesToIntLsb(void* buff, int bytes);
void SkipBytes(std::istream& in, int bytes);
void WriteIntMsb(std::ostream& out, long l, int bytes);
void WriteIntLsb(std::ostream& out, long l, int bytes);


class AudioAbstract {
private:
	AudioAbstract* _previous; // Объект, от которого требуется
	// получить данные.
	AudioAbstract* _next; // Объект, получающий данные
	// из нашего объекта.
	long _samplingRate;
	bool _samplingRateFrozen;

	long _channels;
	bool _channelsFrozen;
public:
	virtual int Channels(void) {
		if (!_channelsFrozen) NegotiateChannels();
		return _channels;
	};
	virtual void Channels(int ch) {
		if (_channelsFrozen) {
			// He могу изменить число каналов.
			std::cerr << "Can't change number of channels.\n";
			exit(1);
		}
		_channels = ch;
	};
	virtual void NegotiateChannels(void);
	virtual void MinMaxChannels(int* min, int* max, int
		* preferred);
	virtual void SetChannelsRecursive(int s);

public:
	AudioAbstract* Previous(void) { return _previous; }
	void Previous(AudioAbstract* c) { 
		_previous = c; 
	}
	AudioAbstract* Next(void) { return _next; }
	void Next(AudioAbstract* a) { _next = a; }
	AudioAbstract(void) {
		_previous = 0;
		_next = 0;
		_samplingRate = 0; _samplingRateFrozen = false;
		_channels = 0; _channelsFrozen = false;
	};

	AudioAbstract(AudioAbstract* audio) {
		_previous = audio;
		_next = 0;
		audio->Next(this);
		_samplingRate = 0; _samplingRateFrozen = false;
		_channels = 0; _channelsFrozen = false;
	};

	virtual ~AudioAbstract(void) {};

	// Возвращает количество реально считанных выборок
	// звука или 0 в случае ошибки. Если не произошло ошибки
	// или не достигнут конец файла, этот метод должен всегда
	// возвращать запрос полностью.
	virtual size_t GetSamples(AudioSample*, size_t) = 0;

	virtual size_t ReadBytes(AudioByte* buff, size_t length) {
		return Previous()->ReadBytes(buff, length);
	};

	virtual long SamplingRate(void) {
		if (!_samplingRateFrozen) // Значение не зафиксировано?
			NegotiateSamplingRate(); // Тогда определяем его.
		return _samplingRate; // Возвращаем значение.
	};

	virtual void SamplingRate(long s) { // Устанавливает
	// частоту дискретизации.
		if (_samplingRateFrozen) {
			// He могу изменить
			// частоту дискретизации.
			std::cerr << "Can't change sampling rate.\n";
			exit(1);
		}
		_samplingRate = s;
	};

	virtual void NegotiateSamplingRate(void);

	virtual void MinMaxSamplingRate(long* min, long* max, long
		* prefer);
	virtual void SetSamplingRateRecursive(long s);
};

#endif