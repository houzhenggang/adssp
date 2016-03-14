VERSION  = 1
REVISE   = 0
BUILD    = $(shell git log | grep -cE 'Author:.*<.*@.*>')
BUILDSHA = $(shell git rev-parse --short HEAD)
PLATFORM_VERSION = "$(VERSION)$(if $(REVISE),.$(REVISE)$(if $(BUILD),.$(BUILD)$(if $(BUILDSHA),.$(BUILDSHA))))"
TIME     = $(shell date)

all:
	@echo "Naga:: Compile time: ${TIME}"
	@echo "Naga:: Current platform version: ${PLATFORM_VERSION}"
	@echo "#ifndef  __NAGA_VERSION_H__" > ./naga_version.h
	@echo "#define __NAGA_VERSION_H__" >> ./naga_version.h
	@echo "#define NAGA_COMPILE_TIME \"$(TIME)\"" >> ./naga_version.h
	@echo "#define NAGA_VERSION \"$(PLATFORM_VERSION)\"" >> ./naga_version.h
	@echo "#endif /* end of__NAGA_VERSION_H__ */" >> ./naga_version.h

clean:
	@rm -rf naga_version.h
