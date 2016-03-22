#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <unistd.h>     
#include <sys/types.h>  
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "link.h"

#define HTTP_PORT 80
#define MAXLEN 500



ssize_t Readline(int sockd, void *vptr, size_t maxlen) {
  ssize_t n, rc;
  char    c, *buffer;

  buffer = vptr;

  for ( n = 1; n < maxlen; n++ ) {	
   if ( (rc = read(sockd, &c, 1)) == 1 ) {
    *buffer++ = c;
    if ( c == '\n' )
	break;
  }
else if ( rc == 0 ) {
    if ( n == 1 )
	    return 0;
    else
	    break;
}
else {
    if ( errno == EINTR )
	    continue;
    return -1;
  }
}

*buffer = 0;
 return n;
}

/**
 * Trimite o comanda 
 si asteapta raspuns de la server.
 * Comanda trebuie sa fie in buffer-ul sendbuf.
 * Sirul expected contine inceputul raspunsului pe care
 * trebuie sa-l trimita serverul in caz de succes (de ex. codul
 * 250). Daca raspunsul semnaleaza o eroare se iese din program.
 */
void send_command(int sockfd, char sendbuf[]) {
  char recvbuf[MAXLEN];
  int nbytes;
  char CRLF[3];

  CRLF[0] = 13; CRLF[1] = 10; CRLF[2] = 0;
  strcat(sendbuf, CRLF);
  printf("Trimit: %s", sendbuf);
  write(sockfd, sendbuf, strlen(sendbuf));
  nbytes = Readline(sockfd, recvbuf, MAXLEN - 1);
  recvbuf[nbytes] = 0;
  printf("Am primit: %s", recvbuf);
  
}

//pentru o cale care arata /dir1/dir2/dir3/fisier.html eu voi trimite doar /dir1/dir2/dir3
char* getCale(char *serverNameAndPage){
  char *page , *Tooken , *aux;

  //in page voi retine calea
  page = (char*) malloc (1400*sizeof(char));
  //voi lua pe rand cate un token 
  Tooken = (char *) malloc (1400*sizeof(char));
  //cu ajutorul lui aux voi vedea daca tokenul curent este pagina html
  aux = (char *) malloc(1400);


  	Tooken = strtok(serverNameAndPage,"/'");
  	strcpy(aux,Tooken);
  	//daca tokenul este diferit de html
  	if(isHtml(aux) == 0){
  		strcat(page,Tooken);
  		strcat(page,"/");
  		}
  		else 
  			return "NULL";
  	//luam toate tokenurile si cat timp sunt diferite de html adaugam in page
  	while(Tooken != NULL){
  		Tooken = strtok(NULL,"/");
  		strcpy(aux,Tooken);
  		if(isHtml(aux) == 0){
  			strcat(page,Tooken);
  			strcat(page,"/");
  		}
  		else 
  			return page;
  	}
  return page;
}

//intoarce ultimul token dintr-un string(/dir1/dir2/dir3/tokenIntors)
char* getLastString(char *serverNameAndPage){
	char *token;
	char *html;
	token = (char *) malloc(1400);
	html = (char *) malloc(1400);
	token = strtok(serverNameAndPage,"/");
	while(token != NULL){
		strcpy(html,token);
		token = strtok(NULL,"/");
	}
	return html;
}

int isHtml(char* sir){
	//verific daca in sirul trimis ca parametru se gaseste subsirul ".html"
	if(strstr(sir,".html") == NULL)
		return 0;
	return 1;
}

//primeste ca argument numele serverului si intoarce adresaIp
char* getIP(char *server){
  int i;
  char *server_ip;
  struct hostent *auxiliar;
  struct in_addr **addr_list;
  server_ip = (char *) malloc(20*sizeof(char));

  if ( (auxiliar = gethostbyname( server ) ) == NULL) {
    // get the host info
    exit(1);
  }

  addr_list = (struct in_addr **) auxiliar->h_addr_list;

  for(i = 0; addr_list[i] != NULL; i++) {
    //Return the first one;
    strcpy(server_ip , inet_ntoa(*addr_list[i]) );
    return server_ip;    
    }
}

//getCwd returneaza calea absoluta pana la directorul curent
char* getCwd(){
	char* cwd;
    char buff[1400];

    cwd = getcwd( buff, 1400 );
    return cwd;
}

