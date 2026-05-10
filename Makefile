CC = gcc
CFLAGS = -Wall -Wextra -g -D_GNU_SOURCE -I src/core -I src/gateway/discord -I src/cli -I library -I library/json -I library/ds -I library/llm -I library/algo -I library/browser
LDFLAGS = -lssl -lcrypto -lm

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# ── Library objects (Phase 1-3 modules, C99 pure) ──
LIB_SRC = library
LIB_OBJS = \
	$(OBJ_DIR)/alchemy_json.o \
	$(OBJ_DIR)/alchemy_llm_router.o \
	$(OBJ_DIR)/alchemy_subagent.o \
	$(OBJ_DIR)/alchemy_retry.o \
	$(OBJ_DIR)/alchemy_skill_sim.o \
	$(OBJ_DIR)/alchemy_slash.o \
	$(OBJ_DIR)/alchemy_session.o \
	$(OBJ_DIR)/alchemy_mcp.o \
	$(OBJ_DIR)/alchemy_plugin.o \
	$(OBJ_DIR)/alchemy_cost.o \
	$(OBJ_DIR)/alchemy_tui.o \
	$(OBJ_DIR)/alchemy_test.o \
	$(OBJ_DIR)/alchemy_browser.o \
	$(OBJ_DIR)/alchemy_memory_auto.o \
	$(OBJ_DIR)/alchemy_worktree.o \
	$(OBJ_DIR)/alchemy_task_queue.o \
	$(OBJ_DIR)/linenoise.o

# ── Core library objects (src/core/) ──
CORE_SRC = src/core
CORE_OBJS = \
	$(OBJ_DIR)/alchemy_llm.o \
	$(OBJ_DIR)/alchemy_http.o \
	$(OBJ_DIR)/alchemy_breath.o \
	$(OBJ_DIR)/alchemy_context.o \
	$(OBJ_DIR)/alchemy_window.o \
	$(OBJ_DIR)/alchemy_memory.o \
	$(OBJ_DIR)/alchemy_tool.o \
	$(OBJ_DIR)/alchemy_skill.o \
	$(OBJ_DIR)/alchemy_agent.o \
	$(OBJ_DIR)/alchemy_dashboard.o \
	$(OBJ_DIR)/alchemy_rpgstatus.o \
	$(OBJ_DIR)/alchemy_permit.o \
	$(OBJ_DIR)/alchemy_hook.o \
	$(OBJ_DIR)/alchemy_env.o \
	$(OBJ_DIR)/alchemy_core_rules.o \
	$(OBJ_DIR)/alchemy_skill_evolve.o

# ── Gateway objects (src/gateway/discord/) ──
GATEWAY_SRC = src/gateway/discord
GATEWAY_OBJS = \
	$(OBJ_DIR)/alchemy_ws.o \
	$(OBJ_DIR)/discord.o

# All objects combined
ALL_OBJS = $(LIB_OBJS) $(CORE_OBJS) $(GATEWAY_OBJS)

# Targets
TARGETS = \
	$(BUILD_DIR)/alchemy \
	$(BUILD_DIR)/dashboard \
	$(BUILD_DIR)/discord \
	$(BUILD_DIR)/test_context \
	$(BUILD_DIR)/test_window \
	$(BUILD_DIR)/test_memory \
	$(BUILD_DIR)/test_tool \
	$(BUILD_DIR)/test_permit \
	$(BUILD_DIR)/test_library

all: $(TARGETS)

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

# ══════════════════════════════════════════════════════════════════
# LIBRARY MODULES
# ══════════════════════════════════════════════════════════════════

