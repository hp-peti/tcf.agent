# Common paths
libtcf.baseDir := $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST)))

include $(libtcf.baseDir)../../mvt-build-sys/make/lib.mk

$(eval $(call Config_Lib,libtcf))

libtcf.includeDir := $(libtcf.baseDir)agent

