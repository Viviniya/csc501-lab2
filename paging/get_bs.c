#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) {

  /* requests a new mapping of npages with ID map_id */
  STATWORD ps;
  disable(ps);
  if((bs_id < 0 || bs_id > TOTAL_BACK_STORES-1) || npages <1 || npages >256 || bsm_tab[bs_id].bs_sem ==1  ){
    return SYSERR;
  }
  if(bsm_tab[bs_id].bs_status == BSM_MAPPED)
  {
    restore(ps);
    return bsm_tab[bs_id].bs_npages;

  }
  else{
    bsm_tab[bs_id].bs_status =BSM_MAPPED;
    bsm_tab[bs_id].bs_pid=currpid;
    bsm_tab[bs_id].bs_sem=0;
    bsm_tab[bs_id].bs_npages = npages;
    restore(ps);
    return npages;
  }

  restore(ps);
  return SYSERR;
  }
