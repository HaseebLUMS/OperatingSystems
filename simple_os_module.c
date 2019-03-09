#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday{
	int day;
	int month;
	int year;
	struct list_head list;
};

LIST_HEAD(birthday_list);

void make_list(void){
	struct birthday *person, *p, *ptr;
	int i;
	//struct birthday *p;

	person = kmalloc(sizeof(*person), GFP_KERNEL);
	person->day = 2;
	person->month = 8;
	person->year = 1995;

	INIT_LIST_HEAD(&person->list);
	list_add_tail(&person->list, &birthday_list);

	printk(KERN_INFO "Creating linked list ... ");
	for(i = 1; i<= 4; i++){
		p = kmalloc(sizeof(*p), GFP_KERNEL);
		p->day = i;
		p->month = (i*i)%12 + 1;
		p->year = 1995 + i;	
		list_add_tail(&p->list, &birthday_list);
	}

	

	printk(KERN_INFO "Scanning the list...");
	list_for_each_entry(ptr, &birthday_list, list){
		printk(KERN_INFO "[day:%d, month:%d, year:%d]",ptr->day, ptr->month, ptr->year);	
	}


}


void rm_list(void){
	struct birthday *ptr, *next;
	printk(KERN_INFO "Deleting the linked list and freeing the memory...");
	list_for_each_entry_safe(ptr, next, &birthday_list, list){
		printk(KERN_INFO "Deleting [day:%d, month:%d, year:%d] ... ",ptr->day, ptr->month, ptr->year);
		list_del(&ptr->list);
		kfree(ptr);	
	}	
	printk(KERN_INFO "All the allocated memory freed.");	
}
/* This function is called when the module is loaded. */
int simple_init(void)
{
       printk(KERN_INFO "Loading Module... \n");
       make_list();
       printk(KERN_INFO "Module Loaded.\n");
       
	
       return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
	printk(KERN_INFO "Removing Module\n");
	rm_list();
	printk(KERN_INFO "Module Removed!");
}

/* Macros for registering module entry and exit points. */
module_init( simple_init );
module_exit( simple_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

