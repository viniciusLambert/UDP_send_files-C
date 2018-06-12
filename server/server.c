//Trabalho de REDES - T2
//Tentativa P2P torrent
//Caique Duarte - 2016001648
//Vinicius Lambert - 2016007051

  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <stdio.h>
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <string.h>

  #define BUFLEN 1024
  #define NPACK 10
  #define PORT 9936
  #define PORT2 9937

  #define SRV_IP "127.0.0.1"

  void diep(char *s)
  {
    perror(s);
    exit(1);
  }


FILE *arquivo_entrada;

// função para abri arquivo como leitura (binaria)
void ler_arquivo(char nome_arquivo[]){
    arquivo_entrada = fopen(nome_arquivo, "rb");
  	if(!arquivo_entrada){
  		printf("ERROR: O arquivo não existe\n");
  	}
}


int main(void)
{
    //declarações necessarias
      struct sockaddr_in serverAdress, clientAdress;
      int receivesock, sendsock, i, slen=sizeof(clientAdress), sockServ, sendbytes;
      char buf[BUFLEN],sendbuf[BUFLEN + 3];
      int completo = 0, bytesEnviados = 0,pacotesEviados = 0, bytes_restantes;

    //prepara o socket
    if ((receivesock=socket(AF_INET, SOCK_DGRAM,0))==-1)
      diep("socket");

    memset((char *) &serverAdress, 0, sizeof(serverAdress));
    //prepara o endereço
    serverAdress.sin_family = AF_INET;
    serverAdress.sin_port = htons(PORT);
    //recebe de uma porta
    serverAdress.sin_addr.s_addr = htonl(INADDR_ANY);
    //o sistema deve guardar o endereço da porta no campo _serverAdress
    //linka o socket com o usuario
    if (bind(receivesock ,(struct sockaddr *) &serverAdress, sizeof(serverAdress))==-1)
      diep("bind");

    //recebe o nome do arquivo no buf
    if (recvfrom(receivesock, buf, BUFLEN, 0,(struct sockaddr*) &clientAdress, &slen)==-1)
      diep("recvfrom()");
    printf("Received packet from %s:%d\nData: %s\n\n",inet_ntoa(clientAdress.sin_addr), ntohs(clientAdress.sin_port), buf);
    //fecha o socket
    close(receivesock);
    //abre o arquivo no .FILE
    ler_arquivo(buf);

    fseek(arquivo_entrada,0,SEEK_END);
    //informa o tamanho do arquivo
    long long int tamanho_arquivo = ftell(arquivo_entrada);
    rewind(arquivo_entrada);
    //printf("\n%lld\n", tamanho_arquivo);
//-------------------------------------------------------------------------------------
//preparar Envio
  //prepara o socket
    if ((sendsock=socket(AF_INET, SOCK_DGRAM, 0))==-1)
      diep("socket");
    memset((char *) &clientAdress, 0, sizeof(clientAdress));
    //prepara o endereço
    clientAdress.sin_family = AF_INET;
    clientAdress.sin_port = htons(PORT2);
    //tranforma o dado em ipv4 e aloca no endereço
    if (inet_aton(SRV_IP, &clientAdress.sin_addr)==0) {
      fprintf(stderr, "inet_aton() failed\n");
      exit(1);
    }

    memset(sendbuf,0x0,BUFLEN);
    //envia os arquivos
    while(!completo){
      //caso os dados ainda n enviado caibam em mais de um buffer
      if(bytesEnviados + BUFLEN  < tamanho_arquivo ){
        //le os dados do arquivo
        fread(&sendbuf, BUFLEN,1,arquivo_entrada);
        //flag de verificação de fim
        sendbuf[BUFLEN] = '0';
        //envia os dados do buffer
        sendbytes = sendto(sendsock, sendbuf, BUFLEN+1, 0,(struct sockaddr *) &clientAdress, sizeof(clientAdress));
      }
      //caso seja o ultimo buffer
      else{
        //verifica a quantidade de bits faltantes
        bytes_restantes = tamanho_arquivo - bytesEnviados;
        //le os bits restantes
        fread(&sendbuf, BUFLEN,1,arquivo_entrada);
        //ativa a flag de fim de envio
        sendbuf[bytes_restantes] = '1';
        sendbytes = sendto(sendsock, sendbuf, bytes_restantes + 1, 0,(struct sockaddr *) &clientAdress, sizeof(clientAdress));
        //sinaliza que o envio esta completo
        completo = 1;
      }
      bytesEnviados += sendbytes-1;
      memset(sendbuf,0x0, BUFLEN);

    }
    printf("Arquivo tranferido\n");
    return 0;
  }
