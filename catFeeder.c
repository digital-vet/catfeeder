// To compile this simple example:
// $ gcc -o catFeeder catFeeder.c -lsqlite3 -lnfc

#include <stdlib.h>
#include <nfc/nfc.h>
#include <sqlite3.h>
#include <string.h>
#include <signal.h>
#include <time.h>

// NFC initialization
static nfc_device *pnd;
static nfc_target nt;

// Allocate a pointer to NFC context
static nfc_context *context;
static sqlite3 *db;

static int timeElapsed;
static int minFeedingInterval;

/* Section to iterate through decimal data, convert to hex, and pretty print */
static void
print_hex(const uint8_t *pbtData, const size_t szBytes)
{
  size_t  szPos;

  for (szPos = 0; szPos < szBytes; szPos++) {
    printf("%02x  ", pbtData[szPos]);
  }
  printf("\n");
}

/* Callback function for sqlite3 queries. Currently only returns values, not
column names */
static int
callback(void *NotUsed, int argc, char **argv, char **azColName){
      int i;
      for(i=0; i<argc; i++){
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      	printf("%s\n", argv[i] ? argv[i] : "NULL");
      }
      printf("\n");
      return 0;
}

// This function sets the global timeElapsed from the sql query result
static int
getTimeElapsed(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
//		printf("%s\n", argv[i] ? argv[i] : "NULL");
		timeElapsed = atoi(argv[i]);
		printf("timeElapsed: %i\n", timeElapsed);
	}
	return 0;
}

// This function sets the global minFeedingInterval from the sql query result
static int
getMinFeedingInterval(void *NotUsed, int argc, char **argv, char **azColName){
        int i;
        for(i=0; i<argc; i++){
//                printf("%s\n", argv[i] ? argv[i] : "NULL");
                minFeedingInterval = atoi(argv[i]);
                printf("minFeedingInterval: %i\n", minFeedingInterval);
        }
        return 0;
}

/* Function to pull the ID string from an RFID tag and return the ID as a string
value */
static void
getIDstring(const uint8_t *pbtData, const size_t szBytes, char *inString)
{
	char returnString[34] = "";
	size_t szPos;
	char temp[2] = { };
	int i;

	for (szPos = 0; szPos < szBytes; szPos++) {
//		printf("%d, %02x\n", pbtData[szPos], pbtData[szPos]);
		sprintf(temp, "%02x", pbtData[szPos]);
//		printf(temp);
//		printf(" ");
		strcat(returnString, temp);
//		printf("%s\n", returnString);
	}
//	printf("\nreturnString: %s\n", returnString);
//	snprintf(returnString, szBytes, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", pbtData);
//	printf("%.30X", pbtData[0]);

	for(i = 0; i < strlen(returnString) + 1; i++){
		inString[i] = returnString[i];
	}
//	printf("\ninString: %s\n", inString);
	return;
//	return inString;
}

// Important function to clean up the connections on the way out!
static void cleanup_catch_function(int signo) {
  printf("\nCleaning up for exit!\n");
  nfc_close(pnd);
  nfc_exit(context);
  sqlite3_close(db);
  exit(EXIT_SUCCESS);
}

