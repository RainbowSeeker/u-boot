// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 *		default_image specific code abstracted from mkimage.c
 *		some functions added to address abstraction
 *
 * All rights reserved.
 */

#include "imagetool.h"
#include "mkimage.h"

#include <image.h>
#include <tee/optee.h>
#include <u-boot/crc.h>
#include <imximage.h>

static struct legacy_img_hdr header;

static int image_check_image_types(uint8_t type)
{
	if (((type > IH_TYPE_INVALID) && (type < IH_TYPE_FLATDT)) ||
	    (type == IH_TYPE_KERNEL_NOLOAD) || (type == IH_TYPE_FIRMWARE_IVT) ||
	    (type == IH_TYPE_FDT_LEGACY))
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

static int image_check_params(struct image_tool_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static void image_print_header(const void *ptr, struct image_tool_params *params)
{
	image_print_contents(ptr);
}

static int image_verify_header(unsigned char *ptr, int image_size,
			struct image_tool_params *params)
{
	uint32_t len;
	const unsigned char *data;
	uint32_t checksum;
	struct legacy_img_hdr header;
	struct legacy_img_hdr *hdr = &header;

	if (image_size < sizeof(struct legacy_img_hdr)) {
		debug("%s: Bad image size: \"%s\" is no valid image\n",
		      params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTRUCTURE;
	}

	/*
	 * create copy of header so that we can blank out the
	 * checksum field for checking - this can't be done
	 * on the PROT_READ mapped data.
	 */
	memcpy(hdr, ptr, sizeof(struct legacy_img_hdr));

	if (be32_to_cpu(hdr->ih_magic) != IH_MAGIC) {
		debug("%s: Bad Magic Number: \"%s\" is no valid image\n",
		      params->cmdname, params->imagefile);
		return -FDT_ERR_BADMAGIC;
	}

	data = (const unsigned char *)hdr;
	len  = sizeof(struct legacy_img_hdr);

	checksum = be32_to_cpu(hdr->ih_hcrc);
	hdr->ih_hcrc = cpu_to_be32(0);	/* clear for re-calculation */

	if (crc32(0, data, len) != checksum) {
		debug("%s: ERROR: \"%s\" has bad header checksum!\n",
		      params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTATE;
	}

	data = (const unsigned char *)ptr + sizeof(struct legacy_img_hdr);
	len = image_get_data_size(hdr);

	if (image_get_type(hdr) == IH_TYPE_FIRMWARE_IVT)
		/* Add size of CSF minus IVT */
		len -= 0x2060 - sizeof(flash_header_v2_t);

	if (image_size - sizeof(struct legacy_img_hdr) < len) {
		debug("%s: Bad image size: \"%s\" is no valid image\n",
		      params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTRUCTURE;
	}

	checksum = be32_to_cpu(hdr->ih_dcrc);
	if (crc32(0, data, len) != checksum) {
		debug("%s: ERROR: \"%s\" has corrupted data!\n",
		      params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTRUCTURE;
	}
	return 0;
}

static void image_set_header(void *ptr, struct stat *sbuf, int ifd,
				struct image_tool_params *params)
{
	uint32_t checksum;
	time_t time;
	uint32_t imagesize;
	uint32_t ep;
	uint32_t addr;
	int type;
	struct legacy_img_hdr *hdr = (struct legacy_img_hdr *)ptr;

	checksum = crc32(0,
			(const unsigned char *)(ptr +
				sizeof(struct legacy_img_hdr)),
			sbuf->st_size - sizeof(struct legacy_img_hdr));

	time = imagetool_get_source_date(params->cmdname, sbuf->st_mtime);
	ep = params->ep;
	addr = params->addr;

	if (params->type == IH_TYPE_FIRMWARE_IVT)
		/* Add size of CSF minus IVT */
		imagesize = sbuf->st_size - sizeof(struct legacy_img_hdr)
			    + 0x2060 - sizeof(flash_header_v2_t);

	else
		imagesize = sbuf->st_size - sizeof(struct legacy_img_hdr);

	if (params->type == IH_TYPE_FDT_LEGACY)
		type = IH_TYPE_FLATDT;
	else
		type = params->type;

	if (params->os == IH_OS_TEE) {
		addr = optee_image_get_load_addr(hdr);
		ep = optee_image_get_entry_point(hdr);
	}

	/* Build new header */
	image_set_magic(hdr, IH_MAGIC);
	image_set_time(hdr, time);
	image_set_size(hdr, imagesize);
	image_set_load(hdr, addr);
	image_set_ep(hdr, ep);
	image_set_dcrc(hdr, checksum);
	image_set_os(hdr, params->os);
	image_set_arch(hdr, params->arch);
	image_set_type(hdr, type);
	image_set_comp(hdr, params->comp);

	image_set_name(hdr, params->imagename);

	checksum = crc32(0, (const unsigned char *)hdr,
				sizeof(struct legacy_img_hdr));

	image_set_hcrc(hdr, checksum);
}

static int image_extract_subimage(void *ptr, struct image_tool_params *params)
{
	const struct legacy_img_hdr *hdr = (const struct legacy_img_hdr *)ptr;
	ulong file_data;
	ulong file_len;

	if (image_check_type(hdr, IH_TYPE_MULTI)) {
		ulong idx = params->pflag;
		ulong count;

		/* get the number of data files present in the image */
		count = image_multi_count(hdr);

		/* retrieve the "data file" at the idx position */
		image_multi_getimg(hdr, idx, &file_data, &file_len);

		if ((file_len == 0) || (idx >= count)) {
			fprintf(stderr, "%s: No such data file %ld in \"%s\"\n",
				params->cmdname, idx, params->imagefile);
			return -1;
		}
	} else {
		file_data = image_get_data(hdr);
		file_len = image_get_size(hdr);
	}

	/* save the "data file" into the file system */
	return imagetool_save_subimage(params->outfile, file_data, file_len);
}

/*
 * Default image type parameters definition
 */
U_BOOT_IMAGE_TYPE(
	defimage,
	"Default Image support",
	sizeof(struct legacy_img_hdr),
	(void *)&header,
	image_check_params,
	image_verify_header,
	image_print_header,
	image_set_header,
	image_extract_subimage,
	image_check_image_types,
	NULL,
	NULL
);
