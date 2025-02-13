/* This case tests the ability of the HTTP server to process HTTP request string in multiple packets.
 * Packet1. GET / HTTP/1.0
 * Packet2. CR
 * Packet3. LF
 * Packet4. CR
 * Packet5. LF
 */

#include    "tx_api.h"
#include    "nx_api.h"
#include    "fx_api.h"
extern void  test_control_return(UINT);
#ifndef NX_DISABLE_IPV4
#ifdef __PRODUCT_NETXDUO__
#include    "nxd_http_client.h"
#include    "nxd_http_server.h"
#else
#include    "nx_http_client.h"
#include    "nx_http_server.h"
#endif

#define     DEMO_STACK_SIZE         2048
#define     SERVER_PORT             80

/* Set up FileX and file memory resources. */
static CHAR             *ram_disk_memory;
static FX_MEDIA         ram_disk;
static unsigned char    media_memory[512];

/* Define device drivers.  */
extern void _fx_ram_driver(FX_MEDIA *media_ptr);
extern void _nx_ram_network_driver_1024(NX_IP_DRIVER *driver_req_ptr);

static UINT            error_counter = 0;

/* Set up the HTTP client global variables. */

#define         CLIENT_PACKET_SIZE  (800)
#define         CLIENT_PACKET_POOL_SIZE ((CLIENT_PACKET_SIZE + sizeof(NX_PACKET)) * 16)
ULONG           client_packet_pool_area[CLIENT_PACKET_POOL_SIZE/4 + 4];

static TX_THREAD       client_thread;
static NX_HTTP_CLIENT  my_client;
static NX_PACKET_POOL  client_pool;
static NX_IP           client_ip;

/* Set up the HTTP server global variables */

#define         SERVER_PACKET_SIZE  (800)
#define         SERVER_PACKET_POOL_SIZE ((SERVER_PACKET_SIZE + sizeof(NX_PACKET)) * 16)
ULONG           server_packet_pool_area[SERVER_PACKET_POOL_SIZE/4 + 4];

static TX_THREAD       server_thread;
static NX_HTTP_SERVER  my_server;
static NX_PACKET_POOL  server_pool;
static NX_IP           server_ip;
#ifdef __PRODUCT_NETXDUO__
static NXD_ADDRESS     server_ip_address;
#else
static ULONG           server_ip_address;
#endif

/* Define the IP thread stack areas.  */

ULONG                  server_ip_thread_stack[2 * 1024 / sizeof(ULONG)];
ULONG                  client_ip_thread_stack[2 * 1024 / sizeof(ULONG)];

/* Define the ARP cache areas.  */

ULONG                  server_arp_space_area[512 / sizeof(ULONG)];
ULONG                  client_arp_space_area[512 / sizeof(ULONG)];

static void thread_client_entry(ULONG thread_input);
static void thread_server_entry(ULONG thread_input);

#define HTTP_SERVER_ADDRESS  IP_ADDRESS(1,2,3,4)
#define HTTP_CLIENT_ADDRESS  IP_ADDRESS(1,2,3,5)

