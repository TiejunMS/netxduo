/* This NetX test concentrates on out of order TCP data packets.  */

#include   "tx_api.h"
#include   "nx_api.h"
#include   "nx_ram_network_driver_test_1500.h"
extern void    test_control_return(UINT status);

#define     DEMO_STACK_SIZE         2048

#if defined(NX_TCP_MAX_OUT_OF_ORDER_PACKETS) && !defined(NX_DISABLE_IPV4)

/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               thread_0;
static TX_THREAD               thread_1;

static NX_PACKET_POOL          pool_0;
static NX_PACKET_POOL          pool_1;
static NX_IP                   ip_0;
static NX_IP                   ip_1;
static NX_TCP_SOCKET           client_socket;
static NX_TCP_SOCKET           server_socket;
static NX_PACKET              *operation_packet;
static CHAR                   *test_buffer = "ABCDEFGH";


/* Define the counters used in the demo application...  */

static ULONG                   thread_0_counter;
static ULONG                   thread_1_counter;
static ULONG                   error_counter;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
static void    thread_1_connect_received(NX_TCP_SOCKET *server_socket, UINT port);
static void    thread_1_disconnect_received(NX_TCP_SOCKET *server_socket);
extern void    _nx_ram_network_driver_256(struct NX_IP_DRIVER_STRUCT *driver_req);
extern UINT    (*advanced_packet_process_callback)(NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT *operation_ptr, UINT *delay_ptr);
static UINT    my_packet_process(NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT *operation_ptr, UINT *delay_ptr);


/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_tcp_out_of_order_packet_max_test_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;
UINT    status;
ULONG   pool_size;

    
    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;

    thread_0_counter  = 0;
    thread_1_counter =  0;
    error_counter =  0;
    operation_packet = NX_NULL;

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;

    /* Create the main thread.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;


    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    pool_size = (sizeof(NX_PACKET) + 256) * 16;
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 256, pointer, pool_size);
    pointer = pointer + pool_size;

    if (status)
        error_counter++;

    /* Create a packet pool.  */
    pool_size = (sizeof(NX_PACKET) + 256) * (NX_TCP_MAX_OUT_OF_ORDER_PACKETS + 1);
    status =  nx_packet_pool_create(&pool_1, "NetX Main Packet Pool", 256, pointer, pool_size);
    pointer = pointer + pool_size;

    if (status)
        error_counter++;

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", IP_ADDRESS(1, 2, 3, 4), 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_256,
                    pointer, 2048, 1);
    pointer =  pointer + 2048;

    /* Create another IP instance.  */
    status += nx_ip_create(&ip_1, "NetX IP Instance 1", IP_ADDRESS(1, 2, 3, 5), 0xFFFFFF00UL, &pool_1, _nx_ram_network_driver_256,
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
NX_PACKET   *my_packet[8];
UINT        i;



    /* Print out test information banner.  */
    printf("NetX Test:   TCP Out of Order Packet Max Test..........................");

    /* Check for earlier error.  */
    if (error_counter)
    {

        printf("ERROR!\n");
        test_control_return(1);
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
    tx_thread_relinquish();
    status =  nx_tcp_client_socket_connect(&client_socket, IP_ADDRESS(1, 2, 3, 5), 12, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Setup callback function to drop packets. */
    advanced_packet_process_callback = my_packet_process;

    for(i = 0; i < sizeof(my_packet) / sizeof(NX_PACKET *); i++)
    {
        nx_packet_allocate(&pool_0, &my_packet[i], NX_TCP_PACKET, NX_WAIT_FOREVER);
        nx_packet_data_append(my_packet[i], test_buffer + i, 1, &pool_0, NX_WAIT_FOREVER);

        /* Drop the first two packets. */
        if(i < 2)
        {
            operation_packet = my_packet[i];
        }

        nx_tcp_socket_send(&client_socket, my_packet[i], NX_WAIT_FOREVER);
    }

    nx_tcp_socket_disconnect(&client_socket, NX_WAIT_FOREVER);
}
    

static void    thread_1_entry(ULONG thread_input)
{

UINT            status;
NX_PACKET       *packet_ptr;
ULONG           actual_status;
UCHAR           recv_buffer[10];
ULONG           recv_len;
ULONG           total_len = 0;


    /* Ensure the IP instance has been initialized.  */
    status =  nx_ip_status_check(&ip_1, NX_IP_INITIALIZE_DONE, &actual_status, 1 * NX_IP_PERIODIC_RATE);

    /* Check status...  */
    if (status != NX_SUCCESS)
    {

        error_counter++;
        test_control_return(1);
    }

    /* Create a socket.  */
    status =  nx_tcp_socket_create(&ip_1, &server_socket, "Server Socket", 
                                NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 200,
                                NX_NULL, thread_1_disconnect_received);
                                
    /* Check for error.  */
    if (status)
        error_counter++;

    /* Setup this thread to listen.  */
    status =  nx_tcp_server_socket_listen(&ip_1, 12, &server_socket, 5, thread_1_connect_received);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Accept a client socket connection.  */
    status =  nx_tcp_server_socket_accept(&server_socket, 5 * NX_IP_PERIODIC_RATE);

    /* Check for error.  */
    if (status)
        error_counter++;

    /* Receive until no data. */
    while(nx_tcp_socket_receive(&server_socket, &packet_ptr, 5 * NX_IP_PERIODIC_RATE) == NX_SUCCESS)
    {
        nx_packet_data_retrieve(packet_ptr, recv_buffer + total_len, &recv_len);
        nx_packet_release(packet_ptr);
        total_len += recv_len;
    }

    nx_tcp_socket_disconnect(&server_socket, NX_WAIT_FOREVER);

    /* Check received data. */
    if(total_len != strlen(test_buffer))
    {
        error_counter++;
    }
    else if(memcmp(test_buffer, recv_buffer, total_len))
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
    }
}


static void  thread_1_connect_received(NX_TCP_SOCKET *socket_ptr, UINT port)
{

    /* Check for the proper socket and port.  */
    if ((socket_ptr != &server_socket) || (port != 12))
        error_counter++;
}


static void  thread_1_disconnect_received(NX_TCP_SOCKET *socket)
{

    /* Check for proper disconnected socket.  */
    if (socket != &server_socket)
        error_counter++;
}

static UINT    my_packet_process(NX_IP *ip_ptr, NX_PACKET *packet_ptr, UINT *operation_ptr, UINT *delay_ptr)
{
    if(packet_ptr == operation_packet)
    {
        operation_packet = NX_NULL;
        *operation_ptr = NX_RAMDRIVER_OP_DROP;
    }

    return NX_TRUE;
}
#else
#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_tcp_out_of_order_packet_max_test_application_define(void *first_unused_memory)
#endif
{
    printf("NetX Test:   TCP Out of Order Packet Max Test..........................N/A\n");
    test_control_return(2);
}
#endif /* NX_TCP_MAX_OUT_OF_ORDER_PACKETS */
