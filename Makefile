# Top-level Makefile forwarding commands to portsync/

all:
	@$(MAKE) -C portsync all

portsync:
	@$(MAKE) -C portsync portsync

test:
	@$(MAKE) -C portsync test

sanitizer:
	@$(MAKE) -C portsync sanitizer

clean:
	@$(MAKE) -C portsync clean

.PHONY: all portsync test sanitizer clean
