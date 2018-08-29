#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h> 


char*chptrarr_DTS[256][256];
char*chptr_delimA=";\n\r";


int Datasetread(char*);
int Bigfolder(char*);
int recursion(char*, int);
int file(char*, int);
char* Templateread(char*, int);
int GetFSize(char*);
char*StringReplace(char*, char*, char*);

int main()
{
	char*Dataset="4car_Dataset.csv";
	char*folder="Project_001";
	chdir("..");
	Datasetread(Dataset);
	Bigfolder(folder);
	chdir("Code");
	return 0;
}

int Datasetread(char* temp_Dataset)
{
	FILE*Dataset;
	char newname[128];
	char bufferDataset[65536];
	char*dts;
	int i;
	int j;
	printf("opening Dataset...\n");
	chdir("Dataset");
	dts=(char*)calloc(128,sizeof(char));
	if((Dataset=fopen(temp_Dataset,"rb+"))==NULL)
	{
		printf("The Dataset file does not exist, Please give the name again!!!\n");
		scanf("%s",newname);
		if((Dataset=fopen(newname,"rb+"))==NULL)
		{
			printf("The Dataset file does not exist!\n");
			exit(1);
		}
    }
	printf("applicating memory for Dataset...\n");
	for(i=0;i<256;i++)
	{
		for(j=0;j<256;j++)
		{
			chptrarr_DTS[i][j]=(char*)calloc(128,sizeof(char));
			if(chptrarr_DTS[i][j]==NULL)
			{
				break;
			}
		}
	}
	j=0;
	printf("extracting the elements from Dataset...\n");
	while(!feof(Dataset))
	{
		if((fgets(bufferDataset,65536,Dataset))==NULL)
		{
			break;
		}
		dts=strtok(bufferDataset,chptr_delimA);
		if(dts==NULL)
		{
			break;
		}
		memmove(chptrarr_DTS[0][j],dts,strlen(dts));
		for(i=1;i<256;i++)
		{
			dts=strtok(NULL,chptr_delimA);
			if(dts==NULL)
			{
				break;
			}
			memmove(chptrarr_DTS[i][j],dts,strlen(dts));
		}
		j++;
	}
	free(dts);
	printf("closing Dataset...\n");
	fclose(Dataset);
	chdir("..");
	return 0;
}

int Bigfolder(char* folder)
{
	int j;
	char*cm;
	cm=(char*)malloc(256*sizeof(char));
	chdir("Output");
	for(j=1;j<256;j++)
	{
		if(*chptrarr_DTS[0][j]==NULL)
		{
			break;
		}
		printf("processing folder '%s'...\n",chptrarr_DTS[0][j]);
		mkdir(chptrarr_DTS[0][j],0777);
		sprintf(cm,"cp -rp ../Template/etc ./%s/",chptrarr_DTS[0][j]);
		system(cm);
		recursion(chptrarr_DTS[0][j],j);
	}									
	free(cm);
	chdir("..");
	return 0;
}

int recursion(char* dirname, int j)
{
    DIR * dir;
    struct dirent * ptr;
    dir = opendir(dirname);
	chdir(dirname);
    while((ptr = readdir(dir)) != NULL)
    {
		printf("d_name : %s\n", ptr->d_name);
		if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)
		{
			if(4==(ptr->d_type))
			{
				recursion(ptr->d_name, j);
			}
			else if(8==(ptr->d_type))
			{
				file(ptr->d_name, j);
			}
		}
    }
    closedir(dir);
	chdir("..");
	return 0;
}

int file(char* filename, int j)
{
	char*TPL;
	char*newname;
	int sz1;
	int sz2;
	int i;
	FILE*Template;
	newname=(char*)malloc(128*sizeof(char));
	sz1=GetFSize(filename);
	TPL=(char*)malloc(2*sz1*sizeof(char));
	TPL=Templateread(filename,sz1);
	if((Template=fopen(filename,"rb+"))==NULL)
	{
		printf("The Template file does not exist, Please give the name again!!!\n");
		scanf("%s",newname);
		if((Template=fopen(newname,"rb+"))==NULL)
		{
			printf("The Template file does not exist!\n");
			exit(2);
		}
    }
	for(i=0;i<256;i++)
	{
		if(*chptrarr_DTS[i][0]==NULL)
		{
			break;
		}
		StringReplace(chptrarr_DTS[i][0],chptrarr_DTS[i][j],TPL);
	}
	sz2=strlen(TPL);
	if(sz2!=sz1)
	{
		fwrite(TPL,sizeof(char),sz2,Template);
	}
	else
	{
		fwrite(TPL,sizeof(char),sz1,Template);
	}
	if((strlen(TPL))>>0)
	{
		free(TPL);
	}
	free(newname);
	return 0;
}

char* Templateread(char*temp_name,int sz)
{
	char*TPL;
	FILE*Template;
	char*newname;
	newname=(char*)malloc(128*sizeof(char));
	TPL=(char*)malloc(2*sz*sizeof(char));
	if((Template=fopen(temp_name,"rb+"))==NULL)
	{
		printf("The Template file does not exist, Please give the name again!!!\n");
		scanf("%s",newname);
		if((Template=fopen(newname,"rb+"))==NULL)
		{
			printf("The Template file does not exist!\n");
			exit(3);
		}
    }
	fread(TPL,sizeof(char),sz,Template);
	TPL[sz]='\0';
	fclose(Template);
	return TPL;
}

int GetFSize(char*strFileName)
{
	int size;
	FILE*Template;
	char*newname;
	newname=(char*)malloc(128*sizeof(char));
	if((Template=fopen(strFileName,"rb+"))==NULL)
	{
		printf("The GetFSize file does not exist, Please give the name again!!!\n");
		
		scanf("%s",newname);
		if((Template=fopen(newname,"rb+"))==NULL)
		{	
			printf("The GetFSize file does not exist!\n");
			exit(4);
		}
	}
    fseek(Template,0L,SEEK_END);
    size=ftell(Template);
    fclose(Template);
	free(newname);
    return size;
}

char*StringReplace(char*search, char*replace, char*string)
{
	char*tempString, *searchStart;
	int len=0;
	if(search==NULL)
	{
		return string;
	}
	searchStart="a";
	while(searchStart!=NULL)
	{
		searchStart=strstr(string, search);
		if(searchStart==NULL)
		{
			return string;
		}
		tempString=(char*)malloc((strlen(string)+1)*sizeof(char));
		if(tempString==NULL)
		{
			return NULL;
		}
		strcpy(tempString, string);
		len=searchStart-string;
		string[len]='\0';
		strcat(string,replace);
		len+=strlen(search);
		strcat(string, (char*)tempString+len);
		free(tempString);
	}
	return string;
}



