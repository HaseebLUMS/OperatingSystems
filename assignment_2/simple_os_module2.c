#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched.h>









				/**********************************************
				  Currently, traversal is by BFS. For linear
				  traversal, de-comment the line linear_traversal
				  in main and comment the line of bfs_traversal.
		
				************************************************/

struct node{
	struct task_struct *tmp;
	int ignore;
	int num;
	struct list_head list;
};
/*birthday -> node*/
/*birthday_list -> node_list*/

int counter = 0;
LIST_HEAD(node_list);

void make_list(void){
	struct node *person;
	//struct birthday *p;

	person = kmalloc(sizeof(*person), GFP_KERNEL);
	person ->tmp = NULL;
	person ->ignore = 1;
	person ->num = counter;
	counter++;
	INIT_LIST_HEAD(&person->list);
	list_add_tail(&person->list, &node_list);
}


void add_element(struct task_struct* ptr){
	struct node *ele;
	ele = kmalloc(sizeof(*ele), GFP_KERNEL);
	ele -> tmp = ptr;
	ele -> ignore = 0;
	ele -> num = counter; counter++;
	list_add_tail(&ele ->list, &node_list);
	
}

void rm_element(void){
	struct node *ptr, *next;
	//printk(KERN_INFO "Deleting the linked list and freeing the memory...");
	list_for_each_entry_safe(ptr, next, &node_list, list){
		if(ptr -> ignore == 0){
			//printk(KERN_INFO "ignoring %d", ptr->num);
			ptr -> ignore = 1;
			break;
		}	
	}
}

int get_size(void){
	struct node *ptr, *next;
	//struct task_struct* ret;
	int total_not_ignored = 0;

	list_for_each_entry_safe(ptr, next, &node_list, list){
		if(ptr -> ignore == 0){
			total_not_ignored++;
		}	
	}
	return total_not_ignored;
}


struct task_struct* front_element(void){
	struct node *ptr, *next;
	struct task_struct* ret;
	

	list_for_each_entry_safe(ptr, next, &node_list, list){
		if(ptr -> ignore == 0){
			//printk(KERN_INFO "ignoring %d", ptr->num);
			ret = ptr-> tmp;
			break;
		}else{
			ret = ret;
		}	
	}
	return ret;
}



void rm_list(void){
	struct node *ptr, *next;
	printk(KERN_INFO "Deleting the linked list and freeing the memory...");
	list_for_each_entry_safe(ptr, next, &node_list, list){
		//printk(KERN_INFO "Del.");
		list_del(&ptr->list);
		kfree(ptr);	
	}	
	printk(KERN_INFO "All the allocated memory freed.");	
}







			/********************************************
			 	ISS SE OPER KHATRA LAHIQ HE
			*********************************************/











void traverse_bfs(void){
	//struct task_struct* p = front_element();
	//printk(KERN_INFO "name: %s, status: %ld, id: %d", p->comm, p->state, p->pid);
	

	while(get_size() > 0){
		struct list_head *list;
		struct task_struct* successor;
		struct task_struct* ptr = front_element();
		if(ptr == NULL){
			printk(KERN_INFO "(i) Terminated process in queue.");		

		}else{
			printk(KERN_INFO "name: %s, status: %ld, id: %d", ptr->comm, ptr->state, ptr->pid);
		}

		
		rm_element(); /*deletes element from queue by turning ignore flag ON*/

		


		list_for_each(list, &ptr->children){
			successor = list_entry(list, struct task_struct, sibling);
			add_element(successor);	/*functions adds the successor to queue*/
		}
	
	}
	

}




void traverse_linearly(void){
	struct task_struct *ptr;
	for_each_process(ptr){
		printk(KERN_INFO "name: %s, state: %ld, id: %d", ptr->comm, ptr->state, ptr->pid);
	}
}




/* This function is called when the module is loaded. */
int simple_init(void)
{
       printk(KERN_INFO "Loading Module... \n");

       //linear traversal
	//traverse_linearly();

       //traverse by BFS
       make_list();
       add_element(&init_task);
       traverse_bfs();
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

