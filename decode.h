#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "common.h"

#define DECODE_FILE_EXTN_SIZE 32

#define DECODE_FILE_SIZE 32

typedef struct  _DecodeInfo
{
	char *stego_image_fname;
	FILE *fptr_stego_image;
	uint image_data_size;
	char image_data[MAX_IMAGE_BUF_SIZE];

	char *output_file_name;
	FILE *fptr_output_file;
	char extn_output_file[MAX_FILE_SUFFIX];
	char decode_data[MAX_SECRET_BUF_SIZE];

	char *passcode;
	uint passcode_size;

}	DecodeInfo;


Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

Status decode_output_file_extn_size(DecodeInfo *decInfo);

Status decode_output_file_extn(uint extn_size, DecodeInfo *decInfo);

Status decode_file_size(DecodeInfo *decInfo);

Status decode_data_to_file(DecodeInfo *decInfo);

Status decode_lsb_to_byte(char *decode_data, char *image_data);

#endif
