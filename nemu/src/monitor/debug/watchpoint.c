#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "monitor/monitor.h"
#include "cpu/reg.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp(){
	if(free_ == NULL){
		printf("There are no free watchpoints.\n");
		return NULL;
	}
	WP *new_watchpoint = free_;
	free_ = free_->next;
	new_watchpoint->next = head;
	head = new_watchpoint;

	return new_watchpoint;
}

void free_wp(int number){
	WP *pre = head;
	WP *cur = head->next;
	if(pre->NO == number){
		head = cur;
	}
	else{
		while(cur != NULL && cur->NO != number){
			pre = cur;
			cur = cur->next;
		}
		if(cur == NULL){
			printf("There is no watchpoint with number %d\n", number);
			return; 
		}
		pre->next = cur->next;
		cur->next = free_;
		free_ = cur;
	}
	return;
}

void check_wp(){
	WP *p = head;
	while(p!=NULL){
		bool success;
		uint32_t curv = expr(p->expr, &success);
		if(curv != p->value){
			//change
			nemu_state = NEMU_STOP;
			printf("Triggered watchpoint %d: %s\nold value: %d\nnew value: %d\n", p->NO, p->expr, p->value, curv);
			p->value = curv;
			//return;
		}
		p = p->next;
	}

}

void print_wp(){
	WP *p = head;
	if(p==NULL){
		printf("There are no watchpoints.\n");
		return;
	}
	while(p!=NULL){
		printf("%d %s %u\n", p->NO, p->expr, p->value);
		p = p->next;
	}
}
