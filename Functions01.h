#ifndef FUNCTIONS01_H_INCLUDED
#define FUNCTIONS01_H_INCLUDED

void printWelcomeNote();

unsigned short CalculateCheckSum(unsigned short *ptr,int nbytes);

int isGivenTwoCorrectInput(char str[]);

int isGivenThreeCorrectInput( char argv2[]);

int isGivenFourCorrectInput(char argv2[], char argv3[]);

int isGivenFiveCorrectInput( char argv2[], char argv3[]);

int checkIfHostnameOrIpIsCorrect(char ip_value[], char argv[]);

void printHowToScan();

int isEqual(char addr[], char arg[]);

char * getServiceName(int portNo);

void initializeForFastScan(int arr[]);

void initializeForDefaultScan(int arr[]);

void PrintFinalInfo(int count, int openAndFiltered[], char *state[], int closedCount, char ip_value[]);



#endif // FUNCTIONS01_H_INCLUDED
