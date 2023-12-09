/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef _HALRF_HWIMG_RAW_DATA_W_BT_8852C_H_
#define _HALRF_HWIMG_RAW_DATA_W_BT_8852C_H_

#if 1
const u32 array_mp_8852c_radiob_with_bt_con[] = {
		//LUT_RXG_GAIN 2022/08/29
		//BT connect, 
		0xEF, 0x04000,
		0x33, 0x00007,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700, 
		0x33, 0x00006,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700, 
		0x33, 0x00005, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x00004,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x00003,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x00002,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00001,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00000,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		//LUT_RXG_GAIN COEX  2022/08/29
		//BT connect, 
		//BTS0 TX when WLS1 RX, 
		//LNA1 change Table by POW_PA_BTS1
		0x33, 0x0000F,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700, 
		0x33, 0x0000E,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700,
		0x33, 0x0000D, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x0000C,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x0000B,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x0000A,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00009,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00008,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		//LUT_RXG_GAIN 2R  2022/08/29
		//BT connect, 
		//LNA1 change Table by POW_LNA2_BTS1
		0x33, 0x00017,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700, 
		0x33, 0x00016,//BT connect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0xA0700,
		0x33, 0x00015, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x00014,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x00013,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x00012,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00011,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00010,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		0xEF, 0x00000, 
		//LUT_RXG_BIAS  2022/08/29
		//BT connect, , 
		0xEE,  0x00010,
		0x33,  0x00000,// normal RX idle BS=2
		0x3F,  0x00002,
		0x33,  0x00001,// normal RX CCK BS=2
		0x3F,  0x00002,
		0x33,  0x00002,// normal RX OFDM BS=2
		0x3F,  0x00002,
		0x33,  0x00006,// LNA5~0 normal HG BS=3
		0x3F, 0x00003,
		0x33, 0x00007,// LNA5~0 BCN HG BS=3
		0x3F,  0x00003,
		0x33,  0x00008,// LNA on RFMODE=3.6.7.8 BS=1
		0x3F,  0x00001,
		0x33, 0x0000E,// LNA5~0 normal LG BS=3
		0x3F,  0x00003,
		0x33, 0x0000F,// LNA5~0 BCN LG,BS=3
		0x3F,  0x00003,
		0xEE,  0x00000, 
		//TIA gain modify LUT , , 
		//BT connect, 
		0xEF, 0x01000, 
		0x33, 0x00000, 
		0x3F, 0x00015, //LNA2 -8 +TIA22
		0x33, 0x00001, 
		0x3F, 0x00005, //LNA2 -8 +TIA28
		0x33, 0x00002, 
		0x3F, 0x00015, //LNA2 -8 +TIA22
		0x33, 0x00003, 
		0x3F, 0x00005, //LNA2 -8 +TIA28
		0xEF, 0x00000, 
};

const u32 array_mp_8852c_radiob_with_bt_discon[] = {

		//LUT_RXG_GAIN 2022/08/29
		//BT disconnect,
		0xEF, 0x04000, 
		0x33, 0x00007,//BT disconnect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0x00700,
		0x33, 0x00006,//BT disconnect, HG 20220829
		0x3E, 0x00000, 
		0x3F, 0x00700, 
		0x33, 0x00005, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x00004,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x00003,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x00002,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00001,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00000,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		//LUT_RXG_GAIN COEX  2022/08/29
		//BT disconnect, 
		//BTS0 TX when WLS1 RX, 
		//LNA1 change Table by POW_PA_BTS1
		0x33, 0x0000F,//BT disconnect, HG 20220829
		0x3E, 0x00000, 
		0x3F, 0x00700, 
		0x33, 0x0000E, //BT disconnect HG 20220829
		0x3E, 0x00000, 
		0x3F, 0x00700,
		0x33, 0x0000D, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x0000C,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x0000B,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x0000A,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00009,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00008,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		//LUT_RXG_GAIN 2R  2022/08/09
		//BT disconnect, 
		//LNA1 change Table by POW_LNA2_BTS1
		0x33, 0x00017, //BT disconnect HG 20220809
		0x3E, 0x00000, 
		0x3F, 0x00700, 
		0x33, 0x00016, //BT disconnect HG 20220809, 
		0x3E, 0x00000, 
		0x3F, 0x00700, 
		0x33, 0x00015, 
		0x3E, 0x00000, 
		0x3F, 0x10500, 
		0x33, 0x00014,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x00400, 
		0x33, 0x00013,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x28B00, 
		0x33, 0x00012,// modify b-cut, 
		0x3E, 0x00000, 
		0x3F, 0x9AB00, 
		0x33, 0x00011,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x01A00, 
		0x33, 0x00010,// modify 20220315, 
		0x3E, 0x00000, 
		0x3F, 0x02900, 
		0xEF, 0x00000, 
		//LUT_RXG_BIAS  2022/08/29
		//BT disconnect, , 
		0xEE, 0x00010,
		0x33, 0x00000,// normal RX idle BS=3
		0x3F,  0x00003,
		0x33,  0x00001,// normal RX CCK BS=3
		0x3F,  0x00003,
		0x33,  0x00002,// normal RX OFDM BS=3
		0x3F,  0x00003,
		0x33, 0x00006,// LNA5~0 normal HG BS=3
		0x3F,  0x00003,
		0x33, 0x00007,// LNA5~0 BCN HG BS=3
		0x3F, 0x00003,
		0x33,  0x00008,// LNA on RFMODE=3.6.7.8 BS=1
		0x3F,  0x00001,
		0x33,  0x0000E,// LNA5~0 normal LG BS=3
		0x3F,  0x00003,
		0x33,  0x0000F,// LNA5~0 BCN LG,BS=3
		0x3F,  0x00003,
		0xEE,  0x00000, 
		//TIA gain modify LUT 
		//BT disconnect, 
		0xEF, 0x01000, 
		0x33, 0x00000, 
		0x3F, 0x00015, //LNA2 -8 +TIA22
		0x33, 0x00001, 
		0x3F, 0x00017, //LNA2 0 +TIA22
		0x33, 0x00002, 
		0x3F, 0x00015, //LNA2 -8 +TIA22
		0x33, 0x00003, 
		0x3F, 0x00017, //LNA2 0 +TIA22
		0xEF, 0x00000, 
};
#endif

#endif /* _HALRF_HWIMG_RAW_DATA_W_BT_8852C_H */