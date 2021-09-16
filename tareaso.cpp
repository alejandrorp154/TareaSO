
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <time.h>

int flanes;
int albondigas;
int elijo;

sem_t *sem_mos;
sem_t *sem_hel;

struct pedido{
	int flanes;
	int albondigas;
};
typedef struct pedido pedido;

void cocinero(int *mos);
void repos(int *hel);
void retiro_platoal(int *mos);
void llenar_hel(int *hel);
void retiro_flanes(int *hel);
void mozo(int *hel,int *mos);
void colocar(int *mos);

int main(){
    int x,y;
	srand(time(NULL));
	sem_mos=sem_open("/semmos.txt",O_CREAT,0644,1);
	sem_hel=sem_open("/semhel.txt",O_CREAT,0644,1);
	x=sem_init(sem_mos,0,1);
	y=sem_init(sem_hel,0,1);
	printf("La heladera y el mostrador empiezan llenos\n\n");


	/*Memoria Compartida*/
	key_t clave;
	clave=ftok("/home/Escritorio/tareaso/smem.txt",25);

	int mos_id;
	mos_id=shmget (clave, 1024, 0777|IPC_CREAT);

	int hel_id;
	hel_id=shmget (clave, 1024, 0777|IPC_CREAT);

	int *mos,*hel;
	mos = (int *) shmat(mos_id, NULL, 0);
	hel = (int *) shmat(hel_id, NULL, 0);
	*mos=27;
	hel=mos-2;
	*hel=25;
	printf("msotrador empieza en %d \n",*mos);

    for(int moz=1;moz<6;moz++){
		int o=fork();
		if(o==0){
			mozo(hel,mos);
		}
	}
	for(int coci=1;coci<4;coci++){
		int c=fork();
		if(c==0){
			cocinero(mos);
		}
	}
	int r=fork();
	if(r==0){
		repos(hel);
	}
	sem_unlink("/semmos.txt");
	sem_unlink("/semhel.txt");
	return 0;
}

void cocinero(int *mos){
    while(1){
	    if(*mos<27){
	    	printf("voy a colocar platos,en el mostrador hay %d \n",*mos);
	        colocar(mos);
	        sleep(5);
    	}
    }
}
void repos(int *hel){
    while(1){
	    if(*hel<=0){
	    	printf("La heladera esta vacia,la lleno\n\n");
	        llenar_hel(hel);
	    }
	}    
}

void retiro_platoal(int *mos){
    sem_wait(sem_mos);
    if(*mos>=albondigas){
    	printf("En el mostrador habia %d platos \n",*mos);
        *mos=*mos-albondigas;
        

        printf("Retiro %d platos de albondigas,ahora hay %d \n",albondigas,*mos);
    }
    sem_post(sem_mos);
    
}

void llenar_hel(int *hel){
		
		sem_wait(sem_hel);
	    
	        *hel=25;
	        printf("soy el repostero,llene la heladera con %d flanes\n\n",*hel);
	    
		sem_post(sem_hel);
		sleep(20);
	
}
void retiro_flanes(int *hel){
    sem_wait(sem_hel);
    if(*hel>=flanes){
        *hel=*hel-flanes;
        printf("Retiro %d flanes, ahora en la heladera hay %d \n",flanes,*hel);
    }
	sem_post(sem_hel);
}
void colocar(int *mos){
	  
	    sem_wait(sem_mos);
	        printf("hay %d platos en el mostrador\n\n",*mos);
	        *mos=*mos+1;
	        
	        printf("coloco platos en el mostrador, ahora hay %d\n\n",*mos);
	    
	    sem_post(sem_mos);
	
}
pedido tomarpedido(){
	pedido x;
	x.flanes=0;
	x.albondigas=0;
	elijo=rand()%2;
	if(elijo==0){
		x.albondigas=1+(rand()%4);
		albondigas=x.albondigas;
		printf("%d plato de albondigas\n\n",x.albondigas);
	}
	else{
		x.flanes=1+(rand()%6);
		flanes=x.flanes;
		printf("%d plato de flan de coco\n\n",x.flanes);
	}
	return x;
}

void mozo(int *hel,int *mos){
	while(1){
		pedido m;
		m=tomarpedido();
		if(elijo==0){
			retiro_platoal(mos);
		}
		else{
			retiro_flanes(hel);
			
		}
		sleep(10);
    }
}
