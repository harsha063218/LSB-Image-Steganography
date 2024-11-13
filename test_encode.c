/*
NAME                      :K.HARSHAVARDHAN
SERIAL NO                 :24016_169
PROJECT NAME              :STEGANOGRAPHY
DESCRIPTION ABOUT PROJECT :Steganography is the art of hiding information within another
                           medium, such as text, image, audio, or video files, in such a 
						   way that the presence of the information is concealed.
						   In this project, we implement an image-based steganography 
						   technique where secret data (such as text) is embedded into the
						   pixel values of an image file (BMP format) by manipulating the 
						   least significant bits (LSB). The project provides a method for
						   encoding and decoding messages, allowing users to securely 
						   communicate hidden information.
*/ 
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char **argv)
{
		if (argc > 1 && argc < 8)
		{		
				if ((check_operation_type(argv)) == e_encode)
				{
						EncodeInfo encInfo;
						if (read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
						{ 
							if (do_encoding(&encInfo) == e_failure)
							{
								fprintf(stderr,"Error: %s function failed\n","do_encoding()");
								return 1;
							}
							fclose(encInfo.fptr_src_image);
							fclose(encInfo.fptr_secret);
							fclose(encInfo.fptr_stego_image);
							printf("INFO: Encoding Done Successfully \n");
						}
						else
						{
								fprintf(stderr, "Error: %s function failed\n", "read_and_validation_encode_args()");
								return 1;
						}
				}
				else if (check_operation_type(argv) == e_decode)
				{
					DecodeInfo decInfo;
					if (read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
					{
						if (do_decoding(&decInfo) == e_success)
						{
							printf("INFO: Decoding Done Successfully\n");
							fclose(decInfo.fptr_stego_image);
							fclose(decInfo.fptr_output_file);
						}
						else
						{
							fprintf(stderr,"Error: %s function failed\n","do_decoding()");
							return e_failure;
						}
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n", "read_and_validate_decode_args()");
						return e_failure;
					}
				}
				else
				{
					fprintf(stderr,"Error: Invalid option\n");
					printf("For Encoding give like this: <./a.exe or ./a.out> -e <.bmp file> <.txt file> [Output file]\nFor Deconding give like this: <./a.exe or ./a.out> -d <.bmp file> [output file]\n");
					return 1;
				}
		}
		else
		{
			printf("For Encoding give like this: <./a.exe or ./a.out> -e <.bmp file> <.txt file> [Output file]\nFor Deconding give like this: <./a.exe or ./a.out> -d <.bmp file> [output file]\n");
			return 1;
		}
		return 0;
}

