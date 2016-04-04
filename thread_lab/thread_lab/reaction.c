//
// Hao Wu
//

#include "pintos_thread.h"

// Forward declaration. This function is implemented in reaction-runner.c,
// but you needn't care what it does. Just be sure it's called when
// appropriate within reaction_o()/reaction_h().
void make_water();

struct reaction {
	struct lock lock;
	struct condition cond_h;
	struct condition cond_o;
	int h;
	int o;
	int release;//number of hydrogens that can be returned
};

void
reaction_init(struct reaction *reaction)
{
	lock_init(&reaction->lock);
	cond_init(&reaction->cond_h);
	cond_init(&reaction->cond_o);
	reaction->h=0;
	reaction->o=0;
	reaction->release=0;
}

void
reaction_h(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	reaction->h++;
	// when there're two hydrogens, let the oxygen to react
	if (reaction->h>1){
		cond_signal(&reaction->cond_o, &reaction->lock);
	}
	// wait when there's no oxygen 
	while(reaction->release==0){
		cond_wait(&reaction->cond_h, &reaction->lock);
	}
	reaction->release--;

	lock_release(&reaction->lock);
	return;
}

void
reaction_o(struct reaction *reaction)
{
	lock_acquire(&reaction->lock);
	reaction->o++;

	// wait when there's no 2 hydrogens yet
	while(reaction->h<2){
		cond_wait(&reaction->cond_o, &reaction->lock);
	}
	// when there's an oxygen, signal 2 hydrogens that are waiting
	cond_signal(&reaction->cond_h, &reaction->lock);
	cond_signal(&reaction->cond_h, &reaction->lock);

	make_water();
	reaction->o --;
	reaction->h =reaction->h-2;
	reaction->release = reaction->release +2;


	lock_release(&reaction->lock);
	return;
}