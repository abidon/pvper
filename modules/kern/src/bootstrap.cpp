#include <priv/base.h>
#include <priv/hw/Gpio.h>
#include <priv/hw/Serial.h>

EXTERNC void __assert(const char *message, const char *filename, const uint32_t line) {
	pvper::hw::Serial &serialLine = pvper::hw::Serial::shared();
	serialLine.puts("Assertion failed: ");
	serialLine.puts(filename);
	serialLine.puts(":");
	serialLine.puts(todec(line, 0));
	serialLine.puts("\r\n\t> ");
	serialLine.puts(message);
	serialLine.puts("\r\n");
	__halt();
}

EXTERNC void kinit(uint32_t r0, uint32_t r1, uint32_t atags);

EXTERNC void bootstrap(void)
{
	// Keep r0, machtype and &atags
	__asm__ volatile("push {r0, r1, r2}");

	// Fill BSS section with zeroes
	extern uint8_t *__bss_start, *__bss_end;
	register uint8_t* bss = __bss_start;
	while (bss < __bss_end)
		*bss++ = 0;

	// Initialize drivers
	pvper::hw::GpioPin::initialize();

	// Jump to kernel entry point
	__asm__ volatile("mov lr, %[entry_point]" : : [entry_point] "r" ((unsigned int)&kinit) );
	__asm__ volatile("pop {r0, r1, r2}");
	__asm__ volatile("bx lr");

	__halt();
}
