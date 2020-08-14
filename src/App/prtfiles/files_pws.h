#ifndef __files_pws_H
#define __files_pws_H

#include "includes.h"
#include "main.h"
#include "ff.h"
#include "tgui.h"




typedef struct
{
	char		mark[12];
	uint32_t	version;
	uint32_t	areanum;
	uint32_t	info_offset;
	uint32_t	undef_1;
	uint32_t	preview_offset;
	uint32_t	undef_2;
	uint32_t	layersdef_offset;
	uint32_t	undef_3;
	uint32_t	layersimg_offset;
} FPWS_HEADER;

typedef struct
{
	char		mark[12];
	uint32_t	header_length;
	float		pixel_size;
	float		layers_thickness;
	float		exp_time;
	float		lightoff_time;
	float		expbottom_time;
	float		bottom_layers;
	float		lift_height;
	float		lift_speed;
	float		down_speed;
	float		resin_volume;
	uint32_t	aa_grade;
	uint32_t	res_x;
	uint32_t	res_y;
	float		resin_weight;
	float		resin_price;
	uint32_t	resin_type;
	uint32_t	use_layer_params;
} FPWS_INFO;

typedef struct
{
	char		mark[12];
	uint32_t	data_length;
	uint32_t	width;
	uint32_t	resolution;
	uint32_t	height;
} FPWS_PREVIEW;





uint8_t			FPWS_ReadFileInfo(FIL *file);
uint8_t			FPWS_SetPointerToPreview(FIL *file);
uint32_t		FPWS_GetPreviewDataOffset();
uint16_t		FPWS_GetPreviewWidth();
uint16_t		FPWS_GetPreviewHeight();
uint8_t			FPWS_DrawPreview(FIL *file, TG_RECT *rect);





#endif /*__files_pws_H */
