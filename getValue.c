#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFSIZE 1000
#define NUMOBISCODES 25

/* According to DSMR(v4.2) secification: http://nbn-assets.netbeheernederland.nl/32768/?k=aHyn8n3Kk9ch_GFgG7TdJk-DFCl-ZaBOna0WUSvqYBfhdggUuSR_pbVF6YbO6SYcWv6IjNhu4-uQCkkVTjXFiJaPWY1T_KytQrYJnnVLQdYOeBw6hGWkqpyZOGTKnhpin2aQD8CS6v3rtlya3Jx2X_4JEd2NV9RcbJaN_CQNppERAvqLFctIKjWN6FCgb6bQ56M9wRdgH5N_ssdkN5VHiiiK6_ZuXzgJEzLIG9gDOALQ-Q8-qCt9dum54l-RAToBRuYVOflju8sh970jmgi1Z4wMxr9fAAxTPAw2waJPthSws_mU4WSE2o_zQu5pngCCqxkkXbGnQCBxf7ReaJ9nHvl9fvRU8igWz9hM6pCl4PdozRoAxHMhmvmO4hRsAm62sr2mdqFlfOQGYLAmwO1cNP0zZo0tnLQm6lynS5ttBEyGsUoFNrBtf81Ad74MUCo3dlTQsMKVtbrEIjR3jrrwpsJgMMy1rhG4WwmtnF-aLqLSBB8YlAcx5Hw8xBPc588OK4LMTuKWfP7DxhaAYsxEvmhUbWERFjdLVSt84PqWMhlURdm23SEOUgV5vryX-CdKQSaYGZ7zVxDdXz2keBLzA3w4QDWUlmLNFWInechuGkZfbLezyPl28ixUp5M3dcIBy-phtNirv4ndpqB8yQC142aLEA2f6b8Mt7_2wW_6EC2F0Umy99jyNfTb6cGOlcv_&u= */
const char *OBIScodes[NUMOBISCODES] =
{ "1-3:0.2.8", /* DSMR version */
"0-0:1.0.0", /* Timestamp */
"0-0:96.1.1", /* S/N */
"1-0:1.8.1", /* +T1 */
"1-0:1.8.2", /* +T2 */
"1-0:2.8.1", /* -T1 */
"1-0:2.8.2", /* -T2 */
"0-0:96.14.0", /* Tariff indicator (T1/T2) */
"1-0:1.7.0", /* +P  */
"1-0:2.7.0", /* -P */
"0-0:96.7.21", /* number of power failures */
"0-0:96.7.9", /* number of long power failures */
"1-0:99.97.0", /* Power failure event log */
"1-0:32.32.0", /* Number of voltage sags */
"1-0:32.36.0", /* number of voltage swells */
"0-0:96.13.1", /* text message code */
"0-0:96.13.0", /* text message */
"1-0:31.7.0", /* Instantaneous current */
"1-0:21.7.0", /* Instantaneous power + */
"1-0:22.7.0", /* Instantaneous power - */
"0-1:24.1.0", /* device type 1*/
"0-2:24.1.0", /* device type 2*/
"0-1:96.1.0", /* electricity meter identifier */
"0-2:96.1.0", /* Gas meter S/N*/
"0-2:24.2.1" }; /* Gas meter last hourly reading*/

time_t *mkTimestamp(char *timeval) {

	char buf[255];

	struct tm tm;
	time_t *timestamp = malloc(sizeof(timestamp));

	tm.tm_isdst = (timeval[strlen(timeval) - 1] == 'W') ? 0 : 1;

	strptime(timeval, "%y%m%d%H%M%S", &tm);

	*timestamp = mktime(&tm);

	strftime(buf, sizeof(buf), "%d %b %Y %H:%M:%S", &tm);

	return timestamp;

}

