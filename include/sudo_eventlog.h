/*
 * SPDX-License-Identifier: ISC
 *
 * Copyright (c) 2020 Todd C. Miller <Todd.Miller@sudo.ws>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef SUDO_EVENTLOG_H
#define SUDO_EVENTLOG_H

#include <sys/types.h>	/* for gid_t, uid_t */
#include <time.h>	/* for struct timespec */

/* Supported event types. */
enum event_type {
    EVLOG_ACCEPT,
    EVLOG_REJECT,
    EVLOG_ALERT
};

/* Supported eventlog types (bitmask). */
#define EVLOG_NONE	0x00
#define EVLOG_SYSLOG	0x01
#define EVLOG_FILE	0x02

/* Supported eventlog formats. */
enum eventlog_format {
    EVLOG_SUDO,
    EVLOG_JSON
};

/* Eventlog flag values. */
#define EVLOG_RAW	0x01
#define EVLOG_MAIL	0x02
#define EVLOG_MAIL_ONLY	0x04

/*
 * Maximum number of characters to log per entry.  The syslogger
 * will log this much, after that, it truncates the log line.
 * We need this here to make sure that we continue with another
 * syslog(3) call if the internal buffer is more than 1023 characters.
 */
#ifndef MAXSYSLOGLEN
# define MAXSYSLOGLEN		960
#endif

/*
 * Event log config, used with eventlog_setconf()
 */
struct eventlog_config {
    int type;
    enum eventlog_format format;
    int syslog_acceptpri;
    int syslog_rejectpri;
    int syslog_alertpri;
    int syslog_maxlen;
    uid_t mailuid;
    bool omit_hostname;
    const char *logpath;
    const char *time_fmt;
    const char *mailerpath;
    const char *mailerflags;
    const char *mailfrom;
    const char *mailto;
    const char *mailsub;
    FILE *(*open_log)(int type, const char *);
    void (*close_log)(int type, FILE *);
};

/*
 * Info present in the eventlog file, regardless of format.
 */
struct eventlog {
    char *iolog_path;
    const char *iolog_file;	/* substring of iolog_path, do not free */
    char *command;
    char *cwd;
    char *runchroot;
    char *runcwd;
    char *rungroup;
    char *runuser;
    char *submithost;
    char *submituser;
    char *submitgroup;
    char *ttyname;
    char **argv;
    char **env_add;
    char **envp;
    struct timespec submit_time;
    int lines;
    int columns;
    uid_t runuid;
    gid_t rungid;
    char sessid[7];
};

/* Callback from eventlog code to write log info */
struct json_container;
typedef bool (*eventlog_json_callback_t)(struct json_container *, void *);

bool eventlog_accept(const struct eventlog *details, int flags, eventlog_json_callback_t info_cb, void *info);
bool eventlog_alert(const struct eventlog *details, int flags, struct timespec *alert_time, const char *reason, const char *errstr);
bool eventlog_reject(const struct eventlog *details, int flags, const char *reason, eventlog_json_callback_t info_cb, void *info);
bool eventlog_setconf(struct eventlog_config *conf);
bool eventlog_store_json(struct json_container *json, const struct eventlog *evlog);
void eventlog_free(struct eventlog *evlog);

#endif /* SUDO_EVENTLOG_H */
