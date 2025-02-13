/* This test case focus on a bug while processing NX_FTP_NLST, TCP MSS less than length of filename would cause integer underflow. */
#include    "tx_api.h"
#include    "fx_api.h" 
#include    "nx_api.h"
#ifdef __PRODUCT_NETXDUO__
#include    "nxd_ftp_server.h"
#else
#include    "nx_ftp_server.h"
#endif

extern void     test_control_return(UINT);

#if !defined(NX_DISABLE_IPV4) && !defined(NX_DISABLE_PACKET_CHAIN)

#define     DEMO_STACK_SIZE         4096

/* Define the ThreadX, NetX, and FileX object control blocks...  */
static TX_THREAD               server_thread;
static TX_THREAD               client_thread;
static TX_THREAD               data_thread;
static NX_PACKET_POOL          server_pool;
static NX_IP                   server_ip;
static NX_PACKET_POOL          client_pool;
static NX_IP                   client_ip;
static FX_MEDIA                ram_disk;


/* Define the NetX FTP object control blocks.  */
static NX_TCP_SOCKET           client_socket;
static NX_TCP_SOCKET           data_socket;
static NX_FTP_SERVER           ftp_server;

/* Define the counters used in the demo application...  */
static ULONG                   error_counter = 0;
static UINT                    test_done = NX_FALSE;

/* Define the memory area for the FileX RAM disk.  */
static UCHAR                   ram_disk_memory[32000];
static UCHAR                   ram_disk_sector_cache[512];

static CHAR                    command_buffer[1024];
static UINT                    delay_close = NX_NO_WAIT;

#define FTP_SERVER_ADDRESS  IP_ADDRESS(1,2,3,4)
#define FTP_CLIENT_ADDRESS  IP_ADDRESS(1,2,3,5)

extern UINT  _fx_media_format(FX_MEDIA *media_ptr, VOID (*driver)(FX_MEDIA *media), VOID *driver_info_ptr, UCHAR *memory_ptr, UINT memory_size,
                        CHAR *volume_name, UINT number_of_fats, UINT directory_entries, UINT hidden_sectors, 
                        ULONG total_sectors, UINT bytes_per_sector, UINT sectors_per_cluster, 
                        UINT heads, UINT sectors_per_track);

/* Define the FileX and NetX driver entry functions.  */
extern void     _fx_ram_driver(FX_MEDIA *media_ptr);
extern void     _nx_ram_network_driver_512(NX_IP_DRIVER *driver_req_ptr);

static void    client_thread_entry(ULONG thread_input);
static void    data_thread_entry(ULONG thread_input);
static void    thread_server_entry(ULONG thread_input);
static UINT    send_data(CHAR *data);
static VOID    tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port);


/* Define server login/logout functions.  These are stubs for functions that would 
   validate a client login request.   */
