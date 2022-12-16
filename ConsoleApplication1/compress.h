#ifndef COMPR_H_INCLUDED
#define COMPR_H_INCLUDED

#include "audio.h"
#include <iostream>
#include <cstddef>

class AbstractDecompressor {
protected:
	AudioAbstract& _dataSource; // Объект, из которого будем
	// получать необработанные байты.
	size_t ReadBytes(AudioByte* buff, size_t length) {
		return _dataSource.ReadBytes(buff, length);
	};
public:
	AbstractDecompressor(AudioAbstract& a) : _dataSource(a) {};
	virtual ~AbstractDecompressor() {};
	virtual size_t GetSamples(AudioSample*, size_t) = 0;
	virtual void MinMaxSamplingRate(long*, long*, long*) {
		std::cerr << "MinMaxSamplingRate undefined\n";
		exit(1);
	}
	virtual void MinMaxChannels(int*, int*, int*) {
		std::cerr << "MinMaxChannels undefined\n";
		exit(1);
	}
};

class DecompressPcm8Signed : public AbstractDecompressor {
public:
	DecompressPcm8Signed(AudioAbstract& a) :
		AbstractDecompressor(a) {
		// Кодировка: 8-битная знаковая (дополнительный код) ИКМ.
		std::cerr << "Encoding: 8-bit signed (two's complement) PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

class DecompressPcm8Unsigned : public AbstractDecompressor {
public:
	DecompressPcm8Unsigned(AudioAbstract& a) :
		AbstractDecompressor(a) {
		// Кодировка:8-6итная беззнаковая (код с избытком 128)ИKM
		std::cerr << "Encoding: 8-bit unsigned (excess-128) PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

class DecompressPcm16MsbSigned : public AbstractDecompressor {
public:
	DecompressPcm16MsbSigned(AudioAbstract& a) :
		AbstractDecompressor(a) {
		// Кодировка: 16-битная MSB ИКМ.
		std::cerr << "Encoding: 16-bit MSB PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

class DecompressPcm16LsbSigned : public AbstractDecompressor {
public:
	DecompressPcm16LsbSigned(AudioAbstract& a) :
		AbstractDecompressor(a) {
		// Кодировка:16-битная LSB ИКМ
		std::cerr << "Encoding: 16-bit LSB PCM\n";
	};
	size_t GetSamples(AudioSample* buffer, size_t length);
};

#endif