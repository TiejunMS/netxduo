/* This NetX test concentrates on the basic BSD RAW non-blocking bind to a specific address and connect + send operation.  */


#include   "tx_api.h"
#include   "nx_api.h"
#if defined(__PRODUCT_NETXDUO__) && !defined(NX_DISABLE_IPV4)
#ifdef NX_BSD_ENABLE
#ifdef __PRODUCT_NETX__
#include   "nx_bsd.h"
#else
#include   "nxd_bsd.h"
#endif

#ifdef FEATURE_NX_IPV6
#include "nx_ipv6.h"
#endif
#include "nx_ipv4.h"
#ifdef __PRODUCT_NETXDUO__
#include   "nx_icmpv6.h"
#endif
#define     DEMO_STACK_SIZE         4096


/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               ntest_0;
static TX_THREAD               ntest_1;

static NX_PACKET_POOL          pool_0;
static NX_IP                   ip_0;
static NX_IP                   ip_1;
static ULONG                   bsd_thread_area[DEMO_STACK_SIZE / sizeof(ULONG)];
#define BSD_THREAD_PRIORITY    2
#define NUM_CLIENTS            20
/* Define the counters used in the test application...  */

static ULONG                   error_counter;


/* Define thread prototypes.  */

static void    ntest_0_entry(ULONG thread_input);
static void    ntest_1_entry(ULONG thread_input);
extern void    test_control_return(UINT status);
extern void    _nx_ram_network_driver_256(struct NX_IP_DRIVER_STRUCT *driver_req);
static void    validate_bsd_structure(void);
static TX_SEMAPHORE sema;
extern NX_BSD_SOCKET  nx_bsd_socket_array[NX_BSD_MAX_SOCKETS];
extern TX_BLOCK_POOL nx_bsd_socket_block_pool;

#ifdef FEATURE_NX_IPV6
static NXD_ADDRESS ipv6_address_ip0[3][3];
static NXD_ADDRESS ipv6_address_ip1[3][3];
static char *bsd6_msg[3][3] = {{"BSD6_MSG IF0 LLA", "BSD6_MSG  IF0  GA0", "MSD6_MSG IF0   GA1"},
                               {"BSD6_MSG IF1 LLA", "BSD6_MSG  IF1  GA0", "MSD6_MSG IF1   GA1"},
                               {"BSD6_MSG IF2 LLA", "BSD6_MSG  IF2  GA0", "MSD6_MSG IF2   GA1"}};
#endif /* FEATURE_NX_IPV6 */
static char *bsd4_msg[3] = {"BSD4_MSG 0", "BSD4_MSG  1", "MSD4_MSG   2"};

static void validate_bsd_structure(void);

#define IP0_IF0_V4_ADDR   IP_ADDRESS(1,2,3,4)  
#define IP0_IF1_V4_ADDR   IP_ADDRESS(2,2,3,4)  
#define IP0_IF2_V4_ADDR   IP_ADDRESS(3,2,3,4)  

#define IP1_IF0_V4_ADDR   IP_ADDRESS(1,2,3,5)  
#define IP1_IF1_V4_ADDR   IP_ADDRESS(2,2,3,5)  
#define IP1_IF2_V4_ADDR   IP_ADDRESS(3,2,3,5)  

