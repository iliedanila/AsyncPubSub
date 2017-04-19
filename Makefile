.PHONY: clean All

All:
	@echo "----------Building project:[ nodeLib - Debug ]----------"
	@cd "nodeLib" && "$(MAKE)" -f  "nodeLib.mk"
clean:
	@echo "----------Cleaning project:[ nodeLib - Debug ]----------"
	@cd "nodeLib" && "$(MAKE)" -f  "nodeLib.mk" clean