char *getValue(char *line) {

	int i = 0, j = 0, len = strlen(line);
	char *result = malloc(1024);

	strcpy(result, "");

	while (line[i] != '(' && line[i] != '\n')
		i++;
	j = ++i;
	/*stop at end of line*/
	if (i >= len)
		return result;

	/*from pos ( search for first ) and replace it with \0 */
	while (line[j] != ')' && line[i] != '\n')
		j++;
	line[j] = '\0';
	strcat(result, line + i);

	return result;

}

/*int main(int argc, char **argv) {

	FILE *fp = fopen(argv[1], "r");

}*/

int main(int argc, char **argv) {

	int i = 0, j, logs;
	char *lineval;

	int version, tariff, shorts, longs, drops, peaks, inst_I;
	time_t *timestamp, *gas_timestamp;
	char serial[35], gas_serial[35];
	float T1, T2, T1_r, T2_r, E, E_r, gas, V, inst_P, inst_P_r;

	FILE *fp = fopen(argv[1], "r");

	char buff[BUFSIZE], text[40];

	while (fgets(buff, BUFSIZE - 1, fp) != NULL) {
		/* buff has one line of the file */
		lineval = getValue(buff);

		for (i = 0; i < NUMOBISCODES; i++) {
			if (strstr(buff, OBIScodes[i]))
				break;
		}

		switch (i) {
		case 0:
			version = atoi(lineval);
			break;
		case 1:
			timestamp = mkTimestamp(lineval);
			break;
		case 2:
			strcpy(serial, lineval);
			break;
		case 3:
			T1 = atof(lineval);
			break;
		case 4:
			T2 = atof(lineval);
			break;
		case 5:
			T1_r = atof(lineval);
			break;
		case 6:
			T2_r = atof(lineval);
			break;
		case 7:
			tariff = atoi(lineval);
			break;
		case 8:
			E = atof(lineval);
			break;
		case 9:
			E_r = atof(lineval);
			break;
		case 10:
			shorts = atoi(lineval);
			break;
		case 11:
			longs = atoi(lineval);
			break;
		case 12:
			strcpy(text, "logs stroomuitval =");
			/*extract further information*/
			logs = atoi(lineval);
			int startv = strlen(lineval) + strlen(OBIScodes[i]) + 15;
			strcpy(lineval, "");
			for (j = 0; j < 2 * logs; j++) {
				//printf("j = %i, len = %i, val = %s startv %i\n", j, (int) strlen(lineval),
						//lineval, startv);
				strcpy(lineval, getValue(buff + startv));
				printf("%s\n", lineval);
				startv += strlen(lineval) + 2;
				//strcat(lineval, " ");
			}
			break;
		case 13:
			drops = atoi(lineval);
			break;
		case 14:
			peaks = atoi(lineval);
			break;
		case 15:
			strcpy(text, "berichtcode =");
			break;
		case 16:
			strcpy(text, "berichttext =");
			break;
		case 17:
			inst_I = atoi(lineval);
			break;
		case 18:
			inst_P = atof(lineval);
			break;
		case 19:
			inst_P_r = atof(lineval);
			break;
		case 20:
			strcpy(text, "apparaat type 1 =");
			break;
		case 21:
			strcpy(text, "apparaat type 2 =");
			break;
		case 22:
			strcpy(text, "identifier 1 =");
			break;
		case 23:
			strcpy(gas_serial, lineval);
			break;
		case 24:
			gas_timestamp = mkTimestamp(lineval);
			gas = atof(getValue(buff + strlen(OBIScodes[i]) + strlen(lineval) + 2));
			break;

		default:
			printf("Unknown value: %s", buff);
			break;
		}

		//printf("%s %s\n", text, lineval);
		free(lineval);

	}
	fclose(fp); /* close the file */

	printf("%i %ld %s %f %f %f %f %i %f %f\n", version, (long)timestamp, serial, T1, T2, T1_r, T2_r, tariff, E, E_r);
	printf("%ld %s %f\n", (long)gas_timestamp, gas_serial, gas);

}