#define ITERATIONS  100
static ULONG ip0_address[3] = {IP0_IF0_V4_ADDR, IP0_IF1_V4_ADDR, IP0_IF2_V4_ADDR};
static ULONG ip1_address[3] = {IP1_IF0_V4_ADDR, IP1_IF1_V4_ADDR, IP1_IF2_V4_ADDR};
/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_bsd_raw_bind_connect_test_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;
UINT    status;

    
    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    error_counter =  0;

    /* Create the main thread.  */
    tx_thread_create(&ntest_0, "thread 0", ntest_0_entry, 0,  
                     pointer, DEMO_STACK_SIZE, 
                     3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;

    /* Create the main thread.  */
    tx_thread_create(&ntest_1, "thread 1", ntest_1_entry, 0,  
                     pointer, DEMO_STACK_SIZE, 
                     4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);
    
    pointer =  pointer + DEMO_STACK_SIZE;


    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 256, pointer, (256 + sizeof(NX_PACKET)) * (NUM_CLIENTS + 4) * 2);
    pointer = pointer + (256 + sizeof(NX_PACKET)) * (NUM_CLIENTS + 4) * 2;

    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP0_IF0_V4_ADDR, 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_256,
                    pointer, 2048, 1);
    pointer =  pointer + 2048;

    /* Attach a 2nd interface */
    status += nx_ip_interface_attach(&ip_0, "ip_0_second", IP0_IF1_V4_ADDR, 0xFFFFFF00UL,  _nx_ram_network_driver_256);
    status += nx_ip_interface_attach(&ip_0, "ip_0_third", IP0_IF2_V4_ADDR, 0xFFFFFF00UL,  _nx_ram_network_driver_256);

    /* Create another IP instance.  */
    status += nx_ip_create(&ip_1, "NetX IP Instance 1", IP1_IF0_V4_ADDR, 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_256,
                    pointer, 2048, 2);
    pointer =  pointer + 2048;

    status += nx_ip_interface_attach(&ip_1, "ip_1_second", IP1_IF1_V4_ADDR, 0xFFFFFF00UL,  _nx_ram_network_driver_256);
    status += nx_ip_interface_attach(&ip_1, "ip_1_third", IP1_IF2_V4_ADDR, 0xFFFFFF00UL,  _nx_ram_network_driver_256);
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 1.  */
    status  =  nx_arp_enable(&ip_1, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

    /* Enable RAW processing for both IP instances.  */
    status =  nx_ip_raw_packet_enable(&ip_0);
    status += nx_ip_raw_packet_enable(&ip_1);

    /* Enable BSD */
    status += bsd_initialize(&ip_0, &pool_0, (CHAR*)&bsd_thread_area[0], sizeof(bsd_thread_area), BSD_THREAD_PRIORITY);

    /* Check RAW enable and BSD init status.  */
    if (status)
        error_counter++;

    status = tx_semaphore_create(&sema, "test done", 0);
    if (status)
        error_counter++;
}




#ifdef FEATURE_NX_IPV6
static void test_raw6_on_interface_address(int iface, int address)
{
int                 sockfd;
struct sockaddr_in6 remote_addr, local_addr;
int                 ret;


    sockfd = socket(AF_INET6, SOCK_RAW, 100);
    if(sockfd < 0)
        error_counter++;
    
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin6_port = htons(100);
    local_addr.sin6_family = AF_INET6;
    if(iface != 3)
    {
        local_addr.sin6_addr._S6_un._S6_u32[0] = htonl(ipv6_address_ip0[iface][address].nxd_ip_address.v6[0]);
        local_addr.sin6_addr._S6_un._S6_u32[1] = htonl(ipv6_address_ip0[iface][address].nxd_ip_address.v6[1]);
        local_addr.sin6_addr._S6_un._S6_u32[2] = htonl(ipv6_address_ip0[iface][address].nxd_ip_address.v6[2]);
        local_addr.sin6_addr._S6_un._S6_u32[3] = htonl(ipv6_address_ip0[iface][address].nxd_ip_address.v6[3]);
    }


    ret = bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if(ret < 0)
        error_counter++;

    remote_addr.sin6_family = AF_INET6;
    remote_addr.sin6_addr._S6_un._S6_u32[0] = htonl(ipv6_address_ip1[iface][address].nxd_ip_address.v6[0]);
    remote_addr.sin6_addr._S6_un._S6_u32[1] = htonl(ipv6_address_ip1[iface][address].nxd_ip_address.v6[1]);
    remote_addr.sin6_addr._S6_un._S6_u32[2] = htonl(ipv6_address_ip1[iface][address].nxd_ip_address.v6[2]);
    remote_addr.sin6_addr._S6_un._S6_u32[3] = htonl(ipv6_address_ip1[iface][address].nxd_ip_address.v6[3]);

    remote_addr.sin6_port = htons(100);

    ret = connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if(ret < 0)
        error_counter++;


    ret = send(sockfd, bsd6_msg[iface][address], strlen(bsd6_msg[iface][address]), 0);
    if(ret != (INT)strlen(bsd6_msg[iface][address]))
        error_counter++;

    /* Close downt he socket. */
    ret = soc_close(sockfd);
    if(ret < 0)
        error_counter++;

}


