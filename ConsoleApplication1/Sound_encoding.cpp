#include <iostream>
#include <malloc.h>
#include <iomanip>  
#include <bitset> 
#include <tuple>
using namespace std;

char* make_delta_encoding(char* S) {
    size_t array_size = _msize(S) / sizeof(char);
    char* D = new char[array_size];

    if (array_size) {
        D[0] = S[0];
    }

    for (int i = 1; i < array_size; ++i) {
        D[i] = S[i] - S[i - 1];
    }

    return D;
}

char* restore_delta_encoding(char* D) {
    size_t array_size = _msize(D) / sizeof(char);
    char* S1 = new char[array_size];
    S1[0] = D[0];

    for (int i = 1; i < array_size; ++i) {
        S1[i] = D[i] + S1[i - 1];
    }

    return S1;
}

void make_delta_encoding_in_the_same_arr(char* S) {
    size_t array_size = _msize(S) / sizeof(char);

    int last = S[0];
    int current;

    for (int i = 1; i < array_size; ++i) {
        current = S[i];
        S[i] = current - last;
        last = current;
    }
}

void restore_delta_encoding_in_the_same_arr(char* S) {
    size_t array_size = _msize(S) / sizeof(char);

    for (int i = 1; i < array_size; ++i) {
        S[i] = S[i] + S[i - 1];
    }
}

char* make_encoding_with_last_array(char* S, const size_t h) {
    size_t array_size = _msize(S) / sizeof(char);
    char* D = new char[array_size];
    char* Last = new char[h];
    _memccpy(Last, S, 0, h);
    _memccpy(D, S, 0, h);
    int last_pos = 0;

    for (size_t i = h; i < array_size; ++i) {
        int sum_of_last = 0;
        for (size_t j = 0; j < h; ++j) {
            sum_of_last += Last[j];
        }
        D[i] = S[i] - sum_of_last / h;
        Last[last_pos] = S[i];
        last_pos = (last_pos + 1) & (h - 1);
    }

    delete[] Last;

    return D;
}

char* restore_with_last_array(char* D, const size_t h) {
    size_t array_size = _msize(D) / sizeof(char);
    char* S1 = new char[array_size];
    _memccpy(S1, D, 0, h);

    for (int i = h; i < array_size; ++i) {
        int sum_of_last = 0;
        for (int j = i - 1; j >= (int) (i - h); --j) {
            sum_of_last += S1[j];
        }
        S1[i] = D[i] + sum_of_last / h;
    }

    return S1;
}

void make_encoding_with_h_one_array(char* S, const int h) {
    int array_size = _msize(S) / sizeof(char);
    int* first = new int[h];
    first[0] = S[0];

    int sum = S[0];
    for (int i = 1; i < h; ++i) {
        first[i] = S[i] - sum / i;
        sum += S[i];
    }

    for (int i = h; i < array_size; ++i) {
        int current_item = S[i - h];
        S[i - h] = S[i] - sum / h;
        sum += S[i] - current_item;
    }

    for (int i = 0; i < h; ++i) {
        S[array_size - h + i] = first[i];
    }

    delete[] first;
}

void restore_with_h_one_array(char* S, const int h) {
    int array_size = _msize(S) / sizeof(char);
    int* last = new int[h];
    last[0] = S[array_size - h];

    int sum = last[0];
    for (int i = 1; i < h; ++i) {
        last[i] = S[array_size - h + i] + sum / i;
        sum += last[i];
    }

    int pos = 0;
    for (int i = 0; i < array_size - h; ++i) {
        int current_item = last[pos & (h - 1)];
        int new_value = S[i] + sum / h;
        S[i] = current_item;
        last[pos++ & (h - 1)] = new_value;
        sum += new_value - current_item;
    }

    for (int i = 0; i < h; ++i) {
        S[array_size - h + i] = last[pos++ & (h - 1)];
    }

    delete[] last;
}

