/* Test processing of solicitated RA. */

#include    "nx_api.h"   

extern void    test_control_return(UINT status);

#ifdef FEATURE_NX_IPV6
#include    "nx_nd_cache.h"
 
#define     DEMO_STACK_SIZE    2048

/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               thread_0;  
static NX_PACKET_POOL          pool_0;
static NX_IP                   ip_0;

/* Define the counters used in the demo application...  */

static ULONG                   error_counter;  

/* Define thread prototypes.  */
static VOID    thread_0_entry(ULONG thread_input);
extern VOID    test_control_return(UINT status);       
extern VOID    _nx_ram_network_driver_1500(struct NX_IP_DRIVER_STRUCT *driver_req);

/* Packet trace from 73 of section 2, TAHI test. */
/* Frame (70 bytes) */
static unsigned char pkt1[70] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0x01, 0x00, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x10, 0x3a, 0x40, 0xfe, 0x80, /* ....:@.. */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x80, 0x00, /* "..3DV.. */
0x7a, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, /* z......# */
0x45, 0x67, 0x89, 0xab, 0xcd, 0xef              /* Eg.... */
};

/* Frame (86 bytes) */
static unsigned char pkt3[86] = {
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

/* Frame (86 bytes) */
static unsigned char pkt5[86] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0xa0, 0xa0, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, /* "..3DV.. */
0xad, 0xe5, 0x20, 0x00, 0x00, 0x00, 0xfe, 0x80, /* .. ..... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0x01, 0x00, 0x02, 0x01, /* ........ */
0x00, 0x00, 0xa0, 0x00, 0xa0, 0xa0              /* ...... */
};

/* Packet trace from 125 of section 2, TAHI test. */
/* Frame (110 bytes) */
static unsigned char ra_pkt[110] = {
0x33, 0x33, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, /* 33...... */
0x00, 0x00, 0xa0, 0xa0, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x38, 0x3a, 0xff, 0xfe, 0x80, /* ...8:... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0xa0, 0xa0, 0xff, 0x02, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x86, 0x00, /* ........ */
0xa0, 0x49, 0x40, 0x00, 0x07, 0x08, 0x00, 0x00, /* .I@..... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, /* ........ */
0x00, 0x00, 0x00, 0x00, 0xa0, 0xa0, 0x03, 0x04, /* ........ */
0x40, 0xc0, 0x00, 0x27, 0x8d, 0x00, 0x00, 0x09, /* @..'.... */
0x3a, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfe, /* :.....?. */
0x05, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00              /* ...... */
};

/* Frame (86 bytes) */
static unsigned char pkt8[86] = {
0x00, 0x11, 0x22, 0x33, 0x44, 0x56, 0x00, 0x00, /* .."3DV.. */
0x00, 0x00, 0xa0, 0xa0, 0x86, 0xdd, 0x60, 0x00, /* ......`. */
0x00, 0x00, 0x00, 0x20, 0x3a, 0xff, 0xfe, 0x80, /* ... :... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0xa0, 0xa0, 0xfe, 0x80, /* ........ */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, /* ........ */
0x22, 0xff, 0xfe, 0x33, 0x44, 0x56, 0x88, 0x00, /* "..3DV.. */
0x0e, 0x05, 0x20, 0x00, 0x00, 0x00, 0xfe, 0x80, /* .. ..... */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, /* ........ */
0x00, 0xff, 0xfe, 0x00, 0xa0, 0xa0, 0x02, 0x01, /* ........ */
0x00, 0x00, 0x00, 0xa0, 0xa0, 0xa0              /* ...... */
};

/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void           netx_icmpv6_na_tlla_changed_test_application_define(void *first_unused_memory)
#endif
{
CHAR       *pointer;
UINT       status;

    /* Setup the working pointer.  */
    pointer = (CHAR *) first_unused_memory;

    /* Initialize the value.  */
    error_counter = 0;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
        pointer, DEMO_STACK_SIZE, 
        4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer = pointer + DEMO_STACK_SIZE;

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 1536, pointer, 1536*16);
    pointer = pointer + 1536*16;

    if(status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP_ADDRESS(1,2,3,4), 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_1500,
        pointer, 2048, 1);
    pointer = pointer + 2048;
  
    /* Enable IPv6 */
    status += nxd_ipv6_enable(&ip_0); 

    /* Check IPv6 enable status.  */
    if(status)
        error_counter++;        

    /* Enable IPv6 ICMP  */
    status += nxd_icmp_enable(&ip_0); 

    /* Check IPv6 ICMP enable status.  */
    if(status)
        error_counter++;
}

