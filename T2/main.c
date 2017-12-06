#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <regex.h>

#define FTP_PORT  21
#define MAX_STRING_SIZE 256

int verifyURL(char * url);
void getFilename(char *path, char * filename);
char read_reply(int sockfd);
void readArgs(char* args, char* user, char* pass, char* host, char* file, int initState);


int main(int argc, char *argv[]) {

	struct  hostent *h;
	int	    sockfd,sockfd_client;
	struct	sockaddr_in server_addr;
	struct  sockaddr_in client_addr;

	//Verify url with regex 
	if(verifyURL(argv[1]))
		return -1;

	char* user   = malloc(MAX_STRING_SIZE);
	char* pass   = malloc(MAX_STRING_SIZE);
	char  mode[] = "pasv\n";
	char* host   = malloc(MAX_STRING_SIZE);
	char* file   = malloc(MAX_STRING_SIZE);

	
	if(strchr(argv[1],'@') != NULL)
		readArgs(argv[1], user, pass, host, file, 0);
	else {
		readArgs(argv[1], user, pass, host, file, 2);
		user = "anonymous\n";
		pass = "clear\n";
	}

	printf("\nUser : %s",user );
	printf("pass : %s",pass );
	printf("host : %s\n",host );
	printf("file : %s\n",file );

	setbuf(stdout, NULL);

	int	bytes;

	if (argc != 2) {
		fprintf(stderr,"usage: getip address\n");
		exit(1);
	}

	h = gethostbyname(host);

	if (h == NULL) {
		herror("gethostbyname");
		exit(1);
	}

	printf("Host name  : %s\n", h->h_name);
	printf("IP Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

	// server address handling
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr)));	/*32 bit Internet address network byte ordered*/
	server_addr.sin_port = htons(FTP_PORT);		/*server TCP port must be network byte ordered */

	// open an TCP socket
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(0);
	}
	// connect to the server
	if(connect(sockfd,
		(struct sockaddr *)&server_addr,
		sizeof(server_addr)) < 0){
		perror("connect()");
		exit(0);
	}


	read_reply(sockfd);

    // send a user to the server
	write(sockfd,"user ",5);
	bytes = write(sockfd, user, strlen(user));
	printf("Written bytes %d\n", bytes);

	char verify_code = read_reply(sockfd);
	if (verify_code == '4' || verify_code == '5'){
		printf("reply code error.\n");
		close(sockfd);
		return -1;
	}

	// send a pass to the server
	write(sockfd,"pass ",5);
	bytes = write(sockfd, pass, strlen(pass));
	printf("Written bytes %d\n", bytes);

	verify_code = read_reply(sockfd);
	if (verify_code == '4' || verify_code == '5'){
		printf("reply code error.\n");
		close(sockfd);
		return -1;
	}

	// send a mode (pasv) to the server
	bytes = write(sockfd, mode, strlen(mode));
	printf("Written bytes %d\n", bytes);

	char temp = 0;
	int adress = 0;
	char temp_buf[7];
	int index = 0;
	while(read(sockfd,&temp,1)){
		printf("%c", temp);
		if(temp == '.')
			break;
		if(temp==')')
			adress=0;
		if (adress >= 4) {
			temp_buf[index]=temp;
			index++;
		}
		if(temp == ',')
			adress++;
	}

	printf("%s\n",temp_buf );

	int second_item = 0;

	for(index = 0; index< strlen(temp_buf); index++){
		if(temp_buf[index]==','){
			temp_buf[index] = '\0';
			second_item = index+1;
			break;
		}
	}

	int second = atoi(&temp_buf[second_item]);
	int first = atoi(&temp_buf[0]);

	printf("First port number = %d\n", first);
	printf("Second port number = %d\n", second);

	int new_port = 256*first+second;

	bzero((char*)&client_addr,sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr)));	/*32 bit Internet address network byte ordered*/
	client_addr.sin_port = htons(new_port);


	if ((sockfd_client = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
		exit(0);
	}

	// connect to the server
	if(connect(sockfd_client,(struct sockaddr *)&client_addr, sizeof(client_addr)) < 0){
		perror("connect()");
		exit(0);
	}

	// send a request to retrieve file to the server
	write(sockfd, "retr ", 5);
	bytes = write(sockfd, file, strlen(file));
	printf("Written bytes %d\n", bytes);


	verify_code = read_reply(sockfd);
	if (verify_code == '4' || verify_code == '5'){
		printf("reply code error.\n");
		close(sockfd);
		close(sockfd_client);
		return -1;
	}

	char filename[MAX_STRING_SIZE];
	getFilename(file, filename);
	FILE *fp;
	fp = fopen(filename,"w");

	temp = 0;
	int read_error = 0;
	while((read_error=read(sockfd_client,&temp,1))){
		fprintf(fp,"%c", temp);
	}

	fclose(fp);
	close(sockfd);
	close(sockfd_client);

	return 0;
}


