#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height, bits_per_pixel;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
	fseek(fptr_image, 2L, SEEK_CUR);
	fread(&bits_per_pixel, sizeof(short), 1, fptr_image);
	rewind(fptr_image);
    return width * height * bits_per_pixel;
}
Status open_encode_files(EncodeInfo *encInfo)
{
	static int open_count = 0;
	if (open_count == 0)
	{
		open_count++;
    	// Src Image file
    	encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_src_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    		return e_failure;
    	}
		else
		{
			printf("Opened %s\n", encInfo->src_image_fname);
		}
	
   		 // Secret file
    	encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    	// Do Error handling
    	if (encInfo->fptr_secret == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    		return e_failure;
    	}
		else
		{
			printf("Opened %s\n", encInfo->secret_fname);
			printf("Done\n");
		}
	}
	else
	{
    	encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    	if (encInfo->fptr_stego_image == NULL)
    	{
    		perror("fopen");
    		fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    		return e_failure;
    	}
		else
		{
			printf("opened %s\n", encInfo->stego_image_fname);
		}
	}
    return e_success;
}
OperationType check_operation_type(char *argv[])
{
	
	if ((strcmp(argv[1],"-e")) == 0)
	{
		return e_encode;
	}
	else if ((strcmp(argv[1], "-d")) == 0)
	{
		return e_decode;
	}
	else
	{
		return e_unsupported;
	}
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
		if (argc > 2)
		{
			if (strcmp(strstr(argv[2],"."), ".bmp") == 0)
			{
				encInfo->src_image_fname = argv[2];
			}
			else
			{
				fprintf(stderr,"Error: Source image %s format should be .bmp\n", argv[2]);
				return e_failure;
			}
		}
		else
		{
				fprintf(stderr,"Error: Arguments are missing\n");
				printf("For Encoding: ./a.exe -e <.bmp file> <.txt file> [Output file]\n");
				printf("For Deconding: ./a.exe -d <.bmp file> [output file]\n");
				return e_failure;
		}
		
		if (argc > 3)
		{
			strncpy(encInfo->extn_secret_file, strstr(argv[3], "."), 4);
			if ((strncmp(encInfo->extn_secret_file, ".txt", 4) == 0))
			{
				encInfo->secret_fname = argv[3];
			}
			else
			{
				fprintf(stderr,"Error: Secrete file %s format should be .txt\n", argv[3]);
				return e_failure;
			}
		}
		else
		{
				fprintf(stderr,"Error: Arguments are missing\n");
				printf("For Encoding: ./a.exe -e <.bmp file> <.txt file> [Output file]\n");
				printf("For Deconding: ./a.exe -d <.bmp file> [output file]\n");
				return e_failure;
		}
		if (argc > 4)
		{
			if (strcmp(strstr(argv[4],"."), ".bmp") == 0)
			{
				encInfo->stego_image_fname = argv[4];
			}
			else
			{
				fprintf(stderr,"Error: Output image %s format should be .bmp\n", argv[2]);
				return e_failure;
			}
		}
		else
		{
			encInfo->stego_image_fname = NULL;
		}
		if (argc > 5)
		{
			if (strcmp(argv[5], "-p") == 0)
			{
				if (argc > 6)
				{
					encInfo->passcode = argv[6];
					encInfo->passcode_size = strlen(encInfo->passcode);
				}
				else
				{
					fprintf(stderr,"Error: No passcode found\n");
					return e_failure;
				}
			}
			else
			{
				fprintf(stderr,"Error: Invalid option\n");
				return e_failure;
			}
		}
		else
		{
			encInfo->passcode = NULL;
			encInfo->passcode_size = 1;
		}

	return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");
    if (open_encode_files(encInfo) == e_success)
    {
    	printf("INFO: Encoding Process Started \n");	
    	printf("INFO: Checking for %s size\n", encInfo->secret_fname);
    	encInfo->size_secret_file = (long) get_file_size(encInfo->fptr_secret);
    	if (encInfo->size_secret_file)
   		{
    		printf("INFO: Done.\n");
        	printf("INFO: Checking for %s capacity %s\n", encInfo->src_image_fname, encInfo->secret_fname);
      		encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
			if (check_capacity(encInfo) == e_success)
			{
				printf("INFO: Done. Found OK\n");
				if (encInfo->stego_image_fname  == NULL)
				{
					printf("INFO: Output File not mentioned. ");
					encInfo->stego_image_fname = "steged_img.bmp";
					printf("INFO: Creating steged_img.bmp as default\n");
					if (open_encode_files(encInfo) == e_failure)
					{
						fprintf(stderr,"Error: %s function failed\n", "open_encode_files");
						return e_failure;		
					}
				}
				else
				{
					printf("INFO: Output File mentioned. ");
					if (open_encode_files(encInfo) == e_failure)
					{
						fprintf(stderr,"Error: %s function failed\n", "open_encode_files");
						return e_failure;
					}
					printf("INFO: Opened %s\n",encInfo->stego_image_fname);
				}

				printf("INFO: Copying Image Header\n");
				if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
				{
					printf("INFO: Done\n");

					printf("INFO: Encoding Magic String\n");
					if (encode_magic_string(encInfo->passcode, encInfo) == e_success)
					{
						printf("INFO: Done\n");

						printf("INFO: Encoding %s File Extenstion Size\n", encInfo->secret_fname);
						if (encode_secret_file_extn_size(encInfo) == e_success)
						{
							printf("INFO: Done\n");
						
							printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
							if (encode_secret_file_extn(encInfo) == e_success)
							{
								printf("INFO: Done\n");
							
								printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
								if (encode_secret_file_size(encInfo) == e_success)
								{
									printf("INFO: Done\n");
								
									printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
									if (encode_secret_file_data(encInfo) == e_success)
									{
										printf("INFO: Done\n");
								
										printf("INFO: Copying remaining  Data\n");
										if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
										{
											printf("INFO: Done\n");
										}
										else
										{
											fprintf(stderr, "Error: %s function failed\n", "copy_remaining_img_data()");
											return e_failure;
										}
									}
									else
									{
										fprintf(stderr, "Error: %s function failed\n", "encode_secret_file_data()");
										return e_failure;
									}	
								}
							}
							else
							{
								fprintf(stderr, "Error: %s function failed\n", "encode_secret_file_extn()");
								return e_failure;
							}
						}
						else
						{
							fprintf(stderr,"Error: %s function failed\n","encode_secret_file_extn()");
							return e_failure;
						}
					}
					else
					{
						fprintf(stderr,"Error: %s function failed\n","encode_magic_string()");
						return e_failure;
					}

				}
				else
				{
					fprintf(stderr,"Error: %s function failed\n","copy_bmp_header()");
					return e_failure;
				}
			
			}
			else
			{
				fprintf(stderr,"Error: %s function failed\n","check_capacity()");
				return e_failure;
			}
     	}
     	else
     	{   
			printf("INFO: Secret file is empty\n");
        	return e_failure;
     	}
	}
 	else
   	{
    	fprintf(stderr, "Error: %s function failed\n","open_encode_files()");
       	return 1;
    }

	 return e_success;
}

