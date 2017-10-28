.PHONY: clean All

All:
	@echo "----------Building project:[ MeshNetwork - Release ]----------"
	@cd "MeshNetwork" && "$(MAKE)" -f  "MeshNetwork.mk"
	@echo "----------Building project:[ LogicalLayer - Release ]----------"
	@cd "LogicalLayer" && "$(MAKE)" -f  "LogicalLayer.mk"
	@echo "----------Building project:[ playground - Release ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk"
clean:
	@echo "----------Cleaning project:[ MeshNetwork - Release ]----------"
	@cd "MeshNetwork" && "$(MAKE)" -f  "MeshNetwork.mk"  clean
	@echo "----------Cleaning project:[ LogicalLayer - Release ]----------"
	@cd "LogicalLayer" && "$(MAKE)" -f  "LogicalLayer.mk"  clean
	@echo "----------Cleaning project:[ playground - Release ]----------"
	@cd "playground" && "$(MAKE)" -f  "playground.mk" clean
