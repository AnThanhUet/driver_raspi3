#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xe9942317, "module_layout" },
	{ 0xf9a482f9, "msleep" },
	{ 0x97255bdf, "strlen" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0xa07281f, "misc_register" },
	{ 0x5f754e5a, "memset" },
	{ 0xc5850110, "printk" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0xe97c4103, "ioremap" },
	{ 0x328a05f1, "strncpy" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0xedc03953, "iounmap" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x9ec748a7, "misc_deregister" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "8F234750AB98A2744D39E4E");
