include $(addsuffix /Makefile.inc, $(addprefix base/, $(HBACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(VBACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(ABACKEND)))
include $(addsuffix /Makefile.inc, $(addprefix base/, $(IBACKEND)))

OBJS := \
	$(addsuffix /hbasei.o, $(addprefix base/, $(HBACKEND))) base/hbase.o \
	$(addsuffix /vbasei.o, $(addprefix base/, $(VBACKEND))) base/vbase.o \
	$(addsuffix /abasei.o, $(addprefix base/, $(ABACKEND))) base/abase.o \
	$(addsuffix /ibasei.o, $(addprefix base/, $(IBACKEND))) base/ibase.o \
	

