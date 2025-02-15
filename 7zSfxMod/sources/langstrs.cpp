/*---------------------------------------------------------------------------*/
/* File:        langstrs.cpp                                                 */
/* Created:     Wed, 10 Jan 2007 23:44:00 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Last update: Sun, 10 Jan 2016 10:19:09 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Revision:    3287                                                         */
/*---------------------------------------------------------------------------*/
/* Revision:    1404                                                         */
/* Updated:     Sun, 14 Nov 2010 01:04:46 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Add 'password' strings                                       */
/*---------------------------------------------------------------------------*/
/* Revision:    1263                                                         */
/* Updated:     Sat, 26 Jun 2010 04:44:01 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: Add 'copyright' strings                                      */
/*---------------------------------------------------------------------------*/
/* Revision:    1167                                                         */
/* Updated:     Mon, 22 Mar 2010 11:17:07 GMT                                */
/*              by Oleg N. Scherbakov, mailto:oleg@7zsfx.info                */
/* Description: New file stamp.                                              */
/*---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "7zSfxModInt.h"
#include "langstrs.h"
#include "version.h"

/* RU-russian */
const UINT SfxSecondaryLangId = 0x419;

LANGSTRING SfxLangStrings[] = {
// бЕПЯХЪ ЛНДСКЪ
	{ STR_SFXVERSION,		"SFX module - Copyright (c) 2005-2016 Oleg Scherbakov\n"
							"\t" VERSION_SFX_TEXT_EN "\n"
							"\n7-Zip archiver - Copyright (c) 1999-2015 Igor Pavlov\n"
							"\t" VERSION_7ZIP_TEXT_EN "\n"
							"\nSupported methods and filters, build options:\n\t",
						"SFX ЛНДСКЭ - Copyright (c) 2005-2016 нКЕЦ ыЕПАЮЙНБ\n"
						"\t" VERSION_SFX_TEXT_RU "\n"
						"\nюПУХБЮРНП 7-Zip - Copyright (c) 1999-2015 хЦНПЭ оЮБКНБ\n"
						"\t" VERSION_7ZIP_TEXT_RU "\n"
						"\nоНДДЕПФХБЮЕЛШЕ ЛЕРНДШ Х ТХКЭРПШ, НОЖХХ ЯАНПЙХ:\n\t", NULL },
// гЮЦНКНБНЙ НЙНМ ЯННАЫЕМХИ ОН СЛНКВЮМХЧ, НРНАПЮФЮЕЛНЕ, ЕЯКХ
// ЛНДСКЭ МЕ ЯЛНЦ НОПЕДЕКХРЭ ХЛЪ exe-ТЮИКЮ.
// еЯКХ SFX ЛНДСКЭ ЯЛНЦ ПЮЯОНГМЮРЭ ЯБНЕ ХЛЪ (Ю ЩРН ОПЮЙРХВЕЯЙХ 100%), БЛЕЯРН МЕЦН
// АСДЕР БШБНДХРЯЪ ХЛЪ exe-ТЮИКЮ АЕГ ПЮЯЬХПЕМХЪ. хКХ, ЕЯКХ ТЮИК ЙНМТХЦСПЮЖХХ
// СЯОЕЬМН ОПНВХРЮМ Х Б МЕЛ СЙЮГЮМ "Title" - ЯНДЕПФХЛНЕ "Title"
	{ STR_TITLE,			"7z SFX",
							"7z SFX", NULL },
	{ STR_ERROR_TITLE,		"7z SFX: error",
							"7z SFX: НЬХАЙЮ", NULL },
// дЮММЮЪ ОНДЯРПНЙЮ АСДЕР ОПХЯНЕДХМЕМЮ Й НЯМНБМНЛС ГЮЦНКНБЙС ДКЪ НЙНМ НЬХАНЙ
// р.Е. ГЮЦНКНБНЙ НЙНМ АСДЕР ВРН-РН БПНДЕ "7-Zip SFX: error"
	{ STR_ERROR_SUFFIX,		": error",
							": НЬХАЙЮ", NULL },
// гЮЦНКНБНЙ ОН СЛНКВЮМХЧ ДКЪ НЙМЮ ПЮЯОЮЙНБЙХ
	{ STR_EXTRACT_TITLE,	"Extracting",
							"пЮЯОЮЙНБЙЮ", NULL },
// мЕБНГЛНФМН ОНКСВХРЭ ХЛЪ ТЮИКЮ SFX СЯРЮМНБЙХ. рЕНПХРХВЕЯЙХ, МХЙНЦДЮ МЕ ОНЪБХРЯЪ,
// Ъ ДЮФЕ МЕ ГМЮЧ, ЙЮЙ ЯЩЛСКХПНБЮРЭ ЩРС ЯХРСЮЖХЧ.
	{ ERR_MODULEPATHNAME,	"Could not get SFX filename.",
							"мЕБНГЛНФМН ОНКСВХРЭ ХЛЪ SFX ЮПУХБЮ.", NULL },
// мЕБНГЛНФМН НРЙПШРЭ ТЮИК ЮПУХБЮ
// лНФЕР БНГМХЙМСРЭ, ЕЯКХ ТЮИК АКНЙХПНБЮМ ДПСЦХЛ ОПХКНФЕМХЕЛ, МЕДНЯРЮРНВМН ОПЮБ Х Р.О.
// бРНПЮЪ ЯРПНЙЮ АСДЕР ЯНДЕПФЮРЭ рейяр ЯХЯРЕЛМНИ НЬХАЙХ мю ъгшйе, БШЯРЮБКЕММНЛ Б ЯХЯРЕЛЕ ОН СЛНКВЮМХЧ
	{ ERR_OPEN_ARCHIVE,		"Could not open archive file \"%s\".",
							"мЕБНГЛНФМН НРЙПШРЭ ТЮИК ЮПУХБЮ \"%s\".", NULL },
// юПУХБ ОНБПЕФДЕМ/МЕ МЮИДЕМЮ ЯХЦМЮРСПЮ, Р.Е БМСРПЕММХЕ ЯРПСЙРСПШ МЕ ЯННРБЕРЯРБСЧР ТНПЛЮРС 7-Zip SFX
// лНФЕР БНГМХЙМСРЭ МЮ "ЙНПЪБШУ" ХКХ ОНБПЕФДЕММШУ 7-Zip SFX СЯРЮМНБЙЮУ
	{ ERR_NON7Z_ARCHIVE,	"Non 7z archive.",
							"тЮИК МЕ ЪБКЪЕРЯЪ 7z ЮПУХБНЛ.", NULL },
// мЕБНГЛНФМН ОПНВХРЮРЭ ТЮИК ЙНМТХЦСПЮЖХХ (ХКХ НМ НРЯСРЯРБСЕР)
	{ ERR_READ_CONFIG,		"Could not read SFX configuration or configuration not found.",
							"мЕБНГЛНФМН ОПНВХРЮРЭ ХКХ НРЯСРЯРБСЕР ТЮИК ЙНМТХЦСПЮЖХХ SFX ЮПУХБЮ.", NULL },
	{ ERR_WRITE_CONFIG,		"Could not write SFX configuration.",
							"мЕБНГЛНФМН ЯНУПЮМХРЭ ТЮИК ЙНМТХЦСПЮЖХХ.", NULL },
// нЬХАЙЮ Б ТЮИКЕ ЙНМТХЦСПЮЖХХ, НРЯСРЯРБХЕ НАЪГЮРЕКЭМШУ ЙЮБШВЕЙ, МЕЯННРБЕРЯРБХЕ
// ТНПЛЮРС оЮПЮЛЕРП="гМЮВЕМХЕ", ГМЮВЕМХЕ ОЮПЮЛЕРПЮ МЕ Б UTF8 Х Р.О.
// "нЬХАЙЮ Б ЯРПНЙЕ у ЙНМТХЦСПЮЖХХ"
	{ ERR_CONFIG_DATA,		"Error in line %d of configuration data:\n\n%s",
							"нЬХАЙЮ Б ЯРПНЙЕ %d ТЮИКЮ ЙНМТХЦСПЮЖХХ:\n\n%s", NULL },
// мЕБНГЛНФМН ЯНГДЮРЭ ОЮОЙС "РЮЙСЧ-РН"
// бРНПЮЪ ЯРПНЙЮ АСДЕР ЯНДЕПФЮРЭ рейяр ЯХЯРЕЛМНИ НЬХАЙХ мю ъгшйе, БШЯРЮБКЕММНЛ Б ЯХЯРЕЛЕ ОН СЛНКВЮМХЧ
	{ ERR_CREATE_FOLDER,	"Could not create folder \"%s\".",
							"мЕБНГЛНФМН ЯНГДЮРЭ ОЮОЙС \"%s\".", NULL },
// мЕБНГЛНФМН СДЮКХРЭ ТЮИК ХКХ ДХПЕЙРНПХЧ "РЮЙНИ-РН-РЮЙЮЪ-РН"
// бРНПЮЪ ЯРПНЙЮ АСДЕР ЯНДЕПФЮРЭ рейяр ЯХЯРЕЛМНИ НЬХАЙХ мю ъгшйе, БШЯРЮБКЕММНЛ Б ЯХЯРЕЛЕ ОН СЛНКВЮМХЧ
	{ ERR_DELETE_FILE,		"Could not delete file or folder \"%s\".",
							"мЕБНГЛНФМН СДЮКХРЭ ТЮИК ХКХ ОЮОЙС \"%s\".", NULL },
// мЕБНГЛНФМН МЮИРХ ЙНЛЮМДС ДКЪ "AutoInstallX"
// лНФЕР БШКЕГРХ, ЙНЦДЮ ОНКЭГНБЮРЕКЭ СЙЮГЮК Б ЙНЛЮМДМНИ ЯРПНЙЕ -aiX ХКХ -ai,
// Ю AutoInstall Я ХМДЕЙЯНЛ X (ХКХ АЕГ ХМДЕЙЯЮ ДКЪ -ai) Б ЙНМТХЦЕ МЕ МЮИДЕМ
	{ ERR_AUTOINSTALL_NOTFOUND,	"Could not find command for \"%s\".",
								"оЮПЮЛЕРП \"%s\" МЕ МЮИДЕМ Б ТЮИКЕ ЙНМТХЦСПЮЖХХ.", NULL },
// мЕ СЙЮГЮМШ МХ "RunProgram", МХ "AutoInstall", Ю ПЮЯОЮЙНБЙЮ ДНКФМЮ ОПНХЯУНДХР
// БН БПЕЛЕММСЧ ОЮОЙС. ю ВЕЦН УНРЕКХ-РН? пЮЯОЮЙНБЮРЭ Х СДЮКХРЭ?
	{ ERR_NO_SETUP_EXE,		"Could not find \"setup.exe\".",
							"тЮИК \"setup.exe\" МЕ МЮИДЕМ.", NULL },
// оПНХГНЬКЮ НЬХАЙЮ ОПХ БШОНКМЕМХХ "РЮЙНИ-РН ОПНЦПЮЛЛШ"
// бРНПЮЪ ЯРПНЙЮ АСДЕР ЯНДЕПФЮРЭ рейяр ЯХЯРЕЛМНИ НЬХАЙХ мю ъгшйе, БШЯРЮБКЕММНЛ Б ЯХЯРЕЛЕ ОН СЛНКВЮМХЧ
	{ ERR_EXECUTE,			"Error during execution \"%s\".",
							"оПНХГНЬКЮ НЬХАЙЮ ОПХ БШОНКМЕМХХ \"%s\".", NULL },
// нЬХАЙХ ЪДПЮ ПЮЯОЮЙНБЙХ
	{ ERR_7Z_UNSUPPORTED_METHOD,	"7-Zip: Unsupported method.",
									"7-Zip: лЕРНД МЕ ОНДДЕПФХБЮЕРЯЪ.", NULL },
	{ ERR_7Z_CRC_ERROR,				"7-Zip: CRC error.",
									"7-Zip: нЬХАЙЮ ЙНМРПНКЭМНИ ЯСЛЛШ (CRC).", NULL },
	{ ERR_7Z_DATA_ERROR,			"7-Zip: Data error.\nThe archive is corrupted"
#ifdef SFX_CRYPTO
									", or invalid password was entered"
#endif // SFX_CRYPTO
									".",
									"7-Zip: нЬХАЙЮ ДЮММШУ.\nюПУХБ ОНБПЕФДЕМ"
#ifdef SFX_CRYPTO
									" ХКХ ББЕДЕМ МЕБЕПМШИ ОЮПНКЭ"
#endif // SFX_CRYPTO
									".",
									NULL },
	{ ERR_7Z_INTERNAL_ERROR,		"7-Zip: Internal error, code %u.",
									"7-Zip: бМСРПЕММЪЪ НЬХАЙЮ. йНД НЬХАЙХ %u.", NULL },
	{ ERR_7Z_EXTRACT_ERROR1,		"7-Zip: Internal error, code 0x%08X.",
									"7-Zip: бМСРПЕММЪЪ НЬХАЙЮ. йНД НЬХАЙХ 0x%08X.", NULL },
	{ ERR_7Z_EXTRACT_ERROR2,		"7-Zip: Extraction error.",
									"7-Zip: нЬХАЙЮ ПЮЯОЮЙНБЙХ.", NULL },

	{ STR_EXTRACT_PATH_TITLE,		"Extraction path",
									"оСРЭ ПЮЯОЮЙНБЙХ", NULL },
	{ STR_EXTRACT_PATH_TEXT,		"Extraction path:",
									"оСРЭ ПЮЯОЮЙНБЙХ:", NULL },
	{ STR_CANCEL_PROMPT,			"Really cancel the installation?",
									"бШ ДЕИЯРБХРЕКЭМН УНРХРЕ НРЛЕМХРЭ СЯРЮМНБЙС?", NULL },
// мЕР БЯРПНЕММНИ ЯОПЮБЙХ (ОЮПЮЛЕРП "HelpText")
	{ STR_DEFAULT_HELP_TEXT,	"No \"HelpText\" in the configuration file.",
								"оЮПЮЛЕРП \"HelpText\" МЕ СЙЮГЮМ Б ТЮИКЕ ЙНМТХЦСПЮЖХХ.", NULL },
// йМНОЙХ ДХЮКНЦНБ
	{ STR_BUTTON_OK,			"OK",		"OK", NULL },
	{ STR_BUTTON_CANCEL,		"Cancel",	"нРЛЕМЮ", NULL },
	{ STR_BUTTON_YES,			"Yes",		"дЮ", NULL },
	{ STR_BUTTON_NO,			"No",		"мЕР", NULL },
	{ STR_SECONDS,				" s",		" Я", NULL },
// Added April 9, 2008
// мЕБНГЛНФМН ЯНГДЮРЭ ТЮИК "РЮЙНИ-РН"
// бРНПЮЪ ЯРПНЙЮ АСДЕР ЯНДЕПФЮРЭ рейяр ЯХЯРЕЛМНИ НЬХАЙХ мю ъгшйе, БШЯРЮБКЕММНЛ Б ЯХЯРЕЛЕ ОН СЛНКВЮМХЧ
	{ ERR_CREATE_FILE,		"Could not create file \"%s\".",
							"мЕБНГЛНФМН ЯНГДЮРЭ ТЮИК \"%s\".", NULL },

	{ ERR_OVERWRITE,		"Could not overwrite file \"%s\".",
							"мЕБНГЛНФМН ОЕПЕГЮОХЯЮРЭ ТЮИК \"%s\".", NULL },
	{ ERR_CONFIG_CMDLINE,	"Error in command line:\n\n%s",
							"нЬХАЙЮ Б ЙНЛЮМДМНИ ЯРПНЙЕ:\n\n%s", NULL },
	{ STR_BUTTON_BACK,		"Back",		"мЮГЮД", NULL },
	{ STR_BUTTON_NEXT,		"Next",		"дЮКЕЕ", NULL },
	{ STR_BUTTON_FINISH,	"Finish",	"цНРНБН", NULL },
	{ STR_BUTTON_CANCEL2,	"Cancel",	"нРЛЕМХРЭ", NULL },
#ifdef _SFX_USE_CUSTOM_EXCEPTIONS
	{ ERR_SFX_EXCEPTION,		"Application error:\n\nException code:\n\t0x%08x\nAddress:\n\t0x%08x\nException data:\n",
								"нЬХАЙЮ ОПХКНФЕМХЪ:\n\nйНД ХЯЙКЧВЕМХЪ:\n\t0x%08x\nюДПЕЯ:\n\t0x%08x\nдЮММШЕ ХЯЙКЧВЕМХЪ:\n" },
#endif // _SFX_USE_CUSTOM_EXCEPTIONS

// Added June 6, 2010: warnings dialogs
#ifdef _SFX_USE_WARNINGS
	{ STR_WARNING_TITLE,		"7z SFX: warning",
								"7z SFX: ОПЕДСОПЕФДЕМХЕ", NULL },
	{ STR_WARNING_SUFFIX,		": warning",
								": ОПЕДСОПЕФДЕМХЕ", NULL },
#ifdef _SFX_USE_CHECK_FREE_SPACE
	{ STR_DISK_FREE_SPACE,		"Not enough free space for extracting.\n\nDo you want to continue?",
								"мЮ ДХЯЙЕ МЕДНЯРЮРНВМН ЛЕЯРЮ ДКЪ ПЮЯОЮЙНБЙХ.\n\nбШ УНРХРЕ ОПНДНКФХРЭ?", NULL },
#endif // _SFX_USE_CHECK_FREE_SPACE
#ifdef _SFX_USE_CHECK_RAM
	{ STR_PHYSICAL_MEMORY,		"Insufficient physical memory.\nExtracting may take a long time.\n\nDo you want to continue?",
								"мЕДНЯРЮРНВМН ТХГХВЕЯЙНИ ОЮЛЪРХ.\nпЮЯОЮЙНБЙЮ ЛНФЕР ГЮМЪРЭ ОПНДНКФХРЕКЭМНЕ БПЕЛЪ.\n\nбШ УНРХРЕ ОПНДНКФХРЭ?", NULL },
#endif // _SFX_USE_CHECK_FREE_SPACE
#endif // _SFX_USE_WARNINGS
#ifdef SFX_CRYPTO
	{ STR_PASSWORD_TEXT,		"Enter password:",
								"бБЕДХРЕ ОЮПНКЭ:", NULL },
#endif // SFX_CRYPTO
	{ 0, "", "", NULL }
};
