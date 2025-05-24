#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/capability.h>
#include <linux/uaccess.h>
#include <linux/pid.h>

#define SEC_sword (1 << 0)
#define SEC_midnight (1 << 1)
#define SEC_clamp (1 << 2)
#define SEC_duty (1 << 3)
#define SEC_isolate (1 << 4)

u8 charToBit(char c){
	if(c == 's') return SEC_sword;
	else if (c == 'm') return SEC_midnight;
	else if (c == 'c') return SEC_clamp;
	else if (c == 'd') return SEC_duty;
	else if (c == 'i') return SEC_isolate;
return 0;
}

asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_sec(int sword, int midnight, int clamp, int duty, int isolate){
	if(sword < 0 || midnight < 0 || clamp < 0 || duty < 0 || isolate < 0) return -EINVAL;
	if(!capable(CAP_SYS_ADMIN)) return -EPERM;
	current->clearance_flags = (sword ? SEC_sword : 0) | (midnight ? SEC_midnight : 0)
	| (clamp ? SEC_clamp : 0) | (duty ? SEC_duty : 0) | (isolate ? SEC_isolate : 0);
return 0;
}
asmlinkage long sys_get_sec(char clr){
	if(!(clr == 's' || clr == 'm' ||clr == 'c' ||clr == 'd' ||clr == 'i')) return -EINVAL;
	u8 toCheck = charToBit(clr);
	return (current->clearance_flags & toCheck) ? 1 : 0;
}
asmlinkage long sys_check_sec(pid_t pid, char clr){
	if(!(clr == 's' || clr == 'm' ||clr == 'c' ||clr == 'd' ||clr == 'i')) return -EINVAL;
	struct task_struct* targetProcess = find_task_by_vpid(pid);
	if(!targetProcess) return -ESRCH;
	u8 toCheck = charToBit(clr);
	if(!(current->clearance_flags & toCheck)) return -EPERM;
	return (targetProcess->clearance_flags & toCheck) ? 1 : 0;
}
asmlinkage long sys_flip_sec_branch(int height, char clr){
	if(!(clr == 's' || clr == 'm' ||clr == 'c' ||clr == 'd' ||clr == 'i') || height <= 0) return -EINVAL;
	u8 toCheck = charToBit(clr);
	if(!(current->clearance_flags & toCheck)) return -EPERM;
	struct task_struct* p = current->parent;
	long numGainedClearance = 0;
	int i;
	for(i = 0; i < height && p != p->parent; ++i){
	bool didHaveClearance = p->clearance_flags & toCheck;
	p->clearance_flags ^= toCheck;
	if(!didHaveClearance) numGainedClearance++;
	p = p->parent;
	}

return numGainedClearance;
}