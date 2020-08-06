// AB_SOFT_test1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <Windows.h>

#define TRIAL_RUNS 2
#define READERS_AMOUNT 5
#define WRITERS_AMOUNT 5
#define SLEEP_TIME_READ_MS 1000
#define SLEEP_TIME_WRITE_MS 1000

using namespace std;

const int DATA_SIZE = 25;

int ActiveWriters = 0;
int WaitingWriters = 0;
int ActiveReaders = 0;
int WaitingReaders = 0;
//количество активных и ожидающих читателей/писателей

int WriteActionCount = 0; //счетчик действий на запись (для изменения массива)

int *Data = new int[DATA_SIZE]; //массив в качестве примера разделяемых данных

HANDLE hWriteEvent, hReadEvent;
HANDLE hMutexCountR,hMutexCountW,hMutexR,hMutexW;

void StartWrite()
{
	WaitForSingleObject(hMutexCountW, INFINITE);
	bool isBusy = ActiveWriters > 0 || ActiveReaders >0;
	if (isBusy)
	{
		WaitingWriters++;
	}
	ReleaseMutex(hMutexCountW);

	WaitForSingleObject(hWriteEvent, INFINITE);
	if(isBusy)
		WaitingWriters--;
	ActiveWriters++;

}

void StartRead()
{
	WaitForSingleObject(hMutexCountR, INFINITE);
	bool isBusy = ActiveWriters > 0 || WaitingWriters > 0;
	if (isBusy)
	{
		WaitingReaders++;
	}
	ReleaseMutex(hMutexCountR);

	WaitForSingleObject(hReadEvent, INFINITE);
	if (isBusy)
		WaitingReaders--;
	ActiveReaders++;
}

void StopWrite()
{
	WaitForSingleObject(hMutexCountW, INFINITE);
	ActiveWriters--;
	HANDLE hEv = NULL;
	if (WaitingWriters > 0)
	{
		hEv = hWriteEvent;
	}
	else if (WaitingReaders > 0)
	{
		hEv = hReadEvent;
	}
	ReleaseMutex(hMutexCountW);

	if (hEv)
		SetEvent(hEv);
}

void StopRead()
{
	WaitForSingleObject(hMutexCountR, INFINITE);
	ActiveReaders--;
}

DWORD WINAPI Reader(LPVOID lpParam)
{
	StartRead();

	for (int i = 0; i < DATA_SIZE; i++)
	{
		cout << Data[i] << " ";
	}

	cout << endl;

	Sleep(SLEEP_TIME_READ_MS);

	StopRead();
	return 0;
}

DWORD WINAPI Writer(LPVOID lpParam)
{
	StartWrite();

	Data[WriteActionCount%DATA_SIZE]++; //инкрементируем элемент с индексом равным текущая_операция_записи % размер_массива;
	WriteActionCount++;
	cout << "data was modified " << endl;
	Sleep(SLEEP_TIME_WRITE_MS);

	StopWrite();
	return 0;
}

int main()
{
	for (int i = 0; i < TRIAL_RUNS; i++)
	{
		cout << "Trial run №" << i + 1<<endl;
		for (int i = 0; i < DATA_SIZE; i++)
		{
			Data[i] = 0;
		}

		HANDLE Readers[READERS_AMOUNT], Writers[WRITERS_AMOUNT];

		hWriteEvent = CreateEventA(NULL, TRUE, FALSE, "E_WRITE");
		hReadEvent = CreateEventA(NULL, FALSE, FALSE, "E_READ");

		hMutexCountR = CreateMutexA(NULL, NULL, "MUTEX_COUNT_R");
		hMutexCountW = CreateMutexA(NULL, NULL, "MUTEX_COUNT_W");
		hMutexR = CreateMutexA(NULL, NULL, "MUTEX_R");
		hMutexW = CreateMutexA(NULL, NULL, "MUTEX_W");

		if (hMutexCountR == NULL || hMutexCountW ==NULL || hMutexR == NULL || hMutexW == NULL)
		{
			cout << "Mutex threads creation error" << GetLastError();
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

		for (int i = 0; i < WRITERS_AMOUNT; i++)
		{
			Writers[i] = CreateThread(NULL, NULL, Writer, NULL, NULL, NULL);
		}

		if (Writers == NULL)
		{
			cout << "Writers creation error" << GetLastError();
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
	}

	CloseHandle(hWriteEvent);
	CloseHandle(hReadEvent);
	CloseHandle(hMutex);

	return 0;
}
