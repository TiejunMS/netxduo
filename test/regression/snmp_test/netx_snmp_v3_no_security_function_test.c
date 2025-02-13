/* This NetX test concentrates on the SNMPv3 operation with no security.  The test examines the response packet length
   and internal statistics of processing a v3 response to pass the test. 
   */

#include   "tx_api.h"
#include   "nx_api.h" 
#if defined(__PRODUCT_NETXDUO__) 
#include   "nxd_snmp.h"
#else
#include   "nx_snmp.h"
#endif
#include   "nx_udp.h"
#include   "small_mib_helper.h"

extern void    test_control_return(UINT);

#if !defined(NX_DISABLE_IPV4)

#define     DEMO_STACK_SIZE         4096


extern MIB_ENTRY   mib2_mib[];

static UINT    v3_mib2_getnext_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data);
static UINT    v3_mib2_set_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data);
static UINT    v3_mib2_get_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data);
static UINT    v3_mib2_username_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *username);
static VOID    v3_mib2_variable_update(NX_IP *ip_ptr, NX_SNMP_AGENT *agent_ptr);


static UINT        query_response_complete = NX_FALSE; /* to synchronize when the agent sends the SNMP trap */
#define     QUERY_COUNT             2

/* To show byte by byte comparison of pre-recorded response with SNMP agent, define this option.  
#define VERBOSE
*/


/* Define the ThreadX and NetX object control blocks...  */

static TX_THREAD               thread_agent;
static TX_THREAD               thread_manager;
static NX_SNMP_AGENT           my_agent;
static NX_PACKET_POOL          pool_0;
static NX_IP                   agent_ip;
static NX_IP                   manager_ip;
static NX_UDP_SOCKET           snmp_manager_socket;

#define SNMP_MANAGER_ADDRESS   IP_ADDRESS(10,0,0,1)  
#define SNMP_AGENT_ADDRESS     IP_ADDRESS(10,0,0,10)  



static UCHAR context_engine_id[] = {0x80, 0x00, 0x0d, 0xfe, 0x03, 0x00, 0x77, 0x23, 0x23, 0x46, 0x69};
static UINT  context_engine_size = 11;

/* Define the counters used in the demo application...  */

static UINT                    status;
static ULONG                   error_counter;


/* Define thread prototypes.  */

static void    thread_0_entry(ULONG thread_input);
static void    thread_1_entry(ULONG thread_input);
extern void    _nx_ram_network_driver_1500(struct NX_IP_DRIVER_STRUCT *driver_req);
static void    snmp_test_initialize();

/* Send SNMP manager query.  */
static UINT    nx_snmp_query_packet_send(NX_UDP_SOCKET *snmp_manager_socket, UINT request_id, UINT packet_number);

extern char v3_nosec_getrequest_pkt[58];
extern int  v3_nosec_getrequest_size;
extern char v3_nosec_getnext_request_pkt[98];
extern int  v3_nosec_getnext_request_size;



typedef struct SNMP_QUERY_STRUCT
{
    char          *snmp_query_pkt_data;
    int            snmp_query_pkt_size;
} SNMP_QUERY;



static SNMP_QUERY       snmp_query[QUERY_COUNT];



