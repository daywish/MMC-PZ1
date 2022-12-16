/*
* Player class for Win32
*/
#ifndef WIN_PLAYER_H_INCLUDED
#define WIN_PLAYER_H_INCLUDED
#include "audio.h"
#include "aplayer.h"
#include <windows.h>
#include <mmsystem.h>
#define winBufferSize 10000 // ���������� �������� � ������.
#pragma comment(lib, "winmm.lib")

class WinPlayer : public AbstractPlayer {
private:
	HWAVEOUT _device; // ��������������� Windows, �������
	// ����� ���������.
	volatile bool _paused; // ������, ���� ����������
	// � ��������� �����.
	int _sampleWidth; // ����������� �������� ������.
	int SelectDevice(void); // ��������� ���������� ����������.
	// ��������� �������� ���������
	// ������ ���������� � ��������-������.
	friend void CALLBACK WaveOutCallback(HWAVEOUT hwo, UINT uMsg,
		DWORD dwInstance, DWORD dwParaml, DWORD
		dwParam2);
	// ������������ ���� ������� ���������
	// ������ - ��� ������ ����������,
	// ������������ � ����� ������.
public:
	void NextBuff(WAVEHDR*);
	WinPlayer(AudioAbstract* a) : AbstractPlayer(a) {
		_device = 0;
		_paused = true;
		_sampleWidth = 0;
	};
	~WinPlayer() {};
	void Play(); // ���������� ��������������� ������.
};
#endif