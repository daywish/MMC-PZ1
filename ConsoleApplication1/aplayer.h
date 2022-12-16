#ifndef ABSTRACTPLAYER_H_INCLUDED
#define ABSTRACTPLAYER_H_INCLUDED
#include "audio.h"

class AbstractPlayer : public AudioAbstract {
protected:
	typedef short Sample16;
	typedef signed char Sample8;
	volatile AudioSample* _queue, * _queueEnd; // Начало конец
	// области памяти,
	// отведенной
	// под очередь.
	volatile AudioSample* volatile _queueFirst; // Первая
// выборка.
	volatile AudioSample* volatile _queueLast; // Последняя
	// выборка.
	void InitializeQueue(unsigned long queueSize); // Создает
	// очередь.
	void FillQueue(void); // Заполняет
	// очередь.
	long FromQueue(Sample8* pDest, long bytes);
	long FromQueue(Sample16* pDest, long bytes);
	bool _endOfSource; // Истина, если из внешнего источника
// считаны последние данные.
	bool _endOfQueue; // Истина, если из внешнего источника
	// считаны последние данные. 
private:
	void DataToQueue(long); // Используется методом FillQueue.
	void DataFromQueue(Sample8*, long); // Используется методом
	// FromQueue(Sample8...) .
	void DataFromQueue(Sample16*, long); // Используется методом
	// FromQueue(Sample16...).
	size_t GetSamples(AudioSample*, size_t) {
		exit(1); return 0;
	};
public:
	AbstractPlayer(AudioAbstract* a);
	~AbstractPlayer();
	virtual void Play() = 0; // Собственно воспроизведение записи.
};

#endif