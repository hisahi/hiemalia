include $(addsuffix /Makefile.inc, $(addprefix base/, $(HBACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(VBACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(ABACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(IBACKEND)))

OBJS := \
	$(addsuffix /hbase.o, $(addprefix base/, $(HBACKEND))) base/hbase.o \
	$(addsuffix /vbase.o, $(addprefix base/, $(VBACKEND))) base/vbase.o \
	$(addsuffix /abase.o, $(addprefix base/, $(ABACKEND))) base/abase.o \
	$(addsuffix /ibase.o, $(addprefix base/, $(IBACKEND))) base/ibase.o \
	

