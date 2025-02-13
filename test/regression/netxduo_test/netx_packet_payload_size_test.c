/* This case tests payload that is only two bytes aligned.  */

#include   "tx_api.h"
#include   "nx_api.h"

extern void    test_control_return(UINT status);

#if !defined(NX_DISABLE_PACKET_CHAIN) && !defined(NX_DISABLE_IPV4)
#define     DEMO_STACK_SIZE         2048


/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               thread_0;
static TX_THREAD               thread_1;

static NX_PACKET_POOL          pool_0;
static NX_PACKET_POOL          pool_1;
#ifdef __PRODUCT_NETXDUO__
static NX_PACKET_POOL          pool_2;
#endif /* __PRODUCT_NETXDUO__ */
static NX_IP                   ip_0;
static NX_IP                   ip_1;
static NX_TCP_SOCKET           client_socket;
static NX_TCP_SOCKET           server_socket;
static CHAR                    send_buff[250];
static CHAR                    recv_buff[250];



/* Define the counters used in the demo application...  */

static ULONG                   thread_0_counter =  0;
static ULONG                   thread_1_counter =  0;
static ULONG                   error_counter =     0;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
extern void    _nx_ram_network_driver_1500(struct NX_IP_DRIVER_STRUCT *driver_req);


/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_packet_payload_size_test_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;
UINT    status;
#ifdef __PRODUCT_NETXDUO__
ULONG   header_size;
#endif /* __PRODUCT_NETXDUO__ */

    
    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    thread_0_counter =  0;
    thread_1_counter =  0;
    error_counter =     0;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;

    /* Create the main thread.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;


    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool with payload not four bytes aligned.  */
    memset(pointer, 0, 8192);
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 202, pointer, 8192);
    pointer = pointer + 8192;

    if (status)
        error_counter++;

    /* Create a packet pool.  */
    memset(pointer, 0, 8192);
    status =  nx_packet_pool_create(&pool_1, "NetX Main Packet Pool", 512, pointer, 8192);
    pointer = pointer + 8192;

    if (status)
        error_counter++;

#ifdef __PRODUCT_NETXDUO__
    pointer += 1;
    pointer = (CHAR *)((((ALIGN_TYPE)pointer + NX_PACKET_ALIGNMENT  - 1) / NX_PACKET_ALIGNMENT) * NX_PACKET_ALIGNMENT);

    /* Calculate the rounded payload size. */
    header_size = ((sizeof(NX_PACKET) + NX_PACKET_ALIGNMENT - 1)/NX_PACKET_ALIGNMENT) * NX_PACKET_ALIGNMENT;
#ifndef NX_DISABLE_ERROR_CHECKING
    /* Create a packet pool with starting address not four byte aligned. The pool size is not enough. */
    status =  nx_packet_pool_create(&pool_2, "NetX Main Packet Pool", 512, (pointer - 1), 512 + header_size);

    if (status == NX_SUCCESS)
        error_counter++;
#endif /* NX_DISABLE_ERROR_CHECKING */

    /* Create a packet pool with starting address not four byte aligned. The pool size is enough. */
    status =  nx_packet_pool_create(&pool_2, "NetX Main Packet Pool", 512, (pointer - 1), 516 + header_size);
    pointer = pointer + 516 + header_size;

    if (status != NX_SUCCESS)
        error_counter++;
#endif /* __PRODUCT_NETXDUO__ */
                                     
    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP_ADDRESS(1, 2, 3, 4), 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_1500,
                    pointer, 2048, 1);
    pointer =  pointer + 2048;

    /* Create another IP instance.  */
    status += nx_ip_create(&ip_1, "NetX IP Instance 1", IP_ADDRESS(1, 2, 3, 5), 0xFFFFFF00UL, &pool_1, _nx_ram_network_driver_1500,
                    pointer, 2048, 1);
    pointer =  pointer + 2048;

    if (status)
        error_counter++;

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *) pointer, 1024);
    pointer = pointer + 1024;

    /* Enable ARP and supply ARP cache memory for IP Instance 1.  */
    status +=  nx_arp_enable(&ip_1, (void *) pointer, 1024);
    pointer = pointer + 1024;

    /* Check ARP enable status.  */
    if (status)
        error_counter++;

    /* Enable TCP processing for both IP instances.  */
    status =  nx_tcp_enable(&ip_0);
    status += nx_tcp_enable(&ip_1);

    /* Check TCP enable status.  */
    if (status)
        error_counter++;
                                     
}