#endif /* FEATURE_NX_IPV6 */

static void    test_raw4_on_interface(int i)
{

int                 sockfd;
struct sockaddr_in  remote_addr, local_addr;
int                 ret;


    sockfd = socket(AF_INET, SOCK_RAW, 100);
    if(sockfd < 0)
        error_counter++;

    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(100);
    local_addr.sin_addr.s_addr = htonl(ip0_address[i]);

    ret = bind(sockfd, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if(ret < 0)
        error_counter++;

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(100);
    remote_addr.sin_addr.s_addr = htonl(ip1_address[i]);

    ret = connect(sockfd, (struct sockaddr*)&remote_addr, sizeof(remote_addr));
    if(ret < 0)
        error_counter++;

    ret = send(sockfd, bsd4_msg[i], strlen(bsd4_msg[i]), 0);
    if(ret != (int)strlen(bsd4_msg[i]))
        error_counter++;


    /* Close downt he socket. */
    ret = soc_close(sockfd);
    if(ret < 0)
        error_counter++;

}



/* Define the test threads.  */
static void    ntest_0_entry(ULONG thread_input)
{
#ifdef FEATURE_NX_IPV6    
static char mac_ip0[6];
static char mac_ip1[6];
int j;
int addr_index;
UINT status;
int i;
#endif
int iface;


    printf("NetX Test:   Basic BSD RAW Bind Connect Send Test..........");

    /* Check for earlier error.  */
    if (error_counter)
    {

        printf("ERROR!\n");
        test_control_return(1);
    }

#ifdef FEATURE_NX_IPV6    

    for(i = 0; i < 3; i++)
    {
        mac_ip0[0] = (char)(ip_0.nx_ip_interface[i].nx_interface_physical_address_msw >> 8);
        mac_ip0[1] = ip_0.nx_ip_interface[i].nx_interface_physical_address_msw & 0xFF;
        mac_ip0[2] = (ip_0.nx_ip_interface[i].nx_interface_physical_address_lsw >> 24) & 0xff;
        mac_ip0[3] = (ip_0.nx_ip_interface[i].nx_interface_physical_address_lsw >> 16) & 0xff;
        mac_ip0[4] = (ip_0.nx_ip_interface[i].nx_interface_physical_address_lsw >> 8) & 0xff;
        mac_ip0[5] = ip_0.nx_ip_interface[i].nx_interface_physical_address_lsw  & 0xff;
        
        mac_ip1[0] = (char)(ip_1.nx_ip_interface[i].nx_interface_physical_address_msw >> 8);
        mac_ip1[1] = ip_1.nx_ip_interface[i].nx_interface_physical_address_msw & 0xFF;
        mac_ip1[2] = (ip_1.nx_ip_interface[i].nx_interface_physical_address_lsw >> 24) & 0xff;
        mac_ip1[3] = (ip_1.nx_ip_interface[i].nx_interface_physical_address_lsw >> 16) & 0xff;
        mac_ip1[4] = (ip_1.nx_ip_interface[i].nx_interface_physical_address_lsw >> 8) & 0xff;
        mac_ip1[5] = ip_1.nx_ip_interface[i].nx_interface_physical_address_lsw  & 0xff;

        for(j = 0; j < 3; j ++)
        {
            if(j == 0)
            {
                /* First set up IPv6 linklocal addresses. */
                ipv6_address_ip0[i][j].nxd_ip_version = NX_IP_VERSION_V6;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[0] = 0xfe800000;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[1] = 0x00000000;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[2] = ((mac_ip0[0] | 0x2) << 24) | (mac_ip0[1] << 16) | (mac_ip0[2] << 8) | 0xFF;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[3] = (0xFE << 24) | ((mac_ip0[3] | 0x2) << 16) | (mac_ip0[4] << 8) | mac_ip0[5];
        
                ipv6_address_ip1[i][j].nxd_ip_version = NX_IP_VERSION_V6;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[0] = 0xfe800000;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[1] = 0x00000000;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[2] = 
                    ((mac_ip1[0] | 0x2) << 24) | (mac_ip1[1] << 16) | (mac_ip1[2] << 8) | 0xFF;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[3] = 
                    (0xFE << 24) | ((mac_ip1[3] | 0x2) << 16) | (mac_ip1[4] << 8) | mac_ip1[5];
        
                status = nxd_ipv6_address_set(&ip_0, i, &ipv6_address_ip0[i][j], 10, NX_NULL);
                status += nxd_ipv6_address_set(&ip_1, i, &ipv6_address_ip1[i][j], 10, NX_NULL);
            }
            else
            {
                /* Global Adddress */
                ipv6_address_ip0[i][j].nxd_ip_version = NX_IP_VERSION_V6;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[0] = 0x20000000 + i;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[1] = j;
                ipv6_address_ip0[i][j].nxd_ip_address.v6[2] = ipv6_address_ip0[i][0].nxd_ip_address.v6[2];
                ipv6_address_ip0[i][j].nxd_ip_address.v6[3] = ipv6_address_ip0[i][0].nxd_ip_address.v6[3];
        
                ipv6_address_ip1[i][j].nxd_ip_version = NX_IP_VERSION_V6;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[0] = 0x20000000 + i;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[1] = j;
                ipv6_address_ip1[i][j].nxd_ip_address.v6[2] = ipv6_address_ip1[i][0].nxd_ip_address.v6[2];
                ipv6_address_ip1[i][j].nxd_ip_address.v6[3] = ipv6_address_ip1[i][0].nxd_ip_address.v6[3];

        
                status = nxd_ipv6_address_set(&ip_0, i, &ipv6_address_ip0[i][j], 64, NX_NULL);
                status += nxd_ipv6_address_set(&ip_1, i, &ipv6_address_ip1[i][j], 64, NX_NULL);
            }
            status += nxd_nd_cache_entry_set(&ip_0, ipv6_address_ip1[i][j].nxd_ip_address.v6, 0,  mac_ip1);
            status += nxd_nd_cache_entry_set(&ip_1, ipv6_address_ip0[i][j].nxd_ip_address.v6, 0,  mac_ip0);        
        }

    }



    status += nxd_ipv6_enable(&ip_0);
    status += nxd_ipv6_enable(&ip_1);
    


    if(status)
        error_counter++;
#endif

    /* Wait for the semaphore to signal the other party is ready. */
    tx_semaphore_get(&sema, 2 * NX_IP_PERIODIC_RATE);

    for(iface = 0; iface < 3; iface++)
    {
        test_raw4_on_interface(iface);
        
        tx_semaphore_get(&sema, 2 * NX_IP_PERIODIC_RATE);

#ifdef FEATURE_NX_IPV6
        for(addr_index = 1; addr_index < 3; addr_index++)
        {

            test_raw6_on_interface_address(iface, addr_index);

            tx_semaphore_get(&sema, 2 * NX_IP_PERIODIC_RATE);
        }

#endif
    }


    tx_semaphore_delete(&sema);
    
    validate_bsd_structure();

    if(error_counter)
        printf("ERROR!\n");
    else
        printf("SUCCESS!\n");

    if(error_counter)
        test_control_return(1);    

    test_control_return(0);    
}

static void    ntest_1_entry(ULONG thread_input)
{
ULONG           actual_status;
UINT            status;
UINT            iface;
NX_PACKET       *packet_ptr;
NX_IPV4_HEADER *ipv4_header;
#ifdef FEATURE_NX_IPV6
NX_IPV6_HEADER  *ipv6_header;
UINT            addr_index;
#endif
    /* Ensure the IP instance has been initialized.  */
    status =  nx_ip_status_check(&ip_1, NX_IP_INITIALIZE_DONE, &actual_status, 1 * NX_IP_PERIODIC_RATE);

    tx_semaphore_put(&sema);


    for(iface = 0; iface < 3; iface++)
    {

        status = nx_ip_raw_packet_receive(&ip_1, &packet_ptr, 1 * NX_IP_PERIODIC_RATE);

        /* Check status...  */
        if (status != NX_SUCCESS)
        {
            error_counter++;
            continue;
        }
        
        ipv4_header = (NX_IPV4_HEADER*)packet_ptr -> nx_packet_ip_header;
        if(ipv4_header -> nx_ip_header_source_ip != ip0_address[iface])
            error_counter++;
        if((ipv4_header -> nx_ip_header_word_2 & 0x00FF0000) != (100 << 16))
            error_counter++;
        if(ipv4_header -> nx_ip_header_destination_ip != ip1_address[iface])
            error_counter++;
        if((packet_ptr -> nx_packet_length != strlen(bsd4_msg[iface])) ||
           (memcmp(packet_ptr -> nx_packet_prepend_ptr, bsd4_msg[iface], packet_ptr -> nx_packet_length)))
            error_counter++;
        
        nx_packet_release(packet_ptr);

        tx_semaphore_put(&sema);

#ifdef FEATURE_NX_IPV6
        for(addr_index = 1; addr_index < 3; addr_index++)
        {
            status = nx_ip_raw_packet_receive(&ip_1, &packet_ptr, 1 * NX_IP_PERIODIC_RATE);
            
            if(status != NX_SUCCESS)
            {
                error_counter++;
                continue;
            }
            
            ipv6_header = (NX_IPV6_HEADER*)packet_ptr -> nx_packet_ip_header;
            
            if((ipv6_header -> nx_ip_header_source_ip[0] != ipv6_address_ip0[iface][addr_index].nxd_ip_address.v6[0]) ||
               (ipv6_header -> nx_ip_header_source_ip[1] != ipv6_address_ip0[iface][addr_index].nxd_ip_address.v6[1]) ||
               (ipv6_header -> nx_ip_header_source_ip[2] != ipv6_address_ip0[iface][addr_index].nxd_ip_address.v6[2]) ||
               (ipv6_header -> nx_ip_header_source_ip[3] != ipv6_address_ip0[iface][addr_index].nxd_ip_address.v6[3]))
                error_counter++;
            if((ipv6_header -> nx_ip_header_destination_ip[0] != ipv6_address_ip1[iface][addr_index].nxd_ip_address.v6[0]) ||
               (ipv6_header -> nx_ip_header_destination_ip[1] != ipv6_address_ip1[iface][addr_index].nxd_ip_address.v6[1]) ||
               (ipv6_header -> nx_ip_header_destination_ip[2] != ipv6_address_ip1[iface][addr_index].nxd_ip_address.v6[2]) ||
               (ipv6_header -> nx_ip_header_destination_ip[3] != ipv6_address_ip1[iface][addr_index].nxd_ip_address.v6[3]))
                error_counter++;
            if((ipv6_header -> nx_ip_header_word_1 & 0x0000FF00) != (100 << 8))
                error_counter++;
            
            if((packet_ptr -> nx_packet_length != strlen(bsd6_msg[iface][addr_index])) ||
               (memcmp(packet_ptr -> nx_packet_prepend_ptr, bsd6_msg[iface][addr_index], packet_ptr -> nx_packet_length)))
                error_counter++;
            
            nx_packet_release(packet_ptr);                
            
            tx_semaphore_put(&sema);
        }    
#endif               
    }
}

static void validate_bsd_structure(void)
{
int i;
    /* Make sure every BSD socket should be free by now. */
    
    for(i = 0; i < NX_BSD_MAX_SOCKETS; i++)
    {
        if(nx_bsd_socket_array[i].nx_bsd_socket_status_flags & NX_BSD_SOCKET_IN_USE)
        {
            error_counter++;
        }

        if(nx_bsd_socket_array[i].nx_bsd_socket_tcp_socket ||
           nx_bsd_socket_array[i].nx_bsd_socket_udp_socket)
        {
            error_counter++;
        }
    }
    
    /* Make sure all the NX SOCKET control blocks are released. */
    if(nx_bsd_socket_block_pool.tx_block_pool_available != 
       nx_bsd_socket_block_pool.tx_block_pool_total)
    {
        error_counter++;

    }

    /* Make sure all the sockets are released */
    if(ip_0.nx_ip_tcp_created_sockets_ptr ||
       ip_0.nx_ip_udp_created_sockets_ptr)
    {
        error_counter++;
        return;
    }
}

#endif /* NX_BSD_ENABLE */

#else /* __PRODUCT_NETXDUO__ */

extern void    test_control_return(UINT status);
#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_bsd_raw_bind_connect_test_application_define(void *first_unused_memory)
#endif
{
    printf("NetX Test:   Basic BSD RAW Bind Connect Send Test..........N/A\n");
    test_control_return(3);
}
#endif /* __PRODUCT_NETXDUO__ */