$(OBJ_DIR)/alchemy_json.o: library/json/alchemy_json.c library/json/alchemy_json.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_llm_router.o: library/llm/alchemy_llm_router.c library/llm/alchemy_llm_router.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_subagent.o: library/algo/alchemy_subagent.c library/algo/alchemy_subagent.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_retry.o: library/algo/alchemy_retry.c library/algo/alchemy_retry.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_slash.o: library/algo/alchemy_slash.c library/algo/alchemy_slash.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_skill_sim.o: library/algo/alchemy_skill_sim.c library/algo/alchemy_skill_sim.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_session.o: library/ds/alchemy_session.c library/ds/alchemy_session.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_mcp.o: library/llm/alchemy_mcp.c library/llm/alchemy_mcp.h library/json/alchemy_json.h library/ds/alchemy_str.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_plugin.o: library/algo/alchemy_plugin.c library/algo/alchemy_plugin.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_cost.o: library/algo/alchemy_cost.c library/algo/alchemy_cost.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_tui.o: library/algo/alchemy_tui.c library/algo/alchemy_tui.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_test.o: library/algo/alchemy_test.c library/algo/alchemy_test.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_browser.o: library/browser/alchemy_browser.c library/browser/alchemy_browser.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_memory_auto.o: library/algo/alchemy_memory_auto.c library/algo/alchemy_memory_auto.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_worktree.o: library/algo/alchemy_worktree.c library/algo/alchemy_worktree.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_task_queue.o: library/algo/alchemy_task_queue.c library/algo/alchemy_task_queue.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/linenoise.o: library/linenoise.c library/linenoise.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# ══════════════════════════════════════════════════════════════════
# CORE MODULES
# ══════════════════════════════════════════════════════════════════

$(OBJ_DIR)/alchemy_llm.o: $(CORE_SRC)/alchemy_llm.c $(CORE_SRC)/alchemy_llm.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_http.o: $(CORE_SRC)/alchemy_http.c $(CORE_SRC)/alchemy_http.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_breath.o: $(CORE_SRC)/alchemy_breath.c $(CORE_SRC)/alchemy_breath.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_context.o: $(CORE_SRC)/alchemy_context.c $(CORE_SRC)/alchemy_context.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_window.o: $(CORE_SRC)/alchemy_window.c $(CORE_SRC)/alchemy_window.h $(CORE_SRC)/alchemy_model.h $(CORE_SRC)/alchemy_context.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_memory.o: $(CORE_SRC)/alchemy_memory.c $(CORE_SRC)/alchemy_memory.h $(CORE_SRC)/alchemy_context.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_tool.o: $(CORE_SRC)/alchemy_tool.c $(CORE_SRC)/alchemy_tool.h $(CORE_SRC)/alchemy_memory.h $(CORE_SRC)/alchemy_skill.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_skill.o: $(CORE_SRC)/alchemy_skill.c $(CORE_SRC)/alchemy_skill.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_agent.o: $(CORE_SRC)/alchemy_agent.c $(CORE_SRC)/alchemy_agent.h $(CORE_SRC)/alchemy_llm.h $(CORE_SRC)/alchemy_window.h $(CORE_SRC)/alchemy_memory.h $(CORE_SRC)/alchemy_skill.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_dashboard.o: $(CORE_SRC)/alchemy_dashboard.c $(CORE_SRC)/alchemy_dashboard.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_rpgstatus.o: $(CORE_SRC)/alchemy_rpgstatus.c $(CORE_SRC)/alchemy_rpgstatus.h $(CORE_SRC)/alchemy_dashboard.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_permit.o: $(CORE_SRC)/alchemy_permit.c $(CORE_SRC)/alchemy_permit.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_hook.o: $(CORE_SRC)/alchemy_hook.c $(CORE_SRC)/alchemy_hook.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_env.o: $(CORE_SRC)/alchemy_env.c $(CORE_SRC)/alchemy_env.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_skill_evolve.o: $(CORE_SRC)/alchemy_skill_evolve.c $(CORE_SRC)/alchemy_skill_evolve.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/alchemy_core_rules.o: $(CORE_SRC)/alchemy_core_rules.c $(CORE_SRC)/alchemy_core_rules.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# ══════════════════════════════════════════════════════════════════
# GATEWAY MODULES
# ══════════════════════════════════════════════════════════════════

