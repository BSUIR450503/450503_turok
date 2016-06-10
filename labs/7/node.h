#pragma once
#include "libraries.h"

class Node {
private:
	char itemName[MAX_NAME_SIZE];
	bool fileOrFolder;
	int sectorsSize;
	int startSectors;
	int branches;
	Node *nextFileOrFolder[MAX_ITEMS_IN_FOLDER];

public:
	Node();
	Node(char*, int, int);
	Node(char*);
	~Node();

	void writeDataInTreeFile();
	void seeDirectoryStruct();
	void createNewFile(char*, int, int);
	void createNewFolder(char*);
	void deleteFileOrFolder(char*);
	void copyFileFromRealSystem(char*);
	void copyFileFromVirtualSystem(char*);
	void defragmentation(int, int);
	Node *moveToNextFolder(char*);
	char *getItemName();
};