#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <librpip.h>

uint32_t setup_socket(int* fd);
uint32_t do_command(int* fd);
uint32_t get_variable(int* cl, char* cmdstr);
uint32_t run_pwm_function(int* cl, char* cmdstr);

uint32_t get_param_uint(uint32_t* error);
float get_param_float(uint32_t* error);
void get_error_response(char* desc, int len);
void get_syntax_response(char* desc, int len, int val);
uint32_t feature_set;

int main(int argc, char *argv[]) {


	int fd;

	if(setup_socket(&fd)) {
	
		feature_set = librpipInit(LIBRPIP_BOARD_DETECT, 0, 0);
		
		while (1) {
			do_command(&fd);
		}
	}
	return 0;
}

uint32_t setup_socket(int* fd) {

	char *socket_path = "/tmp/librpip-socket";
	struct sockaddr_un addr;

	if ( (*fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return 0;
	}
	
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

	unlink(socket_path);

	if (bind(*fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		return 0;
	}

	if (listen(*fd, 5) == -1) {
		perror("listen error");
		return 0;
	}
	return 1;	
}

uint32_t do_command(int* fd) {

	int cl,rc;
	char buf[255];	

	if((cl = accept(*fd, NULL, NULL)) == -1) {
		return 0;
	}	
	
	printf("Client Connected\n");

	rc=read(cl,buf,sizeof(buf)-1);
	
	if(rc > 2 ) {
		buf[rc]='\0';
		printf("Client sent %u bytes (%s)\n", rc, buf);
		switch(buf[0]) {
			case 'V':
				get_variable(&cl,&buf[2]);
				break;					
			case 'P':
				run_pwm_function(&cl,&buf[2]);
				printf("CMD: %c = pwm function\n", buf[0]);
				break;
			default:
				printf("Unknown CMD: %c\n", buf[0]);
		}
		close(cl);
	}
	
	else if(rc > 0) {
		buf[rc]='\0';
		printf("Invalid Request %s\n",buf);
		close(cl);
		return 0;
	}	
	
	else if(rc == 0) {
		printf("Client Dropped Connection\n");
		close(cl);
		return 0;
	}	
    
	else if(rc == -1) {
		perror("read");
		close(cl);
		return 0;
	}

	return 1;
}

uint32_t get_variable(int* cl, char* cmdstr) {

	char *var;
	char buf[255];
	uint32_t valid;

	valid=0;	
	
	var = strtok(cmdstr, " ");

	if(!strncmp("BoardID",var,7)) {
		valid=1;
		sprintf(buf,"Y %u\n",librpipGetBoardID());	
	}			
	else if(!strncmp("FeatureSet",var,10)) {
		valid=1;
		sprintf(buf,"Y %u\n",feature_set);
	} 
	else if(!strncmp("Version",var,7)) {
		valid=1;
		char ver[20];
		librpipVersionStr(&ver[0],sizeof(ver));
		sprintf(buf,"Y %s\n",ver);	
	} 

	if(!valid) {
		printf("Unknown variable %s\n", var);
		return 0;
	}
	
	write(*cl,buf,strlen(buf));
	return 1;	
}

uint32_t run_pwm_function(int* cl, char* cmdstr) {
	char *func;
	char buf[350];
	uint32_t valid;
	uint32_t param_error;

	valid=0;
	param_error=0;	
	
	func = strtok(cmdstr, " ");
	
	if(!strncmp("PwmConfigRead",func,13)) {
		valid=1;
		uint32_t id = get_param_uint(&param_error);
		uint32_t pin;
		uint32_t period;
		uint32_t duty_cycle;
		uint32_t flags;
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),1);
		} else {
			if(librpipPwmConfigRead(id, &pin, &period, &duty_cycle, &flags)) 
				sprintf(buf,"Y %u %u %u %u\n",pin, period, duty_cycle, flags);	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}	
	}	
	else if(!strncmp("PwmConfigWrite",func,14)) {
		valid=1;
		uint32_t id 	= get_param_uint(&param_error);
		uint32_t period	= get_param_uint(&param_error);
		uint32_t duty_cycle= get_param_uint(&param_error);
		uint32_t flags	= get_param_uint(&param_error);
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),4);
		} else {				
			if(librpipPwmConfigWrite(id, period, duty_cycle, flags)) 
				sprintf(buf,"Y\n");	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}		
	}
	else if(!strncmp("PwmStatusRead",func,13)) {
		valid=1;
		uint32_t id	= get_param_uint(&param_error);
		uint32_t status;
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),1);
		} else {
			if(librpipPwmStatusRead(id, &status)) 
				sprintf(buf,"Y %u\n",status);	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}	
	}
	else if(!strncmp("PwmStatusWrite",func,14)) {
		valid=1;
		uint32_t id 	= get_param_uint(&param_error);
		uint32_t status = get_param_uint(&param_error);
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),2);
		} else {		
			if(librpipPwmStatusWrite(id,status)) 
				sprintf(buf,"Y\n");	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}	
	}	
	else if(!strncmp("PwmDutyPercentWrite",func,19)) {
		valid=1;
		uint32_t id	= get_param_uint(&param_error);
		float duty	= get_param_float(&param_error);

		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),2);
		} else {		
			if(librpipPwmDutyPercentWrite(id,duty)) 
				sprintf(buf,"Y\n");	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}
	}		
	else if(!strncmp("ServoConfigRead",func,15)) {
		valid=1;
		uint32_t id	= get_param_uint(&param_error);
		uint32_t range;
		uint32_t pmin;
		uint32_t pmax;

		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),1);
		} else {		
			if(librpipServoConfigRead(id, &range, &pmin, &pmax)) 
				sprintf(buf,"Y %u %u %u\n", range, pmin, pmax);	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}	
	}	
	else if(!strncmp("ServoConfigWrite",func,16)) {
		valid=1;
		uint32_t id 	= get_param_uint(&param_error);
		uint32_t range 	= get_param_uint(&param_error);;
		uint32_t pmin 	= get_param_uint(&param_error);
		uint32_t pmax	= get_param_uint(&param_error);
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),4);
		} else {		
			if(librpipServoConfigWrite(id, range, pmin, pmax)) 
				sprintf(buf,"Y\n");	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}		
	}	
	else if(!strncmp("ServoPositionWrite",func,18)) {
		valid=1;
		uint32_t id 	= get_param_uint(&param_error);
		float angle 	= get_param_float(&param_error);
		
		if(param_error) {
			get_syntax_response(&buf[0], sizeof(buf),2);
		} else {		
			if(librpipServoPositionWrite(id,angle)) 
				sprintf(buf,"Y\n");	
			else 
				get_error_response(&buf[0], sizeof(buf));
		}		
	}

	if(!valid) {
		printf("Unknown function %s\n", func);
		return 0;
	}
	
	write(*cl,buf,strlen(buf));
	return 1;	
}


void get_error_response(char* desc, int len) {

	char errordesc[300];
	librpipErrorGetDescription(&errordesc[0], sizeof(errordesc));
	snprintf(desc, len, "N %u %u %s\n",librpipErrorGetSeverity(),librpipErrorGetCode(),errordesc);

}

void get_syntax_response(char* desc, int len, int val) {

	snprintf(desc, len, "X Syntax Error. Parameter Count Mismatch. Expected %u\n",val);

}

uint32_t get_param_uint(uint32_t* error) {

	char *val;
	
	val=strtok(NULL, " ");
	if(val) {
		return atoi(val);
	} 
	
	*error=1;
	return 0;
}

float get_param_float(uint32_t* error) {

	char *val;
	
	val=strtok(NULL, " ");
	if(val) {
		return atof(val);
	} 
	
	*error=1;
	return 0;
}