$(OBJ_DIR)/alchemy_ws.o: $(GATEWAY_SRC)/alchemy_ws.c $(GATEWAY_SRC)/alchemy_ws.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/discord.o: $(GATEWAY_SRC)/discord.c $(GATEWAY_SRC)/alchemy_ws.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# ══════════════════════════════════════════════════════════════════
# EXECUTABLES
# ══════════════════════════════════════════════════════════════════

# CLI (standalone, no Discord)
$(BUILD_DIR)/alchemy: src/cli/main.c $(LIB_OBJS) $(CORE_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	cp $@ $@.debug
	strip $@

# Dashboard generator
$(BUILD_DIR)/dashboard: src/cli/dashboard_demo.c $(LIB_OBJS) $(CORE_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Discord gateway (full)
$(BUILD_DIR)/discord: $(LIB_OBJS) $(CORE_OBJS) $(GATEWAY_OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	cp $@ $@.debug
	strip $@

# Library test (needs http.o for alchemy_llm_router.o)
$(BUILD_DIR)/test_library: $(LIB_OBJS) $(OBJ_DIR)/alchemy_http.o $(OBJ_DIR)/alchemy_breath.o $(OBJ_DIR)/alchemy_llm.o $(OBJ_DIR)/alchemy_permit.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ src/test_library.c \
		$(OBJ_DIR)/alchemy_json.o \
		$(OBJ_DIR)/alchemy_retry.o \
		$(OBJ_DIR)/alchemy_slash.o \
		$(OBJ_DIR)/alchemy_session.o \
		$(OBJ_DIR)/alchemy_mcp.o \
		$(OBJ_DIR)/alchemy_plugin.o \
		$(OBJ_DIR)/alchemy_memory_auto.o \
		$(OBJ_DIR)/alchemy_worktree.o \
		$(OBJ_DIR)/alchemy_task_queue.o \
		$(OBJ_DIR)/alchemy_http.o \
		$(OBJ_DIR)/alchemy_breath.o \
		$(OBJ_DIR)/alchemy_llm.o \
		$(OBJ_DIR)/alchemy_permit.o \
		$(LDFLAGS) -ldl

# ══════════════════════════════════════════════════════════════════
# TESTS
# ══════════════════════════════════════════════════════════════════

$(BUILD_DIR)/test_context: src/test_context.c $(OBJ_DIR)/alchemy_context.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_window: src/test_window.c \
	$(OBJ_DIR)/alchemy_window.o \
	$(OBJ_DIR)/alchemy_context.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_memory: src/test_memory.c \
	$(OBJ_DIR)/alchemy_memory.o \
	$(OBJ_DIR)/alchemy_context.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_tool: src/test_tool.c \
	$(OBJ_DIR)/alchemy_tool.o \
	$(OBJ_DIR)/alchemy_skill.o \
	$(OBJ_DIR)/alchemy_skill_sim.o \
	$(OBJ_DIR)/alchemy_context.o \
	$(OBJ_DIR)/alchemy_memory.o \
	$(OBJ_DIR)/alchemy_json.o \
	$(OBJ_DIR)/alchemy_permit.o \
	$(OBJ_DIR)/alchemy_skill_evolve.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/test_dump_tools: test_dump_tools.c \
	$(OBJ_DIR)/alchemy_tool.o \
	$(OBJ_DIR)/alchemy_skill.o \
	$(OBJ_DIR)/alchemy_skill_sim.o \
	$(OBJ_DIR)/alchemy_context.o \
	$(OBJ_DIR)/alchemy_memory.o \
	$(OBJ_DIR)/alchemy_json.o \
	$(OBJ_DIR)/alchemy_permit.o \
	$(OBJ_DIR)/alchemy_skill_evolve.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I src/core -o $@ $^ $(LDFLAGS)

test_dump_tools: $(BUILD_DIR)/test_dump_tools
	$(BUILD_DIR)/test_dump_tools

$(BUILD_DIR)/test_permit: src/test_permit.c $(OBJ_DIR)/alchemy_permit.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# ── Browser test (requires Playwright + Firefox) ──
$(BUILD_DIR)/test_browser: src/test_browser.c $(OBJ_DIR)/alchemy_browser.o | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# ══════════════════════════════════════════════════════════════════
# PHONY TARGETS
# ══════════════════════════════════════════════════════════════════

check: $(BUILD_DIR)/test_context $(BUILD_DIR)/test_window $(BUILD_DIR)/test_memory $(BUILD_DIR)/test_tool $(BUILD_DIR)/test_permit
	@test/run_tests.sh

test: $(BUILD_DIR)/test_library
	@./build/test_library

release: CFLAGS = -Wall -Wextra -Os -flto -I src/core -I src/gateway/discord -I src/cli -I library -I library/json -I library/ds -I library/llm -I library/algo -I library/browser
release: LDFLAGS = -lssl -lcrypto -ldl -flto -lm
release: $(BUILD_DIR)/alchemy $(BUILD_DIR)/discord
	@echo "Release build done"
	@ls -lh build/alchemy build/discord

clean:
	rm -rf $(BUILD_DIR) $(OBJ_DIR)

.PHONY: all clean check release

# ── Install / Uninstall ──
PREFIX ?= $(HOME)/.local
BINDIR ?= $(PREFIX)/bin
ALCHEMY_DIR ?= $(HOME)/.alchemy

install: $(BUILD_DIR)/alchemy $(BUILD_DIR)/discord
	@mkdir -p $(BINDIR)
	@cp $(BUILD_DIR)/alchemy $(BINDIR)/alchemy
	@cp $(BUILD_DIR)/discord $(BINDIR)/alchemy-discord
	@mkdir -p $(ALCHEMY_DIR)/sessions $(ALCHEMY_DIR)/skills
	@if [ ! -f $(ALCHEMY_DIR)/settings.json ]; then \
		echo '{"role":"developer","rules":[{"tool":"run_command","pattern":"make *","effect":"allow"},{"tool":"run_command","pattern":"git *","effect":"allow"},{"tool":"run_command","pattern":"cd *","effect":"allow"},{"tool":"run_command","pattern":"ls *","effect":"allow"},{"tool":"run_command","pattern":"*","effect":"ask"},{"tool":"read_file","pattern":"*","effect":"allow"},{"tool":"write_file","pattern":"*","effect":"allow"},{"tool":"write_file","pattern":".env","effect":"deny"},{"tool":"web_search","pattern":"*","effect":"allow"},{"tool":"web_extract","pattern":"*","effect":"allow"},{"tool":"apply_patch","pattern":"*","effect":"allow"},{"tool":"patch","pattern":"*","effect":"allow"},{"tool":"memory_recall","pattern":"*","effect":"allow"},{"tool":"file_search","pattern":"*","effect":"allow"},{"tool":"list_skills","pattern":"*","effect":"allow"},{"tool":"skill_use","pattern":"*","effect":"allow"},{"tool":"search_skills","pattern":"*","effect":"allow"},{"tool":"skill_create","pattern":"*","effect":"allow"},{"tool":"skill_merge","pattern":"*","effect":"allow"},{"tool":"calculator","pattern":"*","effect":"allow"},{"tool":"json_query","pattern":"*","effect":"allow"},{"tool":"json_validate","pattern":"*","effect":"allow"}]}' > $(ALCHEMY_DIR)/settings.json; \
		echo "  Created $(ALCHEMY_DIR)/settings.json"; \
	fi
	@echo "  Installed: $(BINDIR)/alchemy ($(shell ls -lh $(BINDIR)/alchemy | awk '{print $$5}'))"
	@echo "  Config:    $(ALCHEMY_DIR)/"
	@echo "  Run:       alchemy"

uninstall:
	@rm -f $(BINDIR)/alchemy $(BINDIR)/alchemy-discord
	@echo "  Removed: $(BINDIR)/alchemy"
	@echo "  Config at $(ALCHEMY_DIR)/ not removed (rm -rf to delete)"
