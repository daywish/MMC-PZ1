#include "audio.h"

void AudioAbstract::NegotiateSamplingRate(void) {
	if (Next()) // Объект крайний слева?
		Next()->NegotiateSamplingRate(); // Нет, продолжаем работу.
	else { // Да.
		long min = 8000, max = 44100, preferred = 44100;
		MinMaxSamplingRate(&min, &max, &preferred); // Получаем
		// предпочтительные значения.
		if (min > max) { // Проверка на недопустимый ответ.
		// He удается согласовать
		// частоту дискретизации.
			std::cerr << "Couldn't negotiate sampling rate.\n";
			exit(1);
			SetSamplingRateRecursive(preferred); // Устанавливаем
			// во всех объектах
			// выбранные значения.
		}
	}
}

void AudioAbstract::MinMaxSamplingRate(long* min, long* max,
	long* preferred) {
	if (Previous()) Previous() -> MinMaxSamplingRate(min, max, preferred);
	if (_samplingRate) *preferred = _samplingRate;
	if (*preferred < *min) *preferred = *min;
	if (*preferred > *max) *preferred = *max;
}

void AudioAbstract::SetSamplingRateRecursive(long s) {
	if (Previous()) // Сначала устанавливаем
	// для того, что справа.
		Previous()->SetSamplingRateRecursive(s);
	SamplingRate(s); // Устанавливаем.
	_samplingRateFrozen = true; // Да, частота согласована.
}

void AudioAbstract::NegotiateChannels(void) {
	if (Next())
		Next()->NegotiateChannels();
	else {
		int min = 1, max = 2, preferred = 1; // Значения, используемые
		// по умолчанию.
		MinMaxChannels(&min, &max, &preferred);
		if (min > max) {
			// He удалось согласовать
			// частоту дискретизации.
			std::cerr << "Couldn't negotiate sampling rate.\n";
			exit(1);
		}
		SetChannelsRecursive(preferred);
	}
}
void AudioAbstract::MinMaxChannels(int* min, int* max, int* preferred) {
	if (Previous()) Previous()->MinMaxChannels(min, max, preferred);
	if (_channels) *preferred = _channels;
	if (*preferred < *min) *preferred = *min;
	if (*preferred > *max) *preferred = *max;
}

void AudioAbstract::SetChannelsRecursive(int ch) {
	if (Previous()) Previous()->SetChannelsRecursive(ch);
	Channels(ch);
	_channelsFrozen = true;
}

long ReadIntMsb(std::istream& in, int size) {
	if (size <= 0) return 0;
	long l = ReadIntMsb(in, size - 1) << 8;
	l |= static_cast<long>(in.get()) & 255;
	return l;
}
long BytesToIntMsb(void* vBuff, int size) {
	unsigned char* buff = reinterpret_cast<unsigned char
		*>(vBuff);
	if (size <= 0) return 0;
	long l = BytesToIntMsb(buff, size - 1) << 8;
	l |= static_cast<long>(buff[size - 1]) & 255;
	return l;
}
long ReadIntLsb(std::istream& in, int size) {
	if (size <= 0) return 0;
	long l = static_cast<long>(in.get()) & 255;
	l |= ReadIntLsb(in, size - 1) << 8;
	return l;
}
long BytesToIntLsb(void* vBuff, int size) {
	unsigned char* buff = reinterpret_cast<unsigned char
		*>(vBuff);
	if (size <= 0) return 0;
	long l = static_cast<long>(*buff) & 255;
	l |= BytesToIntLsb(buff + 1, size - 1) << 8;
	return l;
}
void SkipBytes(std::istream& in, int size) {
	while (size-- > 0)
		in.get();
}

void WriteIntMsb(std::ostream& out, long l, int size) {
	if (size <= 0) return;
	WriteIntMsb(out, l >> 8, size - 1); // Пишем старшие байты.
	out.put(l & 255); // Пишем младший байт.
}
void WriteIntLsb(std::ostream& out, long l, int size) {
	if (size <= 0) return;
	out.put(l & 255); // Пишем младший байт.
	WriteIntLsb(out, l >> 8, size - 1); // Пишем все остальное.
}
