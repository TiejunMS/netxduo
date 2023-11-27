#include "nx_api.h"
#if defined(NX_TAHI_ENABLE) && defined(FEATURE_NX_IPV6)  
#include "netx_tahi.h"


/* Frame (110 bytes) */
static char pkt1[110] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa6, 0xa6, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa6, 0xa6, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
    0x96, 0x52, 0x00, 0x40, 0x0b, 0xb8, 0x00, 0x00, /* .R.@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa6, 0xa6, 0x03, 0x04, /* ........ */
    0x40, 0xc0, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, /* @....d.. */
    0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .H....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

#if 0
/* Frame (78 bytes) */
static char pkt2[78] = {
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

/* Frame (78 bytes) */
static char pkt3[78] = {
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
static char pkt4[70] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0x3f, 0xfe, /* ....:@?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
    0xe9, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* .z...... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (86 bytes) */
static char pkt5[86] = {
    0x33, 0x33, 0xff, 0x00, 0xa1, 0xa1, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x00, 0xa1, 0xa1, 0x87, 0x00, /* ........ */
    0xdb, 0x28, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* .(....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x01, 0x01, /* ........ */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt6[86] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, /* "..3DV.. */
    0x75, 0x26, 0xe0, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* u&....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x02, 0x01, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1              /* ...... */
};

/* Frame (70 bytes) */
static char pkt7[70] = {
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x11, /* ........ */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0x3f, 0xfe, /* ....:.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x3f, 0xfe, /* "..3DV?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0x81, 0x00, /* ........ */
    0xe8, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* .z...... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (102 bytes) */
static char pkt8[102] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x30, 0x11, 0xff, 0xfe, 0x80, /* ...0.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x30, 0xff, 0xe4, 0x0b, 0x3e, /* .#.0...> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x08, 0x00, 0x02, /* "3DV.... */
    0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* ........ */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

/* Frame (122 bytes) */
static char pkt9[122] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
    0x00, 0x00, 0xa1, 0xa1, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x44, 0x11, 0x40, 0xfe, 0x80, /* ...D.@.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa1, 0xa1, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x02, 0x23, /* "..3DV.# */
    0x02, 0x22, 0x00, 0x44, 0x1f, 0x91, 0x07, 0x3e, /* .".D...> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x02, 0x00, 0x0e, /* "3DV.... */
    0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x17, /* ........ */
    0x00, 0x10, 0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, /* ..?..... */
    0x01, 0x00, 0x02, 0x00, 0x00, 0xff, 0xfe, 0x00, /* ........ */
    0x3f, 0x3e                                      /* ?> */
    };

/* Frame (146 bytes) */
static char pkt10[146] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa2, 0xa2, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x5c, 0x11, 0x40, 0xfe, 0x80, /* ...\.@.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa2, 0xa2, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x5c, 0xe0, 0x34, 0x08, 0x00, /* .#.\.4.. */
    0x00, 0x6b, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* .k...... */
    0x00, 0x01, 0x00, 0x04, 0x93, 0xe0, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0xa2, 0xa2, 0x00, 0x02, 0x00, 0x0e, /* ........ */
    0x00, 0x01, 0x00, 0x01, 0x00, 0x06, 0x1a, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa1, 0xa1, 0x00, 0x03, /* ........ */
    0x00, 0x28, 0x00, 0x01, 0x8a, 0x92, 0x00, 0x00, /* .(...... */
    0x00, 0x32, 0x00, 0x00, 0x00, 0x50, 0x00, 0x05, /* .2...P.. */
    0x00, 0x18, 0x3f, 0xfe, 0x05, 0x01, 0xff, 0xff, /* ..?..... */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0xab, 0xcd, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, /* .....d.. */
    0x00, 0xc8                                      /* .. */
};

/* Frame (86 bytes) */
static char pkt11[86] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0xd4, 0xbe, /* 33...... */
    0xd9, 0xec, 0x37, 0x7e, 0x86, 0xdd, 0x60, 0x00, /* ..7~..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0xff, 0x02, /* ....7~.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, /* ........ */
    0x73, 0xb4, 0x20, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* s. ...?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x02, 0x01, /* ....7~.. */
    0x00, 0x11, 0x33, 0x77, 0x55, 0x11              /* ..3wU. */
};

/* Frame (70 bytes) */
static char pkt12[70] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0xd4, 0xbe, /* .."3DV.. */
    0xd9, 0xec, 0x37, 0x7e, 0x86, 0xdd, 0x60, 0x00, /* ..7~..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0x3f, 0xfe, /* ....:.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x3f, 0xfe, /* ....7~?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
    0x94, 0xde, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* ........ */
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

#if 0
/* Frame (86 bytes) */
static char pkt13[86] = {
    0x33, 0x33, 0xff, 0xec, 0x37, 0x7e, 0x00, 0x11, /* 33..7~.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0xec, 0x37, 0x7e, 0x87, 0x00, /* ....7~.. */
    0xff, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x01, 0x01, /* ....7~.. */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt14[86] = {
    0x33, 0x33, 0xff, 0xec, 0x37, 0x7e, 0x00, 0x11, /* 33..7~.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0xec, 0x37, 0x7e, 0x87, 0x00, /* ....7~.. */
    0xff, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x01, 0x01, /* ....7~.. */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (86 bytes) */
static char pkt15[86] = {
    0x33, 0x33, 0xff, 0xec, 0x37, 0x7e, 0x00, 0x11, /* 33..7~.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0x3f, 0xfe, /* ... :.?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0xec, 0x37, 0x7e, 0x87, 0x00, /* ....7~.. */
    0xff, 0xd7, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x01, 0x01, /* ....7~.. */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};
#endif

/* Frame (110 bytes) */
static char pkt16[110] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0xd4, 0xbe, /* 33...... */
    0xd9, 0xec, 0x37, 0x7e, 0x86, 0xdd, 0x60, 0x00, /* ..7~..`. */
    0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0xff, 0x02, /* ....7~.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
    0xe3, 0xef, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, /* ..@..... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0xd4, 0xbe, 0xd9, 0xec, 0x37, 0x7e, 0x03, 0x04, /* ....7~.. */
    0x40, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* @....... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* ......?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (86 bytes) */
static char pkt17[86] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0xd4, 0xbe, /* 33...... */
    0xd9, 0xec, 0x37, 0x7e, 0x86, 0xdd, 0x60, 0x00, /* ..7~..`. */
    0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0xff, 0x02, /* ....7~.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, /* ........ */
    0x02, 0xb1, 0x20, 0x00, 0x00, 0x00, 0xfe, 0x80, /* .. ..... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x02, 0x01, /* ....7~.. */
    0x00, 0x11, 0x33, 0x77, 0x55, 0x11              /* ..3wU. */
};

/* Frame (70 bytes) */
static char pkt18[70] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0xd4, 0xbe, /* .."3DV.. */
    0xd9, 0xec, 0x37, 0x7e, 0x86, 0xdd, 0x60, 0x00, /* ..7~..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0xfe, 0x80, /* ....7~.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
    0x23, 0xdb, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* #....... */
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

/* Frame (70 bytes) */
static char pkt19[70] = {
    0x00, 0x11, 0x33, 0x77, 0x55, 0x11, 0x00, 0x11, /* ..3wU... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xfe, 0x80, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xd6, 0xbe, /* ........ */
    0xd9, 0xff, 0xfe, 0xec, 0x37, 0x7e, 0x81, 0x00, /* ....7~.. */
    0x22, 0xdb, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, /* "....... */
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08              /* ...... */
};

#if 0
/* Frame (78 bytes) */
static char pkt20[78] = {
    0x33, 0x33, 0xff, 0x33, 0x44, 0x56, 0x00, 0x11, /* 33.3DV.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x33, 0x44, 0x56, 0x87, 0x00, /* ...3DV.. */
    0xd0, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56              /* "..3DV */
};

/* Frame (78 bytes) */
static char pkt21[78] = {
    0x33, 0x33, 0xff, 0x33, 0x44, 0x56, 0x00, 0x11, /* 33.3DV.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x33, 0x44, 0x56, 0x87, 0x00, /* ...3DV.. */
    0xd0, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56              /* "..3DV */
};

/* Frame (78 bytes) */
static char pkt22[78] = {
    0x33, 0x33, 0xff, 0x33, 0x44, 0x56, 0x00, 0x11, /* 33.3DV.. */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x18, 0x3a, 0xff, 0x00, 0x00, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x01, 0xff, 0x33, 0x44, 0x56, 0x87, 0x00, /* ...3DV.. */
    0xd0, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x80, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56              /* "..3DV */
};

/* Frame (70 bytes) */
static char pkt23[70] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x85, 0x00, /* ........ */
    0xad, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (70 bytes) */
static char pkt24[70] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x85, 0x00, /* ........ */
    0xad, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};

/* Frame (70 bytes) */
static char pkt25[70] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x10, 0x3a, 0xff, 0xfe, 0x80, /* ....:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x85, 0x00, /* ........ */
    0xad, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x11, 0x22, 0x33, 0x44, 0x56              /* .."3DV */
};
#endif




