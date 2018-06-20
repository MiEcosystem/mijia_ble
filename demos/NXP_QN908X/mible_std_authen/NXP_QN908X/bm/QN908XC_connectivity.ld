/*
** ###################################################################
**     Processors:          QN908X
**
**     Compiler:            GNU C Compiler
**     Reference manual:    QN908X Series Reference Manual, Rev. 0 , 11/2016
**     Version:             rev. 1.0, 2016-11-02
**     Build:               b161214
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright (c) 2016 Freescale Semiconductor, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Copyright (c) 2016 NXP Semiconductors, Inc.
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of NXP Semiconductors, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
** ###################################################################
*/


/******************* Map of Kinetis FLASH ***********************************

             0x0008_0000    m_flash_end
    - - - +---------------+ - - - - - - - -
          |   _ _ _ _ _   |
          | Flash config  |
    6k    |   - - - - -   |
          |               |
          |  RESERVED     |
          |               | m_reserved_flash_start       0x7e000
    - - - +---------------+ - - - - - - - -
          |               | NV_STORAGE_START_ADDRESS
    8k    |  sNVM_region  |
          | - - - - - - - | --- NV_VIRTUAL_PAGE_BOUND
          |  fNVM_region  |
          |               | NV_STORAGE_END_ADDRESS
    - - - +---------------+ - - - - - - - -
          |               | INT_STORAGE_START
          |               |
          |               |
   ~243k  | OTAP Internal |
          |      Storage* |
          |               |
          |               |
          |               | INT_STORAGE_END
    - - - +---------------+ - - - - - - - -      
          |               | m_text_end
          |               |
          |               |
 ~242k**  |  Application  |
          |               |
          |               |
          |               | m_text_start
    - - - +---------------+ - - - - - - - -
          |               | m_bootFlags_end
 8 bytes  |  Boot Flags*  |
          |               | m_bootFlags_start
    - - - +---------------+ - - - - - - - - 
          |   Interrupt   | m_interrupts_end
  0x114   |    Vectors    |
  bytes   |               | m_interrupts_start
    - - - +---------------+ - - - - - - - -    
          |               | m_bootloader_end
          |               |
   10k    |  Bootloader*  |
          |               |
          |               | m_bootloader_start = m_flash_start
    - - - +---------------+ - - - - - - - -
             0x0000_0000

 * - These sections are optional for certain applications
 ** - Application will occupy all the free space available
 *****************************************************************************/


/* Entry Point */
ENTRY(rom_entry_point)



/*** flash memory characteristics ***/
m_flash_start   = 0x00000000;
m_flash_end     = 0x0007FFFF;
m_flash_size    = 0x00080000;
m_sector_size   = 2048; /* 2k  */



/****************************************************
 *******         User Defines                 *******
 ****************************************************/
UseNVMLink              = DEFINED(gUseNVMLink_d)            ? gUseNVMLink_d             : 0;
NVMSectorCountLink      = DEFINED(gNVMSectorCountLink_d)    ? gNVMSectorCountLink_d     : ( DEFINED(gUseNVMLink_d) ? 4 : 0 );
EraseNVMLink            = DEFINED(gEraseNVMLink_d)          ? gEraseNVMLink_d           : 0;
ram_vector_table        = DEFINED(__ram_vector_table__)     ? __ram_vector_table__      : 0;
UseBootloaderLink       = DEFINED(gUseBootloaderLink_d)     ? gUseBootloaderLink_d      : 0;
UseInternalStorageLink  = DEFINED(gUseInternalStorageLink_d)? gUseInternalStorageLink_d : 0;

HEAP_SIZE               = DEFINED(__heap_size__)            ? __heap_size__             : 0;
STACK_SIZE              = DEFINED(__stack_size__)           ? __stack_size__            : 0x0800;
vector_table_size       = 0x114;



/****************************************************
 ******          Flash regions sizes           ******
 ****************************************************/
