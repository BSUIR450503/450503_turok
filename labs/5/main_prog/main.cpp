#include "libraries.h"

struct IncomingInformation {
#ifdef _WIN32
	HANDLE hFile;
	DWORD number_of_bytes;
	CHAR buffer[100];
	DWORD position_input_file;
	DWORD position_output_file;
	OVERLAPPED Overlapped;
#elif __linux__
	int hFile;
	char  buffer[100];
	size_t number_of_bytes;
	size_t NumberOfBytesTransferred;
	off_t position_input_file;
	off_t position_output_file;
	struct aiocb aiocbStruct;
#endif
} information;

#ifdef _WIN32
HINSTANCE library;
HANDLE events[3]; // finished read,write, exit
#elif __linux__
int(*read_async) (struct IncomingInformation *);
int(*write_async) (struct IncomingInformation *);

pthread_mutex_t event_finish_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t event_finish_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t event_exit = PTHREAD_MUTEX_INITIALIZER;
#endif

#ifdef _WIN32
wchar_t *GetWC(const char *c) {
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
	mbstowcs(wc, c, cSize);

	return wc;
}

const char *GetCC(WCHAR *c) {
	_bstr_t b(c);
	const char *cc = b;
	char a[MAX_STRING_SIZE];
	strcpy(a,cc);
	return a;
}
#endif

char* strcut(char* str, char* _str) { // вырезать _str из str
	char* temp;
	do {
		temp = strstr(str, _str);
		if (temp != NULL) {
			char* _temp = temp + strlen(_str);
			strcpy(temp, _temp);
		}
		else break;
	} while (true);

	return str;
}

