noinst_DATA = apache_build_instruction

apache_build_instruction: p3runConfigure
	-chmod +x p3runConfigure
	-cp -f p3runConfigure $(APACHE13)
	@echo "******************************************************"
	@echo "to build apache with parser3 module:"
	@echo
	@echo "1. cd $(APACHE13)"
	@echo
	@echo "2. ./p3runConfigure [configure options]"
	@echo
	@echo "3. make"
	@echo "******************************************************"

EXTRA_DIST = ApacheModuleParser3.dsp
