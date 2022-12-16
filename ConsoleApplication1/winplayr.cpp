#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include "aplayer.h"
#include "winplayr.h"


void WinPlayer::Play(void) {
	if (SelectDevice()) return; // Открываем подходящее
	// устройство.
	waveOutPause(_device); // Пока не воспроизводим.
	_paused = true;
	InitializeQueue(128 * 1024L); // Выделяем 128 Кб на очередь.
	WAVEHDR waveHdr[2];
	for (int i = 0; i < 2; i++) {
		waveHdr[i].dwBufferLength // Размер в байтах.
			= winBufferSize * _sampleWidth / 8;
		waveHdr[i].dwFlags = 0;
		waveHdr[i].dwLoops = 0;
		waveHdr[i].lpData
			= reinterpret_cast<LPSTR>(
				new BYTE[waveHdr[i].dwBufferLength *
				Channels()]);
		waveOutPrepareHeader(_device, &waveHdr[i], sizeof(waveHdr[i]));
		NextBuff(&waveHdr[i]); // Заполнение и копирование
		// буфера в выходные данные.
	}
	// Ждем, пока воспроизведение
	// не прекратится, и оба буфера
	// не освободятся.
	_paused = false;
	waveOutRestart(_device); // Теперь начинаем
	// воспроизведение.
	while (!_endOfQueue // Очередь пуста?
		|| ((waveHdr[0].dwFlags & WHDR_DONE) == 0) // Данные
		// в буферах закончились?
		|| ((waveHdr[1].dwFlags & WHDR_DONE) == 0)) {
		FillQueue(); // Конец заполнения очереди.
		if (_paused) { // Если поток сервера находится
		// в состоянии паузы,
		// перезапускаем его.
			_paused = false;
			// Вывод звука перезапущен.
			std::cerr << "Sound output restarted.\n";
			waveOutRestart(_device);
		}
		Sleep(50 /* ms */); // Циклические повторения
		// приблизительно 20 раз
		// в секунду.
	}
	MMRESULT err = waveOutClose(_device);
	while (err == WAVERR_STILLPLAYING) { // Если воспроизведение
	// еще не закончилось...
		Sleep(250); // Ждем чуть-чуть...
		waveOutClose(_device); // Пробуем снова...
	};
	for (int i1 = 0; i1 < 2; i1++) {
		waveOutUnprepareHeader(_device, &waveHdr[i1], sizeof(waveHdr[i1]));
		delete[] waveHdr[i1].lpData;
	}
}

// Вызов подсистемы.
void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg,
	DWORD dwInstance, DWORD dwParaml, DWORD dwParam2) {
	WinPlayer* me = reinterpret_cast<WinPlayer*>(dwInstance);
	switch (uMsg) {
	case WOM_DONE: // C этим буфером закончено.
	{
		WAVEHDR* pWaveHdr = reinterpret_cast<WAVEHDR*>(dwParaml);
		me->NextBuff(pWaveHdr);
		break;
	}
	default:
		break;
	}
}

void WinPlayer::NextBuff(WAVEHDR* pWaveHdr) {
	long samplesRead = 0;
	switch (_sampleWidth) {
	case 16:
		samplesRead = FromQueue(
			reinterpret_cast<Sample16*>(pWaveHdr->lpData),
			winBufferSize);
		break;
	case 8:
		samplesRead = FromQueue(
			reinterpret_cast<Sample8*>(pWaveHdr->lpData),
			winBufferSize);
		break;
	}
	if (samplesRead != 0) { // Если есть данные, то их надо
	// записать.
		pWaveHdr->dwBufferLength = samplesRead * _sampleWidth / 8;
		waveOutWrite(_device, pWaveHdr, sizeof(*pWaveHdr));
	}
	else if (!_endOfQueue) { // Ой! Пропал источник сигнала.
		waveOutPause(_device); // Устройство вывода переводим
		// в состояние паузы.
		_paused = true;
		// Воспроизведение прервано из-за
		// перебоев с получением данных.
		std::cerr << "Sound output paused due to lack of data.\n";
		// Пишем несколько нулей, чтобы
		// сохранить блок в очереди
		// Windows.
		memset(pWaveHdr->lpData, 0, winBufferSize);
		pWaveHdr->dwBufferLength = 256;
		waveOutWrite(_device, pWaveHdr, sizeof(*pWaveHdr));
	}
	else { // Данные все, работа закончена.
		pWaveHdr->dwFlags |= WHDR_DONE; // Помечаем буфер как
		// прошедший обработку.
	}
}

// Рассмотрены базовые форматы, поддерживаемые Windows
static struct {
	DWORD format; // Константа.
	UINT rate; // Разделитель для этой константы.
	UINT channels;
	UINT width;
} winFormats[] = {
{WAVE_FORMAT_1S16, 11025, 2, 16},
{WAVE_FORMAT_1S08, 11025, 2, 8},
{WAVE_FORMAT_1M16, 11025, 1, 16},
{WAVE_FORMAT_1M08, 11025, 1, 8},
{WAVE_FORMAT_2S16, 22050, 2, 16},
{WAVE_FORMAT_2S08, 22050, 2, 8},
{WAVE_FORMAT_2M16, 22050, 1, 16},
{WAVE_FORMAT_2M08, 22050, 1, 8},
{WAVE_FORMAT_4S16, 44100, 2, 16},
{WAVE_FORMAT_4S08, 44100, 2, 8},
{WAVE_FORMAT_4M16, 44100, 1, 16},
{WAVE_FORMAT_4M08, 44100, 1, 8},
{0,0,0,0}
};
// Согласуем формат записи и открываем подходящее
// устройство.

