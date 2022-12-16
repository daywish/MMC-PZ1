//#include <fstream>
//#include "au.h"
//#include "aplayer.h"
//// Пробуем выбрать подходящую программу-проигрыватель для
//// систем Win32, Macintosh или UNIX/NAS.
//#if defined(_WIN32)
//#include "winplayr.h"
//typedef WinPlayer Player;
//#elif defined(macintosh)
//#include "macplayr.h"
//typedef MacPlayer Player;
//#else
//#include "nasplayr.h"
//typedef NasPlayer Player;
//#endif
//
//int main() {
//	AuRead auRead(std::cin); // Создаем объект AuRead.
//	Player player(&auRead); // Соединяем выход объекта auRead
//	// с входом программы-проигрывателя.
//	player.Play(); // Теперь запускаем программу-
//	// проигрыватель.
//	return 0;
//}