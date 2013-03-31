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

void Database_close(struct Connection *conn) {
	if (conn) {
		if (conn->file) fclose(conn->file);
		if (conn->db) free(conn->db);
		free(conn);
	}
}

void die(struct Connection *conn, const char *message) {
	if (errno) {
		perror(message);
	} else {
		printf("ERROR: %s\n", message);
	}
	Database_close(conn);
	exit(1);
}

void Address_print(struct Address *addr) {
	printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
	int rc;
	struct Database *db = conn->db;

	rc = fread(&db->size, sizeof(int), 1, conn->file);
	if (rc != 1) die(conn, "Failed to load database");
	rc = fread(&db->max_len, sizeof(int), 1, conn->file);
	if (rc != 1) die(conn, "Failed to load database");

	db->rows = calloc(db->size, sizeof(struct Address));

	for (int i = 0; i < db->size; i++) {
		struct Address *addr = &db->rows[i];

		addr->id = i;

		rc = fread(&addr->set, sizeof(bool), 1, conn->file);
		if (rc != 1) die(conn, "Failed to load database");

		if (addr->set) {
			rc = fread(addr->name, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die(conn, "Failed to load database");

			rc = fread(addr->email, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die(conn, "Failed to load database");
		}
	}
}

struct Connection *Database_open(const char *filename, char mode) {
	struct Connection *conn = malloc(sizeof(struct Connection));
	if (!conn) die(conn, "Memory error");

	conn->db = malloc(sizeof(struct Database));
	if (!conn->db) die(conn, "Memory error");

	if (mode == 'c') {
		conn->file = fopen(filename, "w");
	} else {
		conn->file = fopen(filename, "r+");

		if (conn->file) {
			Database_load(conn);
		}
	}

	if (!conn->file) die(conn, "Failed to open the file");

	return conn;
}

void Database_write(struct Connection *conn) {
	int rc;

	rewind(conn->file);

	struct Database *db = conn->db;

	rc = fwrite(&db->size, sizeof(int), 1, conn->file);
	if (rc != 1) die(conn, "Failed to write database");

	rc = fwrite(&db->max_len, sizeof(int), 1, conn->file);
	if (rc != 1) die(conn, "Failed to write database");

	for (int i = 0; i < db->size; i++) {
		struct Address *addr = &db->rows[i];

		rc = fwrite(&addr->id, sizeof(int), 1, conn->file);
		if (rc != 1) die(conn, "Failed to write database");

		rc = fwrite(&addr->set, sizeof(bool), 1, conn->file);
		if (rc != 1) die(conn, "Failed to write database");

		if (addr->set) {
			rc = fwrite(addr->name, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die(conn, "Failed to write database");

			rc = fwrite(addr->email, sizeof(char), db->max_len, conn->file);
			if (rc != db->max_len) die(conn, "Failed to write database");
		}
	}

	rc = fflush(conn->file);
	if (rc == -1) die(conn, "Cannot flush database");
}

void Database_create(struct Connection *conn, int size, int max_len) {
	conn->db->size = size;
	conn->db->max_len = max_len;
	conn->db->rows = calloc(size, sizeof(struct Address));

	for (int i = 0; i < conn->db->size; i++) {
		conn->db->rows[i] = (struct Address) {.id = i, .set = false};
	}
}

void Database_set(struct Connection *conn, int id, const char *name, const char *email) {
	struct Address *addr = &conn->db->rows[id];
	if (addr->set) die(conn, "Already set, delete it first");

	addr->set = true;
	addr->name = calloc(conn->db->max_len, sizeof(char));
	addr->email = calloc(conn->db->max_len, sizeof(char));
	strlcpy(addr->name, name, conn->db->max_len);
	strlcpy(addr->email, email, conn->db->max_len);
}

void Database_get(struct Connection *conn, int id) {
	struct Address *addr = &conn->db->rows[id];

	if (addr->set) {
		Address_print(addr);
	} else {
		die(conn, "ID is not set");
	}
}

void Database_find(struct Connection *conn, char *name) {
	for (int i = 0; i < conn->db->size; i++) {
		struct Address *addr = &conn->db->rows[i];

		if (addr->set && strncmp(addr->name, name, conn->db->max_len) == 0) {
			Address_print(addr);
		}
	}
}

void Database_delete(struct Connection *conn, int id) {
	conn->db->rows[id] = (struct Address) { .id = id, .set = false };
}

void Database_list(struct Connection *conn) {
	struct Database *db = conn->db;

	for (int i = 0; i < db->size; i++) {
		struct Address *cur = &db->rows[i];

		if (cur->set) {
			Address_print(cur);
		}
	}
}

int main(int argc, char *argv[]) {
	if (argc < 3) die(NULL, "Usage: ex17 <dbfile> <action> [action params]");

	char *filename = argv[1];
	char action = argv[2][0];
	struct Connection *conn = Database_open(filename, action);

	switch (action) {
		case 'c':
			if (argc != 5) die(conn, "Need size and max length");
			Database_create(conn, atoi(argv[3]), atoi(argv[4]));
			Database_write(conn);
			break;
		case 'g':
			if (argc != 4) die(conn, "Need an id to get");
			Database_get(conn, atoi(argv[3]));
			break;
		case 's':
			if (argc != 6) die(conn, "Need id, name, email to set");
			Database_set(conn, atoi(argv[3]), argv[4], argv[5]);
			Database_write(conn);
			break;
		case 'd':
			if (argc != 4) die(conn, "Need id to delete");
			Database_delete(conn, atoi(argv[3]));
			Database_write(conn);
			break;
		case 'l':
			Database_list(conn);
			break;
		case 'f':
			if (argc != 4) die(conn, "Need name to find");
			Database_find(conn, argv[3]);
			break;
		default:
			die(conn, "Invalid action, only: c=create, g=get, s=set, d=del, l=list");
	}

	Database_close(conn);

	return 0;
}
