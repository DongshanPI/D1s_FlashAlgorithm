.PHONY: clean All Project_Title chip_d1 csi Project_Build

All: Project_Title chip_d1 csi Project_Build

Project_Title:
	@echo "----------Building project:[ d1_flashelf - BuildSet ]----------"

chip_d1:
	@make -r -f Obj/BuildSet/Packages/chip_d1/flashelf/Makefile -j 12 -C  ./ 

csi:
	@make -r -f Obj/BuildSet/Packages/csi/v7.5.0/Makefile -j 12 -C  ./ 


Project_Build:
	@make -r -f d1_flashelf.mk -j 12 -C  ./ 


clean:
	@echo "----------Cleaning project:[ d1_flashelf - BuildSet ]----------"

