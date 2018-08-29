#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>																															/*inluding headers*/
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h> 


char*chptrarr_DTS[256][256];																													/*declare the array for Dataset*/
char*chptr_delimA=";\n\r";																														/*declare the symbol for string seperation*/
char*Dataset;																																	/*declare the pointer for directory of Dataset*/
char*Template;																																	/*declare the pointer for directory of Template*/
char*Output;																																	/*declare the pointer for directory of Output*/
char*buf;																																		/*declare the pointer for label content buf*/

int Temp(char*, char*, char*);																													/*declare the function Temp*/
int Datasetread(char*);																															/*declare the function Datasetread*/
int CarIDfolder(char*);																															/*declare the function CarIDfolder*/
int Recursion(char*, int);																														/*declare the function Recursion*/
int File(char*, int);																															/*declare the function File*/
char* Templateread(char*);																														/*declare the function Templateread*/
int GetFSize(char*);																															/*declare the function GetFSize*/
char* StringReplace(char*, char*, char*, int);																									/*declare the function StringReplace*/
int Movefile(char*);																															/*declare the function Movefile*/


void Datasetset(GtkWidget *widget, gpointer *user_data)																							/*Dataset function*/
{
	Dataset=(char*)malloc(256*sizeof(char));																									/*allocate memmory for Dataset*/
	sprintf(Dataset, gtk_file_chooser_get_filename(user_data));																					/*using the file name from filechooserbutton as Dataset*/
}

void Templateset(GtkWidget *widget, gpointer *user_data)																						/*Templateset function*/
{
	Template=(char*)malloc(256*sizeof(char));																									/*allocate memmory for Template*/
	sprintf(Template, gtk_file_chooser_get_filename(user_data));																				/*using the folder name from filechooserbutton as Template*/
}

void Outputset(GtkWidget *widget, gpointer *user_data)																							/*Outputset function*/
{
	Output=(char*)malloc(256*sizeof(char));																										/*allocate memmory for Output*/
	sprintf(Output, gtk_file_chooser_get_filename(user_data));																					/*using the folder name from filechooserbutton as Output*/
}

void Generate(GtkWidget *widget, gpointer *user_data)																							/*Generate function*/
{
	if(Dataset==NULL)																															/*if no Dataset chosen*/
	{
		Dataset=(char*)malloc(256*sizeof(char));																								/*allocate memmory for Dataset*/
		sprintf(Dataset, "/media/sf_debianshare/Project_001/Dataset/4car_Dataset.csv");															/*using default setting as file name*/
	}
	if(Template==NULL)																															/*if no Template chosen*/
	{
		Template=(char*)malloc(256*sizeof(char));																								/*allocate memmory for Template*/
		sprintf(Template, "/media/sf_debianshare/Project_001/Template/Config-Templates.tar.gz");												/*using default setting as folder name*/
	}
	if(Output==NULL)																															/*if no Output chosen*/
	{
		Output=(char*)malloc(256*sizeof(char));																									/*allocate memmory for Output*/
		sprintf(Output, "/media/sf_debianshare/Project_001/");																					/*using default setting as folder name*/
	}
	buf=(char*)malloc(256*sizeof(char));																										/*allocate memmory for buf*/
	Temp(Dataset, Template, Output);																											/*use the function Temp to make folder Temp and subfolder inside it, copy Dataset and Template into Temp, make folder Output*/
	Datasetread(Dataset);																														/*use the function Datasetread to read the Dataset and save the elements in chptrarr_DTS array*/
	CarIDfolder(Output);																														/*use the function CarIDfolder to create new folder, Copy Template into folder, recursively replace the key words from the Template with words from Dataset, finally compress the folder into .tar.gz format file and remove the folder*/
	Movefile(Output);																															/*use the function Movefile to copy foder Temp/Output and files inside it into Output, delete folder Temp*/
	sprintf(buf,"Complete!");																													/*write "Complete!" into buf*/
	gtk_label_set_text(user_data,buf);																											/*set the label with buf*/
	free(Dataset);																																/*free pointer for directory of Dataset*/
	free(Template);																																/*free pointer for directory of Template*/
	free(Output);																																/*free pointer for directory of	Output*/
	free(buf);
}