tuple<short*, char*> make_bigger_output_array(char* S) {
    size_t init_array_size = _msize(S) / sizeof(char);
    size_t result_array_size = init_array_size / 2;
    short* D = new short[result_array_size];
    char* A = new char[result_array_size];

    for (int i = 0; i < result_array_size; ++i) {
        D[i] = S[2 * i] - S[2 * i + 1];
        A[i] = S[2 * i] - D[i] / 2;
    }

    return tie(D, A);
}

template<size_t K>
tuple<char*, bitset<K>> make_delta_encoding_with_flag_array(char* S) {
    char* D = new char[K];
    bitset<K> flags;

    for (int i = 0; i < K; ++i) {
        D[i] = S[i * 2] - S[i * 2 + 1];
        if (D[i] != S[i * 2] - S[i * 2 + 1]) {
            flags[i] = 1;
        }
        else {
            flags[i] = 0;
        }
    }

    return tie(D, flags);
}

template<typename T>
tuple<T*, char*> make_delta_encoding_halfsum(char* S) {
    size_t init_array_size = _msize(S) / sizeof(char);
    size_t result_array_size = init_array_size / 2;
    T* D = new T[result_array_size];
    char* A = new char[result_array_size];

    for (int i = 0; i < result_array_size; ++i) {
        D[i] = S[2 * i] - S[2 * i + 1];
        A[i] = S[2 * i] - D[i] / 2;
    }

    return tie(D, A);
}

template<typename T>
tuple<char*, T*> make_delta_encoding_halfdiff(char* S) {
    size_t init_array_size = _msize(S) / sizeof(char);
    size_t result_array_size = init_array_size / 2;
    char* D = new char[result_array_size];
    T* A = new T[result_array_size];

    for (int i = 0; i < result_array_size; ++i) {
        A[i] = S[2 * i] + S[2 * i + 1];
        D[i] = S[2 * i] - A[i] / 2;
    }

    return tie(D, A);
}

template<typename T>
void print_array(T* arr) {
    size_t array_size = _msize(arr) / sizeof(T);
    for (int i = 0; i < array_size; ++i) {
        cout << setw(4) << int{ arr[i] } << ' ';
    }
    cout << endl;
}

template<size_t K>
void print_bitset(bitset<K> bts) {
    for (int i = 0; i < K; ++i) {
        cout << setw(4) << int{ bts[i] } << ' ';
    }
    cout << endl;
}

char* reverse_transforming_half_sums(char* D, char* A) {
    size_t array_size = _msize(D) / sizeof(char);
    char* S = new char[array_size * 2];

    for (int i = 0; i < array_size; ++i) {
        S[2 * i] = (2 * A[i] + D[i]) / 2;
        S[2 * i + 1] = (2 * A[i] - D[i]) / 2;
    }

    return S;
}

char* reverse_transforming_half_diffs(char* D, char* A) {
    size_t array_size = _msize(D) / sizeof(char);
    char* S = new char[array_size * 2];

    for (int i = 0; i < array_size; ++i) {
        S[2 * i] = (2 * D[i] + A[i]) / 2;
        S[2 * i + 1] = (A[i] - 2 * D[i]) / 2;
    }

    return S;
}



