#pragma once

unsigned int FileSize(const char* filePath);
void FileLoadToMemory(const char* filePath, void* readedData, unsigned int readSize);
void* LoadGameFile(const char* filePath);
void* GeneratePointMemory(char *a1, int a2);