//creeaza o ierarhie de directoare
void makeDirectors(char *page){
	char *token , *director;
	token = (char *) malloc(1400);
	director = (char *) malloc(1400);
	token = strtok(page,"/");
	while(token != NULL){
		strcpy(director,token);
		token = strtok(NULL,"/");
	}
	if(strcmp(director,"..") == 0)
		chdir("..");
	else {
		mkdir(director,0700);
		chdir(director);
	}
}

//functia init se apeleaza o singura data , la inceputul programului
//si are rolul de a adauga primul element in caoda ,mai exact
//ceea ce se transmite ca argument la inceputul programului
struct link* init(struct link*coada , char* serverCale){
	char *server , *cale , *html , *aux , *ip , *token , *caleAbsoluta;
	//folosind strtok sirul initial se va pierde , deci vom avea nevoie de un auxiliar care va fi trimis drept parametru diferitor functii
	aux = (char*) malloc(1400);
	server = (char*) malloc(1400);
	cale = (char*) malloc(1400);
	html = (char*) malloc(1400);
	ip = (char*) malloc(20);
	token = (char*) malloc(1400);
	caleAbsoluta = (char*) malloc(1400);

	//tinand cont de faptul ca linkurile au structura http://serverName/path/f.html
	//retine in server al 2 lea token
	strcpy(aux,serverCale);
	token = strtok(aux,"/");
	token = strtok(NULL,"/");
	strcpy(server,token);

	//creem directorul cu numele serverului si ne mutam in el
	mkdir(server,0700);
	chdir(server);

	//retinem calea absoluta pana la acest director
	strcpy(caleAbsoluta,getCwd());

	//acum vom retine ipul
	strcpy(ip,getIP(server));

	//acum retinem cale si apoi html
	strcpy(aux,serverCale);
	token = strtok(aux,"/");
	token = strtok(NULL,"/");
	token = strtok(NULL,"");
	strcpy(aux,token);
	strcpy(cale,getCale(aux));

	strcpy(aux,serverCale);
	strcpy(html,getLastString(aux));

	//adaugam in coada toate elementele si apoi returnam coada;
	coada = addCoada(coada,cale,caleAbsoluta,html,1,ip);
	return coada;
}

//functia care face conectarea la server
int confSock(char *Ip){
	int sockfd;
	int port = HTTP_PORT;
  	struct sockaddr_in servaddr;
  	struct link elementCurent;

  	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		printf("Eroare la  creare socket.\n");
		exit(-1);
    }

    /* formarea adresei serverului */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_aton(Ip, &servaddr.sin_addr) <= 0 ) {
      printf("Adresa IP invalida.\n");
      exit(-1);
    }
    
    /*  conectare la server  */
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
      printf("Eroare la conectare\n");
      exit(-1);
    }
    return sockfd;  
}

//functia PrelucrareAHref prelucreaza stringul primit ca parametru
//si returneaza doar calea de dupa AHref
char* PrelucrareAHref(char* linie){
	char *token1 , *token2 , *PAhref;
	token1 = (char *) malloc(1400);
	token2 = (char *) malloc(1400);
	PAhref = (char *) malloc(1400);

	//ne pozitionam pe inceputul stringului a href
	PAhref = strstr(linie, "a href=");

	//luam in token1 totul de la a href pana la primul caracter '>'
	token1 = strtok(PAhref,">");

	//luam calea acum care va fi al 2 lea token
	token2 = strtok(token1,"=\"");
	token2 = strtok(NULL,"=\"");
	return token2;

}

//functia scrie intr-un fisier pagina descarcata si adauga in coada linkuri catre poze , documente
// sau alte pagini html in cazul in care -r SI/SAU -e sunt active
struct link* scrieFisier(struct link* coada  , int r_e_o){
	int sockfd;
	char *trimit , sendbuf[MAXLEN] , recvbuf[MAXLEN] , *aux , *newLink , *cale , *caleAbsoluta , *html;
	trimit = (char *) malloc(1400);
   	aux = (char *) malloc(1400);
   	struct link elementCurent;


	//scoatem primul element din coada
	elementCurent = (*coada);
	coada = scoateCoada(coada);

