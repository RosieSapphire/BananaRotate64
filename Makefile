BUILD_DIR := build
include $(N64_INST)/include/n64.mk

TARGET := bananarotate
ROM := $(TARGET).z64
ELF := $(BUILD_DIR)/$(TARGET).elf
DFS := $(BUILD_DIR)/$(TARGET).dfs
CFLAGS := -Wall -Wextra -Werror -pedantic -Os

SRC_DIRS := .
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
O_FILES := $(C_FILES:%.c=$(BUILD_DIR)/%.o)

ASSETS_PNG := $(wildcard assets/*.png)
ASSETS_TTF := $(wildcard assets/*.ttf)
ASSETS_CONV := $(ASSETS_PNG:assets/%.png=filesystem/%.sprite) \
	       $(ASSETS_TTF:assets/%.ttf=filesystem/%.font64)

MKSPRITE_FLAGS := --compress 2
MKFONT_FLAGS := --compress 2 --monochrome --size 16

final: $(ROM)
$(ROM): N64_ROM_TITLE="Banana Rotate"
$(ROM): $(DFS) 
$(DFS): $(ASSETS_CONV)
$(ELF): $(O_FILES)

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -o filesystem "$<"

filesystem/%.font64: assets/%.ttf
	@mkdir -p $(dir $@)
	@echo "    [FONT] $@"
	@$(N64_MKFONT) $(MKFONT_FLAGS) -o filesystem "$<"

clean:
	rm -rf $(BUILD_DIR) $(ROM)

format: $(C_FILES)
	clang-format --style=file -i $^
