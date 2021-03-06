/*******************************************************************************
*
*     Base user interface for LCD with touch
*
********************************************************************************/

#include "tgui_messagebox.h"
#include "tgui_defaultfuncs.h"
#include "unicode_utils.h"
#include "sys_timer.h"
#include "config.h"


extern TG_SCREEN				*tguiActiveScreen;


void		_tgui_MsgBoxShow(MSGBOXTYPE type, char *caption, char *text, pressfunc func_ok, pressfunc func_cancel)
{

	LCDUI_FONT_TYPE	oldfont = LCDUI_SetFont(tguiMsgBox.font_caption);
	uint16_t		oldbcolor = LCDUI_GetCurrentBackColor();
	uint16_t		oldfcolor = LCDUI_GetCurrentColor();
	
	uint16_t		width_button = tguiMsgBox.buttons[0].position.right - tguiMsgBox.buttons[0].position.left + 1;
	uint16_t		height_button = tguiMsgBox.buttons[0].position.bottom - tguiMsgBox.buttons[0].position.top + 1;

	uint16_t		width_capt = LCDUI_GetTextWidth(caption);
	if (width_capt > 420)
		width_capt = 420;
	width_capt += 20;		// 10 pix for left and right sides each
	if (width_capt < width_button * 2 + 40)
		width_capt = width_button * 2 + 40;
	
	LCDUI_SetFont(tguiMsgBox.font_text);
	TSIZE		box_size = {0, 0};
	LCDUI_DrawText(text, LCDUI_TEXT_GETSIZE, 0, 0, -1, -1, &box_size);
	if (box_size.x_size > 420)
		box_size.x_size = 420;
	box_size.x_size += 20;		// 10 pix for left and right sides each
	if (box_size.x_size < width_capt)
		box_size.x_size = width_capt;

	box_size.y_size += tguiMsgBox.caption_height + 20;	// caption, top side space and bottom side
	if (type != MSGBOX_WAIT)
		box_size.y_size += height_button + 10;			// buttons height and space
	if (box_size.y_size > 300)
		box_size.y_size = 300;
	if (box_size.y_size < 130)
		box_size.y_size = 130;
	
	tguiMsgBox.boxpos.left = (LCD_WIDTH - box_size.x_size) / 2;
	tguiMsgBox.boxpos.right = tguiMsgBox.boxpos.left + box_size.x_size - 1;
	tguiMsgBox.boxpos.top = (LCD_HEIGHT - box_size.y_size) / 2;
	tguiMsgBox.boxpos.bottom = tguiMsgBox.boxpos.top + box_size.y_size - 1;

	tguiMsgBox.prevscreen = tguiActiveScreen;
	strncpy(tguiMsgBox.caption, caption, sizeof(tguiMsgBox.caption));
	strncpy(tguiMsgBox.text, text, sizeof(tguiMsgBox.text));

	TG_BUTTON		*btn;
	switch (type)
	{
		case MSGBOX_WAIT:
			tguiMsgBox.buttons[0].options.disabled = 1;
			tguiMsgBox.buttons[1].options.disabled = 1;
			break;
		case MSGBOX_OK:
			btn = &tguiMsgBox.buttons[0];
			btn->position.left = tguiMsgBox.boxpos.left + (box_size.x_size - width_button) / 2;
			btn->position.right = btn->position.left + width_button - 1;
			btn->position.top = tguiMsgBox.boxpos.bottom - 10 - height_button;
			btn->position.bottom = btn->position.top + height_button - 1;
			btn->textposition.left = btn->position.left;
			btn->textposition.right = btn->position.right;
			btn->textposition.top = btn->position.top;
			btn->textposition.bottom = btn->position.bottom;
			btn->text = LSTR_OK;
			if (func_ok != NULL)
				btn->funcs._call_press = func_ok;
			else
				btn->funcs._call_press = NULL;
			tguiMsgBox.buttons[0].options.disabled = 0;
			tguiMsgBox.buttons[1].options.disabled = 1;
			break;

		case MSGBOX_OKCANCEL:
		case MSGBOX_YESNO:
			btn = &tguiMsgBox.buttons[0];
			btn->position.left = tguiMsgBox.boxpos.left + (box_size.x_size - width_button * 2 - 20) / 2;
			btn->position.right = btn->position.left + width_button - 1;
			btn->position.top = tguiMsgBox.boxpos.bottom - 10 - height_button;
			btn->position.bottom = btn->position.top + height_button - 1;
			btn->textposition.left = btn->position.left;
			btn->textposition.right = btn->position.right;
			btn->textposition.top = btn->position.top;
			btn->textposition.bottom = btn->position.bottom;
			if (type == MSGBOX_OKCANCEL)
				btn->text = LSTR_OK;
			else
				btn->text = LSTR_YES;
			if (func_ok != NULL)
				btn->funcs._call_press = func_ok;
			else
				btn->funcs._call_press = NULL;
			
			btn = &tguiMsgBox.buttons[1];
			btn->position.left = tguiMsgBox.boxpos.right - (box_size.x_size - width_button * 2 - 20) / 2 - width_button;
			btn->position.right = btn->position.left + width_button - 1;
			btn->position.top = tguiMsgBox.boxpos.bottom - 10 - height_button;
			btn->position.bottom = btn->position.top + height_button - 1;
			btn->textposition.left = btn->position.left;
			btn->textposition.right = btn->position.right;
			btn->textposition.top = btn->position.top;
			btn->textposition.bottom = btn->position.bottom;
			if (type == MSGBOX_OKCANCEL)
				btn->text = LSTR_CANCEL;
			else
				btn->text = LSTR_NO;
			if (func_cancel != NULL)
				btn->funcs._call_press = func_cancel;
			else
				btn->funcs._call_press = NULL;

			tguiMsgBox.buttons[0].options.disabled = 0;
			tguiMsgBox.buttons[1].options.disabled = 0;
			break;
			break;

	}
	
	
	tguiActiveScreen = (TG_SCREEN*)&tguiMsgBox;
	_tgui_MsgBoxPaint();
}
//==============================================================================




