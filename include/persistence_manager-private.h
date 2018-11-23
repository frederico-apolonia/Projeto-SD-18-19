#include "table.h"
#include "table-private.h"
#include "message-private.h"
#include "persistence_manager.h"

struct pmanager_t{
	char *filename;
	char *logfilename;
	char *ckpfilename;
	char *sttfilename;
	int currsize;
	int logfile;
	int ckpfile;
	int sttfile;
	int logsize;
};

int write_to_table(int fd, struct table_t *table);
int table_write_stt(int fd, struct table_t *table);