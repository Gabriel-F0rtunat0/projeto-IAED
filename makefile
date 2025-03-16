# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wno-unused-result -std=c11
DEBUG_FLAGS = -g
SANITIZE_FLAGS = -fsanitize=address
RELEASE_FLAGS = -O3

# Nome do executável
TARGET = proj

# Diretório de testes
TEST_DIR = testes_publicos

# Fontes e objetos
SRCS = commands.c data_structures.c date_utils.c inoculation.c \
       main.c memory_manager.c vaccine.c
OBJS = $(SRCS:.c=.o)

# Geração da lista de testes formatados (test01 a test46)
TEST_NUMBERS = $(shell seq -f "%02g" 1 46)
TESTS = $(addprefix test,$(TEST_NUMBERS))

.PHONY: all debug sanitize test valgrind clean $(TESTS)

# Compilação padrão (release)
all: $(TARGET)

# Compilação com símbolos de debug
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Compilação com sanitizers
sanitize: CFLAGS += $(DEBUG_FLAGS) $(SANITIZE_FLAGS)
sanitize: LDFLAGS += $(SANITIZE_FLAGS)
sanitize: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para testar todos os casos
test: $(TESTS)
	@echo "Todos os testes concluídos!"

# Regra genérica para cada teste
$(TESTS): $(TARGET)
	@echo "Executando $@..."
	@./$(TARGET) < $(TEST_DIR)/$@.in > $@.myout 2>&1 || true
	@diff -q $(TEST_DIR)/$@.out $@.myout >/dev/null && \
	  echo "$@: PASS" || (echo "$@: FAIL"; exit 1)

# Testar com valgrind
valgrind: debug
	valgrind --leak-check=full --track-origins=yes ./$(TARGET) < $(TEST_IN)

# Limpar arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET) *.myout

# Dependências dos headers
commands.o: commands.h data_structures.h vaccine.h inoculation.h date_utils.h
data_structures.o: data_structures.h
date_utils.o: date_utils.h
inoculation.o: inoculation.h data_structures.h date_utils.h
main.o: commands.h data_structures.h
memory_manager.o: memory_manager.h data_structures.h vaccine.h inoculation.h
vaccine.o: vaccine.h data_structures.h date_utils.h