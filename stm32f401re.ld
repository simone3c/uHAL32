ENTRY(_reset);                                   /* entry point */

MEMORY {
  flash(rx) : ORIGIN = 0x08000000, LENGTH = 512k
  sram(rwx) : ORIGIN = 0x20000000, LENGTH = 92k  /* remaining 64k in a separate address space */
}

_estack     = ORIGIN(sram) + LENGTH(sram);       /* stack points to end of SRAM */

SECTIONS {
  /****************************/
  /*      VECTOR TABLE        */
  /****************************/
  .vectors  : { KEEP(*(.vectors)) }   > flash

  /****************************/
  /*      PROGRAM CODE        */
  /****************************/
  .text     : { *(.text*) }           > flash

  /****************************/
  /*      CONSTANT DATA       */
  /****************************/
  .rodata   : { *(.rodata*) }         > flash

  /****************************/
  /*  GLOB. CONSTRUCTORS PRE  */
  /****************************/
  .preinit_array :
  {
      _preinit_array_start = .;
      KEEP (*(.preinit_array*))
      _preinit_array_end = .;
  } > flash

  /****************************/
  /*    GLOB. CONSTRUCTORS    */
  /****************************/
  .init_array :
  {
      _init_array_start = .;
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array*))
      _init_array_end = .;
  } > flash

  /****************************/
  /*     GLOB. DESTRUCTORS    */
  /****************************/
  .fini_array :
  {
      _fini_array_start = .;
      KEEP (*(SORT(.fini_array.*)))
      KEEP (*(.fini_array*))
      _fini_array_end = .;
  } > flash

  /****************************/
  /*    INITIALIZED DATA      */
  /****************************/
  .data : {
    _sdata = .;                                  /* .data section start */
    *(.first_data)
    *(.data SORT(.data.*))
    _edata = .;                                  /* .data section end */
  } > sram AT > flash
  _sidata = LOADADDR(.data);

  /****************************/
  /*   UNINITIALIZED DATA     */
  /****************************/
  .bss : {
    _sbss = .;                                   /* .bss section start */
    *(.bss SORT(.bss.*) COMMON)
    _ebss = .;                                   /* .bss section end */
  } > sram

  . = ALIGN(8);
  _end = .;     /* for cmsis_gcc.h  */
}
 