/*
* Sensor request response protocol
*/


//SRRP parameter types
#define SRRP_SIZE	1       // Size
#define SRRP_ITTR   2       // Itterations
#define SRRP_PRTO   3       // Protocol
#define SRRP_SPEED	4		// Speed
#define SRRP_DUR 	5		// Duration
#define SRRP_DSCP	6		// DSCP

//SRRP parameter
struct srrp_param{
	uint16_t	param;
	uint16_t	value;
} __attribute__((__packed__));

//SRRP request types 
#define SRRP_HB		1       // Heartbeat
#define SRRP_BW		2       // Bandwidth
#define SRRP_RTT	3       // Round trip time
#define SRRP_UDP	4       // UDP iperf - reports packet loss and jitter
#define SRRP_DNS	5       // DNS status 
#define SRRP_TRT	6       // Traceroute

//SRRP request
struct srrp_request{
	uint32_t			id;
	uint16_t			type;
	uint16_t			length;
	uint32_t			dst_ip;
	struct srrp_param	params[ ];
} __attribute__((__packed__));



//SRRP result types 
#define SRRP_RES_RTTAVG 1       // RTT avgerage
#define SRRP_RES_RTTMAX 2       // RTT max
#define SRRP_RES_RTTMIN 3       // RTT min
#define SRRP_RES_RTTDEV 4       // RTT min
#define SRRP_RES_BW 	5       // Bandwidth
#define SRRP_RES_DUR	6		// Duration
#define SRRP_RES_SIZE	7		// Size
#define SRRP_RES_JTR	8       // Jitter
#define SRRP_RES_PKLS 	9      	// Packet loss
#define SRRP_RES_SPEED	10		// Send speed
#define SRRP_RES_DSCP	11		// DSCP flags 
#define SRRP_RES_HOP 	12      // Traceroute hop


//SRRP result 
struct srrp_result{
	uint32_t	result;
	uint32_t	value;
} __attribute__((__packed__));

//SRRP respone success codes
#define SRRP_SCES	1       // Successful
#define SRRP_PSCES  2       // Partly sucessful
#define SRRP_FAIL   3       // Failed

//SRRP response
struct srrp_response{
	uint32_t			id;
	uint16_t			length;
	uint16_t			success;
	struct srrp_result	results[ ];
} __attribute__((__packed__));

/*
*
* Functions for parsing tools output into srrp_responses
*
*/

/*
* id 		- 
* response 	-
* output 	- the last line outputted by ping. String is destroyed.
*/
int parse_ping(int id, struct srrp_response * response, char * output){

	if(response==NULL || output==NULL)
		return 1;

	strtok(output, "=");

	//header
	response->id = id;
	response->length = 4;
	response->success = SRRP_SCES;

	//add results
	struct srrp_result min_result;
	min_result.result = SRRP_RES_RTTMIN;
	//min_result.value = atoi(strtok(NULL, "/"));
	float min = atof(strtok(NULL, "/"));
	memcpy(&min_result.value, &min, 4);
	response->results[0] = min_result;

	struct srrp_result avg_result;
	avg_result.result = SRRP_RES_RTTAVG;
	//avg_result.value = atoi(strtok(NULL, "/"));
	float avg = atof(strtok(NULL, "/"));
	memcpy(&avg_result.value, &min, 4);
	response->results[1] = avg_result;

	struct srrp_result max_result;
	max_result.result = SRRP_RES_RTTMAX;
	//max_result.value = atoi(strtok(NULL, "/"));
	float max = atof(strtok(NULL, "/"));
	memcpy(&max_result.value, &max, 4);
	response->results[2] = max_result;

	struct srrp_result dev_result;
	dev_result.result = SRRP_RES_RTTDEV;
	//dev_result.value = atoi(strtok(NULL, "/"));
	float dev = atof(strtok(NULL, "/"));
	memcpy(&dev_result.value, &dev, 4);
	response->results[3] = dev_result;

	return 0;
}

/*
* response 	-
* output 	- a comma seperated string produced by iperf using the '-y C' flag
*/
int parse_iperf(struct srrp_response * response, char * output){
	return 1;
}

/*
*
*
*/
int parse_udp(int id, struct srrp_response *response, char * output, int send_speed, int dscp_flag){

	if(response==NULL || output==NULL)
		return 1;

	strtok(output, ",");	//connection
	strtok(NULL, ",");		//ip
	strtok(NULL, ",");		//port
	strtok(NULL, ",");		//ip
	strtok(NULL, ",");		//port
	strtok(NULL, ",");		//id
	strtok(NULL, "-");		//time 

	//header
	response->id = id;
	response->length = 7;
	response->success = SRRP_SCES;

	//add results
	struct srrp_result dur_result;
	dur_result.result = SRRP_RES_DUR;
	dur_result.value = atof(strtok(NULL, ","));
	response->results[0] = dur_result;

	struct srrp_result size_result;
	size_result.result = SRRP_RES_SIZE;
	size_result.value = atof(strtok(NULL, ","));
	response->results[1] = size_result;	

	struct srrp_result bw_result;
	bw_result.result = SRRP_RES_BW;
	bw_result.value = atof(strtok(NULL, ","));
	response->results[2] = bw_result;

	struct srrp_result jit_result;
	jit_result.result = SRRP_RES_JTR;
	jit_result.value = atof(strtok(NULL, ","));
	response->results[3] = jit_result;

	strtok(NULL, ",");		//recvd datagras
	strtok(NULL, ",");		//sent datagrams

	struct srrp_result pkls_result;
	pkls_result.result = SRRP_RES_PKLS;
	pkls_result.value = atof(strtok(NULL, ","));
	response->results[4] = pkls_result;

	struct srrp_result dscp;
	dscp.result = SRRP_RES_DSCP;
	dscp.value = dscp_flag;
	response->results[5] = dscp;

	struct srrp_result sspeed;
	sspeed.result = SRRP_RES_SPEED;
	sspeed.value = send_speed;
	response->results[6] = sspeed;

	return 0;
}

void set_float(float num, uint32_t dst){

}