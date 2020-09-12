#include "config.h"
#include "eeprom.h"
#include "languages.h"
#include "tgui_messagebox.h"




MOTOR_CONFIG			cfgzMotor;
SYSTEM_INFO				systemInfo;
GLOBAL_CONFIG			cfgConfig;
WORK_TIME				cfgTimers;
char					cfgCFileName[512];
TCHAR					cfgTFileName[512];

extern char				msg[512];
extern FIL				ufile;
extern FIL				sfile;








void			CFG_Init()
{
	memset(&systemInfo, 0, sizeof(SYSTEM_INFO));
	
	uint16_t	crc = 0;
	uint8_t		*data;

	// motor settings
	data = (uint8_t*)&cfgzMotor;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_MOTORCONFIG, data, sizeof(MOTOR_CONFIG)) == 0)
	{
		CFG_SetMotorDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgzMotor, sizeof(MOTOR_CONFIG));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgzMotor.cfg_version != FW_VERSION || crc != cfgzMotor.cfg_crc)
		{
			switch (cfgzMotor.cfg_version)
			{
				
				default:
					CFG_SetMotorDefault();
			}
			CFG_SaveMotor();
		}
	}

	// global settings
	data = (uint8_t*)&cfgConfig;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_GLOBALCONFIG, data, sizeof(GLOBAL_CONFIG)) == 0)
	{
		CFG_SetConfigDefault();
		EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgConfig.cfg_version != FW_VERSION || crc != cfgConfig.cfg_crc)
		{
			switch (cfgConfig.cfg_version)
			{
				
				default:
					CFG_SetConfigDefault();
			}
			CFG_SaveConfig();
		}
	}

	// work timers
	data = (uint8_t*)&cfgTimers;
	if (EEPROM_ReadMemBuff(EEPR_ADDR_WORKTIME, data, sizeof(WORK_TIME)) == 0)
	{
		memset(&cfgTimers, 0, sizeof(WORK_TIME));
		EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	}
	else
	{
		// calculate crc
		crc = 0;
		for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
		{
			crc += data[i];
		}
		if (cfgTimers.cfg_version != FW_VERSION || crc != cfgTimers.cfg_crc)
		{
			switch (cfgTimers.cfg_version)
			{
				
				default:
					CFG_SetTimersDefault();
			}
			CFG_SaveTimers();
		}
	}

}
//==============================================================================




void			CFG_SetMotorDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgzMotor;
	
	memset(data, 0, sizeof(MOTOR_CONFIG));

	cfgzMotor.cfg_version = FW_VERSION;
	
	cfgzMotor.invert_dir = 1;
	cfgzMotor.home_dir = -1;
	cfgzMotor.home_pos = 0;
	cfgzMotor.min_pos = -3;
	cfgzMotor.max_pos = 180;
	cfgzMotor.min_endstop_inverting = 1;
	cfgzMotor.max_endstop_inverting = 1;
	
	cfgzMotor.steps_per_mm = 1600;
	cfgzMotor.max_feedrate = 100;
	cfgzMotor.max_acceleration = 100;

	cfgzMotor.homing_feedrate_fast = 6;
	cfgzMotor.homing_feedrate_slow = 1;
	cfgzMotor.acceleration = 0.7;
	cfgzMotor.feedrate = 5;
	cfgzMotor.travel_acceleration = 20;
	cfgzMotor.travel_feedrate = 20;
	cfgzMotor.min_feedrate = 0.01;
	cfgzMotor.min_travel_feedrate = 0.01;
	cfgzMotor.max_jerk = 0.05;
	cfgzMotor.current_vref = 800;
	cfgzMotor.current_hold_vref = 300;
	cfgzMotor.hold_time = 10;
	
	for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgzMotor.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveMotor()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgzMotor;
	
	for (uint16_t i = 0; i < sizeof(MOTOR_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgzMotor.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_MOTORCONFIG, (uint8_t*)&cfgzMotor, sizeof(MOTOR_CONFIG));
	
}
//==============================================================================




void			CFG_SetConfigDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgConfig;

	memset(data, 0, sizeof(GLOBAL_CONFIG));
	cfgConfig.cfg_version = FW_VERSION;
	cfgConfig.language = 0;		// english default
	cfgConfig.zero_pos = cfgzMotor.home_pos;
	cfgConfig.pause_lift = 40.0;
	cfgConfig.buzzer = 1;
	cfgConfig.buzzer_msg = 800;
	cfgConfig.buzzer_touch = 50;
	cfgConfig.motor_disable_time = 30;
	cfgConfig.screen_disable_time = 10;
	cfgConfig.screensaver_type = 1;
	
	for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgConfig.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveConfig()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgConfig;
	
	for (uint16_t i = 0; i < sizeof(GLOBAL_CONFIG) - 2; i++)
	{
		crc += data[i];
	}
	cfgConfig.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_GLOBALCONFIG, (uint8_t*)&cfgConfig, sizeof(GLOBAL_CONFIG));
	
}
//==============================================================================