int main (int argc, char *argv[])																												/*main function*/
{
    GtkBuilder      *builder;																													/*declare GtkBuilder*/
    GtkWidget       *window;																													/*declare GtkWidget window*/
	GtkWidget		*generate;																													/*declare GtkWidget generate*/
	GtkWidget		*dataset;																													/*declare GtkWidget dataset*/
	GtkWidget		*template;																													/*declare GtkWidget template*/
	GtkWidget		*output;																													/*declare GtkWidget output*/
	GtkWidget		*name;																														/*declare GtkWidget name*/
 
    gtk_init (&argc, &argv);																													/*initialize gtk+*/
 
    builder = gtk_builder_new ();																												/*create a gtk builder*/
    gtk_builder_add_from_file (builder, "beta3.glade", NULL);																					/*load builder from glade file*/
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));																			/*load GtkWidget window*/
	generate = GTK_WIDGET (gtk_builder_get_object (builder, "Generate"));																		/*load GtkWidget generate*/
	dataset = GTK_WIDGET (gtk_builder_get_object (builder, "Dataset"));																			/*load GtkWidget dataset*/
	template = GTK_WIDGET (gtk_builder_get_object (builder, "Template"));																		/*load GtkWidget template*/
	output = GTK_WIDGET (gtk_builder_get_object (builder, "Output"));																			/*load GtkWidget output*/
	name = GTK_WIDGET (gtk_builder_get_object (builder, "label1"));																				/*load GtkWidget name*/
	
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);																		/*connect signal "destroy" of GtkWidget window to gtk_main_quit function*/
	g_signal_connect(dataset, "file-set", G_CALLBACK(Datasetset), dataset);																		/*connect signal "file-set" of GtkWidget dataset to Datasetset function, giving GtkWidget dataset as value*/
	g_signal_connect(template, "file-set", G_CALLBACK(Templateset), template);																	/*connect signal "file-set" of GtkWidget template to Templateset function, giving GtkWidget template as value*/
	g_signal_connect(output, "file-set", G_CALLBACK(Outputset), output);																		/*connect signal "file-set" of GtkWidget output to Outputset function, giving GtkWidget output as value*/
	g_signal_connect(generate, "clicked", G_CALLBACK(Generate), name);																			/*connect signal "clicked" of GtkWidget generate to Generate function, giving GtkWidget name as value*/
	
	gtk_widget_show_all(window); 																												/*show all windows*/
	
	gtk_builder_connect_signals(builder,NULL);																									/*automatically connect all signals inside builder*/
	
	g_object_unref(G_OBJECT(builder));																											/*unload builder*/
	
    gtk_main ();																																/*run gtk_main function*/
	
	return 0;																																	/*return 0*/
}

