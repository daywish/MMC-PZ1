//#include <fstream>
//#include "au.h"
//#include "aplayer.h"
//// ������� ������� ���������� ���������-������������� ���
//// ������ Win32, Macintosh ��� UNIX/NAS.
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
//	AuRead auRead(std::cin); // ������� ������ AuRead.
//	Player player(&auRead); // ��������� ����� ������� auRead
//	// � ������ ���������-�������������.
//	player.Play(); // ������ ��������� ���������-
//	// �������������.
//	return 0;
//}