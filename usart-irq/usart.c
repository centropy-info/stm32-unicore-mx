/*
 * Copyright (c) 2016 Centropy <contact@centropy.info>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <errno.h>
#include <unicore-mx/stm32/rcc.h>
#include <unicore-mx/stm32/gpio.h>
#include <unicore-mx/stm32/usart.h>
#include <unicore-mx/cm3/nvic.h>
#include "usart.h"
#include "rb.h"

/* A TX ringbuffer */
struct ring *tx_buf;

/* Struct to hold all the USART device settings. */
struct dev_usart {
	uint32_t base;
	uint32_t irq;
	uint32_t rcc;
	uint32_t tx_rcc;
	uint32_t tx_pin;
	uint32_t tx_port;
	uint32_t tx_af;
	uint32_t baudrate;
	uint8_t databits;
	uint8_t stopbits;
	uint8_t mode;
	uint8_t parity;
	uint8_t fc;

};

/* Define USART options. */
static struct dev_usart devusart = {
	.base = USART1,			/* USART1 memory base address (unicore-mx/stm32/usart.h) */
	.irq = NVIC_USART1_IRQ,		/* USART1 IRQ for NVIC */
	.rcc = RCC_USART1,		/* USART1 RCC (unicore-mx/stm32/rcc.h) */
	.tx_rcc = RCC_GPIOA,		/* USART1 TX port RCC (unicore-mx/stm32/rcc.h) */
	.tx_port = GPIOA,		/* USART1 TX port (unicore-mx/stm32/gpio.h) */
	.tx_pin = GPIO9,		/* USART1 TX pin (unicore-mx/stm32/gpio.h) */
	.tx_af = GPIO_AF7,		/* USART1 TX Alternate Function 7 (unicore-mx/stm32/gpio.h) */
	.baudrate = 115200,		/* Speed of USART */
	.databits = 8,			/* We use 8 data bits */
	.stopbits = USART_STOPBITS_1,	/* And 1 stop bit.. (unicore-mx/stm32/usart.h) */
	.mode = USART_MODE_TX,		/* We do TX only (unicore-mx/stm32/usart.h) */
	.parity = USART_PARITY_NONE,	/* No parity check (unicore-mx/stm32/usart.h) */
	.fc = USART_FLOWCONTROL_NONE,	/* No flow control (unicore-mx/stm32/usart.h) */
};


/* Local function declarations */
void usart_isr(struct dev_usart *usart);


/* Function: _write
 * ----------------
 *  This overrides internal libc _write function,
 *  allowing us to use printf to output on a USART device.
 *
 *  file:	FD.
 *  *ptr:	Pointer to data to write.
 *  len:	Length of data to write.
 *
 *  Returns:	Length writte on success.
 *		-1 and errno EIO on failure.
 */
int _write(int file, char *ptr, int len)
{
	if (file == 1) {
		int new = rb_used(tx_buf);
		rb_write(tx_buf, ptr, len);

		if (!new) {
			usart_enable_tx_interrupt(devusart.base);

			if (usart_is_send_ready(devusart.base)) {
				char c;
				rb_read(tx_buf, &c, 1);
				usart_send(devusart.base, (uint16_t)c);
			}
		}

		return len;
	}

	errno = EIO;
	return -1;
}


/* Function: usart_clock_init
 * --------------------------
 *  Internal function to setup USART clocks.
 *
 *  *usart:	Pointer to a USART device object.
 */
static void usart_clock_init(struct dev_usart *usart)
{
	rcc_periph_clock_enable(usart->tx_rcc);
	rcc_periph_clock_enable(usart->rcc);
}


/* Function: usart_pin_init
 * ------------------------
 *  Internal function to setup USART pins.
 *
 *  *usart:	Pointer to a USART device object.
 */
static void usart_pin_init(struct dev_usart *usart)
{
	gpio_mode_setup(usart->tx_port, GPIO_MODE_AF, GPIO_PUPD_NONE, usart->tx_pin);
	gpio_set_af(usart->tx_port, usart->tx_af, usart->tx_pin);
}


/* Function: usart_config
 * ----------------------
 *  Internal function to configure a USART device.
 *
 *  *usart:	Pointer to a USART device object.
 */
static void usart_config(struct dev_usart *usart)
{
	usart_set_baudrate(usart->base, usart->baudrate);
	usart_set_databits(usart->base, usart->databits);
	usart_set_stopbits(usart->base, usart->stopbits);
	usart_set_mode(usart->base, usart->mode);
	usart_set_parity(usart->base, usart->parity);
	usart_set_flow_control(usart->base, usart->fc);
	nvic_enable_irq(usart->irq);

	usart_enable(usart->base);
}


/* Function: usart_init
 * --------------------
 *  Initialize a USART device.
 */
void usart_init(void)
{
	usart_clock_init(&devusart);
	usart_pin_init(&devusart);
	usart_config(&devusart);
	tx_buf = rb_init(128);
}


/* Function: usart_isr
 * -------------------
 *  Generic function to do the USART ISR action.
 *
 *  *usart:	Pointer to a USART device object.
 */
void usart_isr(struct dev_usart *usart)
{
	/* TX interrupt */
	if (usart_get_interrupt_source(usart->base, USART_SR_TXE)) {
		if (rb_used(tx_buf)) {
			char c;
			rb_read(tx_buf, &c, 1);
			usart_send(usart->base, (uint16_t)c);
		} else {
			usart_disable_tx_interrupt(usart->base);
		}
	}
}


/* Function: usart1_isr
 * --------------------
 *  USART1 ISR wrapper.
 */
void usart1_isr(void)
{
	usart_isr(&devusart);
}

