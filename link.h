#include <stdlib.h>
#include <string.h>
#include <stdio.h>


struct link{
	int nivel;
	char *path;
	char *pathParinte;
	char *html;
	char *serverIp;
	struct link *urm;
};



struct link* addCoada(struct link *tail , char *p , char *pParinte , char* Html ,  int niv , char* IP ){
	if(tail == NULL){
		tail = (struct link *) malloc (sizeof(struct link));
		tail->path = (char*) malloc(strlen(p));
		tail->pathParinte = (char*) malloc(strlen(pParinte));
		tail->html = (char*) malloc(strlen(Html));
		tail->serverIp = (char *) malloc(strlen(IP));
		strcpy(tail->serverIp,IP);
		strcpy(tail->path , p);
		strcpy(tail->pathParinte , pParinte);
		strcpy(tail->html , Html);
		tail->nivel = niv;
		tail->urm = NULL;
		return tail;
	}
	else{
			struct link *curent = tail;
			while(curent->urm != NULL)
				curent = curent->urm;
			struct link *l = (struct link *) malloc (sizeof(struct link));
			l->path = (char*) malloc(strlen(p));
			l->pathParinte = (char*) malloc(strlen(pParinte));
			l->serverIp = (char *) malloc(strlen(IP));
			l->html = (char*) malloc(strlen(Html));			
			strcpy(l->serverIp , IP);
			strcpy(l->path , p);
			strcpy(l->pathParinte , pParinte);
			strcpy(l->html , Html);
			l->nivel = niv;
			l->urm = NULL;
			curent->urm = l;
			return tail; 
		}
}

struct link* scoateCoada(struct link *tail){
	struct link *auxiliar = tail->urm;
	tail->urm = NULL;
	tail = auxiliar;
	return tail;
}


 void afisare(struct link coada){
 		printf("Path = %s\nPath parinte = %s\nHtml = %s\nNivel = %d\nIP = %s\n",coada.path,coada.pathParinte,coada.html,coada.nivel,coada.serverIp);
 	return ;
 }