void		_tgui_MsgBoxProcess()
{
	TG_MSGBOX		*thisscr = (TG_MSGBOX*)&tguiMsgBox;
	TOUCH_STATES	ts = Touch_GetState();
	
	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].options.disabled == 0 && thisscr->buttons[i].funcs._call_process != NULL)
			thisscr->buttons[i].funcs._call_process(&(thisscr->buttons[i]), (void*)&ts);
		
		// if active screen has been not changed and item need to repaint
		if (tguiActiveScreen == (TG_SCREEN*)thisscr)
		{
			if (thisscr->buttons[i].options.needrepaint)
			{
				if (thisscr->buttons[i].funcs._call_paint != NULL)
				{
					thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), NULL);
					thisscr->buttons[i].options.needrepaint = 0;
				}
			}
		}
		else
		{
			// current screen has been changed, no more need to check buttons
			break;
		}
	}


}
//==============================================================================




void		_tgui_MsgBoxPaint()
{
	TG_MSGBOX		*thisscr = &tguiMsgBox;
	
	uint16_t oldcolor = LCDUI_SetColor(thisscr->text_color);
	uint16_t oldbackcolor = LCDUI_SetBackColor(thisscr->box_backcolor);
	LCDUI_FONT_TYPE oldfont = LCDUI_SetFont(thisscr->font_caption);
	
	// paint window background
	LCDUI_SetColor(thisscr->box_backcolor);
	LCDUI_FillRoundRect(thisscr->boxpos.left, thisscr->boxpos.top, thisscr->boxpos.right - thisscr->boxpos.left + 1, thisscr->boxpos.bottom - thisscr->boxpos.top + 1, 3);
	LCDUI_SetColor(thisscr->capt_backcolor);
	LCDUI_DrawRoundRect(thisscr->boxpos.left, thisscr->boxpos.top, thisscr->boxpos.right - thisscr->boxpos.left, thisscr->boxpos.bottom - thisscr->boxpos.top + 1, 3);
	LCDUI_DrawRoundRect(thisscr->boxpos.left-1, thisscr->boxpos.top - 1, thisscr->boxpos.right - thisscr->boxpos.left + 2, thisscr->boxpos.bottom - thisscr->boxpos.top + 3, 5);
	
	// paint caption background
	LCDUI_SetColor(thisscr->capt_backcolor);
	LCDUI_FillRoundRect(thisscr->boxpos.left, thisscr->boxpos.top, thisscr->boxpos.right - thisscr->boxpos.left + 1, thisscr->caption_height, 4);
	
	// paint caption text
	LCDUI_SetColor(thisscr->capt_textcolor);
	LCDUI_SetBackColor(thisscr->capt_backcolor);
	LCDUI_SetFont(thisscr->font_caption);
	LCDUI_DrawText(thisscr->caption, LCDUI_TEXT_ALIGN_CENTER, thisscr->boxpos.left, thisscr->boxpos.top + (thisscr->caption_height - LCDUI_GetCurrentFontHeight()) / 2, thisscr->boxpos.right);

	// paint message text
	LCDUI_SetColor(thisscr->text_color);
	LCDUI_SetBackColor(thisscr->box_backcolor);
	LCDUI_SetFont(thisscr->font_text);
	LCDUI_DrawText(thisscr->text, LCDUI_TEXT_ALIGN_CENTER, thisscr->boxpos.left, thisscr->boxpos.top + thisscr->caption_height + 10, thisscr->boxpos.right);


	for (uint8_t i = 0; i < thisscr->btns_count; i++)
	{
		if (thisscr->buttons[i].options.disabled == 0)
			thisscr->buttons[i].funcs._call_paint(&(thisscr->buttons[i]), 0);
		thisscr->buttons[i].options.needrepaint = 0;
	}

	LCDUI_SetColor(oldcolor);
	LCDUI_SetBackColor(oldbackcolor);
	LCDUI_SetFont(oldfont);
}
//==============================================================================