static CHAR    *pointer;

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_http_request_in_multiple_packets_test_application_define(void *first_unused_memory)
#endif
{

UINT    status;

    error_counter = 0;

    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    /* Create a helper thread for the server. */
    status = tx_thread_create(&server_thread, "HTTP Server thread", thread_server_entry, 5,  
                     pointer, DEMO_STACK_SIZE, 
                     4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;
    if (status)
        error_counter++;
    
    /* Create the HTTP Client thread. */
    status = tx_thread_create(&client_thread, "HTTP Client", thread_client_entry, 0,  
                     pointer, DEMO_STACK_SIZE, 
                     6, 6, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;
    if (status)
        error_counter++;

    /* Initialize the NetX system.  */
    nx_system_initialize();
    

    /* Create the server packet pool.  */
    status =  nx_packet_pool_create(&server_pool, "HTTP Server Packet Pool", SERVER_PACKET_SIZE, 
                                    pointer , SERVER_PACKET_POOL_SIZE);

    pointer = pointer + SERVER_PACKET_POOL_SIZE;
    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&server_ip, 
                          "HTTP Server IP", 
                          HTTP_SERVER_ADDRESS, 
                          0xFFFFFF00UL, 
                          &server_pool, _nx_ram_network_driver_1024,
                          pointer, DEMO_STACK_SIZE, 1);

    pointer = pointer + DEMO_STACK_SIZE;
    
    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for the server IP instance.  */
    status =  nx_arp_enable(&server_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

     /* Enable TCP traffic.  */
    status = nx_tcp_enable(&server_ip);
    if (status)
        error_counter++;

    /* Set up the server's IPv4 address here. */
#ifdef __PRODUCT_NETXDUO__ 
    server_ip_address.nxd_ip_address.v4 = HTTP_SERVER_ADDRESS;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V4;
#else
    server_ip_address = HTTP_SERVER_ADDRESS;
#endif

    /* Create the Client packet pool.  */
    status =  nx_packet_pool_create(&client_pool, "HTTP Client Packet Pool", CLIENT_PACKET_SIZE,
                                    pointer, CLIENT_PACKET_POOL_SIZE);

    pointer = pointer + CLIENT_PACKET_POOL_SIZE;

    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&client_ip, "HTTP Client IP", HTTP_CLIENT_ADDRESS, 
                          0xFFFFFF00UL, &client_pool, _nx_ram_network_driver_1024,
                          pointer, DEMO_STACK_SIZE, 1);

    pointer = pointer + DEMO_STACK_SIZE;
    if (status)
        error_counter++;

    status =  nx_arp_enable(&client_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;
    if (status)
        error_counter++;

     /* Enable TCP traffic.  */
    status = nx_tcp_enable(&client_ip);
    if (status)
        error_counter++;

    /* Create the HTTP Server.  */
    status = nx_http_server_create(&my_server, "My HTTP Server", &server_ip, &ram_disk, 
                          pointer, 2048, &server_pool, NX_NULL, NX_NULL);
    pointer =  pointer + 2048;
    if (status)
        error_counter++;

    /* Save the memory pointer for the RAM disk.  */
    ram_disk_memory =  pointer;
}


void thread_client_entry(ULONG thread_input)
{

UINT            status;
NX_PACKET       *send_packet;
NX_PACKET       *recv_packet;

    /* wait for the server to set up */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);

    /* Format the RAM disk - the memory for the RAM disk was setup in 
      tx_application_define above.  This must be set up before the client(s) start
      sending requests. */
    status = fx_media_format(&ram_disk, 
                            _fx_ram_driver,         /* Driver entry               */
                            ram_disk_memory,        /* RAM disk memory pointer    */
                            media_memory,           /* Media buffer pointer       */
                            sizeof(media_memory),   /* Media buffer size          */
                            "MY_RAM_DISK",          /* Volume Name                */
                            1,                      /* Number of FATs             */
                            32,                     /* Directory Entries          */
                            0,                      /* Hidden sectors             */
                            256,                    /* Total sectors              */
                            128,                    /* Sector size                */
                            1,                      /* Sectors per cluster        */
                            1,                      /* Heads                      */
                            1);                     /* Sectors per track          */

    /* Check the media format status.  */
    if (status != FX_SUCCESS)
        error_counter++;

    /* Open the RAM disk.  */
    status =  fx_media_open(&ram_disk, "RAM DISK", _fx_ram_driver, ram_disk_memory, media_memory, sizeof(media_memory));

    if (status != FX_SUCCESS)
        error_counter++;

    /* Create an HTTP client instance.  */
    status = nx_http_client_create(&my_client, "HTTP Client", &client_ip, &client_pool, 1460);
    if (status)
        error_counter++;

    /* Upload the 1st file to the server domain. */
#ifdef __PRODUCT_NETXDUO__

    status =  nxd_http_client_put_start(&my_client, &server_ip_address, "http://www.abc.com/client_test.htm", 
                                            "name", "password", 103, 3 * NX_IP_PERIODIC_RATE);
#else

    status =  nx_http_client_put_start(&my_client, server_ip_address, "http://www.abc.com/client_test.htm", 
                                       "name", "password", 103, 3 * NX_IP_PERIODIC_RATE);
#endif

    if (status)
        error_counter++;
    
     /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if(status)
    {
        error_counter++;
    }

    /* Build a simple 103-byte HTML page.  */
    nx_packet_data_append(send_packet, "<HTML>\r\n", 8, 
                        &client_pool, NX_WAIT_FOREVER);
    nx_packet_data_append(send_packet, 
                 "<HEAD><TITLE>NetX HTTP Test</TITLE></HEAD>\r\n", 44,
                        &client_pool, NX_WAIT_FOREVER);
    nx_packet_data_append(send_packet, "<BODY>\r\n", 8, 
                        &client_pool, NX_WAIT_FOREVER);
    nx_packet_data_append(send_packet, "<H1>Another NetX Test Page!</H1>\r\n", 25, 
                        &client_pool, NX_WAIT_FOREVER);
    nx_packet_data_append(send_packet, "</BODY>\r\n", 9,
                        &client_pool, NX_WAIT_FOREVER);
    nx_packet_data_append(send_packet, "</HTML>\r\n", 9,
                        &client_pool, NX_WAIT_FOREVER);

    /* Complete the PUT by writing the total length.  */
    status =  nx_http_client_put_packet(&my_client, send_packet, 1 * NX_IP_PERIODIC_RATE);
    if(status)
    {
        error_counter++;
    }


    /* Bind the socket. */
    if (nx_tcp_client_socket_bind(&(my_client.nx_http_client_socket), NX_ANY_PORT, NX_WAIT_FOREVER))
    {
        error_counter++;
    }

    /* Connect to server. */
    if (nx_tcp_client_socket_connect(&(my_client.nx_http_client_socket), HTTP_SERVER_ADDRESS, 
                                     my_client.nx_http_client_connect_port, NX_IP_PERIODIC_RATE))
    {
        error_counter++;
    }
    
    /* Build simple request.  */
    /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status)
    {
        error_counter++;
    }
    nx_packet_data_append(send_packet, "GET / HTTP/1.0", 14, 
                          &client_pool, NX_WAIT_FOREVER);
    if (nx_tcp_socket_send(&(my_client.nx_http_client_socket), send_packet, NX_IP_PERIODIC_RATE))
    {
        nx_packet_release(send_packet);
        error_counter++;
    }

    /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status)
    {
        error_counter++;
    }
    nx_packet_data_append(send_packet, "\r", 1, 
                          &client_pool, NX_WAIT_FOREVER);
    if (nx_tcp_socket_send(&(my_client.nx_http_client_socket), send_packet, NX_IP_PERIODIC_RATE))
    {
        nx_packet_release(send_packet);
        error_counter++;
    }

    /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status)
    {
        error_counter++;
    }
    nx_packet_data_append(send_packet, "\n", 1, 
                          &client_pool, NX_WAIT_FOREVER);
    if (nx_tcp_socket_send(&(my_client.nx_http_client_socket), send_packet, NX_IP_PERIODIC_RATE))
    {
        nx_packet_release(send_packet);
        error_counter++;
    }

    /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status)
    {
        error_counter++;
    }
    nx_packet_data_append(send_packet, "\r", 1, 
                          &client_pool, NX_WAIT_FOREVER);
    if (nx_tcp_socket_send(&(my_client.nx_http_client_socket), send_packet, NX_IP_PERIODIC_RATE))
    {
        nx_packet_release(send_packet);
        error_counter++;
    }

    /* Allocate a packet.  */
    status =  nx_packet_allocate(&client_pool, &send_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status)
    {
        error_counter++;
    }
    nx_packet_data_append(send_packet, "\n", 1, 
                          &client_pool, NX_WAIT_FOREVER);
    if (nx_tcp_socket_send(&(my_client.nx_http_client_socket), send_packet, NX_IP_PERIODIC_RATE))
    {
        nx_packet_release(send_packet);
        error_counter++;
    }

    /* Receive response from HTTP server. */
    if (nx_tcp_socket_receive(&(my_client.nx_http_client_socket), &recv_packet, NX_IP_PERIODIC_RATE))
    {
        error_counter++;
    }
    else
    {
        nx_packet_release(recv_packet);
    }

    /* wait for the server to set up */
    tx_thread_sleep(NX_IP_PERIODIC_RATE);
    
    nx_http_client_delete(&my_client);
    nx_http_server_stop(&my_server);
    nx_http_server_delete(&my_server);

    /* Verify no packet is leak. */
    if ((client_pool.nx_packet_pool_total != client_pool.nx_packet_pool_available) ||
        (server_pool.nx_packet_pool_total != server_pool.nx_packet_pool_available))
    {
        error_counter++;
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
    };
}


/* Define the helper HTTP server thread.  */
void    thread_server_entry(ULONG thread_input)
{

UINT         status;

    /* Print out test information banner.  */
    printf("NetX Test:   HTTP Request in Multiple Packets Test.....................");

    /* Check for earlier error. */
    if(error_counter)
    {
        printf("ERROR!\n");
        test_control_return(1);
    }

    /* OK to start the HTTP Server.   */
    status = nx_http_server_start(&my_server);
    if(status)
    {
        error_counter++;
    }
}
#else
#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_http_request_in_multiple_packets_test_application_define(void *first_unused_memory)
#endif
{
    printf("NetX Test:   HTTP Request in Multiple Packets Test.....................N/A\n");
    test_control_return(3);
}
#endif
