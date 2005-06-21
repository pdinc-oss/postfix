/*++
/* NAME
/*	qmgr 8
/* SUMMARY
/*	old Postfix queue manager
/* SYNOPSIS
/*	\fBqmgr\fR [generic Postfix daemon options]
/* DESCRIPTION
/*	The \fBqmgr\fR(8) daemon awaits the arrival of incoming mail
/*	and arranges for its delivery via Postfix delivery processes.
/*	The actual mail routing strategy is delegated to the
/*	\fBtrivial-rewrite\fR(8) daemon.
/*	This program expects to be run from the \fBmaster\fR(8) process
/*	manager.
/*
/*	Mail addressed to the local \fBdouble-bounce\fR address is
/*	logged and discarded.  This stops potential loops caused by
/*	undeliverable bounce notifications.
/* MAIL QUEUES
/* .ad
/* .fi
/*	The \fBqmgr\fR(8) daemon maintains the following queues:
/* .IP \fBincoming\fR
/*	Inbound mail from the network, or mail picked up by the
/*	local \fBpickup\fR(8) agent from the \fBmaildrop\fR directory.
/* .IP \fBactive\fR
/*	Messages that the queue manager has opened for delivery. Only
/*	a limited number of messages is allowed to enter the \fBactive\fR
/*	queue (leaky bucket strategy, for a fixed delivery rate).
/* .IP \fBdeferred\fR
/*	Mail that could not be delivered upon the first attempt. The queue
/*	manager implements exponential backoff by doubling the time between
/*	delivery attempts.
/* .IP \fBcorrupt\fR
/*	Unreadable or damaged queue files are moved here for inspection.
/* .IP \fBhold\fR
/*	Messages that are kept "on hold" are kept here until someone
/*	sets them free.
/* DELIVERY STATUS REPORTS
/* .ad
/* .fi
/*	The \fBqmgr\fR(8) daemon keeps an eye on per-message delivery status
/*	reports in the following directories. Each status report file has
/*	the same name as the corresponding message file:
/* .IP \fBbounce\fR
/*	Per-recipient status information about why mail is bounced.
/*	These files are maintained by the \fBbounce\fR(8) daemon.
/* .IP \fBdefer\fR
/*	Per-recipient status information about why mail is delayed.
/*	These files are maintained by the \fBdefer\fR(8) daemon.
/* .IP \fBtrace\fR
/*	Per-recipient status information as requested with the
/*	Postfix "\fBsendmail -v\fR" or "\fBsendmail -bv\fR" command.
/*	These files are maintained by the \fBtrace\fR(8) daemon.
/* .PP
/*	The \fBqmgr\fR(8) daemon is responsible for asking the
/*	\fBbounce\fR(8), \fBdefer\fR(8) or \fBtrace\fR(8) daemons to
/*	send delivery reports.
/* STRATEGIES
/* .ad
/* .fi
/*	The queue manager implements a variety of strategies for
/*	either opening queue files (input) or for message delivery (output).
/* .IP "\fBleaky bucket\fR"
/*	This strategy limits the number of messages in the \fBactive\fR queue
/*	and prevents the queue manager from running out of memory under
/*	heavy load.
/* .IP \fBfairness\fR
/*	When the \fBactive\fR queue has room, the queue manager takes one
/*	message from the \fBincoming\fR queue and one from the \fBdeferred\fR
/*	queue. This prevents a large mail backlog from blocking the delivery
/*	of new mail.
/* .IP "\fBslow start\fR"
/*	This strategy eliminates "thundering herd" problems by slowly
/*	adjusting the number of parallel deliveries to the same destination.
/* .IP "\fBround robin\fR
/*	The queue manager sorts delivery requests by destination.
/*	Round-robin selection prevents one destination from dominating
/*	deliveries to other destinations.
/* .IP "\fBexponential backoff\fR"
/*	Mail that cannot be delivered upon the first attempt is deferred.
/*	The time interval between delivery attempts is doubled after each
/*	attempt.
/* .IP "\fBdestination status cache\fR"
/*	The queue manager avoids unnecessary delivery attempts by
/*	maintaining a short-term, in-memory list of unreachable destinations.
/* TRIGGERS
/* .ad
/* .fi
/*	On an idle system, the queue manager waits for the arrival of
/*	trigger events, or it waits for a timer to go off. A trigger
/*	is a one-byte message.
/*	Depending on the message received, the queue manager performs
/*	one of the following actions (the message is followed by the
/*	symbolic constant used internally by the software):
/* .IP "\fBD (QMGR_REQ_SCAN_DEFERRED)\fR"
/*	Start a deferred queue scan.  If a deferred queue scan is already
/*	in progress, that scan will be restarted as soon as it finishes.
/* .IP "\fBI (QMGR_REQ_SCAN_INCOMING)\fR"
/*	Start an incoming queue scan. If an incoming queue scan is already
/*	in progress, that scan will be restarted as soon as it finishes.
/* .IP "\fBA (QMGR_REQ_SCAN_ALL)\fR"
/*	Ignore deferred queue file time stamps. The request affects
/*	the next deferred queue scan.
/* .IP "\fBF (QMGR_REQ_FLUSH_DEAD)\fR"
/*	Purge all information about dead transports and destinations.
/* .IP "\fBW (TRIGGER_REQ_WAKEUP)\fR"
/*	Wakeup call, This is used by the master server to instantiate
/*	servers that should not go away forever. The action is to start
/*	an incoming queue scan.
/* .PP
/*	The \fBqmgr\fR(8) daemon reads an entire buffer worth of triggers.
/*	Multiple identical trigger requests are collapsed into one, and
/*	trigger requests are sorted so that \fBA\fR and \fBF\fR precede
/*	\fBD\fR and \fBI\fR. Thus, in order to force a deferred queue run,
/*	one would request \fBA F D\fR; in order to notify the queue manager
/*	of the arrival of new mail one would request \fBI\fR.
/* STANDARDS
/*	RFC 3463 (Enhanced status codes)
/*	RFC 3464 (Delivery status notifications)
/* SECURITY
/* .ad
/* .fi
/*	The \fBqmgr\fR(8) daemon is not security sensitive. It reads
/*	single-character messages from untrusted local users, and thus may
/*	be susceptible to denial of service attacks. The \fBqmgr\fR(8) daemon
/*	does not talk to the outside world, and it can be run at fixed low
/*	privilege in a chrooted environment.
/* DIAGNOSTICS
/*	Problems and transactions are logged to the \fBsyslog\fR(8) daemon.
/*	Corrupted message files are saved to the \fBcorrupt\fR queue
/*	for further inspection.
/*
/*	Depending on the setting of the \fBnotify_classes\fR parameter,
/*	the postmaster is notified of bounces and of other trouble.
/* BUGS
/*	A single queue manager process has to compete for disk access with
/*	multiple front-end processes such as \fBcleanup\fR(8). A sudden burst of
/*	inbound mail can negatively impact outbound delivery rates.
/* CONFIGURATION PARAMETERS
/* .ad
/* .fi
/*	Changes to \fBmain.cf\fR are not picked up automatically,
/*	as \fBqmgr\fR(8)
/*	is a persistent process. Use the command "\fBpostfix reload\fR" after
/*	a configuration change.
/*
/*	The text below provides only a parameter summary. See
/*	\fBpostconf\fR(5) for more details including examples.
/*
/*	In the text below, \fItransport\fR is the first field in a
/*	\fBmaster.cf\fR entry.
/* COMPATIBILITY CONTROLS
/* .ad
/* .fi
/* .IP "\fBallow_min_user (no)\fR"
/*	Allow a recipient address to have `-' as the first character.
/* ACTIVE QUEUE CONTROLS
/* .ad
/* .fi
/* .IP "\fBqmgr_clog_warn_time (300s)\fR"
/*	The minimal delay between warnings that a specific destination is
/*	clogging up the Postfix active queue.
/* .IP "\fBqmgr_message_active_limit (20000)\fR"
/*	The maximal number of messages in the active queue.
/* .IP "\fBqmgr_message_recipient_limit (20000)\fR"
/*	The maximal number of recipients held in memory by the Postfix
/*	queue manager, and the maximal size of the size of the short-term,
/*	in-memory "dead" destination status cache.
/* DELIVERY CONCURRENCY CONTROLS
/* .ad
/* .fi
/* .IP "\fBqmgr_fudge_factor (100)\fR"
/*	Obsolete feature: the percentage of delivery resources that a busy
/*	mail system will use up for delivery of a large mailing  list
/*	message.
/* .IP "\fBinitial_destination_concurrency (5)\fR"
/*	The initial per-destination concurrency level for parallel delivery
/*	to the same destination.
/* .IP "\fBdefault_destination_concurrency_limit (20)\fR"
/*	The default maximal number of parallel deliveries to the same
/*	destination.
/* .IP \fItransport\fB_destination_concurrency_limit\fR
/*	Idem, for delivery via the named message \fItransport\fR.
/* RECIPIENT SCHEDULING CONTROLS
/* .ad
/* .fi
/* .IP "\fBdefault_destination_recipient_limit (50)\fR"
/*	The default maximal number of recipients per message delivery.
/* .IP \fItransport\fB_destination_recipient_limit\fR
/*	Idem, for delivery via the named message \fItransport\fR.
/* OTHER RESOURCE AND RATE CONTROLS
/* .ad
/* .fi
/* .IP "\fBminimal_backoff_time (1000s)\fR"
/*	The minimal time between attempts to deliver a deferred message.
/* .IP "\fBmaximal_backoff_time (4000s)\fR"
/*	The maximal time between attempts to deliver a deferred message.
/* .IP "\fBmaximal_queue_lifetime (5d)\fR"
/*	The maximal time a message is queued before it is sent back as
/*	undeliverable.
/* .IP "\fBqueue_run_delay (1000s)\fR"
/*	The time between deferred queue scans by the queue manager.
/* .IP "\fBtransport_retry_time (60s)\fR"
/*	The time between attempts by the Postfix queue manager to contact
/*	a malfunctioning message delivery transport.
/* .PP
/*	Available in Postfix version 2.1 and later:
/* .IP "\fBbounce_queue_lifetime (5d)\fR"
/*	The maximal time a bounce message is queued before it is considered
/*	undeliverable.
/* .SH MISCELLANEOUS CONTROLS
/* .ad
/* .fi
/* .IP "\fBconfig_directory (see 'postconf -d' output)\fR"
/*	The default location of the Postfix main.cf and master.cf
/*	configuration files.
/* .IP "\fBdaemon_timeout (18000s)\fR"
/*	How much time a Postfix daemon process may take to handle a
/*	request before it is terminated by a built-in watchdog timer.
/* .IP "\fBdefer_transports (empty)\fR"
/*	The names of message delivery transports that should not be delivered
/*	to unless someone issues "\fBsendmail -q\fR" or equivalent.
/* .IP "\fBhelpful_warnings (yes)\fR"
/*	Log warnings about problematic configuration settings, and provide
/*	helpful suggestions.
/* .IP "\fBipc_timeout (3600s)\fR"
/*	The time limit for sending or receiving information over an internal
/*	communication channel.
/* .IP "\fBprocess_id (read-only)\fR"
/*	The process ID of a Postfix command or daemon process.
/* .IP "\fBprocess_name (read-only)\fR"
/*	The process name of a Postfix command or daemon process.
/* .IP "\fBqueue_directory (see 'postconf -d' output)\fR"
/*	The location of the Postfix top-level queue directory.
/* .IP "\fBsyslog_facility (mail)\fR"
/*	The syslog facility of Postfix logging.
/* .IP "\fBsyslog_name (postfix)\fR"
/*	The mail system name that is prepended to the process name in syslog
/*	records, so that "smtpd" becomes, for example, "postfix/smtpd".
/* FILES
/*	/var/spool/postfix/incoming, incoming queue
/*	/var/spool/postfix/active, active queue
/*	/var/spool/postfix/deferred, deferred queue
/*	/var/spool/postfix/bounce, non-delivery status
/*	/var/spool/postfix/defer, non-delivery status
/*	/var/spool/postfix/trace, delivery status
/* SEE ALSO
/*	trivial-rewrite(8), address routing
/*	bounce(8), delivery status reports
/*	postconf(5), configuration parameters
/*	master(5), generic daemon options
/*	master(8), process manager
/*	syslogd(8), system logging
/* README FILES
/* .ad
/* .fi
/*	Use "\fBpostconf readme_directory\fR" or
/*	"\fBpostconf html_directory\fR" to locate this information.
/* .na
/* .nf
/*	QSHAPE_README, Postfix queue analysis
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

/* Utility library. */