void		_tgui_MsgBoxButtonProcess(void *tguiobj, void *param)
{
	TG_BUTTON		*thisbtn = (TG_BUTTON*)tguiobj;
	TOUCH_STATES	*ts = (TOUCH_STATES*)param;;
	TOUCH_POINT		tp;
	
	if (*ts > TS_PREPRESSED)
	{
		Touch_GetCoords(&tp);
		switch (*ts)
		{
			case TS_SPRESSED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 0)
				{
					if (TGUI_PointInRect(&tp, &thisbtn->position) == 1)
					{
						BUZZ_TimerOn(cfgConfig.buzzer_touch);
						thisbtn->options.pressed = 1;
						if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
						{
							thisbtn->funcs._call_paint((void*)thisbtn, NULL);
							thisbtn->options.needrepaint = 0;
						}
						Touch_SetWorked(TS_SPRESSED);
					}
				}
				break;

			case TS_LPRESSED:
				Touch_SetWorked(TS_LPRESSED);
				break;

			case TS_SRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					// paint
					if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
					{
						thisbtn->funcs._call_paint((void*)thisbtn, NULL);
						thisbtn->options.needrepaint = 0;
					}
					// current messagebox is closed anyway
					thisbtn->options.pressed = 0;
					tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
					// call linked function or predefined action
					if (thisbtn->funcs._call_press != NULL)
					{
						// call linked function or predefined action
						thisbtn->funcs._call_press((void*)thisbtn, NULL);
					}
					TGUI_ForceRepaint();
					
					Touch_SetWorked(TS_SRELEASED);
				}
				break;

			case TS_LRELEASED:
				if (thisbtn->options.disabled == 0 && thisbtn->options.pressed == 1)
				{
					thisbtn->options.pressed = 0;
					if (thisbtn->options.repaintonpress == 1 && thisbtn->funcs._call_paint != NULL)
					{
						thisbtn->funcs._call_paint((void*)thisbtn, NULL);
						thisbtn->options.needrepaint = 0;
					}
					Touch_SetWorked(TS_LRELEASED);
				}
				break;
		}
	}
}
//==============================================================================