m_bootloader_size       = (UseBootloaderLink) ? 5 * m_sector_size : 0;
m_bootFlags_size        = (UseBootloaderLink) ? 0x10 : 0x0;
NV_STORAGE_SIZE         = NVMSectorCountLink * m_sector_size;
m_reserved_flash_size   = (3 * m_sector_size);
used_flash_size         = (m_bootloader_size + vector_table_size + m_bootFlags_size + NV_STORAGE_SIZE + m_reserved_flash_size);
INT_STORAGE_SIZE        = (UseInternalStorageLink) ? (m_sector_size * ( (((m_flash_size - used_flash_size) / 2) / m_sector_size) + 1 )) : 0; /* Half of the free flash, aligned to m_sector_size */



/****************************************************
 ******        Flash Regions, low to high      ******
 ****************************************************/
m_bootloader_start      = m_flash_start; /* + m_bootloader_size , can be zero */
m_interrupts_start      = m_bootloader_size;
m_interrupts_end        = m_interrupts_start + vector_table_size - 1;
m_bootFlags_start       = m_interrupts_end + 1;
m_bootFlags_end         = m_bootFlags_start + m_bootFlags_size - 1;
m_text_start            = m_bootFlags_end + 1;



/****************************************************
 *****        Flash Regions, high to low       ******
 ****************************************************/
m_reserved_flash_start   = m_flash_size - 3 * m_sector_size;
NV_STORAGE_START_ADDRESS = m_reserved_flash_start - 1;
NV_STORAGE_END_ADDRESS  = NV_STORAGE_START_ADDRESS - NV_STORAGE_SIZE + 1;
INT_STORAGE_START       = NV_STORAGE_END_ADDRESS - 1;
INT_STORAGE_END         = INT_STORAGE_START - INT_STORAGE_SIZE + 1;
m_text_end              = INT_STORAGE_END - 1;



/* Other Defines */
m_text_size             = m_text_end - m_text_start + 1;
NV_STORAGE_SECTOR_SIZE  = m_sector_size;
NV_STORAGE_MAX_SECTORS  = NVMSectorCountLink;
__BootFlags_Start__     = m_bootFlags_start;
NV_VIRTUAL_PAGE_BOUND   = NV_STORAGE_START_ADDRESS - (NV_STORAGE_SIZE / 2);
INT_STORAGE_SECTOR_SIZE = m_sector_size;
M_VECTOR_RAM_SIZE       = DEFINED(__ram_vector_table__) ? vector_table_size : 0x0;



/* Specify the memory areas */
MEMORY
{
    m_bootloader        (RW) : ORIGIN = m_bootloader_start,     LENGTH = m_bootloader_size
    m_interrupts        (RX) : ORIGIN = m_interrupts_start,     LENGTH = vector_table_size
    m_bootFlags         (RW) : ORIGIN = m_bootFlags_start,      LENGTH = m_bootFlags_size
    m_text              (RX) : ORIGIN = m_text_start,           LENGTH = m_text_size
    m_InternalStorage   (RW) : ORIGIN = INT_STORAGE_END,        LENGTH = INT_STORAGE_SIZE
    sNVM_region         (RW) : ORIGIN = NV_STORAGE_END_ADDRESS, LENGTH = (NV_STORAGE_SIZE / 2)
    fNVM_region         (RW) : ORIGIN = NV_VIRTUAL_PAGE_BOUND,  LENGTH = (NV_STORAGE_SIZE / 2)
    m_data              (RW) : ORIGIN = 0x04000400,             LENGTH = 0x0001FC00                     /* First 0x400 bytes are reserved for ROM Bootloader use */
}



_RAM_START_ = ORIGIN(m_data);
_RAM_END_   = ORIGIN(m_data) + LENGTH(m_data) - 1;