int	Temp(char*temp_Dataset, char*temp_Template, char*temp_Output)																				/*Temp function*/
{
	char* cm;																																	/*declare cm pointer for command*/
	DIR * dir;																																	/*declare directory pointer dir*/
    struct dirent * ptr;																														/*declare structure ptr*/
	cm=(char*)malloc(256*sizeof(char));																											/*allocate memmory for cm pointer with 256 Bytes*/
	mkdir("Temp",0777);																															/*create folder Temp*/
	mkdir("Temp/Dataset",0777);																													/*create folder Temp/Dataset*/
	mkdir("Temp/Template",0777);																												/*create folder Temp/Template*/
	mkdir("Temp/Output",0777);																													/*create folder Temp/Output*/
	sprintf(cm,"cp -rp %s Temp/Dataset/",Dataset);																								/*combine Dataset with cp command*/
	system(cm);																																	/*copy the Dataset into Temp/Dataset*/
	chdir("Temp");																																/*change directory into Temp*/
	dir = opendir("Dataset");																													/*fill the directory pointer folder Dataset*/
    while((ptr = readdir(dir)) != NULL)																											/*get information from files in folder Dataset, until all files are read*/
    {
		if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)																		/*if the name of the files is not "." or ".."*/
		{
			printf("d_name : %s\n", ptr->d_name);																								/*output the name of Dataset in the folder Dataset*/
			if(8==(ptr->d_type))																												/*if the type of the files are "file"*/
			{
				sprintf(Dataset, ptr->d_name);																									/*write the Datastet pointer with the name of Dataset*/
			}
		}
    }
    closedir(dir);																																/*close the directory pointer*/
	chdir("..");																																/*change into previous directory*/
	sprintf(cm,"cp -rp %s Temp/Template/",Template);																							/*combine Template with cp command*/
	system(cm);																																	/*copy the Template into Temp/Template*/
	chdir("Temp/Template");																														/*change directory into Temp/Template*/
	sprintf(cm,"tar -pxzvf %s",Template);																										/*combine Template with tar command*/
	system(cm);																																	/*decompress the Template into Temp/Template*/
	sprintf(cm,"rm -rp &s",Template);																											/*combine Template with rm command*/
	system(cm);																																	/*delete the Template*/
	chdir("../..");																																/*change into previous directory*/
	mkdir(Output,0777);																															/*create folder Output*/
	free(cm);																																	/*free cm pointer*/
	return 0;																																	/*return 0*/
}

int Datasetread(char* temp_Dataset)																												/*Datasetread funtion*/
{	
	FILE*Dataset;																																/*declare file pointer for Dataset*/
	char*Datasetdir;																															/*declare pointer for directory of Dataset in the Temp folder*/
	char*newname;																																/*declare pointer for input name of the Dataset*/
	char bufferDataset[65536];																													/*declare array for line content of Dataset*/
	char*dts;																																	/*declare pointer for word content of Dataset*/
	int i;																																		/*declare the serial number i for column*/
	int j;																																		/*declare the serial number i for line*/
	printf("opening Dataset...\n");																												/*output "opening Dataset..."*/
	newname=(char*)malloc(128*sizeof(char));																									/*allocate memmory for input name pointer*/
	dts=(char*)calloc(128,sizeof(char));																										/*allocate memmory for word content pointer*/
	Datasetdir=(char*)malloc(512*sizeof(char));																									/*allocate memmory for Dataset directory pointer*/
	sprintf(Datasetdir,"Temp/Dataset/%s", temp_Dataset);																						/*combine "Temp/Dataset/" with Dataset name into Dataset directory*/
	if((Dataset=fopen(Datasetdir,"rb+"))==NULL)																									/*if unable to open Dataset*/
	{
		printf("The Dataset file does not exist, Please give the name again!!!\n");																/*output error and require retry*/
		scanf("%s",newname);																													/*input name of the Dataset again*/
		sprintf(Datasetdir,"Temp/Dataset/%s", newname);																							/*combine "Temp/Dataset/" with newname into Dataset directory*/
		if((Dataset=fopen(Datasetdir,"rb+"))==NULL)																								/*if still unable to open Dataset*/
		{
			printf("The Dataset file does not exist!\n");																						/*output "The Dataset file does not exist!"*/
			exit(1);																															/*exit with error number 1*/
		}
		free(newname);																															/*free pointer for input name*/
    }
	printf("applicating memory for Dataset...\n");																								/*output "applicating memory for Dataset..."*/
	for(i=0;i<256;i++)																															/*loop to allocate memmory for line content*/
	{
		for(j=0;j<256;j++)																														/*loop to allocate memmory for word content*/
		{
			chptrarr_DTS[i][j]=(char*)calloc(128,sizeof(char));																					/*allocate memmory for every word content of Dataset*/
			if(chptrarr_DTS[i][j]==NULL)																										/*if failed*/
			{
				break;																															/*break out of the loop*/
			}
		}
	}
	j=0;																																		/*initialize j with 0*/
	printf("extracting the elements from Dataset...\n");																						/*output "extracting the elements from Dataset..."*/
	while(!feof(Dataset))																														/*loop to get line content of Dataset*/
	{
		if((fgets(bufferDataset,65536,Dataset))==NULL)																							/*if all lines are read*/
		{
			break;																																/*break out of the loop*/
		}
		dts=strtok(bufferDataset,chptr_delimA);																									/*get a word from the line*/
		if(dts==NULL)																															/*if all words are read*/
		{
			break;																																/*break out of the loop*/
		}
		memmove(chptrarr_DTS[0][j],dts,strlen(dts));																							/*move the word into array chptrarr_DTS*/
		for(i=1;i<256;i++)																														/*loop to get the rest of the words*/
		{
			dts=strtok(NULL,chptr_delimA);																										/*get the rest of the words*/
			if(dts==NULL)																														/*if all words are read*/
			{
				break;																															/*break out of the loop*/
			}
			memmove(chptrarr_DTS[i][j],dts,strlen(dts));																						/*move the word into array chptrarr_DTS*/
		}
		j++;																																	/*j plus 1*/
	}
	free(Datasetdir);																															/*free the pointer of Dataset directory*/
	free(dts);																																	/*free the pointer of Dataset word content*/
	printf("closing Dataset...\n");																												/*output "closing Dataset..."*/
	fclose(Dataset);																															/*close Dataset file*/
	return 0;																																	/*return 0*/
}

