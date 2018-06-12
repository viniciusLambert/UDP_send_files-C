//Trabalho de REDES - T2
//Servidor cliente
//Caique Duarte - 2016001648
//Vinicius Lambert - 2016007051

#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#define BUFLEN 1024
#define NPACK 10
#define PORT 9936
#define PORT2 9937
#define SRV_IP "127.0.0.1"

FILE *arquivo_entrada;

//funcao de erro
void diep(char *s)
{
  perror(s);
  exit(1);
}


//escreve no arquivo .FILE
void escrever_arquivo(char nome_arquivo[]){
    char src[10] = "_1";
    strcat(nome_arquivo, src);
  	arquivo_entrada = fopen(nome_arquivo, "wb");
  	if(!arquivo_entrada)
  		printf("ERROR: O arquivo não existe\n");
}


int main(int argc, char const *argv[])
  {
    //declaração nescessaria
    struct sockaddr_in serverAdress, clientAdress;
    struct hostent *h;
    int sendsock, receivesock, i;
    char buf[BUFLEN],recBuf[BUFLEN+3], nomeArquivo[500] ;
    int receivedBytes;
    socklen_t servLen;

    //prepara o socket
    if ((sendsock =socket(AF_INET, SOCK_DGRAM, 0))==-1)
      diep("socket");
    //aloca memoria
    memset((char *) &serverAdress, 0, sizeof(serverAdress));
    //aloca o endereço
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT);
    //tranforma o IP em ipv4 e aloca
    if (inet_aton(SRV_IP, &serverAdress.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
      exit(1);

    }
    //rescebe nome do arquivo que se deseja enviar
    printf("Nome do arquivo :\n");
    scanf("%s", nomeArquivo);
    //aloca o nome do arquivo no buffer
    sprintf(buf,"%s", nomeArquivo);
    //envia o nome para o servido
    if (sendto(sendsock, buf, BUFLEN, 0, (struct sockaddr *) &serverAdress, sizeof(serverAdress))==-1)
        diep("sendto()");


    //abre o arquivo
    escrever_arquivo(nomeArquivo);
    //fecha o socket q n será mais usado
    close(sendsock);
//----------------------------------------------------------------------------------------------
//prepara pra resceber o arquivo
  if ((receivesock=socket(AF_INET, SOCK_DGRAM,0))==-1)
    diep("socket");

    memset((char *) &serverAdress, 0, sizeof(serverAdress));
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT2);
    //recebe de uma porta
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);

    //link o endereço com o socket
    if (bind(receivesock ,(struct sockaddr *) &serverAdress, sizeof(serverAdress))==-1)
      diep("bind");

    servLen = sizeof(serverAdress);

    //loop para receber o arquivo
    while(1){
      //rescebe um apacote de arquivos
        receivedBytes = recvfrom(receivesock, &recBuf, BUFLEN+1, 0,(struct sockaddr *) &serverAdress, &servLen);
        //escreve o conteudo recebido no arquivo
        fwrite(&recBuf, sizeof(char) ,receivedBytes - 1 , arquivo_entrada);
        //caso for o ultimo pacote, encerra o codigo
        if(recBuf[receivedBytes -1 ] == '1'){
					fclose(arquivo_entrada);
					break;
				}
    }
  printf("Arquivo copiado\n");
  //fecha o socket
  close(receivesock);
  return 0;
}
