#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<conio.h>
void getType(char*string, char** arrayOfHeadersTypes);
bool isDigit(char*string);
void strsep(char* string, char** tokens);
void getHead(char*string, int &counterFields, char** arrayOfHeaders);
int insertInto(char* string, FILE* result, int counterFields, int line);
FILE* getFile(char* file_name);
FILE* prepareNewFile();
void createTable(char* tableName, FILE* result);
char**	createArray();
int doFirsContainNames(char n, char* file_name, char* estr, char* row, FILE* fp, int counterFields, char** arrayOfHeaders, char** arrayOfHeadersTypes);
void writeFields(char* tableName, FILE* result, int counterFields, char** arrayOfHeaders, char** arrayOfHeadersTypes);
int writeINSERTS(int error, char* row, FILE* result, int line, char* estr, int counterFields, char* tableName, FILE* fp);


int main()
{
	FILE *fp, *result;
	int i=0, j=0, counterFields=0;
	char row[300], file_name[50];
	char* estr;
	char tableName[50];
	char n;
	char** arrayOfHeaders;
	char** arrayOfHeadersTypes;
	int line=1;
	int error=0;

	fp=getFile(file_name);//get file to be convert
	result=prepareNewFile();// create new SQL-file
	createTable(tableName, result); //ask name of table and write it in SQL- file 
	arrayOfHeadersTypes=createArray(); //create array for Headers Types
	arrayOfHeaders=createArray(); //create array for fields name
	counterFields=doFirsContainNames(n, file_name, estr, row, fp, counterFields, arrayOfHeaders, arrayOfHeadersTypes);/*ask if first row of CSV contain fields name, and count number of fields
	if do, write it in array for fields name. If do not names fields by FIELD 1 ets */
	writeFields(tableName, result, counterFields, arrayOfHeaders, arrayOfHeadersTypes); //write fields name and fields type
	writeINSERTS(error, row, result, line, estr, counterFields, tableName, fp); // write inserts

	fclose(fp);
	fclose(result);
	getch();
	return 0;
}

void getType(char*string, char** arrayOfHeadersTypes)//define type of data
{
	char **tokens;
	int i=0;
	tokens=(char**)calloc(50, sizeof(char*));//array for components (values)
	for(int a=0; a<50; a++){
		tokens[a]=(char*)calloc(300, sizeof(char));
		}	
	double doubleValue;
    strsep(string, tokens);//divide row by comma
	for (i=0; *(tokens[i]) > NULL; i++) {
    	char* token = tokens[i];
        	if(strchr(token, '.') != NULL)//if token contain "." token is float
    	{
    		doubleValue = strtod(token, NULL);
    		sprintf(arrayOfHeadersTypes[i], "float(53)");
		}
        else if (isDigit(token))/*if true token is integer*/ {
			int a = atoi(token);
        	sprintf(arrayOfHeadersTypes[i], "int");
        } else {// else token is string
			sprintf(arrayOfHeadersTypes[i], "varchar(255)");
		}
    }
}

bool isDigit(char*string) //define if string is digit
{
	for (int i=0;i<strlen(string); i++){
		if (!isdigit(string[i]))
        {
           return false;
        }
    }
    return true;
}

void strsep(char* string, char** tokens) //divide string on tokens
{
	int i=0,j=0,k=0;
	while (string[i] !='\n'){
		if(string[i]=='"') {// if string begins with '"'- value should be string
			i++;
			
			while(string[i]!='"' || string[i+1]!=',') {//copy avery item till string ends
				
				tokens[j][k]=string[i];
				i++;
				k++;
			}
			i=i+2;
			j++;
			k=0;
		} else {
			tokens[j][k]=string[i];
			i++;
			k++;
			if(string[i]==',' && string[i+1]==','){//if after comma follow another comma - no value
				j++;
				k=0;
				tokens[j][k]=' ';
				i=i+2;	
				j++;
				k=0;
				
			}
			if(string[i]==','){ // go to next value
				i++;	
				j++;
				k=0;
			}
		}
	}
}

void getHead(char*string, int &counterFields, char** arrayOfHeaders)//define number of columns and take name of columns
{
	char **tokens;
	int i=0;
	tokens=(char**)calloc(50, sizeof(char*));
	for(int a=0; a<50; a++){
		tokens[a]=(char*)calloc(300, sizeof(char));
	}	
	double doubleValue;
    strsep(string, tokens);
	for (i=0; *(tokens[i]) > NULL; i++) {
    	char* token = tokens[i];
        	if(strchr(token, '.') != NULL){
    		doubleValue = strtod(token, NULL);
    		}
        else if (isDigit(token)) {
			int a = atoi(token);
        
        } else {
			sprintf( arrayOfHeaders[i],"%s", token);
		}
    }
    if(counterFields==0){
    	counterFields=i; 
    }
}

