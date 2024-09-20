#include <stdint.h>

#include <furi.h>

#include <gblink/include/gblink.h>
#include "printer_i.h"

#define TAG "printer_receive"

/* NOTE:
 * These numbers are empirically gathered from a few different games thus far.
 * There are many notes floating around the internet of the GB Printer having
 * a 100 ms limit between packets where it will reset. However, I've seen
 * Pokemon Pinball wait 99.5 ms between packets after a print command which is
 * a bit too close for comfort. As this code tracks timestamps _after_ each byte,
 * that ends up just over 110 ms which trips the hard timeout and resets state.
 * This often cofuses the hell out of games.
 *
 * Additionally, on the other end of the spectrum, Pokemon Gold absolutely uses
 * the hard timeout to reset the printer between packets. It waits ~278 ms, when
 * it could just send an init command.
 *
 * Even more silly, Pokemon Pinball has a fun quirk where if the print completes
 * immediately (usually the Flipper will mark a print complete with a single
 * packet turnaround), it asks for status a couple of times, then starts (presumably)
 * another status packet, but the second byte in the transfer is stopped mid-byte.
 * Between that point and the start of the next, real packet, is 30 ms, 32.6 ms
 * if you go from end of last byte received to start of next, real packet.
 *
 * This means there is some "soft" timeout that the printer uses to reset a packet
 * transfer in progress, but don't reset the whole printer state.
 *
 * There are wisps of some "unknown" bit timeout of 1.49 ms. But I've not yet
 * seen that in action.
 *
 * As far as I know, no one has dumped and reverse engineered the ROM of the
 * Game Boy Printer directly. I think all of the existing documentation was from
 * reverse engineering the communication channel. Maybe someday I'll dump the
 * GB Printer ROM and try to better understand all of it.
 */
#define HARD_TIMEOUT_US 125000
#define SOFT_TIMEOUT_US 20000

static void printer_reset(struct printer_proto *printer)
{
	/* Clear out the current packet data */
	memset(printer->packet, '\0', sizeof(struct packet));

	printer->image->data_sz = 0;
	/* This is technically redundant, done for completeness */
	printer->packet->state = START_L;

	/* Packet timeout start */
	printer->packet->time = DWT->CYCCNT;
}

