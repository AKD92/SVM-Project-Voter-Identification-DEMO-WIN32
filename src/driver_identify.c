


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial_win.h"


#define CMD_ENROLL						0xBB
#define CMD_GETENROLLED					0x55
#define CMD_SETTEMPLATE					0xBA
#define CMD_SCANVOTER					0xAB
#define CMD_IDENTIFY					0x77
#define RESP_OK							0xB5
#define RESP_ERROR						0x5B





int main(void) {
    
    char **pFiles;
	char strPort[20];
	char strFileName[256];
	unsigned int baudrate;
	unsigned char bufTemplate[498];
	unsigned char bResponse, bCommand;
	unsigned int index, fRes, fileCount;
	FILE *fpTemplate;
	unsigned char fData;
	unsigned int nbyte;
    
    index = 0;
	pFiles = 0;
	fpTemplate = 0;
	fileCount = 0;
    
    // Initiate serial connection with the device
    
	printf("Enter COMPortName and BaudRate: ");
	scanf("%s %u", strPort, &baudrate);
	fRes = (unsigned int) serial_begin(strPort, baudrate);
	if (fRes != 0) {
		printf("Error occured while connecting to %s\n", strPort);
		return 0;
	}
	else {
		printf("Connection established with: %s\n", strPort);
	}
	
	// Scan for a Live Voter Template (LVT)
	// LVT gets stored to MCU RAM and is unaccessible directly by the host
    
	printf("\nScan Live Voter - Press your finger... ");
    bCommand = CMD_SCANVOTER;
    serial_write(&bCommand, 1);
    serial_read(&bResponse, 1);
    if (bResponse == RESP_ERROR) {
        printf("Error occured while scanning\n");
        goto END;
    }
    else {
        printf("Done scanning\n");
    }
    
    // Collect template file names from the user
    // Calculate & allocate exact amount of memory needed to store file info
    
    printf("Set Template - Enter number of files... ");
    scanf("%u", &fileCount);
    pFiles = (char **) malloc(sizeof(char *) * fileCount);
    memset((void *) pFiles, 0, sizeof(char *) * fileCount);
    printf("Memory allocated for [%u] files\n", fileCount);
    
    printf("\nEnter each file name: ");
    for (index = 0; index < fileCount; index++) {
        scanf("%s", strFileName);
        *(pFiles + index) = (char *) malloc(strlen(strFileName) + 1);
        strcpy(*(pFiles + index), strFileName);
    }
    
    // Send each template from file to device one by one
    
    for (index = 0; index < fileCount; index++) {
        printf("\nSending template %s... ", *(pFiles + index));
        bCommand = CMD_SETTEMPLATE;
        serial_write(&bCommand, 1);
        
        fpTemplate = fopen(*(pFiles + index), "rb");
        if (fpTemplate == 0) {
            printf("File not found: %s\n", *(pFiles + index));
            goto END;
        }
        
        // Read bytes from file one by one until we reach to EOF
        
        nbyte = 0;
        FREAD:
        fread((void *) &fData, 1, 1, fpTemplate);
        if (feof(fpTemplate) != 0)
            goto FCLOSE;
        serial_write(&fData, 1);
        nbyte = nbyte + 1;
        goto FREAD;
        
        FCLOSE:
        fclose(fpTemplate);
        fpTemplate = 0;
        
        // Send location of template storage to device
        
        fData = (unsigned char) index;
        serial_write(&fData, 1);
        serial_read(&bResponse, 1);
        if (bResponse == RESP_OK) {
            printf("Sent [%u] bytes\n", nbyte);
        }
        else {
            printf("Error [%u]\n", nbyte);
        }
    }
    
    // Match our Live Voter Template (LVT) against the stored templates
    
    printf("\nIdentifying Voter... ");
    bCommand = CMD_IDENTIFY;
    serial_write(&bCommand, 1);
    serial_read(&bResponse, 1);
    if (bResponse == RESP_ERROR) {
        printf("Could not identify\n");
    }
    else {
        serial_read(&fData, 1);
        printf("MATCHES to Voter no. %u, File: %s\n", fData, *(pFiles + fData));
    }
    
    // End of program
    // Clean up any allocated memory & terminate serial connection
    
    END:
    serial_end();
    if (pFiles != 0) {
        for (index = 0; index < fileCount; index++) {
            free((void *) *(pFiles + index));
        }
        free((void *) pFiles);
    }
    
    // Return to the host OS
    
	return 0;
}