#include <msg.h>
#include <events.h>
#include <vstream.h>
#include <dict.h>

/* Global library. */

#include <mail_queue.h>
#include <recipient_list.h>
#include <mail_conf.h>
#include <mail_params.h>
#include <mail_proto.h>			/* QMGR_SCAN constants */
#include <mail_flow.h>
#include <flush_clnt.h>

/* Master process interface */

#include <master_proto.h>
#include <mail_server.h>

/* Application-specific. */

#include "qmgr.h"

 /*
  * Tunables.
  */
int     var_queue_run_delay;
int     var_min_backoff_time;
int     var_max_backoff_time;
int     var_max_queue_time;
int     var_dsn_queue_time;
int     var_qmgr_active_limit;
int     var_qmgr_rcpt_limit;
int     var_init_dest_concurrency;
int     var_transport_retry_time;
int     var_dest_con_limit;
int     var_dest_rcpt_limit;
char   *var_defer_xports;
bool    var_allow_min_user;
int     var_qmgr_fudge;
int     var_local_rcpt_lim;		/* XXX */
int     var_local_con_lim;		/* XXX */
int     var_proc_limit;
bool    var_verp_bounce_off;
bool    var_sender_routing;
int     var_qmgr_clog_warn_time;

static QMGR_SCAN *qmgr_incoming;
static QMGR_SCAN *qmgr_deferred;

