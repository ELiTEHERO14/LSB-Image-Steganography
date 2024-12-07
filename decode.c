#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "decode.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo)
{
    if(encoded_open_files(decInfo)==d_success)                                // opening the files
        printf(YELLOW"INFO:"RESET" encoded Open required files\n");
    else
        return d_failure;

    if(skip_bmp_header(decInfo->fptr_image)==d_success)                       // Skipping the bmp Header files
        printf(YELLOW"INFO:"RESET" Skipping Image Header\n");


    if(decode_magic_string(decInfo)==d_success)                             // decoding magic string
        printf(YELLOW"INFO:"RESET" Decoding Magic String Signature\n");
    else
    {
        printf(RED"ERROR:"RESET" Magic_string is not equal\n");
        return d_failure;
    }

    if(decode_secret_file_extn(decInfo)==d_success)                              // decoding sceret file extn size and data
        printf(YELLOW"INFO:"RESET" decoding secret.txt File Extenstion\n");


    if(decode_secret_file_data(decInfo)==d_success)                           // decoding sceret file size
        printf(YELLOW"INFO:"RESET" decoding secret.txt File Data\n");
    
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    
    sleep(1);
    printf("File That Has Decoded in : "BLUE"%s\n"RESET,decInfo->out_fname);

    fclose(decInfo->fptr_image);
    fclose(decInfo->fptr_out);

    sleep(1);
    return d_success;
}

Status encoded_open_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_image = fopen(decInfo->image_fname, "r");
   
    if(decInfo->fptr_image==NULL)
    {
        printf(RED"The File is not Opened\n"RESET);
        return d_failure;
    }
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    // No failure return e_success
    return d_success;
}

Status skip_bmp_header(FILE *fptr_image)
{
    fseek(fptr_image,54,SEEK_SET);
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    return d_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    char * buf = malloc(32);
    fread(buf,32,1,decInfo->fptr_image);
    int len = decode_int_to_lsb(buf);
    if(len!=strlen(MAGIC_STRING))
        return d_failure;
    
    buf=realloc(buf,8);
    char * str = malloc(len);
    for(int i=0; i<len; i++)
    {
        fread(buf,8,1,decInfo->fptr_image);
        str[i]=decode_byte_to_lsb(buf);
    }
    
    char pass[20];
    printf(BLUE"Enter the Magic String Passkey : "RESET);                                      // checkin the magic string encoded and the enter magic string is equal or not
    scanf("%[^\n]",pass);
    if(strcmp(str,pass)==0)
    {
        free(buf);
        return d_success;
    }
    else
    {
		free(buf);
	    return d_failure;
    }

    free(str);
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    return d_success; 
}

Status decode_int_to_lsb(char *image_buffer)
{
    int data;
    for(int i=0; i<32; i++)
    {
        if(image_buffer[i] & 1)
            data = (data << 1) | 1;                                   // Decoding the interger (4 byte) data in the Output File
        else
            data = data << 1;
    }
    return data;
}

Status decode_byte_to_lsb(char *image_buffer)                     // Decoding the char (1 byte) data in the Output File
{
    char data;
    for(int i=0; i<8; i++)
    {
        if(image_buffer[i] & 1)
            data = (data << 1) | 1;
        else
            data = data << 1;
    }

    return data;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char * buf = malloc(32);
    fread(buf,32,1,decInfo->fptr_image);
    int len = decode_int_to_lsb(buf);                                 // decoding the Secret File Extension length from the Output File


    buf=realloc(buf,8);
    char * str = malloc(len+1);
    int i;
    for(i=0; i<len; i++)
    {
        fread(buf,8,1,decInfo->fptr_image);
        str[i]=decode_byte_to_lsb(buf);
    }
    str[i]='\0';
    strcat(decInfo->out_fname,str);                                    // merging the extension with the output secret file

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");

    return d_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    decInfo->fptr_out = fopen(decInfo->out_fname, "w");

    char * buf = malloc(32);
    fread(buf,32,1,decInfo->fptr_image);
    int len = decode_int_to_lsb(buf);

    buf=realloc(buf,8);
    char * str = malloc(len);

    int i = 0;
    for(i=0; i<len; i++)
    {
        fread(buf,8,1,decInfo->fptr_image);                                    // decoding the Secret File Data from the Output File
        str[i]=decode_byte_to_lsb(buf);
    }
    
    fwrite(str,len,1,decInfo->fptr_out);
    free(buf);
    free(str);

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    
    return d_success;

}