char read_reply(int sockfd){
	int state=0;
	char temp;
	char reply_code[4];

	while (state!=99) {
		read(sockfd,&temp,1);
		switch(state){
			case 0://read start, first code char
			printf("%c",temp);
			if(isdigit(temp)){
				reply_code[state]=temp;
				state = 1;
			}else{
				if(temp!=13 && temp!=10)
					printf("\nDidn't read a number on first char of reply code\n");
			}
			break;

			case 1://second code char
			printf("%c",temp);
			if(isdigit(temp)){
				reply_code[state]=temp;
				state = 2;
			}else{
				state=0;
				printf("\nCouldn't read second char of reply code\n");
			}
			break;

			case 2://third code char
			printf("%c",temp);
			if(isdigit(temp)){
				reply_code[state]=temp;
				state = 3;
			}else{
				state=0;
				printf("\nCouldn't read third char of reply code\n");
			}
			break;

			case 3://checking last char of reply code
			printf("%c",temp);
			if(temp == ' '){
				state = 9;
			}else{
				state=4;
			}
			break;

			case 4://waiting for reply code with space
			printf("%c",temp);
			if(isdigit(temp) && temp == reply_code[state-4]){
				state = 5;
			}
			break;

			case 5://read first reply code char
			printf("%c",temp);
			if(isdigit(temp) && temp == reply_code[state-4]){
				state = 6;
			}else{
				state = 4;
			}
			break;

			case 6://read second reply code char
			printf("%c",temp);
			if(isdigit(temp) && temp == reply_code[state-4]){
				state = 7;
			}else{
				state=4;
			}
			break;

			case 7://read third reply code char
			printf("%c",temp);
			if(temp==' '){
				state = 9;
			}else{
				state=4;
			}
			break;

			case 9:
			printf("%c",temp);
			if(temp == '\n')
				state = 99;
			break;
		}
	}
	return reply_code[0];
}


void readArgs(char* args, char* user, char* pass, char* host, char* file, int initState){

	int state = initState, index = 6, inner_index = 0;
	while(index != strlen(args)) {
		switch (state){
			case 0:
			if(args[index] == ':'){
				user[inner_index] = '\n';
				user[inner_index + 1] = '\0';
				state = 1;
				inner_index = 0;
			} else {
				user[inner_index] = args[index];
				inner_index++;
			}
			break;

			case 1:
			if(args[index] == '@'){
				pass[inner_index] = '\n';
				pass[inner_index + 1] = '\0';
				state = 2;
				inner_index = 0;
			} else {
				pass[inner_index] = args[index];
				inner_index++;
			}
			break;

			case 2:
			if(args[index] == '/'){
				host[inner_index] = '\0';
				state = 3;
				inner_index = 0;
			} else {
				host[inner_index] = args[index];
				inner_index++;
			}
			break;
			case 3:
			file[inner_index] = args[index];
			inner_index++;
			break;
		}

		index++;
	}

	file[inner_index] = '\n';
	file[inner_index + 1] = '\0';
}


void getFilename(char *path, char * filename){
	int length = strlen(path);
	int i, filenameIndex = 0;
	for(i = 0; i < length; i++) {
		if(path[i] == '/') {
			memset(filename,0,MAX_STRING_SIZE);
			filenameIndex = 0;
		}
		else{
			filename[filenameIndex] = path[i];
			filenameIndex++;
		}
	}
}

int verifyURL(char * url){
	regex_t regex;
	int reti;
	char msgbuf[100];

	// compile regular expression 
	reti = regcomp(&regex, "^ftp://([a-z0-9]+:[a-z0-9]+@)?([\\.a-z0-9-]+)/([\\./a-z0-9-]+)$", REG_EXTENDED|REG_ICASE);
	if (reti) {
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}

	// execute regular expression
	reti = regexec(&regex, url , 0, NULL, 0);
	if (!reti) {
		puts("\nValid URL checked!");
		return 0;
	}
	else if (reti == REG_NOMATCH) {
		puts("Please insert a ftp url in the format ftp://[<username>:<password>@]<hostame>/<file-path>");
		return 1;
	}
	else {
		regerror(reti, &regex, msgbuf, sizeof(msgbuf));
		fprintf(stderr, "Regex match failed: %s\n", msgbuf);
		exit(1);
	}

	// free memory allocated to the pattern buffer by regcomp()
	regfree(&regex);
}