/* qmgr_deferred_run_event - queue manager heartbeat */

static void qmgr_deferred_run_event(int unused_event, char *dummy)
{

    /*
     * This routine runs when it is time for another deferred queue scan.
     * Make sure this routine gets called again in the future.
     */
    qmgr_scan_request(qmgr_deferred, QMGR_SCAN_START);
    event_request_timer(qmgr_deferred_run_event, dummy, var_queue_run_delay);
}

/* qmgr_trigger_event - respond to external trigger(s) */

static void qmgr_trigger_event(char *buf, int len,
			               char *unused_service, char **argv)
{
    int     incoming_flag = 0;
    int     deferred_flag = 0;
    int     i;

    /*
     * Sanity check. This service takes no command-line arguments.
     */
    if (argv[0])
	msg_fatal("unexpected command-line argument: %s", argv[0]);

    /*
     * Collapse identical requests that have arrived since we looked last
     * time. There is no client feedback so there is no need to process each
     * request in order. And as long as we don't have conflicting requests we
     * are free to sort them into the most suitable order.
     */
    for (i = 0; i < len; i++) {
	if (msg_verbose)
	    msg_info("request: %d (%c)",
		     buf[i], ISALNUM(buf[i]) ? buf[i] : '?');
	switch (buf[i]) {
	case TRIGGER_REQ_WAKEUP:
	case QMGR_REQ_SCAN_INCOMING:
	    incoming_flag |= QMGR_SCAN_START;
	    break;
	case QMGR_REQ_SCAN_DEFERRED:
	    deferred_flag |= QMGR_SCAN_START;
	    break;
	case QMGR_REQ_FLUSH_DEAD:
	    deferred_flag |= QMGR_FLUSH_DEAD;
	    incoming_flag |= QMGR_FLUSH_DEAD;
	    break;
	case QMGR_REQ_SCAN_ALL:
	    deferred_flag |= QMGR_SCAN_ALL;
	    incoming_flag |= QMGR_SCAN_ALL;
	    break;
	default:
	    if (msg_verbose)
		msg_info("request ignored");
	    break;
	}
    }

    /*
     * Process each request type at most once. Modifiers take effect upon the
     * next queue run. If no queue run is in progress, and a queue scan is
     * requested, the request takes effect immediately.
     */
    if (incoming_flag != 0)
	qmgr_scan_request(qmgr_incoming, incoming_flag);
    if (deferred_flag != 0)
	qmgr_scan_request(qmgr_deferred, deferred_flag);
}

