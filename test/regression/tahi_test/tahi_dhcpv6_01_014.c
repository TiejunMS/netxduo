#include "nx_api.h"
#if defined(NX_TAHI_ENABLE) && defined(FEATURE_NX_IPV6)  
#include "netx_tahi.h"

#if 0
/* Frame (110 bytes) */
static char pkt1[110] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa6, 0xa6, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa6, 0xa6, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
    0x42, 0x9e, 0x00, 0x80, 0x0b, 0xb8, 0x00, 0x00, /* B....... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa6, 0xa6, 0x03, 0x04, /* ........ */
    0x40, 0x80, 0x00, 0x00, 0x38, 0x40, 0x00, 0x00, /* @...8@.. */
    0x1c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* . ....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (70 bytes) */
static char pkt2[70] = {
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
#endif

/* Frame (118 bytes) */
static char pkt3[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6d, 0x29, 0x01, 0x3e, /* .#.@m).> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* ........ */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

#if 0
/* Frame (118 bytes) */
static char pkt4[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6c, 0xc5, 0x01, 0x3e, /* .#.@l..> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x00, 0x64, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* .d...... */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

/* Frame (118 bytes) */
static char pkt5[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6b, 0xfd, 0x01, 0x3e, /* .#.@k..> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x01, 0x2c, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* .,...... */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };
#endif
/* Frame (78 bytes) */
static char pkt6[78] = {
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
static char pkt7[78] = {
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
static char pkt8[78] = {
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
static char pkt9[70] = {
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

#if 0
/* Frame (70 bytes) */
static char pkt10[70] = {
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
static char pkt11[70] = {
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

/* Frame (110 bytes) */
static char pkt12[110] = {
    0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
    0x00, 0x00, 0xa6, 0xa6, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
    0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
    0x00, 0xff, 0xfe, 0x00, 0xa6, 0xa6, 0xff, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
    0x42, 0x9e, 0x00, 0x80, 0x0b, 0xb8, 0x00, 0x00, /* B....... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0xa6, 0xa6, 0x03, 0x04, /* ........ */
    0x40, 0x80, 0x00, 0x00, 0x38, 0x40, 0x00, 0x00, /* @...8@.. */
    0x1c, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* . ....?. */
    0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (70 bytes) */
static char pkt13[70] = {
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

/* Frame (118 bytes) */
static char pkt14[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6d, 0x29, 0x01, 0x3e, /* .#.@m).> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* ........ */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

#if 0
/* Frame (118 bytes) */
static char pkt15[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6c, 0xc5, 0x01, 0x3e, /* .#.@l..> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x00, 0x64, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* .d...... */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

/* Frame (118 bytes) */
static char pkt16[118] = {
    0x33, 0x33, 0x00, 0x01, 0x00, 0x02, 0x00, 0x11, /* 33...... */
    0x22, 0x33, 0x44, 0x56, 0x86, 0xdd, 0x60, 0x00, /* "3DV..`. */
    0x00, 0x00, 0x00, 0x40, 0x11, 0xff, 0xfe, 0x80, /* ...@.... */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
    0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0xff, 0x02, /* "..3DV.. */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x02, 0x22, /* ......." */
    0x02, 0x23, 0x00, 0x40, 0x6b, 0xfd, 0x01, 0x3e, /* .#.@k..> */
    0xf0, 0x12, 0x00, 0x01, 0x00, 0x0e, 0x00, 0x01, /* ........ */
    0x00, 0x01, 0xac, 0x7d, 0x87, 0x3a, 0x00, 0x11, /* ...}.:.. */
    0x22, 0x33, 0x44, 0x56, 0x00, 0x03, 0x00, 0x0c, /* "3DV.... */
    0xc0, 0xde, 0xdb, 0xad, 0x00, 0x00, 0x00, 0x00, /* ........ */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, /* ........ */
    0x01, 0x2c, 0x00, 0x06, 0x00, 0x08, 0x00, 0x17, /* .,...... */
    0x00, 0x1f, 0x00, 0x29, 0x00, 0x18              /* ...).. */
    };

/* Frame (78 bytes) */
static char pkt17[78] = {
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
static char pkt18[78] = {
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
static char pkt19[78] = {
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
static char pkt20[70] = {
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
static char pkt21[70] = {
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
static char pkt22[70] = {
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





TAHI_TEST_SEQ tahi_dhcpv6_01_014[] = {
    {TITLE, "dhcpv6 01-014", 13, 0},

    {CHECK,  &pkt3[0], sizeof(pkt3), 30},
    {DHCPV6_REBOOT, NX_NULL, 0, 0},

    {CHECK,  &pkt6[0], sizeof(pkt6), 20},
    {CHECK,  &pkt7[0], sizeof(pkt7), 5},
    {CHECK,  &pkt8[0], sizeof(pkt8), 5},

    {CHECK,  &pkt9[0], sizeof(pkt9), 10},
    {INJECT, &pkt12[0], sizeof(pkt12), 0},
    {INJECT, &pkt13[0], sizeof(pkt13), 0},

    {CHECK,  &pkt14[0], sizeof(pkt14), 30},
   
    {CLEANUP, NX_NULL, 0, 0},
    {DUMP, NX_NULL, 0, 0}
};

int  tahi_dhcpv6_01_014_size = sizeof(tahi_dhcpv6_01_014) / sizeof(TAHI_TEST_SEQ);


#endif