int insertInto(char* string, FILE* result, int counterFields, int line){
	char **tokens;
	int i=0;
	tokens=(char**)calloc(50, sizeof(char*));
	for(int a=0; a<50; a++){
		tokens[a]=(char*)calloc(300, sizeof(char));
		}	
	double doubleValue;
    strsep(string, tokens);
	for (i=0; *(tokens[i]) > NULL; i++) {
		if(i==0) fprintf(result, "(");
    	char* token = tokens[i];
        if(strchr(token, '.') != NULL){
    		char type[20]="float(53)";
    		fprintf(result, "'%s'", token);
		}
        else if (isDigit(token)){
			char type[20]="int";
			fprintf(result, "%s", token);
        } else {
		char type[20]="varchar(255)";
		fprintf(result, "'%s'", token);
		}
		if(i<counterFields-1)fprintf(result, ",");
		if(i==counterFields-1)fprintf(result, ")");
    }
    if (i!=counterFields) {
    	printf("\nERROR: in line %i detected %i fields, Header contain %i\n", line, i, counterFields);//if in number values in row not equal to number values defined erlyer eror in line or file is not CSV file
    	return 1;
	}
	return 0;
}

FILE* getFile(char* file_name){
	FILE* fp;
	
	printf("1.Put the File You want to convert in folder with this aplication and press 'Enter'\n");
	getch();
	printf("2.Enter name of file you whant to convert\n");
	do{
		scanf("%s", file_name);
		fp=fopen(file_name, "r");//serch for file to be convert
		if(fp==NULL){
			printf("File not found, repeat input\n");
		}
	}
	while(fp==NULL);
	return fp;
}

FILE* prepareNewFile(){//create SQL file
	FILE* result;
	result=fopen("Data_Base.sql", "w");
	fclose(result);
	result=fopen("Data_Base.sql", "a");
	return result;
}
void createTable(char* tableName, FILE* result){
	printf("3.Enter name of table(note: do not use space): ");
	scanf("%s", tableName);
	fprintf(result, "CREATE TABLE IF NOT EXISTS %s (\n", tableName);
}

char**	createArray(){//create array end returns pointer to it
		char**arrayOfHeadersTypes;
	arrayOfHeadersTypes=(char**)calloc(50, sizeof(char*));
	for(int a=0; a<50; a++){
		arrayOfHeadersTypes[a]=(char*)calloc(300, sizeof(char));
		}
		return 	arrayOfHeadersTypes;	
}

int doFirsContainNames(char n, char* file_name, char* estr, char* row, FILE* fp, int counterFields, char** arrayOfHeaders, char** arrayOfHeadersTypes){// ask if 1st row contain names of fields
		printf("4.If '%s' first row contain column names press Y, if doesn't press N", file_name);
	do {
	n=getch();
	printf("\n%c\n", n);
	}
	while (n!='Y' && n!='N');
	
	if(n=='Y'||n=='y'){//if yes prepare names of field wor writing to SQL
		estr=fgets(row, 300, fp);
		printf("5.Creating Head...\n");
		getHead(row, counterFields, arrayOfHeaders);
		estr=fgets(row, 300, fp);
		getType(row, arrayOfHeadersTypes);
		
	};
	if(n=='N'||n=='n'){// if do not prepare its own names for field
		estr=fgets(row, 300, fp);
		printf("5.Creating Head...\n");
		getHead(row, counterFields, arrayOfHeaders);
		for(int i=0;i<counterFields;i++){
			sprintf(arrayOfHeaders[i], "Field_%i", i+1);
		}
		getType(row, arrayOfHeadersTypes);
	}
	return counterFields;
}

void writeFields(char* tableName, FILE* result, int counterFields, char** arrayOfHeaders, char** arrayOfHeadersTypes){
	int i=0;
		while(*arrayOfHeaders[i]!=NULL && *arrayOfHeadersTypes[i]!=NULL){
		fprintf(result, "%s %s", arrayOfHeaders[i], arrayOfHeadersTypes[i]);
		if(i<counterFields-1)fprintf(result, ",\n");
		printf("%i %s %s\n", i+1, arrayOfHeaders[i], arrayOfHeadersTypes[i]);
		i++;
	}	
	printf("6.Inputing data...\n");
	fprintf(result, "\n);\n\nINSERT INTO %s (", tableName);
	for(i=0; i<counterFields; i++){
		fprintf(result, "%s", arrayOfHeaders[i]);
		if(i<counterFields-1)fprintf(result, ", ");
	}
	fprintf(result, ") VALUES\n");
}
int writeINSERTS(int error, char* row, FILE* result, int line, char* estr, int counterFields, char* tableName, FILE* fp){// copy values from CSV to SQL
		while(1){
		error=insertInto(row, result, counterFields, line);
		if (error==1) return 0;
		estr=fgets(row, 300, fp);
		if (estr==NULL)break;
		fprintf(result, ",\n", tableName);
		line++;	
	}
	fprintf(result, ";");
	printf("Dump created.");
}