/* qmgr_loop - queue manager main loop */

static int qmgr_loop(char *unused_name, char **unused_argv)
{
    char   *in_path = 0;
    char   *df_path = 0;
    int     token_count;
    int     in_feed = 0;

    /*
     * This routine runs as part of the event handling loop, after the event
     * manager has delivered a timer or I/O event (including the completion
     * of a connection to a delivery process), or after it has waited for a
     * specified amount of time. The result value of qmgr_loop() specifies
     * how long the event manager should wait for the next event.
     */
#define DONT_WAIT	0
#define WAIT_FOR_EVENT	(-1)

    /*
     * Attempt to drain the active queue by allocating a suitable delivery
     * process and by delivering mail via it. Delivery process allocation and
     * mail delivery are asynchronous.
     */
    qmgr_active_drain();

    /*
     * Let some new blood into the active queue when the queue size is
     * smaller than some configurable limit, and when the number of in-core
     * recipients does not exceed some configurable limit. When the system is
     * under heavy load, favor new mail over old mail.
     */
    if (qmgr_message_count < var_qmgr_active_limit
	&& qmgr_recipient_count < var_qmgr_rcpt_limit)
	if ((in_path = qmgr_scan_next(qmgr_incoming)) != 0)
	    in_feed = qmgr_active_feed(qmgr_incoming, in_path);
    if (qmgr_message_count < var_qmgr_active_limit
	&& qmgr_recipient_count < var_qmgr_rcpt_limit)
	if ((df_path = qmgr_scan_next(qmgr_deferred)) != 0)
	    qmgr_active_feed(qmgr_deferred, df_path);

    /*
     * Global flow control. If enabled, slow down receiving processes that
     * get ahead of the queue manager, but don't block them completely.
     */
    if (var_in_flow_delay > 0) {
	token_count = mail_flow_count();
	if (token_count < var_proc_limit) {
	    if (in_feed != 0)
		mail_flow_put(1);
	    else if (qmgr_incoming->handle == 0)
		mail_flow_put(var_proc_limit - token_count);
	} else if (token_count > var_proc_limit) {
	    mail_flow_get(token_count - var_proc_limit);
	}
    }
    if (in_path || df_path)
	return (DONT_WAIT);
    return (WAIT_FOR_EVENT);
}

