/*
* Player class for Win32
*/
#ifndef WIN_PLAYER_H_INCLUDED
#define WIN_PLAYER_H_INCLUDED
#include "audio.h"
#include "aplayer.h"
#include <windows.h>
#include <mmsystem.h>
#define winBufferSize 10000 // Количество отсчетов в буфере.
#pragma comment(lib, "winmm.lib")

class WinPlayer : public AbstractPlayer {
private:
	HWAVEOUT _device; // Аудиоустройство Windows, которое
	// будем открывать.
	volatile bool _paused; // Истина, если устройство
	// в состоянии паузы.
	int _sampleWidth; // Разрядность выходных данных.
	int SelectDevice(void); // Открывает подходящее устройство.
	// Разрешаем функциям обратного
	// вызова обращаться к функциям-членам.
	friend void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParaml, DWORD
		dwParam2);
	// Определенные выше функции обратного
	// вызова - это просто упаковщики,
	// обращающиеся к этому методу.
public:
	void NextBuff(WAVEHDR*);
	WinPlayer(AudioAbstract* a) : AbstractPlayer(a) {
		_device = 0;
		_paused = true;
		_sampleWidth = 0;
	};
	~WinPlayer() {};
	void Play(); // Собственно воспроизведение записи.
};
#endif