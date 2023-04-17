#
# Date:      2013/06/10 15:21
# Author:    Jan Faigl
#

#CXX:=ccache $(CXX)

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

CXXFLAGS:=$(call uniq,$(CXXFLAGS))

bin: $(TARGET)

$(OBJS): %.o: source/%.cpp
	$(CXX) -c $< $(CXXFLAGS) $(CPPFLAGS) -o $(OBJ_DIR)/$@

$(TARGET): create_directories  obj_subdirs $(OBJS) 
	$(CXX) -o $@ $(OBJ_DIR)/*.o $(LDFLAGS)


all_subdirs:    
	echo $(OPSYS)
	@for i in $(SUBDIRS) ;\
	do \
	echo "making" all "in $(CURRENT_DIR)/$$i..."; \
	$(MAKE) -C $$i all || exit 1 ;\
	done

obj_subdirs: all_subdirs
	echo "Copy objs"
	@for i in $(SUBDIRS) ;\
	do \
	$(MAKE) --ignore-errors -C $$i obj ;\
	echo "coping all in $(CURRENT_DIR)/$$i..."; \
	cp $$i/*.o $(OBJ_DIR)/; \
	done

create_directories:
	echo "create dircetory $(OBJ_DIR)"
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) $(TARGET)
	$(RM) $(OBJ_DIR)/*
	@for i in $(SUBDIRS) ;\
        do \
        echo "cleaning" all "in $(CURRENT_DIR)/$$i..."; \
        $(MAKE) -C $$i clean; \
        done


