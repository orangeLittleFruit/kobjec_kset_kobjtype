/**
 *  创建一个新的kset 以及 一个kobject 可以看到 /sys/test/abc 的目录结构
 *  参考drivers/base/core.c
**/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/err.h>
#include <linux/slab.h>

static struct kset *test_kset;
static struct kobject *test_kobject;

static ssize_t test_attr_show(struct kobject *kobj, struct attribute *attr,
			       char *buf)
{

	return 0 ; 
}

static ssize_t test_attr_store(struct kobject *kobj, struct attribute *attr,
				const char *buf, size_t count)
{

	return 0 ; 
}



static void test_kobj_type_release(struct kobject *kobj)
{
	kfree(kobj);
}





static const struct sysfs_ops test_sysfs_ops = {
	.show	   = test_attr_show,
	.store	   = test_attr_store,
};

static struct kobj_type test_kobj_type = {
	.sysfs_ops  = &test_sysfs_ops,  /* 必须实现 否则会报错 */
    .release    = test_kobj_type_release,
};

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
	
	/* 2.申请kobject内存 */
	
	test_kobject = (struct kobject *)kmalloc(sizeof(*test_kobject), GFP_KERNEL);
	if(!test_kobject){
		printk("malloc for test_kobject is failed! test_kobject = %p\n", test_kobject);
		goto err_malloc_test_kobject;
	}
	
	memset(test_kobject, 0, sizeof(*test_kobject)); /* 如果用kmalloc申请，必须使用 */
	
	/* 初始化kobject */
	/* void kobject_init(struct kobject *kobj, struct kobj_type *ktype) */
	
	kobject_init(test_kobject, &test_kobj_type);
	
	
	/** 以下两种方法基于 kobject_add_internal方法中
	 *	如果存在kset（即kobj->kset不为空）同时，如果该kobject没有parent，却存在kset，
     *  则将它的parent设为kset（kset是一个特殊的kobject），并增加kset的引用计数
	 **/
#if 0
	
	/* 第一种给test_kobject中的parent指针赋值方法 */
		
	test_kobject->kset = test_kset; /* 先指定test_kobject的kset */
		
	/* int kobject_add(struct kobject *kobj, struct kobject *parent,
        const char *fmt, ...)   */
	
 	ret = kobject_add(test_kobject, NULL, "abc");/* 添加kobject */
	if(ret < 0){
		printk("kobject_add test_kobject is failed!\n");
		goto err_kobject_add;
	} 

#elif 0

    /* 第二种给test_kobject中的parent指针赋值方法 就是第二个参数赋值为&test_kset->kobj */
		

	/* int kobject_add(struct kobject *kobj, struct kobject *parent,
        const char *fmt, ...)   */
	
 	ret = kobject_add(test_kobject, &test_kset->kobj, "abc");	/* 添加kobject */
	if(ret < 0){
		printk("kobject_add test_kobject is failed!\n");
		goto err_kobject_add;
	} 

#else
	
	/* 第三种 如果没有指定kobject的parent，也没有指定koject的kset，则生成的目录在sysfs的根目录下 即/sys下 */
	ret = kobject_add(test_kobject,NULL , "abc");	/* 添加kobject */
	if(ret < 0){
		printk("kobject_add test_kobject is failed!\n");
		goto err_kobject_add;
	} 

#endif
	

	return 0;
err_kobject_add:
	kobject_put(test_kobject);
err_malloc_test_kobject:
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