void			CFG_SetTimersDefault()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgTimers;

	memset(&cfgTimers, 0, sizeof(WORK_TIME));
	cfgTimers.cfg_version = FW_VERSION;

	for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
	{
		crc += data[i];
	}
	cfgTimers.cfg_crc = crc;
}
//==============================================================================




void			CFG_SaveTimers()
{
	uint16_t	crc = 0;
	uint8_t		*data;
	data = (uint8_t*)&cfgTimers;
	
	for (uint16_t i = 0; i < sizeof(WORK_TIME) - 2; i++)
	{
		crc += data[i];
	}
	cfgTimers.cfg_crc = crc;
	
	EEPROM_WriteMemBuff(EEPR_ADDR_WORKTIME, (uint8_t*)&cfgTimers, sizeof(WORK_TIME));
	
}
//==============================================================================




void			_cfg_GetParamName(char *src, char *dest, uint16_t maxlen)
{
	if (src == NULL || dest == NULL)
		return;
	
	char *string = src;
	// skip spaces
	while (*string != 0 && maxlen > 0 && (*string == ' ' || *string == '\t' || *string == '\r'))
	{
		string++;
		maxlen--;
	}
	// until first space symbol
	while (maxlen > 0 && *string != 0 && *string != ' ' && *string != '\t' && *string != '\r' && *string != '\n' && *string != '=')
	{
		*dest = *string;
		dest++;
		string++;
		maxlen--;
	}
	
	if (maxlen == 0)
		dest--;
	
	*dest = 0;
	return;
}
//==============================================================================




void			_cfg_GetParamValue(char *src, PARAM_VALUE *val)
{
	val->type = PARAMVAL_NONE;
	val->float_val = 0;
	val->int_val = 0;
	val->uint_val = 0;
	val->char_val = (char*)"";
	
	if (src == NULL)
		return;
	if (val == NULL)
		return;
	
	char *string = src;
	// search '='
	while (*string > 0 && *string != '=')
		string++;
	if (*string == 0)
		return;
	
	// skip '='
	string++;
	// skip spaces
	while (*string != 0 && (*string == ' ' || *string == '\t' || *string == '\r'))
		string++;
	if (*string == 0)
		return;

	// check param if it numeric
	if ((*string > 47 && *string < 58) || *string == '.' || (*string == '-' && (*(string+1) > 47 && *(string+1) < 58) || *(string+1) == '.'))
	{
		val->type = PARAMVAL_NUMERIC;
		val->float_val = (float)atof(string);
		val->int_val = atoi(string);
		val->uint_val = strtoul(string, NULL, 10);
	}
	else
	{
		val->type = PARAMVAL_STRING;
		val->char_val = string;
	}
	
	return;
}
//==============================================================================




