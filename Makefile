.PHONY: clean All

All:
	@echo "----------Building project:[ startNode - Release ]----------"
	@cd "startNode" && "$(MAKE)" -f  "startNode.mk"
clean:
	@echo "----------Cleaning project:[ startNode - Release ]----------"
	@cd "startNode" && "$(MAKE)" -f  "startNode.mk" clean
