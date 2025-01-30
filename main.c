#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>


#define SOCKETNAME "mySocket"

//H morfi tou struct
struct catalog
{
    char description[30];
    int stock;
    float timi;
};




int main(void){
    struct sockaddr_un sa;
    (void) unlink(SOCKETNAME);
    strcpy(sa.sun_path, SOCKETNAME);
    sa.sun_family = AF_UNIX;
    int fd_skt, fd_client; char buf[100];
    int written; ssize_t readb;
    int num[20];
    int number=0;
    float clt_total,total=0.0;
    int succesful_orders=0,failed=0,check;
    int total_fails=0,orders=0;

    if(fork()==0){ //Client Process
        if ((fd_skt = socket(PF_UNIX, SOCK_STREAM, 0)) <0){
            perror("message from socket [clt] : ");
            exit(-1);
        }
        printf("[Clt]: Socket %d has been created\n ",fd_skt);
        while (connect(fd_skt, (struct sockaddr *)&sa, sizeof(sa))==-1){
            if(errno == ENOENT){
                sleep(1);
                continue;
            }else{
                perror("Message from connect [Clt]");
                exit(-2);
            }
        }
        printf("[Clt]: Connection has been established...let us write to server\n");

        for (int i = 0; i < 5; i++){ //Pelates 
            srand(time(NULL));
            for (int j = 0; j < 10; j++){
                num[j] = rand() % 20+1;
            }
            for (int j = 0; j < 10; j++){ //Epanaliptiki diadikasia gia tin epilogi ton 10 proiontwn
                written=write(fd_skt, &num[j],sizeof(int));
                if(written==-1){
                    perror("Message from [clt]");
                    exit(-3);
                }
            }
            readb=read(fd_skt,&check,sizeof(int)); //diavazoume apo to server an ontws i paraggelia mporei na oloklirwthei
            if (readb==-1){
                    perror("Message from [clt]");
                    exit(-4);
            }
            
            if (check==0){ //An to check einai 0 i paraggelia tou pelati oloklirwnetai 
                readb=read(fd_skt,&clt_total,sizeof(float));
                if(readb==-1){
                    perror("Message from read [clt]");
                    exit(-5);
                }else{
                    printf("Client [%d]: Order complete, your total is %.2f\n",i+1,clt_total);
                }
            }else{ //an oxi emfanizetai ston client minima oti i paraggelia exei apotixei
                readb=read(fd_skt,buf,sizeof(buf));
                if(readb==-1){
                    perror("Message from read [clt]");
                    exit(-6);
                }else{
                    printf("%s\n",buf);
                }
            }
        }
        //printing the stock
        for (int i = 0; i < 20; i++)
        {
            readb=read(fd_skt,buf,30);
            if (readb==-1){
                perror("Message from read [clt]");
                exit(-7);
            }else{
                printf("%s\t",buf);
            }

            readb=read(fd_skt,&number,sizeof(int));
            if (readb==-1){
                perror("Message from read [clt]");
                exit(-7);
            }else{
                printf("Stock: %d\n",number);
            }
        }

        //Emfanisi ton minimatwn pou periexoun ta telika statistika
        readb=read(fd_skt,&succesful_orders,sizeof(int));
        if (readb==-1){
            perror("Message from read [clt]");
            exit(-8);
        }else{
            printf("Clients ordered succesfully : %d - Unsuccesful client orders: %d \n",succesful_orders,5-succesful_orders);
        }
        
        readb=read(fd_skt,&total,sizeof(float));
        if (readb==-1){
            perror("Message from read [clt]");
            exit(-9);
        }else{
            printf("Total: %.2f\n",total);
        }
        
        readb=read(fd_skt,&orders,sizeof(int));
        if (readb==-1){
            perror("Message from read [clt]");
            exit(-10);
        }else{
            printf("Total orders processed succesfully: %d ",orders);
        }

        readb=read(fd_skt,&total_fails,sizeof(int));
        if (readb==-1){
            perror("Message from read [clt]");
            exit(-11);
        }else{
            printf("Unsuccesful product orders: %d\n",total_fails);
        }
    
    
    

        close(fd_skt);
        exit(0);

    }else{
        //Arxikopoisi tou catalog me proionta
        struct catalog prod[20] = {
            {"Proion 1", 2, 1.0},
            {"Proion 2", 2, 1.5},
            {"Proion 3", 2, 2.0},
            {"Proion 4", 2, 2.5},
            {"Proion 5", 2, 3.0},
            {"Proion 6", 2, 3.5},
            {"Proion 7", 2, 4.0},
            {"Proion 8", 2, 4.5},
            {"Proion 9", 2, 5.0},
            {"Proion 10", 2, 5.5},
            {"Proion 11", 2, 6.0},
            {"Proion 12", 2, 6.5},
            {"Proion 13", 2, 7.0},
            {"Proion 14", 2, 7.5},
            {"Proion 15", 2, 8.0},
            {"Proion 16", 2, 5.5},
            {"Proion 17", 2, 6.0},
            {"Proion 18", 2, 6.5},
            {"Proion 19", 2, 7.0},
            {"Proion 20", 2, 7.5},
        };
        //Diadikasia egkathidrysis syndesis me ton client
        if ((fd_skt = socket(PF_UNIX, SOCK_STREAM, 0)) <0){
            perror("message from socket [server] : ");
        }
        printf("[Clt]: Socket %d has been created\n ",fd_skt);
        if (bind(fd_skt, (struct sockaddr *)&sa, sizeof(sa))){
            perror("Message from bind [server]");
            exit(-2);
        }
        printf("[Server] ===> Socket %d has been bound to address\n",fd_skt);
        if(listen(fd_skt ,5)){
            perror("Message from listen[server]");
            exit(-3);
        }
        printf("[Server] ===> Listening for incoming messages\n");
        if ((fd_client = accept(fd_skt, NULL, 0))<0){
            perror("Message from accept [server]");
            exit(-4);
        }
        printf("[Server] ===> Let us read from client via socket %d\n-----------\n",fd_client);
        
        for (int k = 0; k < 5; k++){ //O arithmos ton pelatwn
            check=0;
            clt_total=0;
            for (int l = 0; l < 10; l++){ //Diavazetai oi tyxaies paraggelies ton pelatwn
                
                readb=read(fd_client,&number,sizeof(int));
                if(readb==-1){
                    perror("Message from read [server]");
                    exit(-5);
                }
                //elenxos gia to stock tou epilegmenou proiontos
                if (prod[number].stock>0){
                    orders++;
                    prod[number].stock--;
                    clt_total=clt_total+prod[number].timi;
                    total=total+clt_total;
                }else{
                    total_fails++;
                    check++;
                }
            }
                //apostoli tou check pros ton pelati
            written=write(fd_client,&check,sizeof(int));
                if(written==-1){
                    perror("Message from write [server]");
                    exit(-6);
                }


            if (check==0){
                succesful_orders++;
                written=write(fd_client,&clt_total,sizeof(float));
                if(written==-1){
                    perror("Message from write [server]");
                    exit(-7);
                }
            }else{
                written=write(fd_client,"Order failed, a selected product is out of stock",49);
                if(written==-1){
                    perror("Message from write [server]");
                    exit(-8);
                }
                failed++;
            }
            sleep(1);
        }
        //Apostoli sto telos to apomeinenta stock
        for (int i = 0; i < 20; i++)
        {
            written=write(fd_client,&prod[i].description,30);
            if (written==-1){
                perror("Message from read [clt]");
                exit(-13);
            }
            written=write(fd_client,&prod[i].stock,sizeof(int));
            if (written==-1){
                perror("Message from read [clt]");
                exit(-14);
            }
        }

       
        //apostoli minimaton kai telikon statistikon
        written=write(fd_client,&succesful_orders,sizeof(int));
        if(written==-1){
            perror("Message from write [server]");
            exit(-9);            
        }
        written=write(fd_client,&total,sizeof(float));
        if(written==-1){
            perror("Message from write [server]");
            exit(-10);
        }
        written=write(fd_client,&orders,sizeof(int));
        if(written==-1){
            perror("Message from write [server]");
            exit(-11);
        }
        written=write(fd_client,&total_fails,sizeof(int));
        if(written==-1){
            perror("Message from write [server]");
            exit(-12);
        }

        close(fd_skt);
        close(fd_client);
        exit(0);
    }
}



