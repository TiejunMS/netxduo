#include "nx_api.h"
#if defined(NX_TAHI_ENABLE) && defined(FEATURE_NX_IPV6)  

#include "netx_tahi.h"

/* Frame (70 bytes) */
static char pkt1[70] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
0x09, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* ........ */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (86 bytes) */
static char pkt2[86] = {
0x33, 0x33, 0xff, 0x00, 0x01, 0x00, 0x00, 0x11, /* 33...... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xab, 0x68, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* .h...... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt3[86] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, /* "..3DV.. */
0xad, 0x86, 0x60, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ..`..... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x02, 0x01, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x01, 0x00              /* ...... */
};

/* Frame (70 bytes) */
static char pkt4[70] = {
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x11, /* ........ */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0xfe, 0x80, /* ....:@.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x81, 0x00, /* ........ */
0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* ........ */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (110 bytes) */
static char pkt5[110] = {
0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
0xdf, 0xf3, 0x40, 0x00, 0x07, 0x0d, 0x00, 0x00, /* ..@..... */
0x75, 0x35, 0x00, 0x00, 0x03, 0xed, 0x01, 0x01, /* u5...... */
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x04, /* ........ */
0x40, 0xc0, 0x00, 0x00, 0x27, 0x10, 0x00, 0x00, /* @...'... */
0x27, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* '.....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (78 bytes) */
static char pkt6[78] = {
0x33, 0x33, 0xff, 0x33, 0x44, 0x56, 0x00, 0x11, /* 33.3DV.. */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x01, 0xff, 0x33, 0x44, 0x56, 0x87, 0x00, /* ...3DV.. */
0x88, 0x85, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56              /* "..3DV */
};

#if 0
/* Frame (78 bytes) */
static char pkt7[78] = {
0x33, 0x33, 0xff, 0x33, 0x44, 0x56, 0x00, 0x11, /* 33.3DV.. */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x01, 0xff, 0x33, 0x44, 0x56, 0x87, 0x00, /* ...3DV.. */
0x88, 0x85, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56              /* "..3DV */
};
#endif

/* Frame (70 bytes) */
static char pkt8[70] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0x3f, 0xfe, /* ....:.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
0x7a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* z....... */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (86 bytes) */
static char pkt9[86] = {
0x33, 0x33, 0xff, 0x00, 0x01, 0x00, 0x00, 0x11, /* 33...... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0x1c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .l....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt10[86] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, /* "..3DV.. */
0xd7, 0x0b, 0x60, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ..`...?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x02, 0x01, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x01, 0x00              /* ...... */
};

/* Frame (70 bytes) */
static char pkt11[70] = {
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x11, /* ........ */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0x3f, 0xfe, /* ....:@?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x81, 0x00, /* ........ */
0x79, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* y....... */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (86 bytes) */
static char pkt12[86] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x2c, 0xff, 0xfe, 0x80, /* ... ,... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3a, 0x00, /* "..3DV:. */
0x00, 0x40, 0x00, 0x00, 0x01, 0xfb, 0x02, 0x02, /* .@...... */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02              /* ...... */
};

/* Frame (94 bytes) */
static char pkt13[94] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x28, 0x2c, 0xff, 0xfe, 0x80, /* ...(,... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3a, 0x00, /* "..3DV:. */
0x00, 0x21, 0x00, 0x00, 0x01, 0xfb, 0x01, 0x01, /* .!...... */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02              /* ...... */
};

/* Frame (94 bytes) */
static char pkt14[94] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x28, 0x2c, 0xff, 0xfe, 0x80, /* ...(,... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3a, 0x00, /* "..3DV:. */
0x00, 0x01, 0x00, 0x00, 0x01, 0xfb, 0x80, 0x00, /* ........ */
0xda, 0x99, 0x01, 0xfb, 0x00, 0x00, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01              /* ...... */
};

#if 0
/* Reassembled IPv6 (88 bytes) */
static char pkt14_1[88] = {
0x80, 0x00, 0xda, 0x99, 0x01, 0xfb, 0x00, 0x00, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02  /* ........ */
};
#endif

/* Frame (142 bytes) */
static char pkt15[142] = {
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x11, /* ........ */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x58, 0x3a, 0x40, 0xfe, 0x80, /* ...X:@.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x81, 0x00, /* ........ */
0xd9, 0x99, 0x01, 0xfb, 0x00, 0x00, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, /* ........ */
0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, /* ........ */
0x02, 0x02, 0x02, 0x02, 0x02, 0x02              /* ...... */
};

