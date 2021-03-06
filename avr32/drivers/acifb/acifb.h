/**
 * \file
 *
 * \brief ACIFB software driver for AVR32 UC3.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _ACIFB_H_
#define _ACIFB_H_

/**
 * \defgroup group_avr32_drivers_acifb ACIFB - Analog Comparator
 * Interface
 *
 * Analog Comparator Interface is able to control a number of Analog
 * Comparators (AC) with identical behavior.
 *
 * @{
 */

#include <avr32/io.h>
#include "compiler.h"

/** The timeguard used for polling. */
#define ACIFB_POLL_TIMEOUT         10000

/** AC channel configuration */
typedef struct {
	/** Filter length */
	uint8_t filter_len;
	/** Hysteresis value */
	uint8_t hysteresis_value;
	/** Output event when ACOUT is zero? */
	bool event_negative;
	/** Output event when ACOUT is one? */
	bool event_positive;
	/** Set the positive input */
	enum {
		PI_ACP,
	} positive_input;
	/** Set the negative input */
	enum {
		NI_ACN,
		NI_ACREFN
	} negative_input;
	/** Set the comparator mode */
	enum {
		MODE_OFF,
		MODE_CONTINUOUS,
		MODE_USER_TRIGGERED,
		MODE_EVENT_TRIGGERED
	} mode;
	/** Interrupt settings */
	enum {
		IS_VINP_GT_VINN,
		IS_VINP_LT_VINN,
		IS_OUTPUT_TGL,
		IS_COMP_DONE
	} interrupt_settings;
	/** Analog comparator number */
	uint8_t ac_n;
} acifb_channel_t;

/** ACIFB configuration */
typedef struct {
	/** Startup time (such that AC startup time = SUT/Fgclk) */
	uint32_t sut;
	/** Test mode */
	enum {
		TESTMODE_OFF,
		TESTMODE_ON
	} actest;
	/** Peripheral Event Trigger Enable */
	bool eventen;
} acifb_t;

/** AC Window configuration */
typedef struct {
	/** Window Mode Enable/Disable */
	bool window_mode;
	/** Window Event from awout Enable/Disable */
	bool window_event_enable;
	/** Window Event output configuration */
	enum {
		EVENT_ON_ACWOUT_RISING_EDGE,
		EVENT_ON_ACWOUT_FALLING_EDGE,
		EVENT_ON_ACWOUT_ON_ANY_EDGE,
		EVENT_ON_INSIDE_WINDOW,
		EVENT_ON_OUTSIDE_WINDOW,
		EVENT_ON_COMPARISON_COMPLETE
	} window_event;
	/** Interrupt settings */
	enum {
		IS_VINP_INSIDE_WINDOW,
		IS_VINP_OUTSIDE_WINDOW,
		IS_WINDOW_OUTPUT_TGL,
		IS_WINDOW_COMP_DONE
	} interrupt_settings;
	/**
	 * Analog comparator combination number
	 * \note ACPn, ACNn, ACP(n+1), ACN(n+1) -> nth combination
	 */
	int ac_combination_number;
} acifb_window_t;

void acifb_channels_setup(volatile avr32_acifb_t *acifb,
		const acifb_channel_t *ac_chan, uint32_t nb_chan);

void acifb_setup_and_enable(volatile avr32_acifb_t *acifb, const acifb_t *ac);

void acifb_windows_setup(volatile avr32_acifb_t *acifb,
		const acifb_window_t *ac_window, uint32_t nb_combinations);

bool acifb_wait_channels_ready(volatile avr32_acifb_t *acifb,
		const uint32_t acrdy_mask);

/**
 * \brief Disable ACIFB
 *
 * \param acifb         Base address of the ACIFB module
 */
static inline void acifb_disable(volatile avr32_acifb_t *acifb)
{
	acifb->ctrl &= ~(AVR32_ACIFB_CTRL_EN_MASK);
}

/**
 * \brief Trigger a Analog comparison.
 *
 * \param acifb        Base address of the ACIFB module
 */
static inline void acifb_user_trigger_single_comparison(
		volatile avr32_acifb_t *acifb)
{
	acifb->ctrl |= AVR32_ACIFB_CTRL_USTART_MASK;
}

/**
 * \brief Test event trigger for single analog comparison.
 *
 * \param acifb         Base address of the ACIFB module
 */
static inline void acifb_test_event_trigger_single_comparison(
		volatile avr32_acifb_t *acifb)
{
	acifb->ctrl |= AVR32_ACIFB_CTRL_ESTART_MASK;
}

/**
 * \brief Enable ACIFB interrupts.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  interrupt_mask Bitmask of interrupts to enable
 */
