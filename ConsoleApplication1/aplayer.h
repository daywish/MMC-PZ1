#ifndef ABSTRACTPLAYER_H_INCLUDED
#define ABSTRACTPLAYER_H_INCLUDED
#include "audio.h"

class AbstractPlayer : public AudioAbstract {
protected:
	typedef short Sample16;
	typedef signed char Sample8;
	volatile AudioSample* _queue, * _queueEnd; // ������ �����
	// ������� ������,
	// ����������
	// ��� �������.
	volatile AudioSample* volatile _queueFirst; // ������
// �������.
	volatile AudioSample* volatile _queueLast; // ���������
	// �������.
	void InitializeQueue(unsigned long queueSize); // �������
	// �������.
	void FillQueue(void); // ���������
	// �������.
	long FromQueue(Sample8* pDest, long bytes);
	long FromQueue(Sample16* pDest, long bytes);
	bool _endOfSource; // ������, ���� �� �������� ���������
// ������� ��������� ������.
	bool _endOfQueue; // ������, ���� �� �������� ���������
	// ������� ��������� ������. 
private:
	void DataToQueue(long); // ������������ ������� FillQueue.
	void DataFromQueue(Sample8*, long); // ������������ �������
	// FromQueue(Sample8...) .
	void DataFromQueue(Sample16*, long); // ������������ �������
	// FromQueue(Sample16...).
	size_t GetSamples(AudioSample*, size_t) {
		exit(1); return 0;
	};
public:
	AbstractPlayer(AudioAbstract* a);
	~AbstractPlayer();
	virtual void Play() = 0; // ���������� ��������������� ������.
};

#endif