int main()
{
    // Task1
    char* task1_S = new char[10] {2, 3, 5, 3, 4, 4, 3, 5, 2};
    cout << "Initial array:" << endl;
    print_array(task1_S);

    cout << "Delta encoded array:" << endl;
    char* task1_D = make_delta_encoding(task1_S);
    print_array(make_delta_encoding(task1_S));

    char* task1_S_restores = restore_delta_encoding(task1_D);
    cout << "Delta restored array:" << endl;
    print_array(task1_S_restores);
    cout << endl;

    size_t task1_S_size = _msize(task1_S) / sizeof(char);
    char* task1_S_copy = new char[task1_S_size];
    _memccpy(task1_S_copy, task1_S, 0, task1_S_size);
    make_delta_encoding_in_the_same_arr(task1_S_copy);
    cout << "Delta encoded array in place:" << endl;
    print_array(task1_S_copy);

    restore_delta_encoding_in_the_same_arr(task1_S_copy);
    cout << "Delta encoded array in place restores:" << endl;
    print_array(task1_S_copy);
    cout << endl;

    delete[] task1_D;

    char* encoded_with_last_h = make_encoding_with_last_array(task1_S, 4);
    cout << "Last h encoded array:" << endl;
    print_array(encoded_with_last_h);

    cout << "Restored with last h encoded array:" << endl;
    print_array(restore_with_last_array(encoded_with_last_h, 4));
    cout << endl;

    make_encoding_with_h_one_array(task1_S, 4);
    cout << "Last h encoded one array:" << endl;
    print_array(task1_S);

    restore_with_h_one_array(task1_S, 4);
    cout << "Restored with last h one array:" << endl;
    print_array(task1_S);
    cout << endl << endl;

    char* task1_S_overflow = new char[10] {
        -128, 120, 126, -125, 125, -126, 113, -100, 122
    };
    cout << "Array with overflow:" << endl;
    print_array(task1_S_overflow);
    cout << endl;

    char* task1_D_overflow = make_delta_encoding(task1_S_overflow);
    cout << "Delta encoded array with overflow:" << endl;
    print_array(task1_D_overflow);
    cout << endl;


    short* task1_D_short;
    char* task1_A_short;
    tie(task1_D_short, task1_A_short) =
        make_bigger_output_array(task1_S_overflow);
    cout << "Delta encoded bigger array:" << endl;
    cout << "Deltas:" << endl;
    print_array(task1_D_short);
    cout << "Halfsums:" << endl;
    print_array(task1_A_short);
    cout << endl;


    char* task1_D_normal;
    bitset<9 / 2> flags_bitset;
    tie(task1_D_normal, flags_bitset) = 
        make_delta_encoding_with_flag_array<9 / 2>(task1_S_overflow);
    cout << "Delta encoded array with overflow flags:" << endl;
    print_array(task1_D_normal);
    cout << "Flags:" << endl;
    print_bitset(flags_bitset);
    cout << endl;

    cout << "Array without overflow:" << endl;
    print_array(task1_S);
    char* task1_D_halfsum;
    char* task1_A_halfsum;
    tie(task1_D_halfsum, task1_A_halfsum) =
        make_delta_encoding_halfsum<char>(task1_S);
    cout << "Deltas:" << endl;
    print_array(task1_D_halfsum);
    cout << "Halfsums:" << endl;
    print_array(task1_A_halfsum);
    cout << endl;

    char* task1_D_halfdiff;
    char* task1_A_halfdiff;
    tie(task1_D_halfdiff, task1_A_halfdiff) =
        make_delta_encoding_halfdiff<char>(task1_S);
    cout << "Deltas:" << endl;
    print_array(task1_D_halfdiff);
    cout << "Halfdiffs:" << endl;
    print_array(task1_A_halfdiff);
    cout << endl;

    delete[] task1_S;

    char* reversed_half_sums = reverse_transforming_half_sums(
        task1_D_halfsum, task1_A_halfsum
    );
    cout << "Reversed by halfsums:" << endl;
    print_array(reversed_half_sums);
    cout << endl;

    char* reversed_half_diffs = reverse_transforming_half_diffs(
        task1_D_halfdiff, task1_A_halfdiff
    );
    cout << "Reversed by halfdiffs:" << endl;
    print_array(reversed_half_diffs);
    cout << endl;
}



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
//	std::ifstream is("sample4.au", std::ifstream::binary);
//	AuRead auRead(is); // Создаем объект AuRead.
//	Player player(&auRead); // Соединяем выход объекта auRead
//	// с входом программы-проигрывателя.
//	player.Play(); // Теперь запускаем программу-
//	// проигрыватель.
//	return 0;
//}




//#include "sinewave.h"
//#include "aplayer.h"
//#include "au.h"
//#include <fstream>
//
//int main(int, char**) {
//	SineWave sinewave(440); // Синусоида 440 Гц.
//	sinewave.SamplingRate(11025); // Частота дискретизации 11025 Гц.
//	std::ofstream os("ownfile.au", std::ofstream::binary);
//	AuWrite player(&sinewave);
//	player.Play();
//	return 0;
//}