/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{
UINT        status;   
UINT        address_index;
NXD_ADDRESS ipv6_address;
NXD_ADDRESS router_address;
NX_PACKET  *packet_ptr;
UINT        interface_index;
ULONG       physical_msw;
ULONG       physical_lsw;
ND_CACHE_ENTRY
           *nd_entry;

    /* Print out test information banner.  */
    printf("NetX Test:   ICMPv6 NA TLLA Changed Test..............................."); 

    /* Check for earlier error.  */
    if(error_counter)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }                     

    ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;
    ipv6_address.nxd_ip_address.v6[0] = 0xFE800000;
    ipv6_address.nxd_ip_address.v6[1] = 0x00000000;
    ipv6_address.nxd_ip_address.v6[2] = 0x021122FF;
    ipv6_address.nxd_ip_address.v6[3] = 0xFE334456;
    router_address.nxd_ip_version = NX_IP_VERSION_V6;
    router_address.nxd_ip_address.v6[0] = 0xFE800000;
    router_address.nxd_ip_address.v6[1] = 0x00000000;
    router_address.nxd_ip_address.v6[2] = 0x020000FF;
    router_address.nxd_ip_address.v6[3] = 0xFE000100;

    /* Set the linklocal address.  */
    status = nxd_ipv6_address_set(&ip_0, 0, &ipv6_address, 10, &address_index); 

    /* Check the status.  */
    if(status)
        error_counter++;  

    /* Sleep 5 seconds for linklocal address DAD.  */
    tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);


    /* Inject echo request packet. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &pkt1[14], sizeof(pkt1) - 14);
    packet_ptr -> nx_packet_length = sizeof(pkt1) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the echo request packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     


    /* Inject NA packet. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &pkt3[14], sizeof(pkt3) - 14);
    packet_ptr -> nx_packet_length = sizeof(pkt3) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the NA packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     

    if (nxd_nd_cache_hardware_address_find(&ip_0, &router_address, &physical_msw, &physical_lsw, &interface_index))
        error_counter++;
    else if ((physical_msw != 0x0) || (physical_lsw != 0x100))
        error_counter++;


    /* Inject NA packet with different TLLA. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &pkt5[14], sizeof(pkt5) - 14);
    packet_ptr -> nx_packet_length = sizeof(pkt5) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the NA packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     

    if (nxd_nd_cache_hardware_address_find(&ip_0, &router_address, &physical_msw, &physical_lsw, &interface_index))
        error_counter++;
    else if ((physical_msw != 0x00) || (physical_lsw != 0xa000a0a0))
        error_counter++;


    /* Inject RA packet. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &ra_pkt[14], sizeof(ra_pkt) - 14);
    packet_ptr -> nx_packet_length = sizeof(ra_pkt) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the RA packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     


    /* Find the ND cache and set te status to created. */
    if (_nx_nd_cache_find_entry(&ip_0, router_address.nxd_ip_address.v6, &nd_entry))
    {
        printf("ERROR!\n");
        test_control_return(1);
    }  
    nd_entry -> nx_nd_cache_nd_status = ND_CACHE_STATE_CREATED;

    /* Inject NA packet with different TLLA. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &pkt5[14], sizeof(pkt5) - 14);
    packet_ptr -> nx_packet_length = sizeof(pkt5) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the NA packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     


    /* Inject NA packet with r bit changed from TRUE to FALSE. */
    status = nx_packet_allocate(&pool_0, &packet_ptr, NX_PHYSICAL_HEADER, NX_WAIT_FOREVER);

    /* Check status */
    if(status)
        error_counter++;

    /* Fill in the packet with data. Skip the MAC header.  */
    memcpy(packet_ptr -> nx_packet_prepend_ptr, &pkt8[14], sizeof(pkt8) - 14);
    packet_ptr -> nx_packet_length = sizeof(pkt8) - 14;
    packet_ptr -> nx_packet_append_ptr = packet_ptr -> nx_packet_prepend_ptr + packet_ptr -> nx_packet_length;

    /* Directly receive the NA packet.  */
    _nx_ip_packet_deferred_receive(&ip_0, packet_ptr);     


    /* Check the error.  */
    if(error_counter)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }  
    else
    {
        printf("SUCCESS!\n");      
        test_control_return(0);
    }
}                     
#else

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void           netx_icmpv6_na_tlla_changed_test_application_define(void *first_unused_memory)
#endif
{

    /* Print out test information banner.  */
    printf("NetX Test:   ICMPv6 NA TLLA Changed Test...............................N/A\n"); 
    test_control_return(3);

}
#endif /* FEATURE_NX_IPV6 */
