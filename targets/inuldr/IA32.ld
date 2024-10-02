ENTRY(BtldrBootstrap)

SECTIONS
{

	. = 1M;
 	StartLocation = .;
	.text :
	{
		*(.multiboot)
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
	LoaderEnd = ALIGN(4K);
	EndLocation = .;
}