int
main(int argc, const char *argv[])
{
  time_t current_time;
  char *c_time_string;

  // Sqlite3 parameters and initialization
  char *zErrMsg = 0;
  int rc;
  rc = sqlite3_open("catfeeder.db", &db);
  if ( rc ){
	fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return(1);
  }

  // Initialize libnfc and set the nfc_context
  nfc_init(&context);
  if (context == NULL) {
    printf("Unable to init libnfc (malloc)\n");
    exit(EXIT_FAILURE);
  }

  // Display libnfc version
  const char *acLibnfcVersion = nfc_version();
  (void)argc;
  printf("%s uses libnfc %s\n", argv[0], acLibnfcVersion);

  // Open, using the first available NFC device which can be in order of selection:
  //   - default device specified using environment variable or
  //   - first specified device in libnfc.conf (/etc/nfc) or
  //   - first specified device in device-configuration directory (/etc/nfc/devices.d) or
  //   - first auto-detected (if feature is not disabled in libnfc.conf) device
  pnd = nfc_open(context, NULL);

  if (pnd == NULL) {
    printf("ERROR: %s\n", "Unable to open NFC device.");
    exit(EXIT_FAILURE);
  }
  // Set opened NFC device to initiator mode
  if (nfc_initiator_init(pnd) < 0) {
    nfc_perror(pnd, "nfc_initiator_init");
    exit(EXIT_FAILURE);
  }

  printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));

  // Poll for a ISO14443A (MIFARE) tag
  const nfc_modulation nmMifare = {
    .nmt = NMT_ISO14443A,
    .nbr = NBR_106,
  };

  char uniqueID[34] = { };

  // Forever loop
  do{
	if (signal(SIGINT, cleanup_catch_function) == SIG_ERR) {
		return EXIT_FAILURE;
  	}

  	if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0) {
    		printf("The following (NFC) ISO14443A tag was found:\n");
    		printf("    ATQA (SENS_RES): ");
    		print_hex(nt.nti.nai.abtAtqa, 2);
    		printf("       UID (NFCID%c): ", (nt.nti.nai.abtUid[0] == 0x08 ? '3' : '1'));
    		print_hex(nt.nti.nai.abtUid, nt.nti.nai.szUidLen);
    		printf("      SAK (SEL_RES): ");
    		print_hex(&nt.nti.nai.btSak, 1);
    		if (nt.nti.nai.szAtsLen) {
      			printf("          ATS (ATR): ");
      			print_hex(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen);
    		}
  	}

  	getIDstring(nt.nti.nai.abtAts, nt.nti.nai.szAtsLen, uniqueID);
	//  printf("Back in main\n");
//  	printf("\n%s\n", uniqueID);

	char* query = sqlite3_mprintf("SELECT name FROM identification WHERE id='%q';", uniqueID);
  	//  printf("%s\n", query);
  	rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
  	if( rc != SQLITE_OK ){
        	fprintf(stderr, "SQL error: %s\n", zErrMsg);
        	sqlite3_free(zErrMsg);
  	}
  	else {
		current_time = time(NULL);
		if (current_time == ((time_t)-1)) {
        		(void) fprintf(stderr, "Failure to compute the current time.\n");
        		return EXIT_FAILURE;
    		}

    		/* Convert to local time format. */
    		c_time_string = ctime(&current_time);

    		if (c_time_string == NULL) {
        		(void) fprintf(stderr, "Failure to convert the current time.\n");
        		return EXIT_FAILURE;
    		}

    		/* Print to stdout. */
    		(void) printf("Current time is %s\n", c_time_string);

		// Grab the minimum time interval from the database
		char* intervalQuery = sqlite3_mprintf("SELECT mintime FROM rules WHERE name IN (SELECT name FROM identification WHERE id='%q')", uniqueID);
		rc = sqlite3_exec(db, intervalQuery, getMinFeedingInterval, 0, &zErrMsg);

		// Determine how many minutes have elapsed since the last dispensing entry in the table log
		char* timeQuery = sqlite3_mprintf("SELECT (strftime('%q','now') - (SELECT MAX(date_time) FROM log WHERE id='%s')) / 60", "%s", uniqueID);
//              printf("%s\n", timeQuery);
                rc = sqlite3_exec(db, timeQuery, getTimeElapsed, 0, &zErrMsg);

		if(timeElapsed >= minFeedingInterval){
			char* addToLog = sqlite3_mprintf("INSERT INTO log VALUES('%q', strftime('%s', 'now'))", uniqueID, "%s");
//			printf("%s\n", addToLog);
			rc = sqlite3_exec(db, addToLog, callback, 0, &zErrMsg);
	        	system("pigs s 18 2500 mils 250 s 18 0");
		}
  	}
  }while (1); // end of forever loop

}
