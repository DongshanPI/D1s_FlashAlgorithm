CPRE := @
ifeq ($(V),1)
CPRE :=
VERB := --verbose
endif

MK_GENERATED_IMGS_PATH:=generated

.PHONY:startup
startup: all

all:
	$(CPRE) scons $(VERB) -j4
	@echo YoC SDK Done
	cp yoc.elf d1_flash.elf

.PHONY:flash
flash:
	$(CPRE) product flashbin -w boot.bin 0x10000 -f d1_flash.elf -x gdbinit -v 

.PHONY:install
install:
	@echo install boot
	$(CPRE) cp boot.bin ../../boards/d1_evb/bootimgs/boot
	$(CPRE) cp boot.elf ../../boards/d1_evb/bootimgs/boot.elf

.PHONY:clean
clean:
	$(CPRE) rm yoc_sdk boot.* yoc.* out -rf
	$(CPRE) scons -c
	$(CPRE) find . -name "*.[od]" -delete
	$(CPRE) rm -rf generated