uint get_file_size(FILE *fptr)
{
	uint size;
	fseek(fptr, 0L, SEEK_END);
	size = (uint) ftell(fptr);
	rewind(fptr);
	return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
	uint bytes_for_secret_file_size = 4;
	if (encInfo->image_capacity >= (MAX_IMAGE_BUF_SIZE * (encInfo->passcode_size + MAX_FILE_SUFFIX + (uint)sizeof(encInfo->extn_secret_file) + bytes_for_secret_file_size + encInfo->size_secret_file)))
	{
		return e_success;
	}
	return e_failure;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	uint raster_data;
	fseek(fptr_src_image, 10L, SEEK_SET);
	fread(&raster_data, sizeof(int), 1, fptr_src_image);
	rewind(fptr_src_image);
	char header_data[raster_data];
	fread(header_data, sizeof(char), (size_t)raster_data, fptr_src_image);
	if (ferror(fptr_src_image))
	{
		fprintf(stderr,"Error: While reading the data from source image file\n");
		clearerr(fptr_src_image);
		return e_failure;
	}
	fwrite(header_data, sizeof(char), (size_t)raster_data, fptr_dest_image);
	if (ferror(fptr_dest_image))
	{
		fprintf(stderr,"Error: While writing the data from destination image file\n");
		clearerr(fptr_dest_image);
		return e_failure;
	}
	return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	if (magic_string != NULL)
	{
		for (uint i = 0; i < encInfo->passcode_size; i++)
		{
			if ((strncpy(encInfo->secret_data, (magic_string + i), 1)) == NULL)
			{
				return e_failure;
			}
			fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
			if (ferror(encInfo->fptr_src_image))
			{
				fprintf(stderr,"Error: While reading the data from source nimage file\n");
				clearerr(encInfo->fptr_src_image);
				return e_failure;
			}
			if (encode_byte_to_lsb(encInfo->secret_data[0], encInfo->image_data) == e_failure)
			{
				fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
				return e_failure;
			}
			fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
			if (ferror(encInfo->fptr_stego_image))
			{
				fprintf(stderr,"Error: While writing the data to destination image file\n");
				clearerr(encInfo->fptr_stego_image);
				return e_failure;
			}
		}
	}
	
	if ((strncpy(encInfo->secret_data, MAGIC_STRING, 1)) == NULL)
	{
			return e_failure;
	}
	fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
	if (ferror(encInfo->fptr_src_image))
	{
			fprintf(stderr,"Error: While reading the data from source image file\n");
			clearerr(encInfo->fptr_src_image);
			return e_failure;
	}
	if (encode_byte_to_lsb(encInfo->secret_data[0], encInfo->image_data) == e_failure)
	{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
	}
	fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}

Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
	uint mask = 0x80000000;
	uint len;
	for (len = 0; (len < MAX_FILE_SUFFIX) && (encInfo->extn_secret_file[len] != '\0'); len++);
	uint size_bits = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
	char data[size_bits];
	for (int i = 0; i < size_bits; i++)
	{
		if ((uint) len & mask)
		{
			data[i] = 1;
		}
		else
		{
			data[i] = 0;
		}
		mask >>= 1;
		data[i] = (char) ((char) (fgetc(encInfo->fptr_src_image) & (~(0x01))) | (data[i]));
	}
	fwrite(data, sizeof(char), (size_t) size_bits, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
	uint len;
	for (len = 0; (len < MAX_FILE_SUFFIX) && (encInfo->extn_secret_file[len] != '\0'); len++);

	for (uint i = 0; i < len; i++)
	{
		fread(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);
		if (ferror(encInfo->fptr_src_image))
		{
			fprintf(stderr,"Error: While reading the data from source nimage file\n");
			clearerr(encInfo->fptr_src_image);
			return e_failure;
		}
		if (encode_byte_to_lsb(encInfo->extn_secret_file[i], encInfo->image_data) == e_failure)
		{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
		}
		fwrite(encInfo->image_data, sizeof(char), MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);
		if (ferror(encInfo->fptr_stego_image))
		{
			fprintf(stderr,"Error: While writing the data to destination image file\n");
			clearerr(encInfo->fptr_stego_image);
			return e_failure;
		}
	}
	return e_success;
}

