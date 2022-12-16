#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
#include <typeinfo>
#include <iostream>
#include <cstddef>

typedef short AudioSample; // ��������� ������������.
typedef unsigned char AudioByte; // 8-������ ����������� ����.
long ReadIntMsb(std::istream& in, int bytes);
long BytesToIntMsb(void* buff, int bytes);
long ReadIntLsb(std::istream& in, int bytes);
long BytesToIntLsb(void* buff, int bytes);
void SkipBytes(std::istream& in, int bytes);
void WriteIntMsb(std::ostream& out, long l, int bytes);
void WriteIntLsb(std::ostream& out, long l, int bytes);


class AudioAbstract {
private:
	AudioAbstract* _previous; // ������, �� �������� ���������
	// �������� ������.
	AudioAbstract* _next; // ������, ���������� ������
	// �� ������ �������.
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
			// He ���� �������� ����� �������.
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

	// ���������� ���������� ������� ��������� �������
	// ����� ��� 0 � ������ ������. ���� �� ��������� ������
	// ��� �� ��������� ����� �����, ���� ����� ������ ������
	// ���������� ������ ���������.
	virtual size_t GetSamples(AudioSample*, size_t) = 0;

	virtual size_t ReadBytes(AudioByte* buff, size_t length) {
		return Previous()->ReadBytes(buff, length);
	};

	virtual long SamplingRate(void) {
		if (!_samplingRateFrozen) // �������� �� �������������?
			NegotiateSamplingRate(); // ����� ���������� ���.
		return _samplingRate; // ���������� ��������.
	};

	virtual void SamplingRate(long s) { // �������������
	// ������� �������������.
		if (_samplingRateFrozen) {
			// He ���� ��������
			// ������� �������������.
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