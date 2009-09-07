#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>





#define C_ACSS_CHANNEL_MAX                   5
#define C_MAX_CARRIER_TYPE                   15
#define C_NB_CSC_CARRIER                     4
#define C_ACSS_PID_MAX                       3
#define C_ACSS_PVC_MAX                       4

#define C_MAX_ST 16000

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
	u_int8_t              mac_address[8];
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

typedef struct {
	T_ACSS_ST_PROFILE_FILE_ELT      st[C_MAX_ST];
} __attribute__ ((packed))  T_ACSS_ST_PROFILE_FILE;


int main() {
	int ret = -1;
	char path[128] = "acss_st_profile.bin";
	struct stat buf;

	printf("T_ACSS_ST_PROFILE_FILE : %d bytes\n", sizeof(T_ACSS_ST_PROFILE_FILE));
	printf("T_ACSS_ST_PROFILE_FILE_ELT : %d bytes\n", sizeof(T_ACSS_ST_PROFILE_FILE_ELT));
	printf("T_ACSS_ST_PROFILE_ELT : %d bytes\n", sizeof(T_ACSS_ST_PROFILE_ELT));
	printf("C_MAX_ST : %d\n", C_MAX_ST);
	/* stats */
	ret = stat(path, &buf);
	if (ret == -1) {
		printf("stat() %s", strerror(errno));
		return EXIT_FAILURE;
	}

	int file_size = (int) buf.st_size;
	int total_size = sizeof(T_ACSS_ST_PROFILE_FILE_ELT) * C_MAX_ST;
	if (file_size != total_size) {
		printf("incorrect file size: %d != %d (%d x %d)\n", file_size, total_size, sizeof(T_ACSS_ST_PROFILE_FILE_ELT), C_MAX_ST);
		return EXIT_FAILURE;
	}


	int fd = open(path, O_RDONLY);
	if (fd == -1) {
		printf("%s", strerror(errno));
		return EXIT_FAILURE;
	}

	T_ACSS_ST_PROFILE_FILE *bin_file;
	bin_file = (T_ACSS_ST_PROFILE_FILE *)mmap(NULL, total_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (bin_file == MAP_FAILED) {
		printf("mmap failed: %s",  strerror(errno));
		return EXIT_FAILURE;
	}

	/* read */
	int i = 0;
	for(i = 0; i < C_MAX_ST; i++) {
		if (bin_file->st[i].validity_logged == -1) {
			continue;
		}
		if (ntohl(bin_file->st[i].validity_logged) != 1) {
		       printf("Invalid validity (%u) for ST%d\n",bin_file->st[i].validity_logged, i);	
		       return EXIT_FAILURE;
		}


		if (ntohl(bin_file->st[i].terminal_status) < 0 || ntohl(bin_file->st[i].terminal_status) > 2) {
		       printf("Invalid terminal status (%u) for ST%d\n",bin_file->st[i].terminal_status, i);	
		       return EXIT_FAILURE;
		}

		printf("*********************************\n");
		printf("Terminal %i status is %d\n", i, ntohl(bin_file->st[i].terminal_status));
		T_ACSS_ST_PROFILE_ELT *c = &bin_file->st[i].content;
		printf("ACSS ref.: %u\n", ntohl(c->ACSS_Reference));
		printf("RLSS ref.: %u\n", ntohl(c->RLSS_Reference));
		printf("MAC address: %.2x.%.2x.%.2x.%.2x.%.2x.%.2x\n",
			c->mac_address[2],
			c->mac_address[3],
			c->mac_address[4],
			c->mac_address[5],
			c->mac_address[6],
			c->mac_address[7]);
		printf("FLU id.: %u\n", ntohl(c->flu_id));
		printf("Trunk id.: %u\n", ntohl(c->trunk_id));
		printf("CRA: %u\n", ntohl(c->cra));
		printf("BDC: %u\n", ntohl(c->bdc));
		printf("Channel Nb: %d\n", ntohl(c->channel_number));
		int i = 0;
		for (i = 0; i < ntohl(c->channel_number); i++) {
			printf("Channel Id: %d\n", ntohl(c->channel_desc[i].channel_id));
			printf("max_rbdc: %d\n", ntohl(c->channel_desc[i].max_rbdc));
			printf("max_avbdc: %d\n", ntohl(c->channel_desc[i].max_avbdc));
		}
		printf("PDR return: %u\n", ntohl(c->pdr_rtn));
		printf("RBDC timeout: %u\n", ntohl(c->rbdc_timeout));
		printf("VBC Timeout: %u\n", ntohl(c->vbdc_timeout));
		printf("FCA : %u\n", ntohl(c->fca));
	}


	return EXIT_SUCCESS;
}