int CarIDfolder(char* temp_Output)																												/*CarIDfolder function*/
{
	int j;																																		/*declare serial number j for line*/
	char*cm;																																	/*declare pointer for command*/
	cm=(char*)malloc(256*sizeof(char));																											/*allocate memmory for cm*/
	chdir("Temp");																																/*change into Temp folder*/
	chdir("Output");																															/*change into Output folder*/
	for(j=1;j<256;j++)																															/*loop to process folder*/
	{
		if(*chptrarr_DTS[0][j]=='\0')																											/*if all folder are processed*/
		{
			break;																																/*break out of the loop*/
		}
		printf("processing folder '%s'...\n",chptrarr_DTS[0][j]);																				/*output "processing folder XXX"*/
		mkdir(chptrarr_DTS[0][j],0777);																											/*create folder XXX*/
		sprintf(cm,"cp -rp ../Template/etc %s/",chptrarr_DTS[0][j]);																			/*combine CarID with cp command*/
		system(cm);																																/*copy /Template/etc into folder XXX*/
		Recursion(chptrarr_DTS[0][j],j);																										/*use Recursion function to process subfolders and files inside folder XXX*/
		chdir(chptrarr_DTS[0][j]);																												/*change into folder XXX*/
		sprintf(cm,"tar -pzcvf ../%s.tar.gz  etc",chptrarr_DTS[0][j]);																			/*combine CarID with tar command*/
		system(cm);																																/*compress folder etc into XXX.tar.gz*/
		chdir("..");																															/*change into previous folder*/
		sprintf(cm,"rm -r %s",chptrarr_DTS[0][j]);																								/*combine CarID with rm command*/
		system(cm);																																/*delete folder XXX*/
	}
	chdir("../..");																																/*change into original folder*/
	free(cm);																																	/*free pointer of command*/
	return 0;																																	/*return 0*/
}

int Recursion(char* dirname, int j)																												/*Recursion function*/
{
    DIR * dir;																																	/*declare directory pointer of dir*/
    struct dirent * ptr;																														/*declare structure ptr*/
    dir = opendir(dirname);																														/*fill the directory pointer folder XXX*/
	chdir(dirname);																																/*change into folder XXX*/
    while((ptr = readdir(dir)) != NULL)																											/*get information from files in folder XXX, until all files are read*/
    {
		if(strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0)																		/*if the name of the files is not "." or ".."*/
		{
			printf("d_name : %s\n", ptr->d_name);																								/*output the name of file and folder in the folder XXX*/
			if(4==(ptr->d_type))																												/*if it's a folder*/
			{
				Recursion(ptr->d_name, j);																										/*use Recursion fuction to process subfolders*/
			}
			else if(8==(ptr->d_type))																											/*if it's a file*/
			{
				File(ptr->d_name, j);																											/*use File function to process files*/
			}
		}
    }
    closedir(dir);																																/*close directory pointer*/
	chdir("..");																																/*change into previous folder*/
	return 0;																																	/*return 0*/
}