TAHI_TEST_SEQ tahi_dhcpv6_07_022[] = {
    {TITLE, "dhcpv6 07-021", 13, 0},

    {WAIT, NX_NULL, 0, 5},

    {INJECT, &pkt1[0],  sizeof(pkt1),  0},      /* RA */

    {WAIT, NX_NULL, 0, 5},

    {INJECT, &pkt4[0], sizeof(pkt4), 0},    /* Echo Request */
    {CHECK,  &pkt5[0], sizeof(pkt5), 5},    /* NS */
    {INJECT, &pkt6[0], sizeof(pkt6), 0},    /* NA */
    {CHECK,  &pkt7[0], sizeof(pkt7), 5},    /* Echo Reply */

    {DHCPV6_INFO_REQUEST, NX_NULL, 0, 0},
    {CHECK,  &pkt8[0],  sizeof(pkt8),  5},
    {INJECT, &pkt9[0],  sizeof(pkt9),  0},

    {INJECT,  &pkt10[0], sizeof(pkt10), 0},

    {N_CHECK, (CHAR*)ANY, 0, 1},

    {INJECT, &pkt11[0], sizeof(pkt11), 0},    /* NA */
    {INJECT, &pkt12[0], sizeof(pkt12), 0},    /* Echo Request */
    
    {WAIT, NX_NULL, 0, 10}, 

    {INJECT, &pkt16[0], sizeof(pkt16), 0},    /* RA */
    {INJECT, &pkt17[0], sizeof(pkt17), 0},    /* NA */

    {INJECT, &pkt18[0], sizeof(pkt18), 0},    /* Echo Request */
    {CHECK,  &pkt19[0], sizeof(pkt19), 5},    /* Echo Reply */

    {CLEANUP, NX_NULL, 0, 0},
    {DUMP, NX_NULL, 0, 0}
};

int  tahi_dhcpv6_07_022_size = sizeof(tahi_dhcpv6_07_022) / sizeof(TAHI_TEST_SEQ);
#endif
