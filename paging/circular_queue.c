#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

struct circularqueue *head;
struct circularqueue *tail;

void init() {
    STATWORD ps;
    disable(ps);
    head = NULL;
    tail = NULL;
    restore(ps);
}
void init_data(circularqueue *framed,int id) {
    STATWORD ps;
    disable(ps);
    framed->frametab_id = id;
    framed->next = NULL;
    restore(ps);
}

void enqueue(int frame_id) {
    STATWORD ps;
    disable(ps);
    int sizeQueue=sizeof (struct circularqueue);
    struct circularqueue *framed = (struct circularqueue*) getmem(sizeQueue);
    init_data(&framed,frame_id);
    if (head == NULL && tail == NULL) {
        head = tail= framed;
    } else {
        tail->next = framed;
        tail = framed;
        tail->next=head;
    }

    restore(ps);
}

int delete_frame(circularqueue *previous, int id) {
    STATWORD ps;
    disable(ps);
    int sizeQueue=sizeof (struct circularqueue);
    struct circularqueue *current = (struct circularqueue*) getmem(sizeQueue);

    if (head != NULL) {
      if(previous ==NULL){ // if the head is to be removed
        current= head;    
        if(current->frametab_id == id){
          head= head->next;
          freemem(current, sizeQueue);
        }
      }
      else{
            current = previous->next;
          if(current->frametab_id == id){
            previous->next=current->next;
            freemem(current, sizeQueue);
          }
          else{
          //  freemem(current, sizeQueue);
            restore(ps);
            return SYSERR;
          }


      }
      restore(ps);
      return OK;
    } else {
        restore(ps);
        return SYSERR;
    }
}
