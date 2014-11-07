/*
 * libconfigini tests
 */

#include <stdio.h>
#include <stdlib.h>

#include "../src/configini.h"


#define CONFIGREADFILE		"../etc/config.cnf"
#define CONFIGSAVEFILE		"../etc/new-config.cnf"

#define ENTER_TEST_FUNC																			\
	do {																						\
		printf("\n-----------------------------------------------------------------------\n");	\
		printf("<TEST: %s>\n\n", __FUNCTION__);													\
	} while (0)




/*
 * Read Config file
 */
static void Test1()
{
	Config *cfg = NULL;

	ENTER_TEST_FUNC;

	if (ConfigReadFile(CONFIGREADFILE, &cfg) != CONFIG_OK) {
		fprintf(stderr, "ConfigOpenFile failed for %s\n", CONFIGREADFILE);
		return;
	}

	ConfigPrintSettings(cfg, stdout);
	ConfigPrint(cfg, stdout);

	ConfigFree(cfg);
}

/*
 * Create Config handle, read Config file, edit and save to new file
 */
static void Test2()
{
	Config *cfg = NULL;

	ENTER_TEST_FUNC;

	/* set settings */
	cfg = ConfigNew();
	ConfigSetBoolString(cfg, "yes", "no");

	/* we can give initialized handle (rules has been set) */
	if (ConfigReadFile(CONFIGREADFILE, &cfg) != CONFIG_OK) {
		fprintf(stderr, "ConfigOpenFile failed for %s\n", CONFIGREADFILE);
		return;
	}

	ConfigRemoveKey(cfg, "SECT1", "a");
	ConfigRemoveKey(cfg, "SECT2", "aa");
	ConfigRemoveKey(cfg, "owner", "title");
	ConfigRemoveKey(cfg, "database", "file");

	ConfigAddBool(cfg, "SECT1", "isModified", true);
	ConfigAddString(cfg, "owner", "country", "Turkey");

	ConfigPrintSettings(cfg, stdout);
	ConfigPrint(cfg, stdout);
	ConfigPrintToFile(cfg, CONFIGSAVEFILE);

	ConfigFree(cfg);
}

/*
 * Create Config handle and add sections & key-values
 */
static void Test3()
{
	Config *cfg = NULL;

	ENTER_TEST_FUNC;

	cfg = ConfigNew();

	ConfigSetBoolString(cfg, "true", "false");

	ConfigAddString(cfg, "SECTION1", "Istanbul", "34");
	ConfigAddInt(cfg, "SECTION1", "Malatya", 44);

	ConfigAddBool(cfg, "SECTION2", "enable", true);
	ConfigAddDouble(cfg, "SECTION2", "Lira", 100);

	ConfigPrintSettings(cfg, stdout);
	ConfigPrint(cfg, stdout);

	ConfigFree(cfg);
}

/*
 * Create Config without any section
 */
static void Test4()
{
	Config *cfg = NULL;
	char s[1024];
	bool b;
	float f;

	ENTER_TEST_FUNC;

	cfg = ConfigNew();

	ConfigAddString(cfg, CONFIG_SECTNAME_DEFAULT, "Mehmet Akif ERSOY", "Safahat");
	ConfigAddString(cfg, CONFIG_SECTNAME_DEFAULT, "Necip Fazil KISAKUREK", "Cile");
	ConfigAddBool(cfg, CONFIG_SECTNAME_DEFAULT, "isset", true);
	ConfigAddFloat(cfg, CONFIG_SECTNAME_DEFAULT, "degree", 35.0);

	ConfigPrint(cfg, stdout);

	///////////////////////////////////////////////////////////////////////////////////////////////

	ConfigReadString(cfg, CONFIG_SECTNAME_DEFAULT, "Mehmet Akif Ersoy", s, sizeof(s), "Poet");
	fprintf(stdout, "Mehmet Akif Ersoy = %s\n", s);

	ConfigReadString(cfg, CONFIG_SECTNAME_DEFAULT, "Mehmet Akif ERSOY", s, sizeof(s), "Poet");
	fprintf(stdout, "Mehmet Akif ERSOY = %s\n", s);

	ConfigReadBool(cfg, CONFIG_SECTNAME_DEFAULT, "isset", &b, false);
	fprintf(stdout, "isset = %s\n", b ? "true" : "false");

	ConfigReadFloat(cfg, CONFIG_SECTNAME_DEFAULT, "degree", &f, 1.5);
	fprintf(stdout, "degree = %f\n", f);

	///////////////////////////////////////////////////////////////////////////////////////////////

	ConfigFree(cfg);
}


int main()
{
	Test1();
	Test2();
	Test3();
	Test4();

	return 0;
}
