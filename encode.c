#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#include "encode.h"
#include "common.h"                                       // In this header file our password is there which used while decoding

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);                                   // this function will give the height and width of the image
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)                                                           // opening the files
        printf(YELLOW"INFO:"RESET" Open required files\n");
    else
        return e_failure;

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)         // coping the bmp header files
        printf(YELLOW"INFO:"RESET" Copying Image Header\n");

    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)                                 // encoding magic string
        printf(YELLOW"INFO:"RESET" Encoding Magic String Signature\n");
    
    char * file_extn = strchr(encInfo->secret_fname,'.');                                  // encoding sceret file size and data

    if(encode_secret_file_extn(file_extn,encInfo)==e_success)                             
        printf(YELLOW"INFO:"RESET" Encoding secret.txt File Extenstion\n");

    int file_size = secret_file_size(encInfo->fptr_secret);

    rewind(encInfo->fptr_secret);                                                                                   // rewind the file
    
    if(encode_secret_file_size(file_size,encInfo)==e_success)
        printf(YELLOW"INFO:"RESET" Encoding secret.txt File Size\n");

    if(encode_secret_file_data(encInfo)==e_success)
        printf(YELLOW"INFO:"RESET" Encoding secret.txt File Data\n");

    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)                             //copying remaining data
        printf(YELLOW"INFO:"RESET" Copying Left Over Data\n");
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    sleep(1);
    printf("File That Has Encoded in : "BLUE"%s\n"RESET,encInfo->stego_image_fname);                                  // showing the output file name
    sleep(1);
    return e_success;
}
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if(encInfo->fptr_src_image==NULL)
    {
        printf(RED"Erorr:"RESET" The File is not Opened\n");
        return e_failure;
    }
    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if(encInfo->fptr_secret==NULL)
    {
        printf(RED"Erorr:"RESET" The File is not Opened\n");
        return e_failure;
    }
    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if(encInfo->fptr_stego_image==NULL)
    {
        printf(RED"Erorr:"RESET" The File is not Opened\n");
        return e_failure;
    }
    // Do Error handling

    if(check_capacity(encInfo)==e_failure)                                               // checking the main file space if its is small than secret file the error occur 
    {
        printf(RED"ERROR:"RESET" File Size is Greater than .bmp file\n");
        return e_failure;
    }
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    // No failure return e_success
    return e_success;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buf[54];
    fread(buf,54,1,fptr_src_image);
    fwrite(buf,54,1,fptr_dest_image);                                  // copying all the header files as it is
    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string);
    char * buf = malloc(32);
    fread(buf,32,1,encInfo->fptr_src_image);
    if(encode_int_to_lsb(len,buf)==e_success)
    {
        fwrite(buf,32,1,encInfo->fptr_stego_image);                         // encoding the magic string length in the RGB data
    }

    buf=realloc(buf,8);

    for(int i=0; i<len; i++)
    {
        fread(buf,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(magic_string[i],buf)==e_success)            // encoding the magic string data in the RGB data
        {
            fwrite(buf,8,1,encInfo->fptr_stego_image);
        }
    }

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");

    free(buf);

    return e_success;
}

Status encode_int_to_lsb(int data, char *image_buffer)
{
    for(int i=31; i>=0; i--)
    {
        if(data & 1<<i)                                    // if msb is set then buff lsb set
            image_buffer[32-i-1]|=1;                      // Encoding the interger (4 byte) data in the RBG Data
        else    
            image_buffer[32-i-1]&=~1;                    // clear the msb is set then buf lsb clear
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=7; i>=0; i--)
    {
        if(data & 1<<i)
            image_buffer[8-i-1]|=1;                       // Encoding the char (1 byte) data in the RBG Data
        else
            image_buffer[8-i-1]&=~1;
    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    int len = strlen(file_extn);
    char * buf = malloc(32);
    fread(buf,32,1,encInfo->fptr_src_image);
    if(encode_int_to_lsb(len,buf)==e_success)                                     // encoding the Extension length in the RGB data 
    {
        fwrite(buf,32,1,encInfo->fptr_stego_image);
    }

    buf=realloc(buf,8);

    for(int i=0; i<len; i++)
    {
        fread(buf,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(file_extn[i],buf)==e_success)                        // encoding the Extension in the RGB data
        { 
            fwrite(buf,8,1,encInfo->fptr_stego_image);
        }
    }

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");

    free(buf);

    return e_success;
}

Status secret_file_size(FILE * secret_file)
{
    fseek(secret_file,0,SEEK_END);
    return ftell(secret_file);
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char * buf = malloc(32);
    fread(buf,32,1,encInfo->fptr_src_image);
    if(encode_int_to_lsb(file_size,buf)==e_success)                                // encoding the Secret File Size in the RGB data
    {
        fwrite(buf,32,1,encInfo->fptr_stego_image);
    }

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");

    free(buf);

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char * buf = malloc(8);
    char ch;
    while((ch=fgetc(encInfo->fptr_secret))!=EOF)
    {
        fread(buf,8,1,encInfo->fptr_src_image);
        if(encode_byte_to_lsb(ch,buf)==e_success)                                      // encoding the Secret File Data in the RGB data
        {
            fwrite(buf,8,1,encInfo->fptr_stego_image);
        }
    }

    sleep(1);
    printf(GREEN"INFO:"RESET" Done\n");
    
    free(buf);

    return e_success;

}

/*Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{

}*/

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char  ch;

    while(fread(&ch,1,1,fptr_src)>0)
        fwrite(&ch,1,1,fptr_dest);                                          // copying all the remaining data in the main file to the Output file

    printf(GREEN"INFO:"RESET" Done\n");

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    unsigned  int bmp_file_size=get_image_size_for_bmp(encInfo->fptr_src_image);
    int file_size = secret_file_size(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    rewind(encInfo->fptr_src_image);
    int total_size =(4+strlen(MAGIC_STRING)+4+strlen(strchr(encInfo->secret_fname,'.'))+4+file_size)*8;
    if(total_size>bmp_file_size)
        return e_failure;
    return e_success; 
}