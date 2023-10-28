/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2016-2020 Toradex
 */

#ifndef _TDX_CFG_BLOCK_H
#define _TDX_CFG_BLOCK_H

#include "tdx-common.h"

struct toradex_hw {
	u16 ver_major;
	u16 ver_minor;
	u16 ver_assembly;
	u16 prodid;
};

struct toradex_eth_addr {
	u32 oui:24;
	u32 nic:24;
} __attribute__((__packed__));

struct toradex_som {
	const char *name;
	int is_enabled;
};

enum {
	COLIBRI_PXA270_V1_312MHZ = 1,
	COLIBRI_PXA270_V1_520MHZ,
	COLIBRI_PXA320,
	COLIBRI_PXA300,
	COLIBRI_PXA310, /* 5 */
	COLIBRI_PXA320_IT,
	COLIBRI_PXA300_XT,
	COLIBRI_PXA270_312MHZ,
	COLIBRI_PXA270_520MHZ,
	COLIBRI_VF50, /* 10 */
	COLIBRI_VF61,
	COLIBRI_VF61_IT,
	COLIBRI_VF50_IT,
	COLIBRI_IMX6S,
	COLIBRI_IMX6DL, /* 15 */
	COLIBRI_IMX6S_IT,
	COLIBRI_IMX6DL_IT,
	/* 18 */
	/* 19 */
	COLIBRI_T20_256MB = 20,
	COLIBRI_T20_512MB,
	COLIBRI_T20_512MB_IT,
	COLIBRI_T30,
	COLIBRI_T20_256MB_IT,
	APALIS_T30_2GB, /* 25 */
	APALIS_T30_1GB,
	APALIS_IMX6Q,
	APALIS_IMX6Q_IT,
	APALIS_IMX6D,
	COLIBRI_T30_IT, /* 30 */
	APALIS_T30_IT,
	COLIBRI_IMX7S,
	COLIBRI_IMX7D,
	APALIS_TK1_2GB,
	APALIS_IMX6D_IT, /* 35 */
	COLIBRI_IMX6ULL,
	APALIS_IMX8QM_WIFI_BT_IT,
	COLIBRI_IMX8QXP_WIFI_BT_IT,
	COLIBRI_IMX7D_EMMC,
	COLIBRI_IMX6ULL_WIFI_BT_IT, /* 40 */
	COLIBRI_IMX7D_EPDC,
	APALIS_TK1_4GB,
	COLIBRI_T20_512MB_IT_SETEK,
	COLIBRI_IMX6ULL_IT,
	COLIBRI_IMX6ULL_WIFI_BT, /* 45 */
	APALIS_IMX8QXP_WIFI_BT_IT,
	APALIS_IMX8QM_IT,
	APALIS_IMX8QP_WIFI_BT,
	APALIS_IMX8QP,
	COLIBRI_IMX8QXP_IT, /* 50 */
	COLIBRI_IMX8DX_WIFI_BT,
	COLIBRI_IMX8DX,
	APALIS_IMX8QXP,
	APALIS_IMX8DXP,
	VERDIN_IMX8MMQ_WIFI_BT_IT, /* 55 */
	VERDIN_IMX8MNQ_WIFI_BT,
	VERDIN_IMX8MMDL,
	VERDIN_IMX8MPQ_WIFI_BT_IT,
	VERDIN_IMX8MMQ_IT,
	VERDIN_IMX8MMDL_WIFI_BT_IT, /* 60 */
	VERDIN_IMX8MPQ,
	COLIBRI_IMX6ULL_IT_EMMC,
	VERDIN_IMX8MPQ_IT,
	VERDIN_IMX8MPQ_2GB_WIFI_BT_IT,
	VERDIN_IMX8MPQL_IT, /* 65 */
	VERDIN_IMX8MPQ_8GB_WIFI_BT,
	APALIS_IMX8QM_8GB_WIFI_BT_IT,
	VERDIN_IMX8MMQ_WIFI_BT_IT_NO_CAN,
	VERDIN_AM62Q_WIFI_BT_IT,
	VERDIN_IMX8MPQ_8GB_WIFI_BT_IT, /* 70 */
	VERDIN_AM62S_512MB,
	VERDIN_AM62S_512MB_WIFI_BT_IT,
	VERDIN_AM62D_1G_ET,
	VERDIN_AM62D_1G_IT,
	VERDIN_AM62D_1G_WIFI_BT_IT, /* 75 */
	VERDIN_AM62Q_2G_WIFI_BT_IT,
};

enum {
	DAHLIA = 155,
	VERDIN_DEVELOPMENT_BOARD = 156,
	YAVIA = 173,
};

enum {
	VERDIN_DSI_TO_HDMI_ADAPTER = 157,
	VERDIN_DSI_TO_LVDS_ADAPTER = 159,
};

extern const struct toradex_som toradex_modules[];
extern bool valid_cfgblock;
extern struct toradex_hw tdx_hw_tag;
extern struct toradex_hw tdx_car_hw_tag;
extern struct toradex_eth_addr tdx_eth_addr;
extern u32 tdx_serial;
extern u32 tdx_car_serial;

int read_tdx_cfg_block(void);
int read_tdx_cfg_block_carrier(void);
const char * const get_toradex_carrier_boards(int pid4);
const char * const get_toradex_display_adapters(int pid4);
int try_migrate_tdx_cfg_block_carrier(void);

void get_mac_from_serial(u32 tdx_serial, struct toradex_eth_addr *eth_addr);

#endif /* _TDX_CFG_BLOCK_H */
