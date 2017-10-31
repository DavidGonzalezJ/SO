#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/* Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	char c;
	if(origin==NULL)return -1;
	int i = 0;
	for(; i < nBytes && origin != EOF; i++){
		/* Copy byte from origin */
		c = fgetc(origin);
		/* Write byte to destination */
		if(fputc(c, destination) ==EOF) return -1;
	}
    
	//If nBytes is not equal to the bytes copied we return -1
	if (i < nBytes)return i;
                            // invoca desde el createtarball byte a byte se copia
	return nBytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
    int strSize = 0;
    char c;
    /* Look for the size of the file name */
    while(c != EOF && c != '\0'){
        c = fgetc(file);
        strSize++;
    }
    
    if(strSize !=0){
        /* Reserve the memory to allocate the file descriptor */
        char * heapStore = malloc(sizeof(char) * strSize);
        /* Put the pointer at the beggining of the file and then it reads the file
         descriptor allocating it in the reserved memory */
        if( fseek(file,0,SEEK_SET) == -1) return NULL;
        if( fread(heapStore, sizeof(char), strSize, file) < strSize) return NULL;
        return heapStore;
    }
    else return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int * nFiles)
{
    stHeaderEntry* pairs;
    if(fread(nFiles, sizeof(int), 1, tarFile) < 1) return NULL;
    
    pairs = malloc (sizeof(stHeaderEntry)*nFiles);
    if(pairs == NULL)return NULL;
    
    char* fileName;
    
    if(fseek(tarFile, sizeof(int),SEEK_SET) == -1) return NULL;
    
    for (int i = 0; i < nFiles; i++) {
        fileName = loadstr(tarFile);
        if(fileName == NULL)return NULL;
        
        pairs[i].name = fileName;
        if(fread(pairs[i].size, sizeof(int),1 ,tarFile) < 1) return NULL;
    }
    
    if(fseek(pairs,0,SEEK_SET) == -1) return NULL;
    
    return pairs;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
    int bytesHeader = 0, copiedBytes = 0;

    FILE* inputFile; // Used for reading each .txt file
    FILE* outputFile; // Used for writing in the output file.
    
    stHeaderEntry* stHeader; // Pointer to the program header struct.
    
    // Create a stHeader in the heap with the correct size
    
    stHeader = malloc(sizeof(stHeaderEntry) * nFiles); // Allocate memory for an array of stHeader Structs
    bytesHeader += sizeof(int) + nFiles * sizeof(unsigned int); // other int for each struct to store the bytes of the file
    
    for (int i = 0; i < nFiles; i++) {
        bytesHeader += strlen(fileNames[i]) + 1; // Sum the bytes for each filename (+1 for the '\0' character)
    }
    
    outputFile =  fopen(tarName, "w"); // Open the name.mtar file for writing the header and data of the files.
    fseek(outputFile, bytesHeader, SEEK_SET); // Move the file's position indicator to the data section (skip the header)
    
    //Copies the files into OutputFile
    for(int i= 0; i < nFiles; i++){
        inputFile = fopen(fileNames[i],"r");
        if(inputFile == NULL) return EXIT_FAILURE;
        
        //Copy the information in outputFile
        int aux;
        fread(&aux, sizeof(int), inputFile);
        copiedBytes = copynFile(inputFile, outputFile, aux);
       
        //Create the pair in memory for the header
        if(copiedBytes == -1) return EXIT_FAILURE;
        else{
            stHeader[i].size = copiedBytes;
            stHeader[i].name = malloc( strlen(fileNames[i]) + 1 );
            strcopy(stHeader[i].name,fileNames[i]);
        }
        
        if(fclose(inputFile)==EOF) return EXIT_FAILURE;
    }
    
    fseek(outputFile, 0, SEEK_SET);
    fwrite(nFiles, sizeof(int), 1, outputFile);
    
    //Copies the heather allocated in memory into OutputFile
    for (int i = 0; i < nFiles; i++) {
        fwrite(stHeader[i].name, strlen(stHeader[i].name)+1, 1, outputFile);
        fwrite(stHeader[i].size, sizeof(int), 1, outputFile);
    }
    
    for(int i = 0; i < nFiles; i++){
        free(stHeader[i].name);
    }
    free(stHeader);
    
    if(fclose(outputFile) == EOF) return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
    //First load the tarball's header into memory.
    stHeaderEntry* header;
    
    FILE* inputFile, outputFile;
    inputFile = fopen(tarName, "r");
    if(inputFile == NULL) return EXIT_FAILURE;
    
    int nFiles;
    if(fread(&nFiles, sizeof(int), 1, inputFile) < 1)return EXIT_FAILURE;
    
    header = readHeader(inputFile, nFiles);
    if(header==NULL) return EXIT_FAILURE;
    
    for (int i= 0; i < nFiles; i++) {
        outputFile = fopen(header[i].name,"w");
        if(outputFile==NULL)return EXIT_FAILURE;
        
        if(copynFile(inputFile, outputFile, header[i].size) == -1) return EXIT_FAILURE;
        
        if(fclose(outputFile) == EOF) return EXIT_FAILURE;
    }
    
    for(int i = 0; i < nFiles; i++){
        free(header[i].name);
    }
    free(header);

	return EXIT_SUCESS;
}