#ifdef _WIN32
DWORD WINAPI ReadThread(PVOID find_folder) {
	puts((const char*)find_folder);
	string folder = (((const char*)find_folder)); //".\\input";//
	puts(folder.c_str());
	folder.append("\\");
	string fileMask = folder + "*.txt";
	char FileRead[MAX_PATH];

	WIN32_FIND_DATA FindFile;
	HANDLE find_Handle, hReadFile = NULL;
	BOOL ReadInfo = false;

	BOOL(*Read)(IncomingInformation*) = (BOOL(*)(IncomingInformation*))GetProcAddress(library, "read");

	find_Handle = FindFirstFile(GetWC(fileMask.c_str()), &FindFile);

	if (find_Handle == INVALID_HANDLE_VALUE) {
		printf(" Error: %d\n", GetLastError());
		return 0;
	}

	while (1) {
		WaitForSingleObject(events[EVENT_FINISH_WRITE], INFINITE);
		if (ReadInfo == false) { // поиск следующего файла
			information.position_input_file = 0;
			puts((const char*)folder.c_str());
			strcpy(FileRead, folder.c_str());
			strcat(FileRead, "\0");
			puts((const char*)folder.c_str());
			strcat(FileRead, GetCC(FindFile.cFileName));
			hReadFile = CreateFile(GetWC(FileRead), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		}
		information.hFile = hReadFile;
		ReadInfo = (Read)(&information);
		if (ReadInfo == false && GetLastError() == ERROR_HANDLE_EOF) { // файл закончился
			if (FindNextFile(find_Handle, &FindFile)) {
				CloseHandle(hReadFile);
				SetEvent(events[EVENT_FINISH_WRITE]);
				continue;
			}
			else {
				break; // все файлы прочитаны
			}
		}
		SetEvent(events[EVENT_FINISH_READ]);
	}
	FindClose(find_Handle);
	CloseHandle(hReadFile);
	puts("exit event set\n");
	SetEvent(events[EVENT_EXIT]);
	
	puts("return\n");
	return 0;
#elif __linux__
void *ReadThread(void *find_folder) {
	DIR *folder;
	struct dirent recording;
	struct dirent *mark;
	const char* FindFolder = (const char *)find_folder;
	int hReadFile;
	char FindFile[MAX_STRING_SIZE];
	int readInfo = 0;

	folder = opendir(FindFolder);
	if (folder == NULL) {
		printf("\nOpen ERROR");
		return NULL;
	}

	while (readdir_r(folder, &recording, &mark) == 0 && mark != NULL) {
		if (strcmp(recording.d_name, ".") != 0 && strcmp(recording.d_name, "..") != 0) {
			break;
		}
	}
	if (mark == NULL) {
		printf("\nFolder is empty");
		return NULL;
	}

	while (1) {
		pthread_mutex_lock(&event_finish_write);
		if (readInfo == 0) {
			information.position_input_file = 0;
			strcpy(FindFile, FindFolder);
			strcat(FindFile, "/");
			strcat(FindFile, recording.d_name);
			hReadFile = open(FindFile, O_RDONLY);
		}
		information.hFile = hReadFile;

		readInfo = read_async(&information);

		if (readInfo == 0) {
			while (readdir_r(folder, &recording, &mark) == 0 && mark != NULL) {
				if (strcmp(recording.d_name, ".") != 0 && strcmp(recording.d_name, "..") != 0) {
					break;
				}
			}
			if (mark != NULL) {
				close(hReadFile);
				pthread_mutex_unlock(&event_finish_write);
				continue;
			}
			else {
				break;
			}
		}
		pthread_mutex_unlock(&event_finish_read);
	}

	pthread_mutex_unlock(&event_exit);
	pthread_mutex_unlock(&event_finish_read);
	close(hReadFile);
	return NULL;
#endif
}

#ifdef _WIN32
DWORD WINAPI WriteThread(PVOID out_file) {
	HANDLE hOutFile = CreateFile(GetWC((const char*)out_file), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	BOOL(*Write)(IncomingInformation*) = (BOOL(*)(IncomingInformation*))GetProcAddress(library, "write");

	HANDLE write_events[2] = {
		events[EVENT_FINISH_READ],
		events[EVENT_EXIT]
	};

	while (1) {
		int event = WaitForMultipleObjects(2, write_events, FALSE, INFINITE) - WAIT_OBJECT_0;
		puts("event1\n");
		if (event == EVENT_EXIT) {
			break;
		}
		information.hFile = hOutFile;
		puts("event2\n");
		(Write)(&information);
		SetEvent(events[EVENT_FINISH_WRITE]);
		puts("set finish write\n");
	}
	CloseHandle(hOutFile);
	return 0;
#elif __linux__
void *WritThread(void *out_file) {
	const char *output_file = (const char *)out_file;
	int hOutputFile = open(output_file, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	while (1) {
		pthread_mutex_lock(&event_finish_read);
		if (pthread_mutex_trylock(&event_exit) == 0) {
			break;
		}
		information.hFile = hOutputFile;
		write_async(&information);

		pthread_mutex_unlock(&event_finish_write);
	}
	close(hOutputFile);
	return NULL;
#endif
}

int main(int argc, char *argv[]) {
	char find_folder[MAX_STRING_SIZE];
	char out_file[MAX_STRING_SIZE];
	strcpy(find_folder, argv[0]);
	strcpy(out_file, argv[0]);
	strcut(find_folder, "spovm_lab5.exe");
	strcut(out_file, "spovm_lab5.exe");
	strcat(find_folder, "input");
	strcat(out_file, "output.txt");

#ifdef _WIN32
	HANDLE hEvent;
	HANDLE hThreads[2];

	hEvent = CreateEvent(NULL, FALSE, TRUE, TEXT("Event"));
	events[EVENT_FINISH_WRITE] = CreateEvent(NULL, FALSE, TRUE, NULL);
	events[EVENT_FINISH_READ] = CreateEvent(NULL, FALSE, FALSE, NULL);
	events[EVENT_EXIT] = CreateEvent(NULL, TRUE, FALSE, NULL);

	information.Overlapped.Offset = 0;
	information.Overlapped.OffsetHigh = 0;
	information.Overlapped.hEvent = hEvent;
	information.position_output_file = 0;
	information.number_of_bytes = sizeof(information.buffer);

	library = LoadLibrary(GetWC("library.dll"));

	hThreads[0] = CreateThread(NULL, 0, WriteThread, (LPVOID)out_file, 0, NULL);
	hThreads[1] = CreateThread(NULL, 0, ReadThread, (LPVOID)find_folder, 0, NULL);

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);
	CloseHandle(events[EVENT_FINISH_WRITE]);
	CloseHandle(events[EVENT_FINISH_READ]);
	CloseHandle(events[EVENT_EXIT]);
	CloseHandle(hEvent);
	FreeLibrary(library);
#elif __linux__
	void *library = dlopen("./library.so", RTLD_NOW);
	if (library == NULL) {
		puts(dlerror());
		
		return 0;
	}

	read_async = (int(*)(struct IncomingInformation*)) dlsym(library, "read_async");
	write_async = (int(*)(struct IncomingInformation*))dlsym(library, "write_async");

	pthread_mutex_lock(&event_finish_read);
	pthread_mutex_lock(&event_exit);

	pthread_t ThreadRead;
	pthread_t ThreadWrite;

	information.aiocbStruct.aio_offset = 0;
	information.aiocbStruct.aio_buf = information.buffer;
	information.number_of_bytes = sizeof(information.buffer);
	information.aiocbStruct.aio_sigevent.sigev_notify = SIGEV_NONE;
	information.position_input_file = 0;
	information.position_output_file = 0;

	pthread_create(&ThreadRead, NULL, ReadThread, (void *)find_folder);
	pthread_create(&ThreadWrite, NULL, WritThread, (void *)out_file);
	pthread_join(ThreadRead, NULL);
	pthread_join(ThreadWrite, NULL);
#endif
	return 0;
}