static UINT    server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
static UINT    server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_ftp_server_mss_too_small_test_application_define(void *first_unused_memory)
#endif
{

UINT    status;
UCHAR   *pointer;

    
    /* Setup the working pointer.  */
    pointer =  (UCHAR *) first_unused_memory;

    /* Create a helper thread for the server. */
    tx_thread_create(&server_thread, "FTP Server thread", thread_server_entry, 0,  
                     pointer, DEMO_STACK_SIZE, 
                     4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;

    /* Initialize NetX.  */
    nx_system_initialize();

    /* Create the packet pool for the FTP Server.  */
    status = nx_packet_pool_create(&server_pool, "NetX Server Packet Pool", 512, pointer, 8192);
    pointer = pointer + 8192;
    if (status)
        error_counter++;

    /* Create the IP instance for the FTP Server.  */
    status = nx_ip_create(&server_ip, "NetX Server IP Instance", FTP_SERVER_ADDRESS, 0xFFFFFF00UL, 
                                        &server_pool, _nx_ram_network_driver_512, pointer, 2048, 1);
    pointer = pointer + 2048;
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for server IP instance.  */
    status = nx_arp_enable(&server_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

    /* Enable TCP.  */
    status = nx_tcp_enable(&server_ip);
    if (status)
        error_counter++;

    /* Create the FTP server.  */
    status =  nx_ftp_server_create(&ftp_server, "FTP Server Instance", &server_ip, &ram_disk, pointer, DEMO_STACK_SIZE, &server_pool,
                                   server_login, server_logout);
    pointer =  pointer + DEMO_STACK_SIZE;
    if (status)
        error_counter++;

    /* Now set up the FTP Client. */

    /* Create the main FTP client thread.  */
    status = tx_thread_create(&client_thread, "FTP Client thread ", client_thread_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            6, 6, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE ;
    if (status)
        error_counter++;

    /* Create FTP data thread.  */
    status = tx_thread_create(&data_thread, "FTP Data thread ", data_thread_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            6, 6, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer = pointer + DEMO_STACK_SIZE ;
    if (status)
        error_counter++;

    /* Create a packet pool for the FTP client.  */
    status =  nx_packet_pool_create(&client_pool, "NetX Client Packet Pool", 256, pointer, 16384);
    pointer =  pointer + 16384;
    if (status)
        error_counter++;

    /* Create an IP instance for the FTP client.  */
    status = nx_ip_create(&client_ip, "NetX Client IP Instance", FTP_CLIENT_ADDRESS, 0xFFFFFF00UL, 
                                                &client_pool, _nx_ram_network_driver_512, pointer, 2048, 1);
    pointer = pointer + 2048;
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for the FTP Client IP.  */
    status = nx_arp_enable(&client_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

    /* Enable TCP for client IP instance.  */
    status = nx_tcp_enable(&client_ip);
    if (status)
        error_counter++;

}

/* Define the FTP client thread.  */

void    client_thread_entry(ULONG thread_input)
{

UINT status;
UINT i;

    /* Format the RAM disk - the memory for the RAM disk was defined above.  */
    status = _fx_media_format(&ram_disk, 
                            _fx_ram_driver,                  /* Driver entry                */
                            ram_disk_memory,                 /* RAM disk memory pointer     */
                            ram_disk_sector_cache,           /* Media buffer pointer        */
                            sizeof(ram_disk_sector_cache),   /* Media buffer size           */
                            "MY_RAM_DISK",                   /* Volume Name                 */
                            1,                               /* Number of FATs              */
                            320,                             /* Directory Entries           */
                            0,                               /* Hidden sectors              */
                            256,                             /* Total sectors               */
                            128,                             /* Sector size                 */
                            1,                               /* Sectors per cluster         */
                            1,                               /* Heads                       */
                            1);                              /* Sectors per track           */

    /* Check status.  */
    if (status)
        error_counter++;

    /* Open the RAM disk.  */
    status = fx_media_open(&ram_disk, "RAM DISK", _fx_ram_driver, ram_disk_memory, ram_disk_sector_cache, sizeof(ram_disk_sector_cache));
    if (status)
        error_counter++;

    /* Create an FTP client socket.  */
    status =  nx_tcp_socket_create(&client_ip, &client_socket, "FTP Client Socket", NX_IP_NORMAL,
                                   NX_DONT_FRAGMENT, NX_IP_TIME_TO_LIVE, 512, NX_NULL, NX_NULL);
    if (status) 
        error_counter++;

    /* Bind to any port.  */
    status =  nx_tcp_client_socket_bind(&client_socket, NX_ANY_PORT, NX_NO_WAIT);
    if (status) 
        error_counter++;
    
    /* Now connect with the NetX FTP (IPv4) server. */
    status =  nx_tcp_client_socket_connect(&client_socket, FTP_SERVER_ADDRESS, NX_FTP_SERVER_CONTROL_PORT,
                                        NX_IP_PERIODIC_RATE);
    if (status) 
        error_counter++;

    status =  send_data("USER aaa\n");
    if (status) 
        error_counter++;

    status =  send_data("PASS aaa\n");
    if (status) 
        error_counter++;

    status =  send_data("TYPE I\n");
    if (status) 
        error_counter++;

    status =  send_data("PORT 1,2,3,5,0,255\r");
    if (status) 
        error_counter++;

    for (i = 0; i < 10; i++)
    {

        /* Initialize command buffer.  */
        strncpy(command_buffer, "STOR 1", 6);
        memset(command_buffer + 6, '0' + i, 200);
        command_buffer[206] = '\0';
        strncat(command_buffer, ".txt\n", 6);

        status =  send_data(command_buffer);
        if (status) 
            error_counter++;

        tx_thread_sleep(NX_IP_PERIODIC_RATE);
    }

    tx_thread_sleep(NX_IP_PERIODIC_RATE);

    delay_close = NX_IP_PERIODIC_RATE;
    status =  send_data("NLST /\0");
    if (status) 
        error_counter++;

    /* Disconnect from the server.  */
    nx_tcp_socket_disconnect(&client_socket, NX_IP_PERIODIC_RATE);
    nx_tcp_client_socket_unbind(&client_socket);

    /* Cleanup the socket.  */
    nx_tcp_client_socket_unbind(&client_socket);
    nx_tcp_socket_delete(&client_socket);

    /* Set the flag.  */
    test_done = NX_TRUE;
}

/* Define the FTP data thread.  */

void    data_thread_entry(ULONG thread_input)
{

UINT status;
NX_PACKET *packet_ptr;

    /* Create an FTP data socket.  */
    status =  nx_tcp_socket_create(&client_ip, &data_socket, "FTP Data Socket", NX_IP_NORMAL,
                                   NX_DONT_FRAGMENT, NX_IP_TIME_TO_LIVE, 512, NX_NULL, NX_NULL);
    if (status) 
        error_counter++;

    /* Set MSS to 100.  */
    status =  nx_tcp_socket_mss_set(&data_socket, 100);

    /* Listen to port 255.  */
    status =  nx_tcp_server_socket_listen(&client_ip, 255, &data_socket, 5, NX_NULL);
    if (status) 
        error_counter++;

    while (test_done == NX_FALSE)
    {        

        /* Loop to accept connection.  */
        status =  nx_tcp_server_socket_accept(&data_socket, NX_WAIT_FOREVER);
        if (status) 
            error_counter++;

        status = nx_packet_allocate(&client_pool, &packet_ptr, NX_TCP_PACKET, NX_IP_PERIODIC_RATE);
        if (status)
            error_counter++;

        status = nx_packet_data_append(packet_ptr, "data", sizeof("data") - 1, &client_pool, NX_IP_PERIODIC_RATE);
        if (status)
        {
            nx_packet_release(packet_ptr);
            error_counter++;
        }

        status = nx_tcp_socket_send(&data_socket, packet_ptr, NX_IP_PERIODIC_RATE);
        if (status)
        {
            nx_packet_release(packet_ptr);
            error_counter++;
        }

        if (delay_close)
        {
            tx_thread_sleep(delay_close);
            delay_close = NX_NO_WAIT;
        }

        nx_tcp_socket_disconnect(&data_socket, NX_IP_PERIODIC_RATE);

        status = nx_tcp_server_socket_unaccept(&data_socket);
        if (status) 
            error_counter++;

        status = nx_tcp_server_socket_relisten(&client_ip, 255, &data_socket);
        if (status) 
            error_counter++;
    }
}


/* Define the helper FTP server thread.  */
void    thread_server_entry(ULONG thread_input)
{

UINT    status;
NX_PACKET *my_packet;

    /* Print out test information banner.  */
    printf("NetX Test:   FTP Server MSS Too Small Test.............................");

    /* Check for earlier error. */
    if(error_counter)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }

    /* OK to start the ftp Server.   */
    status = nx_ftp_server_start(&ftp_server);
    if (status)
        error_counter++;

    /* OK to restart the ftp Server.   */
    status = nx_ftp_server_stop(&ftp_server);
    status += nx_ftp_server_start(&ftp_server);
    if (status)
        error_counter++;

    /* Wait for test.  */
    while(test_done == NX_FALSE)
        tx_thread_sleep(NX_IP_PERIODIC_RATE);

    status = nx_ftp_server_delete(&ftp_server);
    if(status)
        error_counter++;

    /* Make sure there is no invalid release.  */
    if (server_pool.nx_packet_pool_invalid_releases)
        error_counter++;

    /* Make sure the packet pool is not corrupted.  */
    while (server_pool.nx_packet_pool_available)
    {
        if (nx_packet_allocate(&server_pool, &my_packet, 0, NX_NO_WAIT) ||
            (my_packet -> nx_packet_pool_owner != &server_pool))
        {

            error_counter++;
            break;
        }
    }

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

static UINT  send_data(CHAR *data)
{
UINT status;
NX_PACKET *packet_ptr;

    status = nx_packet_allocate(&client_pool, &packet_ptr, NX_TCP_PACKET, NX_IP_PERIODIC_RATE);
    if (status)
        return(status);

    status = nx_packet_data_append(packet_ptr, data, strlen(data), &client_pool, NX_IP_PERIODIC_RATE);
    if (status)
    {
        nx_packet_release(packet_ptr);
        return(status);
    }

    status = nx_tcp_socket_send(&client_socket, packet_ptr, NX_IP_PERIODIC_RATE);
    if (status)
    {
        nx_packet_release(packet_ptr);
        return(status);
    }

    /* Return success.  */
    return(NX_SUCCESS);
}


static UINT  server_login(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    /* Always return success.  */
    return(NX_SUCCESS);
}

static UINT  server_logout(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, ULONG client_ip_address, UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    /* Always return success.  */
    return(NX_SUCCESS);
}
#else

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_ftp_server_mss_too_small_test_application_define(void *first_unused_memory)
#endif
{

    /* Print out test information banner.  */
    printf("NetX Test:   FTP Server MSS Too Small Test.............................N/A\n");

    test_control_return(3);  
}      
#endif