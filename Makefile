# ========================
#  Kompilyator va flaglar
# ========================
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11

SRC1 = cipher.c
SRC2 = logger.c
OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)

BUILD_DIR = ../build
Q1 = $(BUILD_DIR)/cipher
Q2 = $(BUILD_DIR)/logging_cipher

TXT_FILES = text.txt abc.txt love.txt test.txt

.PHONY: all clean rebuild cppcheck style valgrind_all cipher logging_cipher test

# ========================
#  Default
# ========================
all: cipher logging_cipher

# ========================
#  Quest 1 - cipher
# ========================
cipher: $(OBJ1) $(OBJ2)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(Q1) $(OBJ1) $(OBJ2)

# ========================
#  Quest 2 - logging_cipher
# ========================
logging_cipher: $(OBJ1) $(OBJ2)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -DLOG -o $(Q2) $(SRC1) $(SRC2)

# ========================
#  Static code checks
# ========================
cppcheck:
	@echo "🧠 Running cppcheck..."
	cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem .

style:
	@echo "✨ Auto-formatting code..."
	clang-format -i *.c *.h
	@echo "✅ Style fixed."

# ========================
#  Valgrind test
# ========================
valgrind_all: all
	@echo "🧩 Running valgrind tests..."
	valgrind --leak-check=full $(Q1)
	valgrind --leak-check=full $(Q2)

# ========================
#  Test fayllarni yaratish
# ========================
test:
	@for f in $(TXT_FILES); do \
		if [ ! -f $$f ]; then \
			echo "Yaratilmoqda: $$f"; \
			case $$f in \
				text.txt) echo "Aloha!" > $$f ;; \
				abc.txt) echo "Hihi_haha" > $$f ;; \
				love.txt) echo "i_love_sh21" > $$f ;; \
				test.txt) echo "I_love" > $$f ;; \
			esac; \
		else \
			echo "Mavjud: $$f"; \
		fi; \
	done

# ========================
#  Tozalash
# ========================
clean:
	rm -f *.o
	rm -f *.txt
	rm -rf $(BUILD_DIR)

rebuild: clean all
