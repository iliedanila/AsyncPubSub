.PHONY: clean All

All:
	@echo "----------Building project:[ playground - Debug ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk"
clean:
	@echo "----------Cleaning project:[ playground - Debug ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk" clean