static void byte_callback(void *context, uint8_t val)
{
	struct printer_proto *printer = context;
	struct packet *packet = printer->packet;
	const uint32_t time_ticks = furi_hal_cortex_instructions_per_microsecond() * HARD_TIMEOUT_US;
	uint8_t data_out = 0x00;

	if ((DWT->CYCCNT - packet->time) > time_ticks)
		printer_reset(printer);

	if ((DWT->CYCCNT - packet->time) > furi_hal_cortex_instructions_per_microsecond() * SOFT_TIMEOUT_US)
		packet->state = START_L;

	/* Packet timeout restart */
	packet->time = DWT->CYCCNT;

	/* TODO: flash led? */

	switch (packet->state) {
	case START_L:
		if (val == PKT_START_L) {
			packet->state = START_H;
			packet->zero_counter = 0;
		}
		if (val == 0x00) {
			packet->zero_counter++;
			if (packet->zero_counter == 16)
				printer_reset(printer);
		}
		break;
	case START_H:
		if (val == PKT_START_H)
			packet->state = COMMAND;
		else
			packet->state = START_L;
		break;
	case COMMAND:
		packet->cmd = val;
		packet->state = COMPRESS;
		packet->cksum_calc += val;
		break;
	case COMPRESS:
		packet->cksum_calc += val;
		packet->state = LEN_L;
		if (val) {
			FURI_LOG_E(TAG, "Compression not supported!");
			packet->status |= STATUS_PKT_ERR;
		}
		break;
	case LEN_L:
		packet->cksum_calc += val;
		packet->state = LEN_H;
		packet->len = (val & 0xff);
		break;
	case LEN_H:
		packet->cksum_calc += val;
		packet->len |= ((val & 0xff) << 8);
		/* Override length for a TRANSFER */
		if (packet->cmd == CMD_TRANSFER)
			packet->len = TRANSFER_RECV_SZ;

		if (packet->len) {
			packet->state = COMMAND_DAT;
		} else {
			packet->state = CKSUM_L;
		}
		break;
	case COMMAND_DAT:
		packet->cksum_calc += val;
		packet->recv_data[packet->recv_data_sz] = val;
		packet->recv_data_sz++;
		if (packet->recv_data_sz == packet->len) 
			packet->state = CKSUM_L;
		break;
	case CKSUM_L:
		packet->state = CKSUM_H;
		packet->cksum = (val & 0xff);
		break;
	case CKSUM_H:
		packet->state = ALIVE;
		packet->cksum |= ((val & 0xff) << 8);
		if (packet->cksum != packet->cksum_calc)
			packet->status |= STATUS_CKSUM;
		// TRANSFER does not set checksum bytes
		if (packet->cmd == CMD_TRANSFER)
			packet->status &= ~STATUS_CKSUM;
		data_out = PRINTER_ID;
		break;
	case ALIVE:
		packet->state = STATUS;
		data_out = packet->status;
		break;
	case STATUS:
		packet->state = START_L;
		switch (packet->cmd) {
		case CMD_INIT:
			printer_reset(printer);
			break;
		case CMD_DATA:
			if (printer->image->data_sz < PRINT_FULL_SZ) {
				if ((printer->image->data_sz + packet->len) <= PRINT_FULL_SZ) {
					memcpy((printer->image->data)+printer->image->data_sz, packet->recv_data, packet->len);
					printer->image->data_sz += packet->len;
				} else {
					memcpy((printer->image->data)+printer->image->data_sz, packet->recv_data, ((printer->image->data_sz + packet->len)) - PRINT_FULL_SZ);
					printer->image->data_sz += (PRINT_FULL_SZ - (printer->image->data_sz + packet->len));
					furi_assert(printer->image->data_sz <= PRINT_FULL_SZ);
				}
			}

			/* Any time data is written to the buffer, READY is set */
			packet->status |= STATUS_READY;

			furi_thread_flags_set(printer->thread, THREAD_FLAGS_DATA);
			break;
		case CMD_TRANSFER:
			/* XXX: TODO: Check to see if we're still printing when getting
			 * a transfer command. If so, then we have failed to beat the clock.
			 */
		case CMD_PRINT:
			/* TODO: Be able to memcpy these */
			printer->image->num_sheets = packet->recv_data[0];
			printer->image->margins = packet->recv_data[1];
			printer->image->palette = packet->recv_data[2];
			printer->image->exposure = packet->recv_data[3];
			packet->status &= ~STATUS_READY;
			packet->status |= (STATUS_PRINTING | STATUS_FULL);
			furi_thread_flags_set(printer->thread, THREAD_FLAGS_PRINT);
			break;
		case CMD_STATUS:
			/* READY cleared on status request */
			packet->status &= ~STATUS_READY;
			if ((packet->status & STATUS_PRINTING) &&
			    (packet->status & STATUS_PRINTED)) {
				packet->status &= ~(STATUS_PRINTING | STATUS_PRINTED);
				furi_thread_flags_set(printer->thread, THREAD_FLAGS_COMPLETE);
			}
		}

		packet->recv_data_sz = 0;
		packet->cksum_calc = 0;


		/* XXX: TODO: if the command had something we need to do, do it here. */
		/* done! flush our buffers, deal with any status changes like
		 * not printing -> printing -> not printing, etc.
		 */
		/* Do a callback here?
		 * if so, I guess we should wait for callback completion before accepting more recv_data?
		 * but that means the callback is in an interrupt context, which, is probably okay?
		 */
		/* XXX: TODO: NOTE: FIXME:
		 * all of the notes..
		 * This module needs to maintain the whole buffer, but it can be safely assumed that the buffer
		 * will never exceed 20x18 tiles (no clue how many bytes) as that is the max the printer can
		 * take on in a single print. Printing mulitples needs a print, and then a second print with
		 * no margin. So the margins are important and need to be passed to the final application,
		 * SOMEHOW.
		 *
		 * More imporatntly, is the completed callback NEEDS to have a return value. This allows
		 * the end application to take that whole panel, however its laid out, and do whatever
		 * it wants to do with it. Write it to a file, convert, etc., etc., so that this module
		 * will forever return that it is printing until the callback returns true.
		 *
		 * Once we call the callback and it shows a true, then we can be sure the higher module
		 * is done with the buffer, and we can tell the host that yes, its done, you can continue
		 * if you want.
		 */
		/* XXX: On TRANSFER, there is no checking of status, it is only two packets in total.
		 * I can assume that if we delay a bit in moving the buffer around that should be okay
		 * but we probably don't want to wait too long.
		 * Upon testing, transfer seems to doesn't 
		 */
		break;
	default:
		FURI_LOG_E(TAG, "unknown status!");
		break;
	}

	/* transfer next byte */
	gblink_transfer(printer->gblink_handle, data_out);
}

void printer_receive_start(void *printer_handle)
{
	struct printer_proto *printer = printer_handle;

	/* Set up defaults the receive path needs */
	gblink_callback_set(printer->gblink_handle, byte_callback, printer);
	gblink_clk_source_set(printer->gblink_handle, GBLINK_CLK_EXT);

	printer_reset(printer);

	gblink_start(printer->gblink_handle);
}

void printer_receive_print_complete(void *printer_handle)
{
	struct printer_proto *printer = printer_handle;

	printer->packet->status |= STATUS_PRINTED;
}