/* Define what the initial system looks like.  */

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_snmp_v3_no_security_application_define(void *first_unused_memory)
#endif
{

CHAR    *pointer;
    
    /* Setup the working pointer.  */
    pointer =  (CHAR *) first_unused_memory;
    
    /* Create the SNMP agent thread.  */
    status = tx_thread_create(&thread_agent, "Agent thread", thread_0_entry, 0,  
            pointer, DEMO_STACK_SIZE,  4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    pointer =  pointer + DEMO_STACK_SIZE;

    /* Create the SNMP Manager thread.  */
    status += tx_thread_create(&thread_manager, "Manager thread", thread_1_entry, 0,  
            pointer, DEMO_STACK_SIZE, 
            3, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
    pointer =  pointer + DEMO_STACK_SIZE;

        /* Check for IP create errors.  */
    /* Check for IP create errors.  */
    if (status)
    {
        error_counter++;
        test_control_return(1);
    }        

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status =  nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 1000, pointer, 8192);
    pointer = pointer + 8192;

        /* Check for IP create errors.  */
    if (status)
    {
        error_counter++;
    }        

    /* Create an IP instance.  */
    status = nx_ip_create(&agent_ip, "Agent IP", SNMP_AGENT_ADDRESS, 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_1500, pointer , 2048, 1);

    pointer += 2048;

    /* Create another IP instance.  */
    status += nx_ip_create(&manager_ip, "Manager IP", SNMP_MANAGER_ADDRESS, 0xFFFFFF00UL, &pool_0, _nx_ram_network_driver_1500, pointer, 2048, 1);

    pointer += 2048;

    /* Check for IP create errors.  */
    if (status)
    {
        error_counter++;
    }        

    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&agent_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;

    /* Enable ARP and supply ARP cache memory for IP Instance 1.  */
    status +=  nx_arp_enable(&manager_ip, (void *) pointer, 1024);
    pointer = pointer + 1024;

    /* Check for ARP enable errors.  */
    if (status)
    {
        error_counter++;
    }          

    /* Enable UDP traffic.  */
    status =  nx_udp_enable(&agent_ip);
    status += nx_udp_enable(&manager_ip);

    /* Check for UDP enable errors.  */
    if (status)
    {
        error_counter++;
    }          

    /* Create an SNMP agent instance.  */
    status = nx_snmp_agent_create(&my_agent, "public", &agent_ip, pointer, 4096, &pool_0, 
                         v3_mib2_username_processing, v3_mib2_get_processing, 
                         v3_mib2_getnext_processing, v3_mib2_set_processing);
    pointer =  pointer + 4096;


    if (status)
    {
        error_counter++;
    }          

    return;
}

/* Define the test threads.  */

static void    thread_0_entry(ULONG thread_input)
{

UINT status;
ULONG boot_count = 0x1;
UINT version = NX_SNMP_VERSION_3;
UINT i;


    printf("NetX Test:   SNMP V3 No Security Test..................................");
    tx_thread_sleep(20);

    if (error_counter)
    {
        error_counter++;
    }

    /* Make sure boot time and engine boot ID match the pre-recorded data. */
    status = nx_snmp_agent_context_engine_set(&my_agent, context_engine_id, context_engine_size);
    status |= nx_snmp_agent_v3_context_boots_set(&my_agent, boot_count);

    if (status)
    {
        error_counter++;

    } 

    /* Otherwise, store the context engine.  */
    for (i = 0; i < context_engine_size; i++)
    {

        /* Check each byte of the context engine.  */
        if (my_agent.nx_snmp_agent_v3_context_engine[i] !=  context_engine_id[i])
        {
            error_counter++;
        }
    }

    /* Check the context engine length.  */
    if (my_agent.nx_snmp_agent_v3_context_engine_size !=  context_engine_size)
    {
        error_counter++;
    }

    if (my_agent.nx_snmp_agent_v3_context_engine_boots != boot_count)
    {
        error_counter++;
    }

    /* Reset the system clock so we can reasonably fit in the 150 second Time Window. */
    tx_time_set(0);

    /* Start the SNMP instance.  */
    status = nx_snmp_agent_start(&my_agent);
    
    /* Return the test result.  */
    if (status)
    {
       error_counter++;
    }

    while (!query_response_complete)
        tx_thread_sleep(50);

    /* Verify that we are receiving SNMPv3 packets. */
    status = nx_snmp_agent_current_version_get(&my_agent, &version);

    if (status || (version != NX_SNMP_VERSION_3))
    {
         error_counter++;
    }

    if (error_counter )
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

NX_PACKET   *agent_packet;
UINT        i;
USHORT      request_id = 1;

    /* Let the agent get set up first! */
    tx_thread_sleep(100);

    status = nx_udp_socket_create(&manager_ip, &snmp_manager_socket, "Manager Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, 0x80, 5);

    /* Bind the UDP socket to the IP port.  */
    status |=  nx_udp_socket_bind(&snmp_manager_socket, 161, 100);

    /* Check status.  */
    if (status)
    {

        error_counter++;

        /* Indicate the query response is complete. */
        query_response_complete = NX_TRUE;

        error_counter++;
    }

    /* Load the test data. */
    snmp_test_initialize();

    /* Send SNMP queries to the agent..  */
    for (i = 0; i < QUERY_COUNT; i++ )
    {

        /* Send the SNMP manager query packet.  */
        status = nx_snmp_query_packet_send(&snmp_manager_socket, request_id,  i);

        /* Check status.  */
        if (status)
        {        
            error_counter++; 
            break;
        }    

        /* Receive the SNMP agent response.  */
        status =  nx_udp_socket_receive(&snmp_manager_socket, &agent_packet, 100);

        /* Check status.  */
        if (status)
        {
            error_counter++;
            break;
        }       

        /* Check for errors on discovery/report exchange. */
        if (i == 0)
        {
            if ((my_agent.nx_snmp_agent_reports_sent != 1) || (my_agent.nx_snmp_agent_unknown_engineid_count != 1))
            {
                error_counter++;
                break;
            } 

            /* Allow variability for size of certain timer variables (unpredictable). */
            if ((agent_packet -> nx_packet_length < 108) || (agent_packet -> nx_packet_length > 112))
            {
                error_counter++;
                break;
            }
        }

        /* Check for errors on the completion of V3 handshake. */
        if (i == 1)
        {
            if ((my_agent.nx_snmp_agent_getresponse_sent != 1) ||(my_agent.nx_snmp_agent_getnext_requests != 1))
            {
                error_counter++;
                break;
            }     

            /* Allow variability for size of certain timer variables (unpredictable). */
            if ((agent_packet -> nx_packet_length < 110) || (agent_packet -> nx_packet_length > 114))
            {
                error_counter++;
                break;
            }
        }

        /* Release the packet.  */
        nx_packet_release(agent_packet);

        request_id++;
    }

    /* Check for general errors. */
    if (my_agent.nx_snmp_agent_request_errors || my_agent.nx_snmp_agent_internal_errors)
    {

        error_counter++;
    }

    /* Indicate the query response is complete. */
    query_response_complete = NX_TRUE;

}


static UINT   nx_snmp_query_packet_send(NX_UDP_SOCKET *snmp_manager_socket, UINT snmp_request_id, UINT packet_number)
{
UINT        status;
NX_PACKET   *query_packet;

    /* Allocate a response packet.  */
    status =  nx_packet_allocate(&pool_0, &query_packet, NX_UDP_PACKET, 100);
    
    /* Check status.  */
    if (status)
    {
        return status;
    }

    memset(query_packet -> nx_packet_prepend_ptr, 0, (query_packet -> nx_packet_data_end - query_packet -> nx_packet_prepend_ptr));

    /* Write the SMTP response messages into the packet payload!  */
    memcpy(query_packet -> nx_packet_prepend_ptr, 
           snmp_query[packet_number].snmp_query_pkt_data, 
           snmp_query[packet_number].snmp_query_pkt_size); 

    /* Adjust the write pointer.  */
    query_packet -> nx_packet_length =  snmp_query[packet_number].snmp_query_pkt_size; 
    query_packet -> nx_packet_append_ptr =  query_packet -> nx_packet_prepend_ptr + query_packet -> nx_packet_length;

    /* Send the UDP packet with the correct port.  */
    status =  nx_udp_socket_send(snmp_manager_socket, query_packet, IP_ADDRESS(10, 0, 0, 10), 161);

    /* Check the status.  */
    if (status)      
        nx_packet_release(query_packet);         

    return status;
}


static void  snmp_test_initialize()
{

     /* Contact - no security*/
     snmp_query[0].snmp_query_pkt_data = &v3_nosec_getrequest_pkt[0];
     snmp_query[0].snmp_query_pkt_size = v3_nosec_getrequest_size;
     snmp_query[1].snmp_query_pkt_data = &v3_nosec_getnext_request_pkt[0];  
     snmp_query[1].snmp_query_pkt_size = v3_nosec_getnext_request_size;

}


/* Define the application's GET processing routine.  */

UINT    v3_mib2_get_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{

UINT    i;
UINT    status;


    /* Loop through the sample MIB to see if we have information for the supplied variable.  */
    i =  0;
    status =  NX_SNMP_ERROR;
    while (mib2_mib[i].object_name)
    {

        /* See if we have found the matching entry.  */
        status =  nx_snmp_object_compare(object_requested, mib2_mib[i].object_name);

        /* Was it found?  */
        if (status == NX_SUCCESS)
        {

            /* Yes it was found.  */
            break;
        }

        /* Move to the next index.  */
        i++;
    }

    /* Determine if a not found condition is present.  */
    if (status != NX_SUCCESS)
    {

        /* The object was not found - return an error.  */
        return(NX_SNMP_ERROR_NOSUCHNAME);
    }

    /* Determine if the entry has a get function.  */
    if (mib2_mib[i].object_get_callback)
    {

        /* Yes, call the get function.  */
        status =  (mib2_mib[i].object_get_callback)(mib2_mib[i].object_value_ptr, object_data);
    }
    else
    {

        /* No get function, return no access.  */
        status =  NX_SNMP_ERROR_NOACCESS;
    }

    /* Return the status.  */
    return(status);
}


/* Define the application's GETNEXT processing routine.  */

UINT    v3_mib2_getnext_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{

UINT    i;
UINT    status;


    /* Loop through the sample MIB to see if we have information for the supplied variable.  */
    i =  0;
    status =  NX_SNMP_ERROR;
    while (mib2_mib[i].object_name)
    {

        /* See if we have found the next entry.  */
        status =  nx_snmp_object_compare(object_requested, mib2_mib[i].object_name);

        /* Is the next entry the mib greater?  */
        if (status == NX_SNMP_NEXT_ENTRY)
        {

            /* Yes it was found.  */
            break;
        }

        /* Move to the next index.  */
        i++;
    }

    /* Determine if a not found condition is present.  */
    if (status != NX_SNMP_NEXT_ENTRY)
    {

        /* The object was not found - return an error.  */
        return(NX_SNMP_ERROR_NOSUCHNAME);
    }


    /* Copy the new name into the object.  */
    nx_snmp_object_copy(mib2_mib[i].object_name, object_requested);

    /* Determine if the entry has a get function.  */
    if (mib2_mib[i].object_get_callback)
    {

        /* Yes, call the get function.  */
        status =  (mib2_mib[i].object_get_callback)(mib2_mib[i].object_value_ptr, object_data);

        /* Determine if the object data indicates an end-of-mib condition.  */
        if (object_data -> nx_snmp_object_data_type == NX_SNMP_END_OF_MIB_VIEW)
        {

            /* Copy the name supplied in the mib table.  */
            nx_snmp_object_copy(mib2_mib[i].object_value_ptr, object_requested);
        }
    }
    else
    {

        /* No get function, return no access.  */
        status =  NX_SNMP_ERROR_NOACCESS;
    }

    /* Return the status.  */
    return(status);
}


/* Define the application's SET processing routine.  */

UINT    v3_mib2_set_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *object_requested, NX_SNMP_OBJECT_DATA *object_data)
{

UINT    i;
UINT    status;


    /* Loop through the sample MIB to see if we have information for the supplied variable.  */
    i =  0;
    status =  NX_SNMP_ERROR;
    while (mib2_mib[i].object_name)
    {

        /* See if we have found the matching entry.  */
        status =  nx_snmp_object_compare(object_requested, mib2_mib[i].object_name);

        /* Was it found?  */
        if (status == NX_SUCCESS)
        {

            /* Yes it was found.  */
            break;
        }

        /* Move to the next index.  */
        i++;
    }

    /* Determine if a not found condition is present.  */
    if (status != NX_SUCCESS)
    {

        /* The object was not found - return an error.  */
        return(NX_SNMP_ERROR_NOSUCHNAME);
    }


    /* Determine if the entry has a set function.  */
    if (mib2_mib[i].object_set_callback)
    {

        /* Yes, call the set function.  */
        status =  (mib2_mib[i].object_set_callback)(mib2_mib[i].object_value_ptr, object_data);
    }
    else
    {

        /* No get function, return no access.  */
        status =  NX_SNMP_ERROR_NOACCESS;
    }

    
    /* Return the status.  */
    return(status);
}

/* Create an error code if matching user not found. */
#define USER_NOT_FOUND 1

/* Define the username callback routine routine. Usernames should be 
   associated with permissions (public or private string) and what version
   of SNMP the user is configured for. The username callback should verify
   the incoming username MIB access permissions.  */
UINT  v3_mib2_username_processing(NX_SNMP_AGENT *agent_ptr, UCHAR *username)
{

    v3_mib2_variable_update(&agent_ip, &my_agent);

    return NX_SUCCESS;

}

extern ULONG    sysUpTime;
/* Define the application's update routine.  */ 

VOID  v3_mib2_variable_update(NX_IP *ip_ptr, NX_SNMP_AGENT *agent_ptr)
{

    /* Update the snmp parameters.  */
    //sysUpTime =                 tx_time_get();
}

#else

#ifdef CTEST
VOID test_application_define(void *first_unused_memory)
#else
void    netx_snmp_v3_no_security_application_define(void *first_unused_memory)
#endif
{

    /* Print out test information banner.  */
    printf("NetX Test:   SNMP V3 No Security Test..................................N/A\n"); 

    test_control_return(3);  
}      
#endif
