//
// HAO WU
// 

#include "pintos_thread.h"

struct station {
	struct lock lock;
	struct condition cond_t;
	struct condition cond_p;
	int isTrain;// if the train is in station, isTrain =1 and else =0
	int waitP; // number of waiting passengers
	int boardP; // number of boarding passengers
	int seats; // number of available seats 
};

void
station_init(struct station *station)
{
	lock_init(&station->lock);
	cond_init(&station->cond_t);
	cond_init(&station->cond_p);
	station->isTrain=0;
	station->waitP=0;
	station->boardP=0;
	station->seats=0;
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(&station->lock);
	station->isTrain=1;
	station->seats=count;
	// wait when the train is not full and there're boarding passengers
	while ((station->seats !=0 && station->waitP !=0 )||
		(station->boardP !=0)) {
		//puts("train waits\n");
		//printf("waitP is %d in \n", station->waitP);
		//printf("boardP is %d in \n", station->boardP);
		//printf("seats is %d in \n", station->seats);
		cond_broadcast(&station->cond_p, &station->lock);
		cond_wait(&station->cond_t, &station->lock);
		//puts("end waits\n");
	}

	station->isTrain =0;
	lock_release(&station->lock);
	return;
}	

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&station->lock);
	station->waitP++;
	// wait when the train is not in the station, 
	//or there're no free seats in the train
	while ((station->isTrain ==0 )|| 
		(station->isTrain ==1 && station->seats ==0)) {
		cond_wait(&station->cond_p, &station->lock);
	} 
	station->waitP--;
	station->seats--;
	station->boardP ++;

	lock_release(&station->lock);
	return;
}

void
station_on_board(struct station *station)
{
	lock_acquire(&station->lock);
	station->boardP --;
	// the train is satisfactorily loaded, 
	// when the train is full, or there're no waiting passengers
	if ((station->seats ==0 || station->waitP ==0) 
		&& (station->boardP ==0)) {
		cond_signal(&station->cond_t, &station->lock);
	}
	lock_release(&station->lock);
	return;
}