int WinPlayer::SelectDevice(void) {
	// Остальные предложения по форматам.
	int channelsMin = 1, channelsMax = 2, channelsPreferred = 0;
	long rateMin = 8000, rateMax = 44100, ratePreferred = 22050;
	MinMaxChannels(&channelsMin, &channelsMax, &channelsPreferred);
	if (channelsMin > channelsMax) {
		// He удалось согласовать каналы.
		std::cerr << "Couldn't negotiate channels.\n";
		exit(1);
	}
	MinMaxSamplingRate(&rateMin, &rateMax, &ratePreferred);
	if (rateMin > rateMax) {
		// He удалось согласовать частоту
		// дискретизации.
		std::cerr << "Couldn't negotiate rate.\n";
		exit(1);
	}
	// Сначала проверяем на точное совпадение.
	static const int NO_MATCH = 100000;
	UINT matchingDevice = NO_MATCH;
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = channelsPreferred;
	waveFormat.nSamplesPerSec = ratePreferred;
	waveFormat.wBitsPerSample = 8 * sizeof(Sample16);
	waveFormat.nBlockAlign = waveFormat.nChannels
		* waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign
		* waveFormat.nSamplesPerSec;
	waveFormat.cbSize = 0;
	MMRESULT err = waveOutOpen(0, WAVE_MAPPER, &waveFormat,
		0, 0, WAVE_FORMAT_QUERY);
	if (err == 0) {
		matchingDevice = WAVE_MAPPER;
		channelsMax = channelsMin = channelsPreferred;
		rateMax = rateMin = ratePreferred;
		_sampleWidth = 16;
	}
	else {
		// WinPlay: He удалось использовать
		// нестандартный формат, пробуем стандартные.
		std::cerr << "WinPlay: Custom format failed, "; 
			std::cerr << "trying standard formats.\n";
	}
	// Пересчитываем доступные устройства.
	UINT numDevs = waveOutGetNumDevs();
	if (numDevs == 0) {
		// Устройств вывода звука не найдено?
		std::cerr << "No sound output devices found?!\n";
		exit(1);
	}
	// Проверяем все доступные устройства.
	for (UINT i = 0; (i < numDevs) && (matchingDevice == NO_MATCH);
		i++) {
		// Какие форматы поддерживает это устройство?
		WAVEOUTCAPS waveOutCaps;
		MMRESULT err =
			waveOutGetDevCaps(i, &waveOutCaps, sizeof(waveOutCaps));
		if (err != MMSYSERR_NOERROR) {
			// He удалось получить информацию о возможностях устройства.
			std::cerr << "Couldn't get capabilities of device " << i <<
				"\n";
			continue;
		}
		// Проверяем все стандартные форматы.
		for (UINT j = 0; winFormats[j].format != 0; j++) {
			if ((winFormats[j].format & waveOutCaps.dwFormats)
				// Поддерживает?
				&& (rateMin <= winFormats[j].rate)
				// Частота подходит?
				&& (rateMax >= winFormats[j].rate)
				&& (channelsMin <= winFormats[j].channels)
				// Количество каналов подходит?
				&& (channelsMax >= winFormats[j].channels)) {
				// Устанавливаем мои параметры.
				matchingDevice = i;
				rateMin = rateMax = ratePreferred =
					winFormats[j].rate;
				channelsPreferred = winFormats[j].channels;
				channelsMin = channelsMax = channelsPreferred;
				_sampleWidth = winFormats[j].width;
				// Соответственно устанавливаем структуру WAVEFORMATEX.
				waveFormat.wFormatTag = WAVE_FORMAT_PCM;
				waveFormat.nChannels = winFormats[j].channels;
				waveFormat.nSamplesPerSec = winFormats[j].rate;
				waveFormat.wBitsPerSample = winFormats[j].width;
				waveFormat.nBlockAlign = waveFormat.wBitsPerSample / 8
					* waveFormat.nChannels;
				waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign
					* waveFormat.nSamplesPerSec;
				waveFormat.cbSize = 0;
			}
		}
	}
	if (matchingDevice == NO_MATCH) {
		// Данный формат не поддерживается.
		// Далее следуют параметры формата.
		std::cerr << "Can't handle this sound format.\n";
		std::cerr << "Rate: " << rateMin << "-" << rateMax << "\n";
		std::cerr << "Channels: " << channelsMin << "-" << channelsMax
			<< "\n";
		return 1;
	}
	// Если удалось найти совпадающие параметры, задаем все.
	SetChannelsRecursive(channelsPreferred);
	SetSamplingRateRecursive(ratePreferred);
	// Открываем подходящее устройство.
	MMRESULT err2 = waveOutOpen(&_device, matchingDevice,
		&waveFormat, reinterpret_cast<DWORD>(WaveOutCallback),
		reinterpret_cast<DWORD>(this), CALLBACK_FUNCTION);
	if (err2) {
		// He удалось открыть устройство воспроизведения WAVE.
		std::cerr << "Couldn't open WAVE output device.\n";
		exit(1);
	}
	return 0;
}