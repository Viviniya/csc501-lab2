/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm()
{
  STATWORD ps;
  disable(ps); //disabling interrupts during initialize
  int frame_tab;
  for(frame_tab=0; frame_tab < NFRAMES; frame_tab++){
    initialize_frames(frame_tab);
  }

  restore(ps); //enabling interrupts after initialize
  return OK;
}
void initialize_backstore(int frame_tab){
  frm_tab[frame_tab].fr_status = FRM_UNMAPPED;
	frm_tab[frame_tab].fr_pid = -1001;
  frm_tab[frame_tab].fr_vpno = 0;
  frm_tab[frame_tab].fr_refcnt = 0; //?
	frm_tab[frame_tab].fr_type = FR_INIT;  // INIT as not belonging to any section
  frm_tab[frame_tab].fr_dirty= 0;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail)
{
  kprintf("To be implemented!\n");
  return OK;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
  STATWORD ps;
  disable(ps);
	pd_t *pd_entry;
	pt_t *pt_entry;
  if(frm_tab[i].fr_type == FR_PAGE){
    int frame_id=frm_tab[i].fr_pid;
    int backstore_id= proctab[frame_id].store;
    unsigned long v_addr = frm_tab[i].fr_vpno;
    unsigned int pd_index= v_addr / NFRAMES;
    unsigned int pt_index= v_addr & 1023;
    unsigned long pdbr = proctab[frame_id].pdbr;
    pd_entry = (pd_index*sizeof(pd_t)) + pdbr;
		pt_entry = (pd_entry->pd_base*NBPG) + (pt_offset*sizeof(pt_t));
    int page_id=frm_tab[i].fr_vpno-proctab[frame_id].vhpno;
    write_bs((FRAME0+i)*NBPG, backstore_id, page_id);
    frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt -= 1;
    int ref_count=frm_tab[pd_entry->pd_base-FRAME0].fr_refcnt;
    pt_entry->pt_pres = 0;
    if(ref_count<=0)
      reset_pagetable(&pd_entry);
  }
  else{
    kprintf("Cannot free any frame other than page!\n");
  }

  return OK;
}
 void reset_pagetable(pd_t *pd_entry){
   int i=pd_entry->pd_base-FRAME0;
   frm_tab[i].fr_status = FRM_UNMAPPED;
   frm_tab[i].fr_pid = -1001;
	 frm_tab[i].fr_type = FR_INIT;
	frm_tab[i].fr_vpno = 0;
	pd_entry->pd_pres = 0;
 }
