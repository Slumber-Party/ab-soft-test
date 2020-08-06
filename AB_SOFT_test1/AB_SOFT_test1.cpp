// AB_SOFT_test1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

#define TRIAL_RUNS 3
#define READERS_AMOUNT 7
#define WRITERS_AMOUNT 5
#define SLEEP_TIME_READ_MS 25
#define SLEEP_TIME_WRITE_MS 50

using namespace std;

const int DATA_SIZE = 25;

int ActiveWriters = 0;
int WaitingWriters = 0;
int ActiveReaders = 0;
int WaitingReaders = 0;
//количество активных и ожидающих читателей/писателей

//int WriteActionCount = 0; //счетчик действий на запись (для изменения массива)

//int *Data = new int[DATA_SIZE]; //массив в качестве примера разделяемых данных

HANDLE hWriteEvent, hReadEvent;
HANDLE hMutexCount,hMutexR,hMutexW;

void StartWrite()
{
	WaitForSingleObject(hMutexCount, INFINITE);
	bool isBusy = ActiveWriters > 0 || ActiveReaders >0;
	if (isBusy)
	{
		WaitingWriters++;
	}
	else
	{
		ActiveWriters++;
	}
	ReleaseMutex(hMutexCount);

	if (isBusy)
	{
		WaitForSingleObject(hWriteEvent, INFINITE);
	}
}

void StartRead()
{
	WaitForSingleObject(hMutexCount, INFINITE);
	bool isBusy = ActiveWriters > 0 || WaitingWriters > 0;
	if (isBusy)
	{
		WaitingReaders++;
		ResetEvent(hReadEvent);
	}
	else
	{
		ActiveReaders++;
	}
	ReleaseMutex(hMutexCount);
	if (isBusy)
	{
		WaitForSingleObject(hReadEvent, INFINITE);
	}
}

void StopWrite()
{
	WaitForSingleObject(hMutexCount, INFINITE);
	ActiveWriters--;
	HANDLE hEv = NULL;
	if (WaitingWriters > 0)
	{
		WaitingWriters--;
		ActiveWriters++;
		hEv = hWriteEvent;
	}
	else if (WaitingReaders > 0)
	{
		ActiveReaders = WaitingReaders;
		WaitingReaders = 0;
		hEv = hReadEvent;
	}
	ReleaseMutex(hMutexCount);

	if (hEv)
		SetEvent(hEv);
}

void StopRead()
{
	WaitForSingleObject(hMutexCount, INFINITE);
	ActiveReaders--;
	HANDLE hEv = NULL;

	if (ActiveReaders == 0 && WaitingWriters > 0)
	{
		WaitingWriters--;
		ActiveWriters++;
		hEv = hWriteEvent;
	}
	else {
		ActiveReaders = WaitingReaders;
		WaitingReaders = 0;
		hEv = hReadEvent;
	}
	ReleaseMutex(hMutexCount);

	if (hEv)
		SetEvent(hEv);
}

DWORD WINAPI Reader(LPVOID lpParam)
{
	StartRead();
	Sleep(SLEEP_TIME_READ_MS);

	cout << "data was read" << endl;

	StopRead();
	return 0;
}

DWORD WINAPI Writer(LPVOID lpParam)
{
	StartWrite();

	//Data[WriteActionCount%DATA_SIZE]++; //инкрементируем элемент с индексом равным текущая_операция_записи % размер_массива;
	//WriteActionCount++;
	Sleep(SLEEP_TIME_WRITE_MS);
	cout << "data was modified " << endl;

	StopWrite();
	return 0;
}

int main()
{
	
	for (int i = 0; i < TRIAL_RUNS; i++)
	{
		cout << "Trial run #" << i + 1<<endl;
		//for (int i = 0; i < DATA_SIZE; i++)
	//	{
			//Data[i] = 0;
		//}

		HANDLE Readers[READERS_AMOUNT], Writers[WRITERS_AMOUNT];

		hWriteEvent = CreateEventA(NULL, FALSE, FALSE, "E_WRITE");
		hReadEvent = CreateEventA(NULL, FALSE, FALSE, "E_READ");

		if (hWriteEvent == NULL || hReadEvent == NULL)
		{
			cout << "Event cration error: " << GetLastError();
		}

		hMutexCount = CreateMutexA(NULL, NULL, "MUTEX_COUNT");

		if (hMutexCount == NULL)
		{
			cout << "Mutex creation error: " << GetLastError();
			return 1;
		}

		for (int i = 0; i < WRITERS_AMOUNT; i++)
		{
			Writers[i] = CreateThread(NULL, NULL, Writer, NULL, NULL, NULL);
		}

		if (Writers == NULL)
		{
			cout << "Writers creation error: " << GetLastError();
			return 1;
		}

		for (int i = 0; i < READERS_AMOUNT; i++)
		{
			Readers[i] = CreateThread(NULL, NULL, Reader, NULL, NULL, NULL);
		}

		if (Readers == NULL)
		{
			cout << "Readers threads creation error: " << GetLastError();
			return 1;
		}

		WaitForMultipleObjects(READERS_AMOUNT, Readers, TRUE, INFINITE);
		for (int i = 0; i < READERS_AMOUNT; i++)
		{
			CloseHandle(Readers[i]);
		}

		WaitForMultipleObjects(WRITERS_AMOUNT, Writers, TRUE, INFINITE);
		for (int i = 0; i < WRITERS_AMOUNT; i++)
		{
			CloseHandle(Writers[i]);
		}

		CloseHandle(hWriteEvent);
		CloseHandle(hReadEvent);
		CloseHandle(hMutexCount);
	}

	return 0;
}
