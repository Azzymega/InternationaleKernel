ENTRY(KeBootstrap)

SECTIONS
{
	. = 0x80400000;
	StartLocation = .;

	.text ALIGN(4K) :
	{
		*(.text)
	}

	.rodata ALIGN(4K) :
	{
		*(.rodata)
	}

	.data ALIGN(4K) :
	{
		*(.data)
	}

	.bss ALIGN(4K) :
	{
		*(COMMON)
		*(.bss)
	}

	KernelEnd = ALIGN(4K);
	EndLocation = .;
}