/* pre_accept - see if tables have changed */

static void pre_accept(char *unused_name, char **unused_argv)
{
    const char *table;

    if ((table = dict_changed_name()) != 0) {
	msg_info("table %s has changed -- restarting", table);
	exit(0);
    }
}

/* qmgr_pre_init - pre-jail initialization */

static void qmgr_pre_init(char *unused_name, char **unused_argv)
{
    flush_init();
}

/* qmgr_post_init - post-jail initialization */

static void qmgr_post_init(char *unused_name, char **unused_argv)
{

    /*
     * Sanity check.
     */
    if (var_qmgr_rcpt_limit < var_qmgr_active_limit) {
	msg_warn("%s is smaller than %s - adjusting %s",
	      VAR_QMGR_RCPT_LIMIT, VAR_QMGR_ACT_LIMIT, VAR_QMGR_RCPT_LIMIT);
	var_qmgr_rcpt_limit = var_qmgr_active_limit;
    }
    if (var_dsn_queue_time > var_max_queue_time) {
	msg_warn("%s is larger than %s - adjusting %s",
		 VAR_DSN_QUEUE_TIME, VAR_MAX_QUEUE_TIME, VAR_DSN_QUEUE_TIME);
	var_dsn_queue_time = var_max_queue_time;
    }

    /*
     * This routine runs after the skeleton code has entered the chroot jail.
     * Prevent automatic process suicide after a limited number of client
     * requests or after a limited amount of idle time. Move any left-over
     * entries from the active queue to the incoming queue, and give them a
     * time stamp into the future, in order to allow ongoing deliveries to
     * finish first. Start scanning the incoming and deferred queues.
     * Left-over active queue entries are moved to the incoming queue because
     * the incoming queue has priority; moving left-overs to the deferred
     * queue could cause anomalous delays when "postfix reload/start" are
     * issued often.
     */
    var_use_limit = 0;
    var_idle_limit = 0;
    qmgr_move(MAIL_QUEUE_ACTIVE, MAIL_QUEUE_INCOMING, event_time());
    qmgr_incoming = qmgr_scan_create(MAIL_QUEUE_INCOMING);
    qmgr_deferred = qmgr_scan_create(MAIL_QUEUE_DEFERRED);
    qmgr_scan_request(qmgr_incoming, QMGR_SCAN_START);
    qmgr_deferred_run_event(0, (char *) 0);
}

