#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

struct Address {
	int id;
	int set;
	char *name;
	char *email;
};

struct Database {
	int size;
	int max_len;
	struct Address *rows;
};

struct Connection {
	FILE *file;
	struct Database *db;
};

static struct Connection *conn;

void Database_close() {
	if (conn) {
		if (conn->file) fclose(conn->file);
		if (conn->db) free(conn->db);
		free(conn);
	}
}

void die(const char *message) {
	if (errno) {
		perror(message);
	} else {
		printf("ERROR: %s\n", message);
	}
	Database_close();
	exit(1);
}

void Address_print(struct Address *addr) {
	printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load() {
	int rc;
	struct Database *db = conn->db;

	rc = fread(&db->size, sizeof(int), 1, conn->file);
	if (rc != 1) die("Failed to load database");

	rc = fread(&db->max_len, sizeof(int), 1, conn->file);
	if (rc != 1) die("Failed to load database");

	db->rows = calloc(db->size, sizeof(struct Address));

	for (int i = 0; i < db->size; i++) {
		struct Address *addr = &db->rows[i];

		rc = fread(&addr->id, sizeof(int), 1, conn->file);
		if (rc != 1) die("Failed to load database");

		rc = fread(&addr->set, sizeof(bool), 1, conn->file);
		if (rc != 1) die("Failed to load database");

		if (addr->set) {
			addr->name = calloc(db->max_len, sizeof(char));
			rc = fread(addr->name, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die("Failed to load database");

			addr->email = calloc(db->max_len, sizeof(char));
			rc = fread(addr->email, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die("Failed to load database");
		}
	}
}

void Database_open(const char *filename, char mode) {
	conn = malloc(sizeof(struct Connection));
	if (!conn) die("Memory error");

	conn->db = malloc(sizeof(struct Database));
	if (!conn->db) die("Memory error");

	if (mode == 'c') {
		conn->file = fopen(filename, "w");
	} else {
		conn->file = fopen(filename, "r+");

		if (conn->file) {
			Database_load();
		}
	}

	if (!conn->file) die("Failed to open the file");
}

void Database_write() {
	int rc;

	rewind(conn->file);

	struct Database *db = conn->db;

	rc = fwrite(&db->size, sizeof(int), 1, conn->file);
	if (rc != 1) die("Failed to write database");

	rc = fwrite(&db->max_len, sizeof(int), 1, conn->file);
	if (rc != 1) die("Failed to write database");

	for (int i = 0; i < db->size; i++) {
		struct Address *addr = &db->rows[i];

		rc = fwrite(&addr->id, sizeof(int), 1, conn->file);
		if (rc != 1) die("Failed to write database");

		rc = fwrite(&addr->set, sizeof(bool), 1, conn->file);
		if (rc != 1) die("Failed to write database");

		if (addr->set) {
			rc = fwrite(addr->name, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die("Failed to write database");

			rc = fwrite(addr->email, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die("Failed to write database");
		}
	}

	rc = fflush(conn->file);
	if (rc == -1) die("Cannot flush database");
}

void Database_create(int size, int max_len) {
	conn->db->size = size;
	conn->db->max_len = max_len;
	conn->db->rows = calloc(size, sizeof(struct Address));

	for (int i = 0; i < conn->db->size; i++) {
		conn->db->rows[i] = (struct Address) {.id = i, .set = false};
	}
}

void Database_set(int id, const char *name, const char *email) {
	struct Address *addr = &conn->db->rows[id];
	if (addr->set) die("Already set, delete it first");

	addr->set = true;
	addr->name = calloc(conn->db->max_len, sizeof(char));
	addr->email = calloc(conn->db->max_len, sizeof(char));
	strlcpy(addr->name, name, conn->db->max_len);
	strlcpy(addr->email, email, conn->db->max_len);
}

void Database_get(int id) {
	struct Address *addr = &conn->db->rows[id];

	if (addr->set) {
		Address_print(addr);
	} else {
		die("ID is not set");
	}
}

void Database_find(char *name) {
	for (int i = 0; i < conn->db->size; i++) {
		struct Address *addr = &conn->db->rows[i];

		if (addr->set && strncmp(addr->name, name, conn->db->max_len) == 0) {
			Address_print(addr);
		}
	}
}

void Database_delete(int id) {
	conn->db->rows[id] = (struct Address) { .id = id, .set = false };
}

void Database_list() {
	struct Database *db = conn->db;

	for (int i = 0; i < db->size; i++) {
		struct Address *cur = &db->rows[i];

		if (cur->set) {
			Address_print(cur);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 3) die("Usage: ex17 <dbfile> <action> [action params]");

	char *filename = argv[1];
	char action = argv[2][0];
	Database_open(filename, action);

	switch (action) {
		case 'c':
			if (argc != 5) die("Need size and max length");
			Database_create(atoi(argv[3]), atoi(argv[4]));
			Database_write();
			break;
		case 'g':
			if (argc != 4) die("Need an id to get");
			Database_get(atoi(argv[3]));
			break;
		case 's':
			if (argc != 6) die("Need id, name, email to set");
			Database_set(atoi(argv[3]), argv[4], argv[5]);
			Database_write();
			break;
		case 'd':
			if (argc != 4) die("Need id to delete");
			Database_delete(atoi(argv[3]));
			Database_write();
			break;
		case 'l':
			Database_list();
			break;
		case 'f':
			if (argc != 4) die("Need name to find");
			Database_find(argv[3]);
			break;
		default:
			die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
	}

	Database_close();

	return 0;
}
