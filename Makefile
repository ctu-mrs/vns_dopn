
# Date:      2011/07/11 17:55
# Author:    Jan Faigl
#


include Mk/libs.mk
include Mk/flags.mk

OBJS=coords.o MathCommon.o HeuristicTypes.o DatasetLoader.o dopn.o gui.o vns_dopn.o main.o

TARGET=main
SUBDIRS=opendubins
OBJ_DIR=obj

include Mk/comrob.mk
