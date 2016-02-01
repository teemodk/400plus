#include "firmware.h"

#include "main.h"

#include "autoiso.h"
#include "display.h"
#include "persist.h"
#include "settings.h"
#include "scripts.h"
#include "utils.h"
#include "intercom.h"

#include "shortcuts.h"

void toggle_CfMLU           (void);
void toggle_CfEmitFlash     (void);
void toggle_CfFlashSyncRear (void);
void toggle_AEB             (void);

void repeat_last_script     (void);

void shortcut_start(shortcut_action_t action);

void shortcut_iso_toggle (void);
void shortcut_iso_set    (iso_t iso);

#ifdef DEV_BTN_ACTION
void dev_btn_action() {
	// quick shortcut for developers to test stuff
	beep();
	ptp_dump_info();
}
#endif

void toggle_CfMLU() {
	char message[LP_MAX_WORD];

	send_to_intercom(IC_SET_CF_MIRROR_UP_LOCK, !DPData.cf_mirror_up_lock);

	sprintf(message, "%s: %s", LP_WORD(L_A_MIRROR_LOCKUP), DPData.cf_mirror_up_lock ? LP_WORD(L_A_YES) : LP_WORD(L_A_NO));
	display_message_set(message, ACTION_MSG_TIMEOUT);
}

void toggle_CfEmitFlash() {
	char message[LP_MAX_WORD];

	send_to_intercom(IC_SET_CF_EMIT_FLASH, !DPData.cf_emit_flash);

	sprintf(message, "%s: %s",LP_WORD(L_A_FLASH),(DPData.cf_emit_flash ? LP_WORD(L_A_NO) : LP_WORD(L_A_YES)));
	display_message_set(message, ACTION_MSG_TIMEOUT);
}

void toggle_CfFlashSyncRear() {
	send_to_intercom(IC_SET_CF_FLASH_SYNC_REAR, !DPData.cf_flash_sync_rear);
}

void toggle_AEB() {
	int aeb;
	static int last_toggle = 0;

	if (timestamp() - last_toggle < ACTION_AEB_TIMEOUT)
		// Button was pressed recently: roll over all range
		aeb = (EV_TRUNC(DPData.ae_bkt) + EV_CODE(1, 0)) % EV_CODE(6, 0);
	else if (DPData.ae_bkt)
		// Button was pressed long ago, and AEB is on: switch it off
		aeb = EC_ZERO;
	else
		// Button was pressed long ago, and AEB is off: switch it on
		aeb = persist.last_aeb;

	send_to_intercom(IC_SET_AE_BKT, aeb);

	persist.aeb = aeb;

	if (persist.aeb)
		persist.last_aeb = persist.aeb;

	enqueue_action(persist_write);
	last_toggle = timestamp();
}

void repeat_last_script(void) {
	switch (persist.last_script) {
	case SCRIPT_EXT_AEB:
		script_ext_aeb();
		break;
	case SCRIPT_EFL_AEB:
		script_efl_aeb();
		break;
	case SCRIPT_ISO_AEB:
		script_iso_aeb();
		break;
	case SCRIPT_INTERVAL:
		script_interval();
		break;
	case SCRIPT_WAVE:
		script_wave();
		break;
	case SCRIPT_TIMER:
		script_self_timer();
		break;
	default:
		break;
	}
}

void shortcut_jump() {
	shortcut_start(settings.shortcut_jump);
}

void shortcut_trash() {
	shortcut_start(settings.shortcut_trash);
}

void shortcut_start(shortcut_action_t action) {
	status.shortcut_running = action;

	switch (action) {
	case SHORTCUT_ACTION_ISO:
		break;
	case SHORTCUT_ACTION_SCRIPT:
		repeat_last_script();
		break;
	case SHORTCUT_ACTION_MLU:
		toggle_CfMLU();
		break;
	case SHORTCUT_ACTION_AEB:
		toggle_AEB();
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		menu_main_start();
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		toggle_CfEmitFlash();
		break;
#ifdef DEV_BTN_ACTION
	case SHORTCUT_ACTION_DEV_BTN:
		dev_btn_action();
		break;
#endif
	default:
		break;
	}
}

void shortcut_event_end() {
	status.shortcut_running = SHORTCUT_ACTION_NONE;
}

void shortcut_event_set   (void) {
	switch (status.shortcut_running) {
	case SHORTCUT_ACTION_ISO:
		shortcut_iso_toggle();
		break;
	case SHORTCUT_ACTION_SCRIPT:
		break;
	case SHORTCUT_ACTION_MLU:
		break;
	case SHORTCUT_ACTION_AEB:
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		break;
	default:
		break;
	}
}

void shortcut_event_up    (void) {
	switch (status.shortcut_running) {
	case SHORTCUT_ACTION_ISO:
		shortcut_iso_set(iso_next(DPData.iso));
		break;
	case SHORTCUT_ACTION_SCRIPT:
		break;
	case SHORTCUT_ACTION_MLU:
		break;
	case SHORTCUT_ACTION_AEB:
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		break;
	default:
		break;
	}
}

void shortcut_event_down  (void) {
	switch (status.shortcut_running) {
	case SHORTCUT_ACTION_ISO:
		shortcut_iso_set(iso_prev(DPData.iso));
		break;
	case SHORTCUT_ACTION_SCRIPT:
		break;
	case SHORTCUT_ACTION_MLU:
		break;
	case SHORTCUT_ACTION_AEB:
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		break;
	default:
		break;
	}
}

void shortcut_event_right (void) {
	switch (status.shortcut_running) {
	case SHORTCUT_ACTION_ISO:
		shortcut_iso_set(iso_inc(DPData.iso));
		break;
	case SHORTCUT_ACTION_SCRIPT:
		break;
	case SHORTCUT_ACTION_MLU:
		break;
	case SHORTCUT_ACTION_AEB:
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		break;
	default:
		break;
	}
}

void shortcut_event_left  (void) {
	switch (status.shortcut_running) {
	case SHORTCUT_ACTION_ISO:
		shortcut_iso_set(iso_dec(DPData.iso));
		break;
	case SHORTCUT_ACTION_SCRIPT:
		break;
	case SHORTCUT_ACTION_MLU:
		break;
	case SHORTCUT_ACTION_AEB:
		break;
	case SHORTCUT_ACTION_HACK_MENU:
		break;
	case SHORTCUT_ACTION_TOGGLE_FLASH:
		break;
	default:
		break;
	}
}

void shortcut_iso_toggle() {
	char label[8] = "AUTO";

	settings.autoiso_enable = ! settings.autoiso_enable;
	enqueue_action(settings_write);

	if (!settings.autoiso_enable)
		iso_print(label, DPData.iso);

	dialog_item_set_label(hMainDialog, 0x08, label, 4, 0x04);
	display_refresh();
}

void shortcut_iso_set(iso_t iso) {
	char label[8] = "AUTO";

	if (settings.autoiso_enable) {
		settings.autoiso_enable = FALSE;
		enqueue_action(settings_write);
	}

	send_to_intercom(IC_SET_ISO, iso);
	iso_print(label, iso);

	dialog_item_set_label(hMainDialog, 0x08, label, 4, 0x04);
	display_refresh();
}