/* Define output sections */
SECTIONS
{
    .interrupts :
    {
        __VECTOR_TABLE = .;
        . = ALIGN(4);
        KEEP(*(.isr_vector)) /* Interrupt vectors */
        . = ALIGN(4);
    } > m_interrupts

    .boot_flags :
    {
        . = ALIGN(4);
        *(.BootloaderFlags) /* Boot Flags */
        . = ALIGN(4);
    } > m_bootFlags

    /* The program code and other data goes into internal flash */
    .text :
    {
        . = ALIGN(4);
        *(.text)                /* .text sections (code) */
        *(.text*)               /* .text* sections (code) */
        *(.rodata)              /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)             /* .rodata* sections (constants, strings, etc.) */
        *(.glue_7)              /* glue arm to thumb code */
        *(.glue_7t)             /* glue thumb to arm code */
        *(.eh_frame)
        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(4);
    } > m_text

    .NVM_TABLE :
    {
      . = ALIGN(4);
      PROVIDE(__start_NVM_TABLE = .);
      KEEP(*(.NVM_TABLE));
      PROVIDE(__stop_NVM_TABLE = .);
      . = ALIGN(4);
    } > m_text

    .VERSION_TAGS :
    {
        . = ALIGN(4);
        PROVIDE(__start_VERSION_TAGS = .);
        KEEP(*(.VERSION_TAGS));
        PROVIDE(__stop_VERSION_TAGS = .);
        . = ALIGN(4);
    } > m_text

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > m_text

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } > m_text

    .ctors :
    {
        __CTOR_LIST__ = .;
        /* gcc uses crtbegin.o to find the start of
         the constructors, so we make sure it is
         first.  Because this is a wildcard, it
         doesn't matter if the user does not
         actually link against crtbegin.o; the
         linker won't look for a file to match a
         wildcard.  The wildcard also means that it
         doesn't matter which directory crtbegin.o
         is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
         from the crtend.o file until after the sorted ctors.
         The .ctor section from the crtend file contains the
         end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
        __CTOR_END__ = .;
    } > m_text

    .dtors :
    {
        __DTOR_LIST__ = .;
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
        __DTOR_END__ = .;
    } > m_text

    .preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > m_text

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > m_text

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > m_text

  __etext = .;    /* define a global symbol at end of code */
    __DATA_ROM = .; /* Symbol is used by startup for data initialization */

    .fNVM :
    {
        . = ALIGN(4);
        *(.fEraseNVM) /* Boot Flags */
        . = ALIGN(4);
    } > fNVM_region

    .sNVM :
    {
        . = ALIGN(4);
        *(.sEraseNVM) /* Boot Flags */
        . = ALIGN(4);
    } > sNVM_region

    .interrupts_ram :
    {
        . = ALIGN(4);
        __VECTOR_RAM__ = .;
        __interrupts_ram_start__ = .;   /* Create a global symbol at data start */
        *(.m_interrupts_ram)            /* This is a user defined section */
        . += M_VECTOR_RAM_SIZE;
        . = ALIGN(4);
        __interrupts_ram_end__ = .;     /* Define a global symbol at data end */
    } > m_data

    __VECTOR_RAM = DEFINED(__ram_vector_table__) ? __VECTOR_RAM__ : ORIGIN(m_interrupts);
    __RAM_VECTOR_TABLE_SIZE_BYTES = DEFINED(__ram_vector_table__) ? (__interrupts_ram_end__ - __interrupts_ram_start__) : 0x0;

    .data : AT(__DATA_ROM)
    {
        . = ALIGN(4);
        __DATA_RAM = .;
        __data_start__ = .;     /* create a global symbol at data start */
        *(.ramfunc*)            /* for functions in ram */
        *(.data)                /* .data sections */
        *(.data*)               /* .data* sections */
        KEEP(*(.jcr*))
        . = ALIGN(4);
        __data_end__ = .;       /* define a global symbol at data end */
    } > m_data

    __DATA_END = __DATA_ROM + (__data_end__ - __data_start__);
    text_end = ORIGIN(m_text) + LENGTH(m_text);
    ASSERT(__DATA_END <= text_end, "region m_text overflowed with text and data")

    /* Uninitialized data section */
    .bss :
    {
        /* This is used by the startup in order to initialize the .bss section */
        . = ALIGN(4);
        __START_BSS = .;
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.textrw)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        __END_BSS = .;
    } > m_data

    .heap :
    {
        . = ALIGN(8);
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += HEAP_SIZE;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > m_data

    .stack :
    {
        . = ALIGN(8);
        . += STACK_SIZE;
    } > m_data

    /* Initializes stack on the end of block */
    __StackTop = ORIGIN(m_data) + LENGTH(m_data);

    /* Initializes stack just end of . */
    /* __StackTop   = .; */

    __StackLimit = __StackTop - STACK_SIZE;
    PROVIDE(__stack = __StackTop);

    .ARM.attributes 0 : { *(.ARM.attributes) }

    ASSERT(__StackLimit >= __HeapLimit, "region m_data overflowed with stack and heap")
}
