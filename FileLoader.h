#pragma once

void FileLoad(const char* filePath, void* readedData, int readSize);
__int32 FileSize(const char* filePath);

void FileWrite(const char* filePath, void* readedData, int writeSize);