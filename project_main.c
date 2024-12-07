/*--------------------------------------------------------------------------------------------------------------------------------------
														STEGANOGRAPHY PROJECT
---------------------------------------------------------------------------------------------------------------------------------------*/
/* 
	Name : Aditya Shete
	Date : 18/09/2024
	Description : STEGANOGRAPHY PROJECT 
				  It is practice of Encryption a file, message, image,or video within another file, message, image, or video.
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"                                      // It is header file which contains all the function prototypes which are used in encoding 
#include "decode.h"                                     // It is header file which contains all the function prototypes which are used in decoding
#include "encode.c"
#include"decode.c"
int main(int argc, char * argv[])                                     // CLA arguments
{
    FILE * file;                                                    // checking for empty file
    
    if(argc>5)
	{
		fprintf(stderr,RED"ERROR:"RESET" Too many Arguments\n");          // showing error if CLA are not proper
        fprintf(stderr,RED"Error %s:"RESET" Encoding: %s -e <.bmp_file> <.text_file> [output file]\n"RED"Error %s:"RESET" Decoding: %s -d <.bmp_file> [output file]\n",argv[0],argv[0],argv[0],argv[0]);
		return e_success;
	}

    if(argc==1)
    {
        fprintf(stderr,RED"Error %s:"RESET" Encoding: %s -e <.bmp_file> <.text_file> [output file]\n"RED"Error %s:"RESET" Decoding: %s -d <.bmp_file> [output file]\n",argv[0],argv[0],argv[0],argv[0]);
        return e_success;
    } 

    if(check_operation_type (argv)==e_encode)
    {
        if(argc<=2)
        {
            fprintf(stderr,RED"Usage:"RESET" %s %s <.bmp> <secret file> [output file]\n",argv[0],argv[1]);               // checking the files are .bmp and .txt/.c/.sh or not
            return e_success;
        }

        EncodeInfo encoInfo;
        if(!strstr(argv[2],".bmp"))
        {
            fprintf(stderr,RED"Usage:"RESET" of <.bmp file> \n");
            return e_success;
        }
        else
        {
            file =fopen(argv[2],"r");
            if(file==NULL)
            {
                printf(RED"Error:"RESET" No such %s file\n",argv[2]);
                return e_failure;
            }

        }       
        if(argc==4 || argc>4)
		{
			if(!((strstr(argv[3],".txt")) || (strstr(argv[3],".c")) || (strstr(argv[3],".sh")) || (strstr(argv[3],".jpg")) || (strstr(argv[3],".bmp")) ))
			{
				fprintf(stderr,RED"ERROR:"RESET" <sercet.extion file> Required\n");
				return e_success;
			}
		}
		else
		{
			fprintf(stderr,RED"ERROR:"RESET" <secret file> Required\n");
			return e_success;
		}
        file = fopen(argv[3],"r");                                          // for checking file is empty or not
        if (file == NULL) 
        {
            printf(RED"Error:"RESET" No such %s file\n",argv[3]);
            return e_failure;
        }
        else if(fgetc(file)==EOF)
        {
            printf(RED"Error:"RESET" %s file is empty !!\n",argv[3]);
            return e_success;
        }
        fclose(file);
        encoInfo.src_image_fname=argv[2];
        encoInfo.secret_fname=argv[3];
        if(argc==5)
        {
            char *ptr = strstr(argv[4],".bmp");
            if ( ptr == NULL)
            {
                if(strchr(argv[4],'.'))
                {
                    printf(RED"ERROR:"RESET" Give only file name<Output file>\n");
                    return e_success;
                }
                strcpy(encoInfo.stego_image_fname,argv[4]);
                strcat(encoInfo.stego_image_fname,".bmp");
            }
            else
            {
                if ( ptr[4] == '\0')
                {
                    strcpy(encoInfo.stego_image_fname,argv[4]);
                }
                else
                {
                    fprintf(stderr,RED"Usage:"RESET" of <.bmp file> \n");
                    return e_success;
                }
            }
        }
        else    
            strcpy(encoInfo.stego_image_fname,"encoded.bmp");

        printf(YELLOW"INFO:"RESET" Doing encoding process\n");

        if(do_encoding(&encoInfo)==e_success)                                     // Encoding function is called
        {
            printf(GREEN"INFO:"RESET" ## Encoding Done Successfully ##\n");
        }
        else
            printf(RED"ERROR:"RESET" ## Encoding Fail ##\n"); 

        return e_success;
    }  

    else if(check_operation_type (argv)==e_decode)                             // checking the operations wheather they are done or not
    {
        if(argc<=2)
        {
            fprintf(stderr,RED"Usage:"RESET" %s %s <.bmp> [output file]\n",argv[0],argv[1]);
            return d_failure;
        }
        DecodeInfo decoInfo;

        if(!strstr(argv[2],".bmp"))
        {
            fprintf(stderr,RED"Usage:"RESET" of <.bmp file>\n");                          // checking the file is .bmp
            return d_failure;
        }
        else
        {
            file =fopen(argv[2],"r");
            if(file==NULL)
            {
                printf(RED"Error:"RESET" No such %s file\n",argv[2]);
                return d_failure;
            }
        }

        decoInfo.image_fname=argv[2];

        if(argc==4)
        {
            if(strchr(argv[3],'.'))
            {
                printf(RED"Error:"RESET" No Required of Extenision!! <file Name only>\n");
                return d_failure;
            }
            strcpy(decoInfo.out_fname,argv[3]);
        }
        else
            strcpy(decoInfo.out_fname,"decoded");
            
        printf(YELLOW"INFO:"RESET" Doing decoding process\n");

        fclose(file);
        
        if(do_decoding(&decoInfo)==d_success)                                         // Decoding function is called
            printf(GREEN"INFO:"RESET" ## Decoding Done Successfully ##\n");
        else
            printf(RED"ERROR:"RESET" ## Decoding Fail ##\n");
        
        return d_success;
    }
    else if(check_operation_type(argv)==e_unsupported)
    {
        printf(RED"usage:"RESET" <option> -e or -d\n");                           // If not given -e or -d then it will show error
    }

    return 0;
}



OperationType check_operation_type (char * argv[])                                 // checking option 
{
    if(strcmp(argv[1],"-e")==e_success || strcmp(argv[1],"-E")==e_success)        // checking by strcmp 
        return e_encode;
    else if(strcmp(argv[1],"-d")==e_success || strcmp(argv[1],"-d")==e_success)
        return e_decode;
    else    
        return e_unsupported;
}
