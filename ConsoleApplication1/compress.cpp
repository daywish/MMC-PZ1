#include "compress.h"
#include "audio.h"
#include "g711.h"

static unsigned char numBits[] = {
0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

size_t DecompressPcm8Signed::GetSamples(AudioSample* buffer,
	size_t length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t samplesRead = ReadBytes(byteBuff, length);
	for (long i = samplesRead - 1; i >= 0; i--)
		buffer[i] = static_cast<AudioSample>(byteBuff[i])
		<< ((sizeof(AudioSample) - 1) * 8);
	return samplesRead;
}

size_t DecompressPcm8Unsigned::GetSamples(AudioSample* buffer, size_t length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t samplesRead = ReadBytes(byteBuff, length);
	for (long i = samplesRead - 1; i >= 0; i--)
		buffer[i] = static_cast<AudioSample>(byteBuff[i] ^ 0x80)
		<< ((sizeof(AudioSample) - 1) * 8);
	return samplesRead;
}

size_t DecompressPcm16MsbSigned::GetSamples(AudioSample* buffer,
	size_t length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t read = ReadBytes(byteBuff, length * 2) / 2;
	for (long i = read - 1; i >= 0; i--) {
		short s = static_cast<AudioSample>(byteBuff[2 * i]) << 8;
		s |= static_cast<AudioSample>(byteBuff[2 * i + 1]) & 255;
		buffer[i] = static_cast<AudioSample>(s)
			<< ((sizeof(AudioSample) - 2) * 8);
	}
	return read;
}

size_t DecompressPcm16LsbSigned::GetSamples(AudioSample* buffer,
	size_t length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t read = ReadBytes(byteBuff, length * 2) / 2;
	for (long i = read - 1; i >= 0; i--) {
		short s = static_cast<AudioSample>(byteBuff[2 * i + 1]) << 8;
		s |= static_cast<AudioSample>(byteBuff[2 * i]) & 255;
		buffer[i] = static_cast<AudioSample>(s)
			<< ((sizeof(AudioSample) - 2) * 8);
	}
	return read;
}

#include "g711.h"

static bool muLawDecodeTableInitialized = false;
static AudioSample muLawDecodeTable[256];
// Конструктор инициализирует таблицу декодирования.
DecompressG711MuLaw::DecompressG711MuLaw(AudioAbstract& a)
	: AbstractDecompressor(a) {
	// Кодировка: мю-типа, стандарт ITU G.711
	std::cerr << "Encoding: ITU G.711 mu-Law\n";
	if (!muLawDecodeTableInitialized) {
		muLawDecodeTableInitialized = true;
		for (int i = 0; i < 256; i++)
			muLawDecodeTable[i] = MuLawDecode(i);
	}
}

size_t DecompressG711MuLaw::GetSamples(AudioSample* buffer,
	size_t length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t read = ReadBytes(byteBuff, length);
	for (long i = read - 1; i >= 0; i--)
		buffer[i] = muLawDecodeTable[byteBuff[i]];
	return read;
};

AudioByte MuLawEncode(AudioSample s) {
	unsigned char sign = (s < 0) ? 0 : 0x80; // Записываем знак.
	if (s < 0) s = -s; // Делаем выборку
	// положительной.
	signed long adjusted = static_cast<long>(s) << (16 -
		sizeof(AudioSample) * 8);
	adjusted += 128L + 4L;
	if (adjusted > 32767) adjusted = 32767;
	unsigned char exponent = numBits[(adjusted >> 7) & 0xFF] - 1;
	unsigned char mantissa = (adjusted >> (exponent + 3)) & 0xF;
	return ~(sign | (exponent << 4) | mantissa);
};

AudioSample MuLawDecode(AudioByte ulaw) {
	ulaw = ~ulaw;
	unsigned char exponent = (ulaw >> 4) & 0x7;
	unsigned char mantissa = (ulaw & 0xF) + 16;
	unsigned long adjusted = (mantissa << (exponent + 3)) - 128 - 4;
	return (ulaw & 0x80) ? adjusted : adjusted;
};

static bool aLawDecodeTableInitialized = false;
static AudioSample aLawDecodeTable[256];
DecompressG711ALaw::DecompressG711ALaw(AudioAbstract& a)
	: AbstractDecompressor(a) {
	// /Кодировка А-типа стандарта ITU G.711
	std::cerr << "Encoding: ITU G.711 A-Law\n";
	if (!aLawDecodeTableInitialized) {
		aLawDecodeTableInitialized = true;
		for (int i = 0; i < 256; i++)
			aLawDecodeTable[i] = ALawDecode(i);
	}
}
size_t DecompressG711ALaw::GetSamples(AudioSample* buffer, size_t
	length) {
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	size_t read = ReadBytes(byteBuff, length);
	for (long i = read - 1; i >= 0; i--)
		buffer[i] = aLawDecodeTable[byteBuff[i]];
	return read;
}
AudioByte ALawEncode(AudioSample s) {
	unsigned char sign = (s < 0) ? 0 : 0x80; // Сохраняем знак.
	if (s < 0) s = -s; // Делаем отсчет
	// положительным.
	signed long adjusted = static_cast<long>(s) + 8L;
	// Округляем его .
	if (adjusted > 32767) adjusted = 32767; // Ограничиваем.
	unsigned char exponent = numBits[(adjusted >> 8) & 0x7F];
	unsigned char mantissa = (adjusted >> (exponent + 4)) & 0xF;
	return sign | (((exponent << 4) | mantissa) ^ 0x55);
};
AudioSample ALawDecode(AudioByte alaw) {
	alaw ^= 0x55;
	unsigned char exponent = (alaw >> 4) & 0x7;
	unsigned char mantissa = (alaw & 0xF) + (exponent ? 16 : 0);
	unsigned long adjusted = (mantissa << (exponent + 4));
	return (alaw & 0x80) ? adjusted : adjusted;
};