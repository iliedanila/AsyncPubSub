.PHONY: clean All

All:
	@echo "----------Building project:[ startNode - Debug ]----------"
	@cd "startNode" && "$(MAKE)" -f  "startNode.mk"
clean:
	@echo "----------Cleaning project:[ startNode - Debug ]----------"
	@cd "startNode" && "$(MAKE)" -f  "startNode.mk" clean
