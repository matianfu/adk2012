# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2011, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

EXAMPLES = adc12                 \
  adc12_temp_sensor              \
  adc12_threshold_wakeup         \
  cm3_bit_banding                \
  cm3_nvic                       \
  dac12_pdc_sinewave             \
  eefc_pgm                       \
  eefc_uniqueid                  \
  getting-started                \
  low_power                      \
  periph_protect                 \
  pio_alternate_function         \
  pmc_clock_switching            \
  pwm_pdc                        \
  rtc                            \
  rtt                            \
  sdramc                         \
  smc_psram                      \
  spi_dma_dataflash              \
  spi_dma_slave                  \
  ssc_dma_audio                  \
  tc_capture_waveform            \
  twi_dma_eeprom                 \
  twi_pdc_eeprom                 \
  twi_slave                      \
  twi_temp_sensor                \
  usart_hard_handshaking         \
  usart_irda                     \
  usart_iso7816                  \
  usart_pdc_spi                  \
  usart_rs485                    \
  usart_serial                   \
  usart_synchronous              \
  wdg_irq

EXAMPLES_FILESYSTEM = smc_nandflash_fatfs    \
  hsmci_multimedia_card_fatfs                \
  hsmci_sdcard_fatfs

EXAMPLES_CAN = can

EXAMPLES_ETH = emac              \
  emac_lwip                      \
  emac_uip_helloworld            \
  emac_uip_telnetd               \
  emac_uip_webserver

EXAMPLES_GRAPHICS = smc_lcd

EXAMPLES_QTOUCH  = qtouch

EXAMPLES_STORAGE = hsmci_multimedia_card \
  hsmci_sdcard                   \
  smc_nandflash                  \
  smc_norflash                   \

EXAMPLES_USB_HOST = usb_host_hid

all: libraries $(EXAMPLES) $(EXAMPLES_CAN) $(EXAMPLES_ETH) $(EXAMPLES_FILESYSTEM) $(EXAMPLES_GRAPHICS) $(EXAMPLES_QTOUCH) $(EXAMPLES_STORAGE)

libraries:
	(cd ../../libraries/build/gcc && "$(MAKE)" -f Makefile)

define COMPILS
$(1):
	(cd ../../$(2)/$(1)/build/gcc && "$(MAKE)" -f Makefile clean all)
endef


$(foreach EXAMPLE, $(EXAMPLES), $(eval $(call COMPILS,$(EXAMPLE),examples)))
$(foreach EXAMPLE, $(EXAMPLES_CAN), $(eval $(call COMPILS,$(EXAMPLE),examples_can)))
$(foreach EXAMPLE, $(EXAMPLES_ETH), $(eval $(call COMPILS,$(EXAMPLE),examples_eth)))
$(foreach EXAMPLE, $(EXAMPLES_FILESYSTEM), $(eval $(call COMPILS,$(EXAMPLE),examples_filesystem)))
$(foreach EXAMPLE, $(EXAMPLES_GRAPHICS), $(eval $(call COMPILS,$(EXAMPLE),examples_graphics)))
$(foreach EXAMPLE, $(EXAMPLES_QTOUCH), $(eval $(call COMPILS,$(EXAMPLE),examples_qtouch)))
$(foreach EXAMPLE, $(EXAMPLES_STORAGE), $(eval $(call COMPILS,$(EXAMPLE),examples_storage)))
#$(foreach EXAMPLE, $(EXAMPLES_USB_HOST), $(eval $(call COMPILS,$(EXAMPLE),examples_usb_host)))




