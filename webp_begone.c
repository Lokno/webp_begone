#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "webp/decode.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_POSSIBLE_FILE_PATH 2048

char* getDesktopPath(size_t* size) {
    char *userProfile = getenv("USERPROFILE");

    char *desktopPath = NULL;
    if (userProfile != NULL) {
        *size = strnlen_s(userProfile,MAX_POSSIBLE_FILE_PATH) + 9;
        desktopPath = (char*)malloc(sizeof(char) * *size);
        if( desktopPath == NULL )
        {
            fprintf(stderr, "Error: Failed to allocated memory\n");
            exit(2);
        }
        snprintf(desktopPath, *size, "%s\\Desktop", userProfile);
    } else {
        fprintf(stderr, "Error: Could not get USERPROFILE\n");
        exit(1);
    }
    return desktopPath;
}

int convertWebPToPNG(const char *webpFilePath, const char *pngFilePath) {
    FILE *file = fopen(webpFilePath, "rb");
    if (!file) {
        fprintf(stderr, "Error: Failed to open WebP file %s\n", webpFilePath);
        return 0;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read WebP data into memory
    unsigned char *webpData = (unsigned char *)malloc(fileSize);

    if( webpData == NULL )
    {
        fprintf(stderr, "Error: Failed to allocated memory\n");
        exit(2);
    }

    fread(webpData, fileSize, 1, file);
    fclose(file);

    // Decode WebP file into RGBA format
    int width, height;
    uint8_t *decodedData = WebPDecodeRGBA(webpData, fileSize, &width, &height);
    free(webpData);

    if (!decodedData) {
        fprintf(stderr, "Error: Failed to decode WebP image %s\n", webpFilePath);
        return 0;
    }

    // Write PNG using stb_image_write
    if (!stbi_write_png(pngFilePath, width, height, 4, decodedData, width * 4)) {
        fprintf(stderr, "Error: Failed to write PNG file %s\n", pngFilePath);
        WebPFree(decodedData);
        return 0;
    }

    // Free the decoded image data
    WebPFree(decodedData);
    return 1;
}

// Function to scan the Desktop for .webp files and convert them to PNG using the Windows API
void scanAndConvertImageFiles(const char *desktopPath, const size_t desktopPathSize) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char* searchPath = NULL;
    size_t searchPathSize = desktopPathSize + 7u;

    searchPath = (char*)malloc(sizeof(char) * searchPathSize);

    if( searchPath == NULL )
    {
        fprintf(stderr, "Error: Failed to allocated memory\n");
        exit(2);
    }
    
    snprintf(searchPath, searchPathSize, "%s\\*.webp", desktopPath);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No .webp files found on the Desktop.\n");
        free(searchPath);
        return;
    }

    free(searchPath); searchPath = NULL;

    size_t webpFilePathAllocatedSize = MAX_PATH;
    size_t pngFilePathAllocatedSize = MAX_PATH;

    char* webpFilePath = (char*)malloc(sizeof(char) * webpFilePathAllocatedSize);
    char* pngFilePath = (char*)malloc(sizeof(char) * pngFilePathAllocatedSize);

    if( webpFilePath == NULL || pngFilePath == NULL )
    {
        fprintf(stderr, "Error: Failed to allocated memory\n");
        exit(2);
    }

    do {

        size_t fileNameSize = strnlen_s(findFileData.cFileName,MAX_POSSIBLE_FILE_PATH);
        size_t webpFilePathSize = desktopPathSize + 1u + fileNameSize;
        size_t pngFilePathSize = desktopPathSize + fileNameSize;

        if( webpFilePathSize > webpFilePathAllocatedSize )
        {
            webpFilePathAllocatedSize = webpFilePathSize * 2u;
            free(webpFilePath);
            webpFilePath = (char*)malloc(sizeof(char) * webpFilePathAllocatedSize);
        }

        if( pngFilePathSize > pngFilePathAllocatedSize )
        {
            pngFilePathAllocatedSize = pngFilePathSize * 2u;
            free(pngFilePath);
            pngFilePath = (char*)malloc(sizeof(char) * pngFilePathAllocatedSize);
        }

        snprintf(webpFilePath, webpFilePathAllocatedSize, "%s\\%s", desktopPath, findFileData.cFileName);

        snprintf(pngFilePath, pngFilePathAllocatedSize, "%s\\%.*s.png", desktopPath, (int)fileNameSize-5, findFileData.cFileName);

        printf("Converting %s to %s...\n", webpFilePath, pngFilePath);
        if (!convertWebPToPNG(webpFilePath, pngFilePath)) {
            fprintf(stderr, "Error: Conversion failed for %s\n", webpFilePath);
        }

        if (!DeleteFile(webpFilePath)) {
            // If deletion fails, print an error message
            fprintf(stderr, "Error: Could not delete WebP file %s. Error code: %lu\n", webpFilePath, GetLastError());
        } else {
            printf("Successfully deleted %s after conversion.\n", webpFilePath);
        }

    } while (FindNextFile(hFind, &findFileData) != 0);

    if( webpFilePath != NULL ) free(webpFilePath);
    if( pngFilePath != NULL ) free(pngFilePath);

    FindClose(hFind);
}

int main() {
    size_t desktopPathSize;
    char* desktopPath = getDesktopPath(&desktopPathSize);

    printf("Scanning for .webp files in %s...\n", desktopPath);
    scanAndConvertImageFiles(desktopPath, desktopPathSize);

    free(desktopPath);

    return 0;
}
