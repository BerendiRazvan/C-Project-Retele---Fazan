#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>



int main(){

	int c;
	struct sockaddr_in server;

	c=socket(AF_INET,SOCK_STREAM,0);
	if(c<0){
		printf("Err la creare socket\n");
		return 1;
	}

	memset(&server,0,sizeof(server));
	server.sin_port=htons(42873);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");

	if(connect(c,(struct sockaddr *) &server,sizeof(server))<0){
		printf("Err conect la server\n");
		return 0;
	}

	//Jucator

	printf("BUN VENIT LA JOCUL FAZAN! Incearca sa nu fi unul\n");

	//IDentificare jucator
	uint16_t idJ;
	recv(c,&idJ,sizeof(idJ),0);
	idJ=ntohs(idJ);
	printf("Sunteti jucatorul %hu\n\n",idJ);


	//JOC
	char fc,cuv1[100],cuv2[100];
	bzero(cuv1,100);
	bzero(cuv2,100);
	uint16_t len1=0,len2=0,cl;
	int i;
	if(idJ==1){
		//J1 primeste caracter si trimite cuvant
		recv(c,&fc,sizeof(char),0);
		printf("Litera de inceput este: %c\n",fc);
		printf("Dati un cuvant:");
		while(1){
			fgets(cuv1,sizeof(cuv1),stdin);
			if(cuv1[0]==fc)break;
			else printf("Dati cuvant care incepe cu %c:",fc);
		}
		len1=strlen(cuv1);
		cl=htons(len1);
		send(c,&cl,sizeof(cl),0);
		for(i=0;i<len1;i++)
			send(c,&cuv1[i],sizeof(char),0);
	}


	while(1){
		uint16_t semnal=0;
		if(idJ==2){
			//J2 primeste cuv de la J1
			bzero(cuv2,100);
			recv(c,&len2,sizeof(len2),0);
			len2=ntohs(len2);
			for(i=0;i<len2;i++)
				recv(c,&cuv2[i],sizeof(char),0);
			printf("Jucatorul 1 a ales cuvantul: %s",cuv2);
		}


		//J2 trimite cuv
		if(idJ==2){
			printf("Dati un cuvant:");
			bzero(cuv2,100);
			fgets(cuv2,sizeof(cuv2),stdin);
			len2=strlen(cuv2);
			cl=htons(len2);
			send(c,&cl,sizeof(cl),0);
			for(i=0;i<len2;i++)
				send(c,&cuv2[i],sizeof(char),0);

		}


		//update stare joc
		recv(c,&semnal,sizeof(semnal),0);
		semnal=ntohs(semnal);
		if(semnal==1)break;
		else semnal=0;
		//J1 primeste cuv de la J2
		if(idJ==1){
			bzero(cuv1,100);
			recv(c,&len1,sizeof(len1),0);
			len1=ntohs(len1);
			for(i=0;i<len1;i++)
				recv(c,&cuv1[i],sizeof(char),0);
			printf("Jucatorul 2 a ales cuvantul: %s",cuv1);
		}


		//J1 trimite cuv
		if(idJ==1){
			printf("Dati un cuvant:");
			bzero(cuv1,100);
			fgets(cuv1,sizeof(cuv1),stdin);
			len1=strlen(cuv1);
			cl=htons(len1);
			send(c,&cl,sizeof(cl),0);
			for(i=0;i<len1;i++)
				send(c,&cuv1[i],sizeof(char),0);

		}

		//vrificare stare joc
		recv(c,&semnal,sizeof(semnal),0);
		semnal=ntohs(semnal);
		if(semnal==1)break;
		else  semnal=0;
	}


	//rezultat joc
	uint16_t rezultat;
	recv(c,&rezultat,sizeof(rezultat),0);
	rezultat=ntohs(rezultat);

	if(rezultat==idJ)printf("ATI CASTIGAT! :)\n");
	else printf("FAZAN! :(\n");

	printf("\n");
	close(c);
}