	chdir(elementCurent.pathParinte);
	//luam ultimul substring din stringul element.path si creem folderul
	//cu numele substringului doar daca nu se mai gaseste in pathulparintelui

	strcpy(aux,elementCurent.path);
	strcpy(aux,getLastString(aux));
	if(strstr(elementCurent.pathParinte,aux) == NULL)
		makeDirectors(aux);

	//ne conectam la server
	sockfd = confSock(elementCurent.serverIp);

	//creem fisierul html in care vom downloada pagina
	FILE* f  = fopen (elementCurent.html,"w");

	//trimit Get /cale/path HTTP/1.0
	strcpy (trimit,"GET /"); 
    strcat (trimit,elementCurent.path);
    strcat (trimit,elementCurent.html);
    strcat (trimit, " HTTP/1.0 \n\n");

    sprintf(sendbuf, trimit);
    send_command(sockfd, sendbuf);

    //cazul in care nu avem: -r -e -o
    if(r_e_o == 0 || elementCurent.nivel == 6){
  		if(coada == NULL)
  			exit(1);
  		else return coada;
    }

    //cazul in care avem -r
    if(r_e_o == 1){

    	//alocam memorie
    	newLink = (char *) malloc(1400);
    	cale = (char *) malloc(1400);
    	caleAbsoluta = (char *) malloc(1400);
    	html = (char *) malloc(1400);

    	//retinem calea absoluta
		strcpy(caleAbsoluta,getCwd());
    	while (Readline(sockfd, recvbuf, MAXLEN -1) > 0){
    		fprintf(f,"%s\n",recvbuf);
    		strcpy(newLink , recvbuf);
    		//verificam daca linia citita contine a href si nu incepe cu http si are si o pagina html
    		if((strstr(newLink,"a href=") != NULL) && (strstr(newLink,"http://") == NULL) && isHtml(newLink) == 1){
    			strcpy(aux,newLink);
    			strcpy(newLink,PrelucrareAHref(aux));//punem in newLInk cale pana la fisierul html
    			if(strstr(newLink,"\'") == NULL ){

    				//scoatem din stringul aux doar cale pana la fisierul html apoi o concatenam cu calea parintelui
    				strcpy(aux,newLink);
    				strcpy(cale,getCale(aux));
    				if(strcmp(cale,"NULL") == 0)
    					strcpy(cale,"");
    				strcpy(aux,elementCurent.path);
    				strcat(aux,cale);
    				strcpy(cale,aux);

    				//scoatem din stringul aux doar stringul care contine .html
    				strcpy(aux,newLink);
    				strcpy(html,getLastString(aux));

    				//adaugam in coada
    				coada = addCoada(coada,cale,caleAbsoluta,html,(elementCurent.nivel + 1),elementCurent.serverIp); 
    			}
    		}

    	}
    	return coada;
    } 
}



void Download(struct link *coada , int r , int e , int o){
	struct link elementCurent;
	int sockfd;
	char *aux ;

	aux = (char *) malloc(1400);

	

	if(r == 0 && e == 0 && o == 0)
		scrieFisier(coada , 0);

	if(r == 1 && e == 0 && o == 0){
		while(coada != NULL)
			coada = scrieFisier(coada , 1);
	}
	
}

int main(int argc, char **argv) {
	char* serverCale ,*aux,*aux2;
	struct link *coada = NULL , elementCurent;
	int r = 0, e = 0 , o = 0 , i; // flaguri care-mi semnifica daca r , e sau o sunt activate.

	//cazul in care se trimit mai putine argumente decat sunt necesare rularii programului
	if (argc < 2){
    	printf("Utilizare: ./send_msg adresa_server");
    	exit(-1);
  	}

  	//asignarea flagurilor r e o
  for(i = 0 ; i < argc ; i ++){
  	if(strcmp(argv[i],"-r") == 0)r = 1;
  	if(strcmp(argv[i],"-e") == 0)e = 1;
  	if(strcmp(argv[i],"-o") == 0)o = 1;
  }

  //alocam memorie si retinem argv[1] in serverCale
  serverCale = (char*) malloc (3 + strlen(argv[1]));
  aux = (char*) malloc (3 + strlen(argv[1]));
  strcpy(serverCale,argv[1]);

  //initializam coada
  coada = init(coada,serverCale);


  Download(coada , r , e , o);

  return 0;
}