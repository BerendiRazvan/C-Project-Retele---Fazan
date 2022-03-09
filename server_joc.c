#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <time.h>
void deservire_client(int c1,int c2) {

	printf("\nJocul incepe!\n\n");
	// deservirea clientilor
	srand(time(0));
	char rchr = 'a' + (random() % 26);
	

	//jucatorii se identifica
	uint16_t kk=1,kkk=2;
	kk=htons(kk);
	send(c1,&kk,sizeof(kk),0);
	kkk=htons(kkk);
	send(c2,&kkk,sizeof(kkk),0);

	//J1 incepe
	send(c1,&rchr,sizeof(char),0);


	char cuv1[100],cuv2[100];
	bzero(cuv1,100);
	bzero(cuv2,100);
	uint16_t len1=0,len2=0;
	int i;
	//J1 trimite cuvantul de incepere
	recv(c1,&len1,sizeof(len1),MSG_WAITALL);
	len1=ntohs(len1);
	for(i=0;i<len1;i++)
		recv(c1,&cuv1[i],sizeof(char),MSG_WAITALL);

	
	
	uint16_t j=0;
	while(1){
		//printf("Bucla\n");
		uint16_t semnal=0,stopjoc=0;
		//trimitem cuv la J2, cuvantul lui J1
		uint16_t cl=htons(len1);
		send(c2,&cl,sizeof(cl),0);
		for(i=0;i<len1;i++)
			send(c2,&cuv1[i],sizeof(char),0);

		
		//primim cuv de la J2
		bzero(cuv2,100);
		recv(c2,&len2,sizeof(len2),MSG_WAITALL);
		len2=ntohs(len2);
		for(i=0;i<len2;i++)
			recv(c2,&cuv2[i],sizeof(char),MSG_WAITALL);

		//printf("Am primit la comparat J1(%s);J2(%s)\n",cuv1,cuv2);
		//avem cuv de la J1 si J2, verificam daca J2 continua
		if(cuv2[0]==cuv1[len1-3]&&cuv2[1]==cuv1[len1-2]){printf("Jocul continua!(J-2)\n");}
		else{ stopjoc=1;j=1;}

		//0 - continua jocul; 1 - stop joc
		semnal=htons(stopjoc);
		send(c1,&semnal,sizeof(semnal),0);
		send(c2,&semnal,sizeof(semnal),0);

		if(stopjoc==1)break;
		semnal=0;
		stopjoc=0;

		//trimitem cuv la J1, de la J2
		cl=htons(len2);
		send(c1,&cl,sizeof(cl),0);
		for(i=0;i<len2;i++)
			send(c1,&cuv2[i],sizeof(char),0);

		//primim cuv de la J1
		bzero(cuv1,100);
		recv(c1,&len1,sizeof(len1),MSG_WAITALL);
		len1=ntohs(len1);
		for(i=0;i<len1;i++)
			recv(c1,&cuv1[i],sizeof(char),MSG_WAITALL);

		//printf("Am primit la comparat J1(%s);J2(%s)\n",cuv1,cuv2);
		//avem cuv de la J1 si J2, verificam daca J1 continua
		if(cuv1[0]==cuv2[len2-3]&&cuv1[1]==cuv2[len2-2]){printf("Jocul continua!(J-1)\n");}
		else{ stopjoc=1;j=2;}

		//0 - continua jocul; 1 - stop joc
		semnal=htons(stopjoc);
		send(c1,&semnal,sizeof(semnal),0);
		send(c2,&semnal,sizeof(semnal),0);

		if(stopjoc==1)break;
		semnal=0;
		stopjoc=0;

	}

	j=htons(j);
	send(c1,&j,sizeof(j),0);
	send(c2,&j,sizeof(j),0);
	j=ntohs(j);

	//+trimite mesaj: ATI CASTIGAT/FAZAN
	if(j==1)printf("\nCASTIGATOR: Jucatorul 1\nFAZAN: Jucatorul 2\n\n");
	else printf("\nCASTIGATOR: Jucatorul 2\nFAZAN: Jucatorul 1\n\n");


	close(c1);
	close(c2);
	printf("Joc terminat!\n\n\n");
	// sfarsitul deservirii clientilor
}



int main() {
	int s;
	struct sockaddr_in server, client1, client2;
	int c1,c2, l1,l2;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		printf("Eroare la crearea socketului server\n");
		return 1;
	}

	memset(&server, 0, sizeof(server));
	server.sin_port = htons(42873);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
		printf("Eroare la bind\n");
		return 1;
	}

	listen(s, 5);


	l1 = sizeof(client1);
        memset(&client1, 0, sizeof(client1));


	l2 = sizeof(client2);
	memset(&client2, 0, sizeof(client2));
	//probabil cu 2 clienti separati nu doar c1 si c2
	while (1) {
		c1 = accept(s, (struct sockaddr *) &client1, &l1);
		printf("S-a conectat primul jucator.\n");
		c2 = accept(s, (struct sockaddr *) &client2, &l2);
		printf("S-a conectat al doilea jucator.\n");

		if (fork() == 0) { // fiu
			deservire_client(c1,c2);
			return 0;
		}
		// se executa doar in parinte pentru ca fiul se termina mai sus din cauza exit-ului
	}
}

