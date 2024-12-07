#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *image_fname;
    FILE *fptr_image;

    /* Secret File Info */
    char out_fname[50];
    FILE *fptr_out;

} DecodeInfo;

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status encoded_open_files(DecodeInfo *decInfo);

/* bmp image header */
Status skip_bmp_header(FILE *fptr_image);

/* decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int file_size, DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_to_lsb(char *image_buffer);

/* Decode a int length into LSB of image data array */
Status decode_int_to_lsb(char *image_buffer);

/* Copy data bytes from src to stego image after encoding */
//Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif

