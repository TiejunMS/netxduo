#include "nx_api.h"
#if defined(NX_TAHI_ENABLE) && defined(FEATURE_NX_IPV6) && defined(NX_IPSEC_ENABLE)
#include "netx_tahi.h"



static char pkt1[] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0f, 0x86, 0xdd, 0x60, 0x00, 
0x00, 0x00, 0x00, 0x34, 0x32, 0x40, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x00, 0x02, 0x11, 
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x00, 0x00, 
0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb2, 0x3b, 
0x39, 0xba, 0x4e, 0x12, 0xa2, 0xff, 0xbb, 0xa2, 
0x71, 0xa4, 0x13, 0x3e, 0x91, 0x5f, 0x2e, 0xa6, 
0x93, 0xcd, 0xba, 0x91, 0x6d, 0x38, 0xe1, 0xf0, 
0x67, 0x77, 0x03, 0xc5, 0x55, 0x38, 0xb0, 0x1f, 
0x5e, 0x4f };

static char pkt2[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x11, 
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, 
0x00, 0x00, 0x00, 0x3c, 0x32, 0x40, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x00, 0x02, 0x11, 
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
0x20, 0x00, 0x00, 0x00, 0x00, 0x05, 0xa5, 0x2c, 
0xa7, 0x6f, 0x57, 0x41, 0xa8, 0x2a, 0x48, 0xc6, 
0x65, 0xe7, 0x0b, 0x84, 0x6e, 0x78, 0x52, 0xf4, 
0x83, 0x67, 0x4a, 0x71, 0x15, 0xfe, 0x8d, 0x85, 
0x27, 0x2c, 0xe4, 0x03, 0x9a, 0x37, 0xa7, 0x9e, 
0xcf, 0x14, 0x20, 0x06, 0x86, 0x14, 0xcd, 0x24, 
0x60, 0xd4, 0xd2, 0x5d, 0x2e, 0x18, 0x7c, 0x6a, 
0xe3, 0x92 };

#if 0
static char pkt3[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x11, 
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, 
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x00, 0x02, 0x11, 
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 
0x00, 0xff, 0xfe, 0x00, 0x00, 0x0f, 0x87, 0x00, 
0x65, 0x50, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 
0x00, 0xff, 0xfe, 0x00, 0x00, 0x0f, 0x01, 0x01, 
0x00, 0x11, 0x22, 0x33, 0x44, 0x56 };
#endif

static char pkt4[] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x0f, 0x86, 0xdd, 0x60, 0x00, 
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 
0x00, 0xff, 0xfe, 0x00, 0x00, 0x0f, 0x3f, 0xfe, 
0x05, 0x01, 0xff, 0xff, 0x00, 0x00, 0x02, 0x11, 
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, 
0xe9, 0xda, 0xe0, 0x00, 0x00, 0x00, 0xfe, 0x80, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 
0x00, 0xff, 0xfe, 0x00, 0x00, 0x0f, 0x02, 0x01, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f };

TAHI_TEST_SEQ tahi_ipsec_014[] = {
    {TITLE, "IPSEC-014", 9, 0, 0, 0, 0, 0},
    {WAIT, NX_NULL, 0, 5, 0, 0, 0, 0},

    {INJECT, &pkt1[0], sizeof(pkt1), 0, 0, 0, 0, 0},
    {INJECT, &pkt4[0], sizeof(pkt4), 0, 0, 0, 0, 0},    /* Send NA packet first.  */
    {D_CHECK, &pkt2[0], sizeof(pkt2), 3, NX_NULL, 0, 0, 0},

    {CLEANUP, NX_NULL, 0, 0, 0, 0, 0, 0},
    {DUMP, NX_NULL, 0, 0, 0, 0, 0, 0}
};

int tahi_ipsec_014_size = sizeof(tahi_ipsec_014) / sizeof(TAHI_TEST_SEQ);

#endif
