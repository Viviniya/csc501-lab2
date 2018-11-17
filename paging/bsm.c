/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm()
{
  STATWORD ps;
	disable(ps); //disabling interrupts during initialize
  int backstore_tab=0;
  for(backstore_tab=0; backstore_tab < TOTAL_BACK_STORES; backstore_tab++){
    initialize_backstore(backstore_tab);
  }

  restore(ps); //enabling interrupts after initialize
	return OK;
}
void initialize_backstore(int backstore_tab){
  bsm_tab[backstore_tab].bs_status = BSM_UNMAPPED;
	bsm_tab[backstore_tab].bs_pid = -1001;
  bsm_tab[backstore_tab].bs_vpno = 4096;
  bsm_tab[backstore_tab].bs_npages = 0;
	bsm_tab[backstore_tab].bs_sem = 0;  // using to track private heap
  bsm_tab[backstore_tab].bs_size = 0;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
  STATWORD ps;
	disable(ps); //disabling interrupts during initialize
  int backstore_tab=0;
  for(backstore_tab=0; backstore_tab < TOTAL_BACK_STORES; backstore_tab++){
    if(  bsm_tab[backstore_tab].bs_status == BSM_UNMAPPED){ //the first unmapped back store is returned
          *avail=backstore_tab;
          kprintf("Success: Backing store fetched\n");
          restore(ps); //enabling interrupts after initialize
          return OK;
    }
  }
  kprintf("ERROR: Backing Store not available\n");
  restore(ps); //enabling interrupts after initialize
  return OK;
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
  STATWORD ps;
  disable(ps); //disabling interrupts during initialize
  initialize_backstore(i); //re-initialize and free the back store
  restore(ps); //enabling interrupts after initialize
  return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
  STATWORD ps;
  disable(ps);
  int backstore_tab=0;
  for(backstore_tab=0; backstore_tab < TOTAL_BACK_STORES; backstore_tab++){
    if(  bsm_tab[backstore_tab].bs_status == BSM_MAPPED && bsm_tab[backstore_tab].bs_pid == pid ){
          *store=backstore_tab;
          int npages= vaddr/NBPG;
          *pageth=npages-bsm_tab[backstore_tab].vpno;
          kprintf("Lookup  of store :Success\n");
          restore(ps); //enabling interrupts after initialize
          return OK;
    }
  }
  kprintf("Lookup  of store :Failed\n");
  restore(ps);
  return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages, int flag)
{
  STATWORD ps;
  disable(ps); //disabling interrupts during initialize
  if(bsm_tab[source].bs_status  == BSM_MAPPED && flag == 0){
    //memory allocation
      bsm_tab[source].bs_size= bsm_tab[source].bs_size+1;
      bsm_tab[source].bs_processtab[bsm_tab[source].bs_size].pid=pid;
      bsm_tab[source].bs_processtab[bsm_tab[source].bs_size].vpno=pid;

  }
  else{
  bsm_tab[source].bs_status = BSM_MAPPED;
  bsm_tab[source].bs_pid = pid;
  bsm_tab[source].bs_vpno = vpno;
  bsm_tab[source].bs_npages = npages;
  bsm_tab[source].bs_sem = flag;  // 1 - private heap
  bsm_tab[source].bs_size =0
  bsm_tab[source].bs_processtab[0].pid=pid;
  bsm_tab[source].bs_processtab[0].vhpno=pid;
  }

  restore(ps); //enabling interrupts after initialize
  return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
  int backstore_tab=-1;
  //lookup to find the back_store to be unmapped -vivin
  initialize_backstore(backstore_tab);
}