int File(char* filename, int j)																													/*File function*/
{
	char*TPL;																																	/*declare pointer TPL for content of the Template*/
	char*newname;																																/*declare pointer for input name of the Template*/
	int sz, sz1, sz2, i;																														/*declare integer sz, sz1, sz2 for size of the Template and i for serial number of column*/
	FILE*Template;																																/*declare file pointer for Template*/
	newname=(char*)malloc(128*sizeof(char));																									/*allocate memmory for input name pointer*/
	sz=GetFSize(filename);																														/*using GetFSize to get the size of file */
	TPL=(char*)malloc(2*sz*sizeof(char));																										/*allocate memmory for pointer TPL*/
	TPL=Templateread(filename);																													/*use Templateread to get content of Template*/
	sz1=strlen(TPL);																															/*write sz1 with length of TPL befor replacement*/
	if((Template=fopen(filename,"wb+"))==NULL)																									/*if unable to open Template with binary write mode*/
	{
		printf("The Template file does not exist, Please give the name again!!!\n");															/*output error and require retry*/
		scanf("%s",newname);																													/*input Template name*/
		if((Template=fopen(newname,"wb+"))==NULL)																								/*if still unable to open Template with binary write mode*/
		{
			printf("The Template file does not exist!\n");																						/*output "The Template file does not exist!"*/
			exit(2);																															/*exit with error number 2*/
		}
		free(newname);																															/*free the pointer of input name*/
    }
	for(i=0;i<256;i++)																															/*loop to replace word with word from Dataset*/
	{
		if(*chptrarr_DTS[i][0]=='\0')																											/*if all the elements we search are replaced*/
		{
			break;																																/*break out of the loop*/
		}
		StringReplace(chptrarr_DTS[i][0],chptrarr_DTS[i][j],TPL,sz1);																			/*use StringReplace function to search and replace*/
	}
	sz2=strlen(TPL);																															/*write sz2 with length of TPL after replacement*/
	if(sz1!=sz2)																																/*if length doesn't change*/
	{
		fwrite(TPL,sizeof(char),sz2,Template);																									/*write the Template with content in TPL and length after replacement*/
	}
	else																																		/*otherwise*/
	{
		fwrite(TPL,sizeof(char),sz,Template);																									/*write the Template with content in TPL and length before replacement*/
	}
	fclose(Template);																															/*close Template file*/
	if((strlen(TPL))>>0)																														/*if TPL is not empty*/
	{
		free(TPL);																																/*free pointer TPL*/
	}
	return 0;																																	/*return 0*/
}

char* Templateread(char*temp_name)																												/*Templateread function*/
{
	char*TPL;																																	/*declare pointer TPL for content of the Template*/
	FILE*Template;																																/*declare file pointer Template*/
	char*newname;																																/*declare pointer for input name of the Template*/
	int sz;																																		/*declare integer sz for size of the Template*/
	sz=GetFSize(temp_name);																														/*use GetFSize to get the size of the Template*/
	newname=(char*)malloc(128*sizeof(char));																									/*allocate memmory for input name of the Template*/
	TPL=(char*)calloc((2*sz),sizeof(char));																										/*allocate memmory for pointer TPL*/
	if((Template=fopen(temp_name,"rb+"))==NULL)																									/*if unable to open Template with binary read mode*/
	{
		printf("The Template file does not exist, Please give the name again!!!\n");															/*output error and require retry*/
		scanf("%s",newname);																													/*input Template name*/
		if((Template=fopen(newname,"rb+"))==NULL)																								/*if still unable to open Template with binary read mode*/
		{
			printf("The Template file does not exist!\n");																						/*output "The Template file does not exist!"*/
			exit(3);																															/*exit with error number 3*/
		}
		free(newname);																															/*free the pointer of input name*/
    	}
	fread(TPL,sizeof(char),sz,Template);																										/*get the content of the Template and put it in TPL*/
	TPL[sz]='\0';																																/*put an end to it*/
	fclose(Template);																															/*close Template file*/
	return TPL;																																	/*return pointer TPL*/
}

