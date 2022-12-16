#include "au.h"
#include <chrono>
#include <thread>

bool IsAuFile(std::istream& file) {
	file.seekg(0); // B ������ �����.
	long magic = ReadIntMsb(file, 4);
	return (magic == 0x2E736E64); // ������ ���� ".snd".
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
	// ������ ��������� �����
	// ���������� �� AU.
		std::cerr << "Input file is not an AU file.\n";
		exit(1);
	}
	long headerLength = BytesToIntMsb(header + 4, 4);
	_dataLength = BytesToIntMsb(header + 8, 4);
	int format = BytesToIntMsb(header + 12, 4);
	_headerRate = BytesToIntMsb(header + 16, 4);
	_headerChannels = BytesToIntMsb(header + 20, 4);
	SkipBytes(_stream, headerLength - 24); // �����������
	// ����������
	// ����� ���������.
	// ������� ���������� ������-������������.
	switch (format) {
	case 1: // ITU G.711 ��-�������.
		_decoder = new DecompressG711MuLaw(*this);
		break;
	case 2: // 8-������ ��������.
		_decoder = new DecompressPcm8Unsigned(*this);
		break;
	case 3: // 16-������ ��������.
		_decoder = new DecompressPcm16MsbSigned(*this);
		break;
	default:
		// AU ������ ........ �� ��������������.
		std::cerr << "AU format " << format << " not supported.\n";
		exit(1);
	}
	// ������� �������������:
	std::cerr << "Sampling Rate: " << _headerRate;
	// ������:
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
	// ������ ��������� AU-�����.
	_stream.write(".snd", 4); // "���������".
	WriteIntMsb(_stream, 28L, 4); // ����� ���������.
	WriteIntMsb(_stream, 0x7FFFFFFFL, 4); // ����� ������.
	WriteIntMsb(_stream, 3L, 4); // 1�-������ ��������.
	WriteIntMsb(_stream, samplingRate, 4); // ������� �������������.
	WriteIntMsb(_stream, channels, 4); // ����� �������.
	WriteIntMsb(_stream, 0, 4); // ��� ������������.
	// ������ �����������
	// � ����� �� � ����.
	long length;
	do {
		const int BuffSize = 10240;
		AudioSample buff[BuffSize];
		length = Previous()->GetSamples(buff, BuffSize);
		WriteBuffer(_stream, buff, length);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	} while (length > 0);
}