/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT        status;
NX_PACKET   *my_packet;
UINT        i;

    /* Print out some test information banners.  */
    printf("NetX Test:   Packet Payload Size Test..................................");

    /* Check for earlier error.  */
    if (error_counter)
    {

        printf("ERROR!\n");
        test_control_return(1);
    }

    /* Reset send buffer and recv buffer. */
    for (i = 0; i < sizeof(send_buff); i++)
    {
        send_buff[i] = 0x01;
        recv_buff[i] = 0x00;
    }

    /* Create a socket.  */
    status =  nx_tcp_socket_create(&ip_0, &client_socket, "Client Socket", 
                            NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 300,
                            NX_NULL, NX_NULL);
                            
    /* Check for error.  */
    if (status)
        error_counter++;

    /* Bind the socket.  */
    status =  nx_tcp_client_socket_bind(&client_socket, 12, NX_WAIT_FOREVER);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Attempt to connect the socket.  */
    status =  nx_tcp_client_socket_connect(&client_socket, IP_ADDRESS(1, 2, 3, 5), 12, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Allocate a packet.  */
    status =  nx_packet_allocate(&pool_0, &my_packet, NX_TCP_PACKET, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status != NX_SUCCESS)
        error_counter++;

    /* Write send_buff into the packet payload!  */
    status = nx_packet_data_append(my_packet, send_buff, sizeof(send_buff), &pool_0, NX_WAIT_FOREVER);

    /* Check status.  */
    if (status != NX_SUCCESS)
        error_counter++;

    /* Send the packet out!  */
    status =  nx_tcp_socket_send(&client_socket, my_packet, 5 * NX_IP_PERIODIC_RATE);

    /* Determine if the status is valid.  */
    if (status)
    {
        error_counter++;
        nx_packet_release(my_packet);
    }

    /* Disconnect this socket.  */
    status =  nx_tcp_socket_disconnect(&client_socket, 5 * NX_IP_PERIODIC_RATE);

    /* Determine if the status is valid.  */
    if (status)
        error_counter++;

    /* Unbind the socket.  */
    status =  nx_tcp_client_socket_unbind(&client_socket);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Delete the socket.  */
    status =  nx_tcp_socket_delete(&client_socket);

    /* Check for error.  */
    if (status)
        error_counter++;
 
    /* Check status.  */
    if (error_counter)
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
    

static void    thread_1_entry(ULONG thread_input)
{

UINT            status;
NX_PACKET       *packet_ptr;
ULONG           actual_status;
ULONG           byte_copied;


    /* Ensure the IP instance has been initialized.  */
    status =  nx_ip_status_check(&ip_1, NX_IP_INITIALIZE_DONE, &actual_status, NX_IP_PERIODIC_RATE);

    /* Check status...  */
    if (status != NX_SUCCESS)
    {

        error_counter++;
        test_control_return(1);
    }

    /* Create a socket.  */
    status =  nx_tcp_socket_create(&ip_1, &server_socket, "Server Socket", 
                                NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 300,
                                NX_NULL, NX_NULL);
                                
    /* Check for error.  */
    if (status)
        error_counter++;

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Setup this thread to listen.  */
    status =  nx_tcp_server_socket_listen(&ip_1, 12, &server_socket, 5, NX_NULL);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Accept a client socket connection.  */
    status =  nx_tcp_server_socket_accept(&server_socket, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Receive a TCP message from the socket.  */
    status =  nx_tcp_socket_receive(&server_socket, &packet_ptr, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;
    else
    {

        /* Retrieve data.  */
        status = nx_packet_data_retrieve(packet_ptr, recv_buff, &byte_copied);

        /* Check for error.  */
        if (status)
            error_counter++;

        /* Compare with send buffer. */
        if (byte_copied != sizeof(send_buff))
            error_counter++;
        else if (memcmp(send_buff, recv_buff, sizeof(send_buff)))
            error_counter++;

        /* Release the packet.  */
        nx_packet_release(packet_ptr);
    }
    
    /* Disconnect the server socket.  */
    status =  nx_tcp_socket_disconnect(&server_socket, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Unaccept the server socket.  */
    status =  nx_tcp_server_socket_unaccept(&server_socket);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Unlisten on the server port.  */
    status =  nx_tcp_server_socket_unlisten(&ip_1, 12);

    /* Check for error.  */
    if (status)
        error_counter++;
}
#else
#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_packet_payload_size_test_application_define(void *first_unused_memory)
#endif
{

    /* Print out test information banner.  */
    printf("NetX Test:   Packet Payload Size Test..................................N/A\n");

    test_control_return(3);

}
#endif