/* Frame (86 bytes) */
static char pkt16[86] = {
0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, /* ........ */
0x3e, 0x08, 0x20, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* >. ...?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x02, 0x01, /* ........ */
0x00, 0x11, 0x33, 0x77, 0x55, 0x11              /* ..3wU. */
};

/* Frame (70 bytes) */
static char pkt17[70] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0x3f, 0xfe, /* ....:.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
0x7a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* z....... */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (70 bytes) */
static char pkt18[70] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0x3f, 0xfe, /* ....:@?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x81, 0x00, /* ........ */
0x79, 0x08, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* y....... */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (86 bytes) */
static char pkt19[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xd3, 0x71, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .q....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt20[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xd3, 0x71, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .q....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt21[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xd3, 0x71, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .q....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (110 bytes) */
static char pkt22[110] = {
0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
0x35, 0x21, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, /* 5!@..... */
0x75, 0x35, 0x00, 0x00, 0x03, 0xed, 0x01, 0x01, /* u5...... */
0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x04, /* ........ */
0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* @....... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (86 bytes) */
static char pkt23[86] = {
0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, /* ........ */
0xcd, 0x04, 0x20, 0x00, 0x00, 0x00, 0xfe, 0x80, /* .. ..... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x02, 0x01, /* ........ */
0x00, 0x11, 0x33, 0x77, 0x55, 0x11              /* ..3wU. */
};

/* Frame (70 bytes) */
static char pkt24[70] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
0x09, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* ........ */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (70 bytes) */
static char pkt25[70] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0xfe, 0x80, /* ....:@.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x81, 0x00, /* ........ */
0x08, 0x05, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* ........ */
0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

#if 0
/* Frame (86 bytes) */
static char pkt26[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xa9, 0xec, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt27[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xa9, 0xec, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt28[86] = {
0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x87, 0x00, /* ........ */
0xa9, 0xec, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};
#endif


TAHI_TEST_SEQ tahi_01_043[] = {
    {TITLE, "01-043", 6, 0},
    {WAIT, NX_NULL, 0, 5},

    {INJECT, &pkt1[0], sizeof(pkt1), 0},
    {CHECK, &pkt2[0], sizeof(pkt2), 5},
    {INJECT, &pkt3[0], sizeof(pkt3), 0},
    {CHECK, &pkt4[0], sizeof(pkt4), 5},

    {INJECT, &pkt5[0], sizeof(pkt5), 0},
    {CHECK, &pkt6[0], sizeof(pkt6), 5}, 

    {WAIT, NX_NULL, 0, 5},
    {INJECT, &pkt8[0], sizeof(pkt8), 0},
    {CHECK, &pkt9[0], sizeof(pkt9), 5}, 
    {INJECT, &pkt10[0], sizeof(pkt10), 0},
    {CHECK, &pkt11[0], sizeof(pkt11), 5}, 

    {INJECT, &pkt12[0], sizeof(pkt12), 0},
    {INJECT, &pkt13[0], sizeof(pkt13), 0},
    {INJECT, &pkt14[0], sizeof(pkt14), 0},
    {CHECK, &pkt15[0], sizeof(pkt15), 5}, 

    {INJECT, &pkt16[0], sizeof(pkt16), 0},
    {INJECT, &pkt17[0], sizeof(pkt17), 0},
    {CHECK, &pkt18[0], sizeof(pkt18), 10},
    {WAIT, NX_NULL, 0, 10},
    {CHECK, &pkt19[0], sizeof(pkt19), 5}, 
    {CHECK, &pkt20[0], sizeof(pkt20), 5}, 
    {CHECK, &pkt21[0], sizeof(pkt21), 5}, 

    {INJECT, &pkt22[0], sizeof(pkt22), 0},
    {INJECT, &pkt23[0], sizeof(pkt23), 0},
    {INJECT, &pkt24[0], sizeof(pkt24), 0},
    {CHECK, &pkt25[0], sizeof(pkt25), 10},
    {WAIT, NX_NULL, 0, 10},
    //{CHECK, &pkt26[0], sizeof(pkt26), 5},
    //{CHECK, &pkt27[0], sizeof(pkt27), 5},
    //{CHECK, &pkt28[0], sizeof(pkt28), 5},

    {CLEANUP, NX_NULL, 0, 0},
    {DUMP, NX_NULL, 0, 0}
};

int tahi_01_043_size = sizeof(tahi_01_043) / sizeof(TAHI_TEST_SEQ);

#endif /* NX_TAHI_ENABLE */
