LibName = libtcf
SourcesDir = agent
IncludeDir = 

# Uncomment to see commands
# ECHO=

include ../config/config.mk

# We do not include the common compiler flags
# include $(CompilerFlags.mk)

compilerFlags.c = -w -fPIC

include $(Default.mk)

# We only need to compile `.c` files
SourceExt = c

# ------------------
# Internal variables
# ------------------

MoviTcfAgentDir = ../MoviTcfAgent

AgentFiles=services.c cmdline.c server.c framework.c

TcfMainFiles := $(addprefix tcf/main/,$(AgentFiles))
TcfFrameworkFiles := tcf/framework/*.c
TcfServiceFiles := tcf/services/*.c

LibTcfFiles := $(TcfMainFiles) $(TcfServiceFiles) $(TcfFrameworkFiles)

# TCF extensibility logic: we exclude source files already existent in the MoviTcfAgent
AllSources := $(call SourceWildcard,$(LibTcfFiles))
ExcludeSources := $(call SourceWildcard,$(LibTcfFiles),$(MoviTcfAgentDir))

# -----------------------------
# Build system config variables
# -----------------------------

Sources := $(filter-out $(ExcludeSources),$(AllSources))

# First include must come from MoviTcfAgent
# where we have the correct tcf/config.h
includes += -I$(MoviTcfAgentDir) -Iagent

# -------------------------------------------
# Uncomment these to check proper source list
#$(call PrintVariable,AllSources)
#$(call PrintVariable,ExcludeSources)
#$(call PrintVariable,Sources)
# -------------------------------------------

include $(Objects.mk)

#$(call PrintVariable,Objects.Debug)

include $(LibConfig.mk)

include $(ArchiveLib.mk)
include $(Clean.mk)

include $(BuildLibs.mk)
include $(Compile.mk)

# Uncomment to verify includes
#$(call PrintVariable,includes)
