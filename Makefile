.PHONY: clean All

All:
	@echo "----------Building project:[ playground - Release ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk"
clean:
	@echo "----------Cleaning project:[ playground - Release ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk" clean