Status encode_secret_file_size(EncodeInfo *encInfo)
{
	uint mask = 0x80000000;
	uint len = (uint) encInfo->size_secret_file;
	uint size_bits = MAX_IMAGE_BUF_SIZE * (uint) sizeof(int);
	char data[size_bits];
	for (int i = 0; i < size_bits; i++)
	{
		if ((uint) len & mask)
		{
			data[i] = 1;
		}
		else
		{
			data[i] = 0;
		}
		mask >>= 1;
		data[i] = (char) ((char) (fgetc(encInfo->fptr_src_image) & ~(0x01))| (data[i]));
	}
	fwrite(data, sizeof(char), (size_t) size_bits, encInfo->fptr_stego_image);
	if (ferror(encInfo->fptr_stego_image))
	{
 		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	char data[(int) encInfo->size_secret_file];
	fread(data, sizeof(char), (uint) encInfo->size_secret_file, encInfo->fptr_secret);
	if (ferror(encInfo->fptr_secret))
	{
		fprintf(stderr,"Error: While reading the data to secret file\n");
		clearerr(encInfo->fptr_secret);
		return e_failure;
	}
	if (encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
		fprintf(stderr, "Error: %s function failed\n", "encode_data_to_image()");
		return e_failure;
	}
	return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char image_buffer[MAX_IMAGE_BUF_SIZE];
	for(uint i = 0; i < size; i++)
	{
		fread(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_src_image);
		if (encode_byte_to_lsb(data[i], image_buffer)  == e_success)
		{

			fwrite(image_buffer, sizeof(char), MAX_IMAGE_BUF_SIZE, fptr_stego_image);
		}
		else
		{
			fprintf(stderr,"Error: %s function failed\n", "encode_byte_to_lsb()");
			return e_failure;
		}
	}
	return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
	unsigned char mask = 0x80;
	for(uint i = 0; i < MAX_IMAGE_BUF_SIZE; i++)
	{
		if ( data & mask)
		{
			image_buffer[i] = (image_buffer[i] | (unsigned char) (0x01));
		}
		else
		{
			image_buffer[i] = (image_buffer[i] & (unsigned char) (~(0x01)));
		}
		mask >>= 1;
	}
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	int ch;
	while ((ch = getc(fptr_src)) != EOF)  
    {
		if (ferror(fptr_src))
		{
			fprintf(stderr, "Error: unable to read from %s", "Source image file");
			clearerr(fptr_src);
			return e_failure;
		}
      	putc(ch, fptr_dest);
		if (ferror(fptr_dest))
		{
			fprintf(stderr, "Error: unable to write to  %s", "Destination image file");
			clearerr(fptr_dest);
			return e_failure;
		}
    }
	return e_success;
}
