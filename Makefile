TARGET = rockstar-lexer

OUTPUT_DIR := out

INCLUDE_PATHS = 
INCLUDE_PATHS += lib/magic_enum/include

SOURCE_PATHS = 
SOURCE_PATHS += .
SOURCE_PATHS += src

CXX_FILES := $(foreach dir, $(SOURCE_PATHS), $(wildcard $(dir)/*.cpp))

build:
	@rm -rf $(OUTPUT_DIR)/*
	@mkdir -p $(OUTPUT_DIR)
	g++ -g -Wall $(CXX_FILES) -o $(OUTPUT_DIR)/$(TARGET) $(addprefix -I, $(INCLUDE_PATHS))

run:
	./$(OUTPUT_DIR)/$(TARGET)

clean:
	rm -rd $(OUTPUT_DIR)
