#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>





#define C_ACSS_MAC_SIZE                      6
#define C_ACSS_CHANNEL_MAX                   5
#define C_MAX_CARRIER_TYPE                   15
#define C_NB_CSC_CARRIER                     4
#define C_ACSS_PID_MAX                       3
#define C_ACSS_PVC_MAX                       4

#define NB_ST                                8000

struct st_status {
	int val;
	char name[128];
};

struct st_status st_status[] = {
	{0, "Not regestered"},
	{1, "Registration in Progress"},
	{2, "Registered"}
};


typedef struct
{
	u_int16_t vp;
	u_int16_t vc;
	u_int32_t vp_vc_encaps;
	u_int32_t channel_id;
} __attribute__ ((packed)) T_ACSS_PVC;

typedef struct {
	u_int32_t reserved:8;
	u_int32_t dvb_s2_flag:1;
	u_int32_t dra_flag:1;
	u_int32_t gw_power_control:1;
	u_int32_t fast_frequency_hopping:1;
	u_int32_t gw_freq_correction:1;
	u_int32_t dra_power_offset_in_st:1;
	u_int32_t reserved2:2;

	u_int32_t reserved3:4;
	u_int32_t manufacturer:4;
	u_int32_t version:8;
} __attribute__ ((packed)) T_ACSS_ST_TYPE;

typedef struct {
	u_int32_t channel_id;
	u_int32_t max_avbdc;
	u_int32_t max_rbdc;
} __attribute__ ((packed)) T_ACSS_CHANNEL_DESC;


typedef struct {
	u_int32_t             ACSS_Reference;
	u_int32_t             RLSS_Reference;
	u_int8_t              mac_address[C_ACSS_MAC_SIZE];
	u_int32_t             flu_id;
	u_int32_t             trunk_id;
	T_ACSS_ST_TYPE         st_type;
	u_int32_t              cra;
	u_int32_t              bdc;
	u_int32_t              channel_number;
	T_ACSS_CHANNEL_DESC    channel_desc[C_ACSS_CHANNEL_MAX];
	u_int32_t              pdr_rtn;
	u_int32_t              rbdc_timeout;
	u_int32_t              vbdc_timeout;
	u_int32_t              fca;
	u_int32_t              nb_scheme;
	u_int32_t              scheme_list[C_MAX_CARRIER_TYPE-C_NB_CSC_CARRIER];
	u_int32_t              original_network_id;
	u_int32_t              ts_id;
	u_int32_t              nb_pid_trf;
	u_int32_t              pid_trf[C_ACSS_PID_MAX];
	u_int32_t              nb_pvc;
	T_ACSS_PVC             pvc[C_ACSS_PVC_MAX];
} __attribute__ ((packed)) T_ACSS_ST_PROFILE_ELT; 

typedef struct {
	u_int32_t              validity_logged;
	u_int32_t              terminal_status;
	T_ACSS_ST_PROFILE_ELT  content;
} __attribute__ ((packed)) T_ACSS_ST_PROFILE_FILE_ELT;



int main() {

	u_int32_t total_size = sizeof(T_ACSS_ST_PROFILE_FILE_ELT) * NB_ST;
	printf("struct size= %d bytes\n",  sizeof(T_ACSS_ST_PROFILE_FILE_ELT));
	printf("total_size= %d bytes\n", total_size);

	int fd = open("acss_st_profile.bin", O_RDONLY);
	if (fd == -1) {
		printf("%s", strerror(errno));
		return EXIT_FAILURE;
	}

	T_ACSS_ST_PROFILE_FILE_ELT *bin_file;
	bin_file = (T_ACSS_ST_PROFILE_FILE_ELT *)mmap(NULL, total_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (bin_file == MAP_FAILED) {
		printf("mmap failed: %s",  strerror(errno));
		return EXIT_FAILURE;
	}

	/* read */
	int i = 0;
	for(i = 0; i < NB_ST; i++) {
		if (bin_file[i].validity_logged == -1) {
			continue;
		}

		if (bin_file[i].terminal_status < 0 || bin_file[i].terminal_status > 2) {
		       printf("Invalid terminal status (%u) for ST%d\n",bin_file[i].terminal_status, i);	
		       return EXIT_FAILURE;
		}

		printf("Terminal %i is %s\n", i, st_status[bin_file[i].terminal_status].name);
		T_ACSS_ST_PROFILE_ELT *c = &bin_file[i].content;
	}


	return EXIT_SUCCESS;
}
