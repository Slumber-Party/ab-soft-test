// AB_SOFT_test1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

#define TRIAL_RUNS 2
#define READERS 5
#define WRITERS 5
#define SLEEP_TIME_READ_MS 1000
#define SLEEP_TIME_WRITE_MS 1000

using namespace std;

int ActiveWriters = 0;
int WaitingWriters = 0;
int ActiveReaders = 0;
int WaitingReaders = 0;

HANDLE hWriteEvent, hReadEvent;
HANDLE hMutex;

void StartWrite()
{

}

void StartRead()
{

}

void StopWrite()
{


}

void StopRead()
{

}

DWORD WINAPI Reader(LPVOID lpParam)
{
	StartRead();
}

DWORD WINAPI Writer(LPVOID lpParam)
{

}

int main()
{
	HANDLE Readers[READERS], Writers[WRITERS];

	hWriteEvent = CreateEventA(NULL, TRUE, FALSE, "E_WRITE");
	hReadEvent = CreateEventA(NULL, FALSE, FALSE, "E_READ");

	hMutex = CreateMutexA(NULL, NULL, "MUTEX");

	for (int i = 0; i < TRIAL_RUNS; i++)
	{
		for (int i = 0; i < READERS; i++)
		{
			Readers[i] = CreateThread(NULL, NULL, Reader, NULL, NULL, NULL);
		}

		for (int i = 0; i < WRITERS; i++)
		{
			Writers[i] = CreateThread(NULL, NULL, Writer, NULL, NULL, NULL);
		}
	}
}