/* main - the main program */

int     main(int argc, char **argv)
{
    static CONFIG_STR_TABLE str_table[] = {
	VAR_DEFER_XPORTS, DEF_DEFER_XPORTS, &var_defer_xports, 0, 0,
	0,
    };
    static CONFIG_TIME_TABLE time_table[] = {
	VAR_QUEUE_RUN_DELAY, DEF_QUEUE_RUN_DELAY, &var_queue_run_delay, 1, 0,
	VAR_MIN_BACKOFF_TIME, DEF_MIN_BACKOFF_TIME, &var_min_backoff_time, 1, 0,
	VAR_MAX_BACKOFF_TIME, DEF_MAX_BACKOFF_TIME, &var_max_backoff_time, 1, 0,
	VAR_MAX_QUEUE_TIME, DEF_MAX_QUEUE_TIME, &var_max_queue_time, 0, 8640000,
	VAR_DSN_QUEUE_TIME, DEF_DSN_QUEUE_TIME, &var_dsn_queue_time, 0, 8640000,
	VAR_XPORT_RETRY_TIME, DEF_XPORT_RETRY_TIME, &var_transport_retry_time, 1, 0,
	VAR_QMGR_CLOG_WARN_TIME, DEF_QMGR_CLOG_WARN_TIME, &var_qmgr_clog_warn_time, 0, 0,
	0,
    };
    static CONFIG_INT_TABLE int_table[] = {
	VAR_QMGR_ACT_LIMIT, DEF_QMGR_ACT_LIMIT, &var_qmgr_active_limit, 1, 0,
	VAR_QMGR_RCPT_LIMIT, DEF_QMGR_RCPT_LIMIT, &var_qmgr_rcpt_limit, 1, 0,
	VAR_INIT_DEST_CON, DEF_INIT_DEST_CON, &var_init_dest_concurrency, 1, 0,
	VAR_DEST_CON_LIMIT, DEF_DEST_CON_LIMIT, &var_dest_con_limit, 0, 0,
	VAR_DEST_RCPT_LIMIT, DEF_DEST_RCPT_LIMIT, &var_dest_rcpt_limit, 0, 0,
	VAR_QMGR_FUDGE, DEF_QMGR_FUDGE, &var_qmgr_fudge, 10, 100,
	VAR_LOCAL_RCPT_LIMIT, DEF_LOCAL_RCPT_LIMIT, &var_local_rcpt_lim, 0, 0,
	VAR_LOCAL_CON_LIMIT, DEF_LOCAL_CON_LIMIT, &var_local_con_lim, 0, 0,
	VAR_PROC_LIMIT, DEF_PROC_LIMIT, &var_proc_limit, 1, 0,
	0,
    };
    static CONFIG_BOOL_TABLE bool_table[] = {
	VAR_ALLOW_MIN_USER, DEF_ALLOW_MIN_USER, &var_allow_min_user,
	VAR_VERP_BOUNCE_OFF, DEF_VERP_BOUNCE_OFF, &var_verp_bounce_off,
	VAR_SENDER_ROUTING, DEF_SENDER_ROUTING, &var_sender_routing,
	0,
    };

    /*
     * Use the trigger service skeleton, because no-one else should be
     * monitoring our service port while this process runs, and because we do
     * not talk back to the client.
     */
    trigger_server_main(argc, argv, qmgr_trigger_event,
			MAIL_SERVER_INT_TABLE, int_table,
			MAIL_SERVER_STR_TABLE, str_table,
			MAIL_SERVER_BOOL_TABLE, bool_table,
			MAIL_SERVER_TIME_TABLE, time_table,
			MAIL_SERVER_PRE_INIT, qmgr_pre_init,
			MAIL_SERVER_POST_INIT, qmgr_post_init,
			MAIL_SERVER_LOOP, qmgr_loop,
			MAIL_SERVER_PRE_ACCEPT, pre_accept,
			MAIL_SERVER_SOLITARY,
			0);
}