static inline void acifb_enable_interrupt(volatile avr32_acifb_t *acifb,
		uint32_t interrupt_mask)
{
	acifb->ier = interrupt_mask;
}

/**
 * \brief Enable ACIFB startup interrupt.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_enable_startup_interrupt(
		volatile avr32_acifb_t *acifb, uint8_t ac_channel)
{
	acifb->ier = (AVR32_ACIFB_IER_SUTINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Enable ACIFB comparison complete interrupt.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_enable_comparison_interrupt(
		volatile avr32_acifb_t *acifb,
		uint8_t ac_channel)
{
	acifb->ier = (AVR32_ACIFB_IER_ACINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Enable ACIFB comparison window mode interrupt.
 *
 * \param acifb           Base address of the ACIFB module
 * \param  ac_combination  ACIFB Channel combination Used
 * \note ACPn, ACNn, ACP(n+1), ACN(n+1) -> nth combination
 */
static inline void acifb_enable_window_interrupt(volatile avr32_acifb_t *acifb,
		uint8_t ac_combination)
{
	acifb->ier = (AVR32_ACIFB_IER_WFINT0_MASK) << (ac_combination);
}

/**
 * \brief Disable all ACIFB interrupts.
 *
 * \param acifb         Base address of the ACIFB module
 */
static inline void acifb_disable_all_interrupts(volatile avr32_acifb_t *acifb)
{
	acifb->idr = 0xFFFFFFFF;
}

/**
 * \brief Disable ACIFB interrupts.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  interrupt_mask Bitmask of interrupts to disable
 */
static inline void acifb_disable_interrupt(volatile avr32_acifb_t *acifb,
		uint32_t interrupt_mask)
{
	acifb->idr = interrupt_mask;
}

/**
 * \brief Disable ACIFB startup interrupt.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_disable_startup_interrupt(
		volatile avr32_acifb_t *acifb,
		uint8_t ac_channel)
{
	acifb->idr = (AVR32_ACIFB_IDR_SUTINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Disable ACIFB comparison complete interrupt.
 *
 * \param *acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_disable_comparison_interrupt(
		volatile avr32_acifb_t *acifb, uint8_t ac_channel)
{
	acifb->idr = (AVR32_ACIFB_IDR_ACINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Disable ACIFB comparison window mode interrupt.
 *
 * \param acifb           Base address of the ACIFB module
 * \param  ac_combination  ACIFB Channel combination Used
 * \note ACPn, ACNn, ACP(n+1), ACN(n+1) -> nth combination
 */
static inline void acifb_disable_window_interrupt(
		volatile avr32_acifb_t *acifb, uint8_t ac_combination)
{
	acifb->idr = (AVR32_ACIFB_IDR_WFINT0_MASK) << (ac_combination);
}

/**
 * \brief Clear ACIFB all interrupt flags.
 *
 * \param acifb         Base address of the ACIFB module
 */
static inline void acifb_clear_all_interrupt_flags(
		volatile avr32_acifb_t *acifb)
{
	acifb->icr = 0xFFFFFFFF;
}

/**
 * \brief Clear ACIFB interrupt flags.
 *
 * \param *acifb         Base address of the ACIFB module
 * \param  interrupt_mask Bitmask of interrupts to clear
 */
static inline void acifb_clear_interrupt_flag(volatile avr32_acifb_t *acifb,
		uint32_t interrupt_mask)
{
	acifb->icr = interrupt_mask;
}

/**
 * \brief Clear ACIFB startup interrupt flags.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_clear_startup_interrupt_flag(
		volatile avr32_acifb_t *acifb,
		uint8_t ac_channel)
{
	acifb->icr = (AVR32_ACIFB_ICR_SUTINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Clear ACIFB comparison complete interrupt flags.
 *
 * \param acifb         Base address of the ACIFB module
 * \param  ac_channel    ACIFB Channel Used
 */
static inline void acifb_clear_comparison_interrupt_flag(
		volatile avr32_acifb_t *acifb,
		uint8_t ac_channel)
{
	acifb->icr = (AVR32_ACIFB_ICR_ACINT0_MASK) << (2 * ac_channel);
}

/**
 * \brief Clear ACIFB comparison window mode interrupt flags.
 *
 * \param acifb           Base address of the ACIFB module
 * \param  ac_combination  ACIFB Channel combination Used
 * \note ACPn, ACNn, ACP(n+1), ACN(n+1) -> nth combination
 */
static inline void acifb_clear_window_interrupt_flag(
		volatile avr32_acifb_t *acifb,
		uint8_t ac_combination)
{
	acifb->icr = (AVR32_ACIFB_ICR_WFINT0_MASK) << (ac_combination);
}

/**
 * @}
 */

#endif  /* _ACIFB_H_ */