int GetFSize(char*strFileName)																													/*GetFSize function*/
{
	int size;																																	/*declare integer size*/
	FILE*Template;																																/*declare file pointer for Template*/
	char*newname;																																/*declare pointer for input name*/
	newname=(char*)malloc(128*sizeof(char));																									/*allocate memmory for input name*/
	if((Template=fopen(strFileName,"rb+"))==NULL)																								/*if unable to open Template with binary read mode*/
	{
		printf("The GetFSize file does not exist, Please give the name again!!!\n");															/*output error and require retry*/
		scanf("%s",newname);																													/*input Template name*/
		if((Template=fopen(newname,"rb+"))==NULL)																								/*if still unable to open Template with binary read mode*/
		{
			printf("The GetFSize file does not exist!\n");																						/*output "The Template file does not exist!"*/
			exit(4);																															/*exit with error number 4*/
		}
		free(newname);																															/*free the pointer of input name*/
	}
    fseek(Template,0L,SEEK_END);																												/*go through the whole file with Template pointer*/
    size=ftell(Template);																														/*get the position of the Template pointer in order to get the size of that file*/
    fclose(Template);																															/*close Template file*/
    return size;																																/*return size*/
}

char*StringReplace(char*search, char*replace, char*string, int sz)																				/*StringReplace function*/
{
	char*tempString, *searchStart;																												/*declare the temporary pointer tempString, searchStart and integer len*/
	int len=0;																																	/*initialize the length with 0*/
	if(search==NULL)																															/*if there is nothing to search*/
	{
		return string;																															/*return string*/
	}
	searchStart="a";																															/*initialize the searchStart pointer with "a"*/
	while(searchStart!=NULL)																													/*loop to search and replace the target string until all the replacements are done*/
	{
		searchStart=strstr(string, search);																										/*get the substring started with search string from target string*/
		if(searchStart==NULL)																													/*if the search string is not in the target string*/
		{
			return string;																														/*return the target string*/
		}
		tempString=(char*)malloc((sz+1)*sizeof(char));																							/*allocate memmory for the pointer of temporary string*/
		if(tempString==NULL)																													/*if there is no free space*/
		{
			return NULL;																														/*return empty pointer*/
		}
		strcpy(tempString, string);																												/*point the temporary pointer tempString to target string*/
		len=searchStart-string;																													/*get the length of the first string part before search string*/
		string[len]='\0';																														/*cut the target string with putting an end on the position before search string*/
		strcat(string,replace);																													/*combine the first string part with the replace string*/
		len+=strlen(search);																													/*get the length of the first string plus the replace string*/
		strcat(string, (char*)tempString+len);																									/*combine the first string part, seplace string and the rest string part*/
		free(tempString);																														/*free the tempString pointer*/
	}
	return string;																																/*return the string after replacements*/
}

int Movefile(char* temp_Output)																													/*Movefile function*/
{
	char* cm;																																	/*declare pointer cm for command*/
	cm=(char*)malloc(256*sizeof(char));																											/*allocate memmory for pointer cm*/
	sprintf(cm, "cp -rp Temp/Output %s", temp_Output);																							/*combine directory Output with Temp/Output*/
	system(cm);																																	/*copy the folder Temp/Output into Output*/
	system("rm -r Temp");																														/*Delete Temp folder*/
	free(cm);																																	/*free pointer cm*/
	return 0;																																	/*return 0*/
}
