/**
 *  创建一个新的kset 以及 一个kobject 
 *  参考drivers/base/core.c
**/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/err.h>


static struct kset *test_kset;
static struct kobject *test_kobject;

static int __init test_init(void)
{
	int ret = 0;
	
	/* 1.建立一个kset 并且注册到内核中去 */
	test_kset = kset_create_and_add("test", NULL, NULL); /* 第二个参数 NULL 表示当前kset对uevent上报不进行过滤， 
															第三个参数 NULL 表示当前kset结构中的成员koject无父节点
														    此函数等价于 kset_create  和  kset_register	
														 */
	if(IS_ERR(test_kset)){
		ret = PTR_ERR(test_kset);
		return ret;
	}
	
	/* 2.建立一个koject */
	
	test_kobject = kobject_create_and_add("abc", &test_kset->kobj);
	if(!test_kobject){
		goto err_create_test_kobject;
	}
		
	/* 疑问：1、只把koject的parent指向 kset的koject (可以关联abc文件夹和tes文件夹) 
	 *			test_kobject = kobject_create_and_add("abc", &test_kset->kobj);
	 *      
	 *	2、只把Koject的kset指向kset (无法关联abc文件夹和tes文件夹) 
	 *				test_kobject = kobject_create_and_add("abc", NULL);
	 *				test_kobject->kset = test_kset->kobj.kset;		
	 */	
	
	printk("test_kset:%p , test_kset->kobj.kset:%p\n", test_kset, test_kset->kobj.kset);
	
	/**
	 *  [ 9621.312379] test_kset:ffffffc019d686c0 , test_kset->kobj.kset:          (null)
	 *	test_kset->kobj.kset在 kset_create创建时初始化为NULL
	 **/
	
	// test_kobject->kset = test_kset->kobj.kset;// error  est_kset->kobj.kset = NULL
	test_kobject->kset = test_kset;/* 指定test_kobject属于test_kset */
	
	return 0;
	
err_create_test_kobject:
	kset_unregister(test_kset);
	
	return -ENOMEM;
}

static void __exit test_exit(void)
{
	kobject_put(test_kobject);
	kset_unregister(test_kset);
}

module_init(test_init);
module_exit(test_exit)

MODULE_LICENSE("GPL");
