#include "au.h"
#include <chrono>
#include <thread>

bool IsAuFile(std::istream& file) {
	file.seekg(0); // B начало файла.
	long magic = ReadIntMsb(file, 4);
	return (magic == 0x2E736E64); // Должно быть ".snd".
}

size_t AuRead::ReadBytes(AudioByte* buffer, size_t length) {
	if (length > _dataLength) { length = _dataLength; }
	_stream.read(reinterpret_cast<char*>(buffer), length);
	size_t lengthRead = _stream.gcount();
	_dataLength -= lengthRead;
	return lengthRead;
}

void AuRead::ReadHeader(void) {
	if (_headerRead) return;
	_headerRead = true;
	char header[24];
	_stream.read(header, 24);
	long magic = BytesToIntMsb(header + 0, 4);
	if (magic != 0x2E736E64) { // ".snd".
	// Формат исходного файла
	// отличается от AU.
		std::cerr << "Input file is not an AU file.\n";
		exit(1);
	}
	long headerLength = BytesToIntMsb(header + 4, 4);
	_dataLength = BytesToIntMsb(header + 8, 4);
	int format = BytesToIntMsb(header + 12, 4);
	_headerRate = BytesToIntMsb(header + 16, 4);
	_headerChannels = BytesToIntMsb(header + 20, 4);
	SkipBytes(_stream, headerLength - 24); // Отбрасываем
	// оставшуюся
	// часть заголовка.
	// Создаем подходящий объект-декомпрессор.
	switch (format) {
	case 1: // ITU G.711 мю-функция.
		_decoder = new DecompressG711MuLaw(*this);
		break;
	case 2: // 8-битный линейный.
		_decoder = new DecompressPcm8Unsigned(*this);
		break;
	case 3: // 16-битный линейный.
		_decoder = new DecompressPcm16MsbSigned(*this);
		break;
	default:
		// AU формат ........ не поддерживается.
		std::cerr << "AU format " << format << " not supported.\n";
		exit(1);
	}
	// Частота дискретизации:
	std::cerr << "Sampling Rate: " << _headerRate;
	// Каналы:
	std::cerr << " Channels: " << _headerChannels;
	std::cerr << "\n";
};

static void WriteBuffer
(std::ostream& out, AudioSample* buffer, int length) {
	AudioSample* sampleBuff = buffer;
	AudioByte* byteBuff =
		reinterpret_cast<AudioByte*>(buffer);
	int i = length;
	while (i-- > 0) {
		int sample = *sampleBuff++;
		*byteBuff++ = sample >> 8;
		*byteBuff++ = sample;
	}
	out.write(reinterpret_cast<char*>(buffer), length * 2);
};

void AuWrite::Play(void) {
	int samplingRate = SamplingRate();
	int channels = Channels();
	// Запись заголовка AU-файла.
	_stream.write(".snd", 4); // "Волшебный".
	WriteIntMsb(_stream, 28L, 4); // Длина заголовка.
	WriteIntMsb(_stream, 0x7FFFFFFFL, 4); // Длина данных.
	WriteIntMsb(_stream, 3L, 4); // 1б-битный линейный.
	WriteIntMsb(_stream, samplingRate, 4); // Частота дискретизации.
	WriteIntMsb(_stream, channels, 4); // Число каналов.
	WriteIntMsb(_stream, 0, 4); // Для выравнивания.
	// Читаем аудиоданные
	// и пишем их в файл.
	long length;
	do {
		const int BuffSize = 10240;
		AudioSample buff[BuffSize];
		length = Previous()->GetSamples(buff, BuffSize);
		WriteBuffer(_stream, buff, length);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	} while (length > 0);
}