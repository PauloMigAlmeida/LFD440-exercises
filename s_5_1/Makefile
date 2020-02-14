export KROOT=/lib/modules/$(shell uname -r)/build

obj-m +=  s_5_1.o

s_5_1-y :=  module.o        \

allofit:  modules
modules:
	@$(MAKE) -C $(KROOT) M=$(shell pwd) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(shell pwd) modules_install
kernel_clean:
	@$(MAKE) -C $(KROOT) M=$(shell pwd) clean
clean: kernel_clean
	rm -rf   Module.symvers modules.order