void			CFG_LoadFromFile(void *par1, void *par2)
{
	sprintf(msg, LANG_GetString(LSTR_MSG_CFGFILE_LOADING), cfgCFileName);
	TGUI_MessageBoxWait(LANG_GetString(LSTR_WAIT), msg);

	UTF8ToUnicode_Str(cfgTFileName, cfgCFileName, sizeof(cfgTFileName)/2);
	if (f_open(&ufile, cfgTFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
			tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), LANG_GetString(LSTR_MSG_FILE_OPEN_ERROR));
		return;
	}

	uint16_t		cnt = 0;
	uint32_t		readed = 0, totalreaded = 0;
	char			*string = msg;
	char			lexem[128];
	PARAM_VALUE		pval;
	CFGREAD_STATE	rdstate = CFGR_COMMON;
	int16_t			numstr = 0;
	
	while (1)
	{
		// read one string
		cnt = 0;
		readed = 0;
		string = msg;
		while (cnt < sizeof(msg))
		{
			if (f_read(&ufile, string, 1, &readed) != FR_OK || readed == 0 || *string == '\n')
			{
				*string = 0;
				break;
			}
			cnt++;
			string++;
			totalreaded += readed;
		}
		if (cnt == sizeof(msg))
		{
			string--;
			*string = 0;
		}
		numstr++;
		string = msg;
		
		// trim spaces/tabs at begin and end
		strtrim(string);
		
		// if string is empty
		if (*string == 0)
		{
			// if end of file
			if (readed == 0)
				break;
			else
				continue;
		}
		
		// skip comments
		if (*string == '#')
			continue;
		
		// upper all letters
		strupper_utf(string);
		
		// get parameter name
		_cfg_GetParamName(string, lexem, sizeof(lexem));
		
		// check if here section name
		if (*lexem == '[')
		{
			if (strcmp(lexem, (char*)"[ZMOTOR]") == 0)
			{
				rdstate = CFGR_ZMOTOR;
				continue;
			}
			else if (strcmp(lexem, (char*)"[COMMON]") == 0)
			{
				rdstate = CFGR_COMMON;
				continue;
			}
			else
			{
				rdstate = CFGR_ERROR;
				string = LANG_GetString(LSTR_MSG_UNKNOWN_SECTNAME_IN_CFG);
				sprintf(msg, string, numstr);
				break;
			}
		}
		
		// get parameter value
		_cfg_GetParamValue(string, &pval);
		if (pval.type == PARAMVAL_NONE)
		{
			rdstate = CFGR_ERROR;
			string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
			sprintf(msg, string, numstr);
			break;
		}
		
		// check and setup parameter
		switch (rdstate)
		{
			case CFGR_ZMOTOR:
				rdstate = CFGR_ERROR;
				if (*lexem == 'A')
				{
					if (strcmp(lexem, (char*)"ACCELERATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						cfgzMotor.acceleration = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'C')
				{
					if (strcmp(lexem, (char*)"CURRENT_HOLD_VREF") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 100)
							pval.uint_val = 100;
						if (pval.uint_val > 1000)
							pval.uint_val = 1000;
						cfgzMotor.current_hold_vref = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"CURRENT_VREF") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 100)
							pval.uint_val = 100;
						if (pval.uint_val > 1000)
							pval.uint_val = 1000;
						cfgzMotor.current_vref = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'F')
				{
					if (strcmp(lexem, (char*)"FEEDRATE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 40)
							pval.float_val = 40;
						cfgzMotor.feedrate = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'H')
				{
					if (strcmp(lexem, (char*)"HOLD_TIME") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 1)
							pval.uint_val = 1;
						if (pval.uint_val > 100000)
							pval.uint_val = 100000;
						cfgzMotor.hold_time = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOME_DIRECTION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val != -1.0 && pval.int_val != 1.0)
							pval.int_val = -1;
						cfgzMotor.home_dir = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOME_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgzMotor.home_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOMING_FEEDRATE_FAST") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 40)
							pval.float_val = 40;
						cfgzMotor.homing_feedrate_fast = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"HOMING_FEEDRATE_SLOW") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						if (pval.float_val > 40)
							pval.float_val = 40;
						cfgzMotor.homing_feedrate_slow = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'I')
				{
					if (strcmp(lexem, (char*)"INVERT_DIR") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgzMotor.invert_dir = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'M')
				{
					if (strcmp(lexem, (char*)"MAX_ENDSTOP_INVERTING") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgzMotor.max_endstop_inverting = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MAX_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgzMotor.max_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MIN_ENDSTOP_INVERTING") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.int_val < 0 || pval.int_val > 1)
							pval.int_val = 1;
						cfgzMotor.min_endstop_inverting = pval.int_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"MIN_POS") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						cfgzMotor.min_pos = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'S')
				{
					if (strcmp(lexem, (char*)"STEPS_PER_MM") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.uint_val < 1)
							pval.uint_val = 1;
						if (pval.uint_val > 200000)
							pval.uint_val = 200000;
						cfgzMotor.steps_per_mm = pval.uint_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				} else
				if (*lexem == 'T')
				{
					if (strcmp(lexem, (char*)"TRAVEL_ACCELERATION") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						cfgzMotor.travel_acceleration = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
					if (strcmp(lexem, (char*)"TRAVEL_FEEDRATE") == 0)
					{
						if (pval.type != PARAMVAL_NUMERIC)
						{
							string = LANG_GetString(LSTR_MSG_INVALID_PARAMVAL_IN_CFG);
							sprintf(msg, string, numstr);
							break;
						}
						if (pval.float_val < 0.1)
							pval.float_val = 0.1;
						cfgzMotor.travel_feedrate = pval.float_val;
						rdstate = CFGR_ZMOTOR;
						break;
					}
				}

				string = LANG_GetString(LSTR_MSG_UNKNOWN_PARAMNAME_IN_CFG);
				sprintf(msg, string, numstr);
				break;
		}
		if (rdstate == CFGR_ERROR)
			break;
		
		
	}
	f_close(&ufile);
	
	
	if (tguiActiveScreen == (TG_SCREEN*)&tguiMsgBox)
	{
		tguiActiveScreen = (TG_SCREEN*)((TG_MSGBOX*)tguiActiveScreen)->prevscreen;
	}

	if (rdstate == CFGR_ERROR)
	{
		TGUI_MessageBoxOk(LANG_GetString(LSTR_ERROR), msg);
	}
	else
	{
		CFG_SaveMotor();
		TGUI_MessageBoxOk(LANG_GetString(LSTR_COMPLETED), LANG_GetString(LSTR_MSG_CFGFILE_LOADED));
	}
}
//==============================================================================




