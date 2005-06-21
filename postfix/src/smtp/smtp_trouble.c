/*++
/* NAME
/*	smtp_trouble 3
/* SUMMARY
/*	error handler policies
/* SYNOPSIS
/*	#include "smtp.h"
/*
/*	int	smtp_sess_fail(state, why)
/*	SMTP_STATE *state;
/*	DSN_BUF	*why;
/*
/*	int	smtp_site_fail(state, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	int	smtp_mesg_fail(state, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	void	smtp_rcpt_fail(state, recipient, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	RECIPIENT *recipient;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	int	smtp_stream_except(state, exception, description)
/*	SMTP_STATE *state;
/*	int	exception;
/*	const char *description;
/* DESCRIPTION
/*	This module handles all non-fatal errors that can happen while
/*	attempting to deliver mail via SMTP, and implements the policy
/*	of how to deal with the error. Depending on the nature of
/*	the problem, delivery of a single message is deferred, delivery
/*	of all messages to the same domain is deferred, or one or more
/*	recipients are given up as non-deliverable and a bounce log is
/*	updated. In any case, the recipient is marked as either KEEP
/*	(try again with a backup host) or DROP (delete recipient from
/*	delivery request).
/*
/*	In addition, when an unexpected response code is seen such
/*	as 3xx where only 4xx or 5xx are expected, or any error code
/*	that suggests a syntax error or something similar, the
/*	protocol error flag is set so that the postmaster receives
/*	a transcript of the session. No notification is generated for
/*	what appear to be configuration errors - very likely, they
/*	would suffer the same problem and just cause more trouble.
/*
/*	In case of a soft error, action depends on whether the error
/*	qualifies for trying the request with other mail servers (log
/*	an informational record only and try a backup server) or
/*	whether this is the final server (log recipient delivery status
/*	records and delete the recipient from the request).
/*
/*	smtp_sess_fail() takes a pre-formatted error report after
/*	failure to complete some protocol handshake.  The policy is
/*	as with smtp_site_fail().
/*
/*	smtp_site_fail() handles the case where the program fails to
/*	complete the initial handshake: the server is not reachable,
/*	is not running, does not want talk to us, or we talk to ourselves.
/*	The \fIcode\fR gives an error status code; the \fIformat\fR
/*	argument gives a textual description.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the host is being skipped; soft error, final server:
/*	defer delivery of all remaining recipients and mark the destination
/*	as problematic; hard error: bounce all remaining recipients.
/*	The session is marked as "do not cache".
/*	The result is non-zero.
/*
/*	smtp_mesg_fail() handles the case where the smtp server
/*	does not accept the sender address or the message data,
/*	or when the local MTA is unable to convert the message data.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the host is being skipped; soft error, final server:
/*	defer delivery of all remaining recipients; hard error: bounce all
/*	remaining recipients.
/*	The result is non-zero.
/*
/*	smtp_rcpt_fail() handles the case where a recipient is not
/*	accepted by the server for reasons other than that the server
/*	recipient limit is reached.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the recipient is being skipped; soft error, final server:
/*	defer delivery of this recipient; hard error: bounce this
/*	recipient.
/*
/*	smtp_stream_except() handles the exceptions generated by
/*	the smtp_stream(3) module (i.e. timeouts and I/O errors).
/*	The \fIexception\fR argument specifies the type of problem.
/*	The \fIdescription\fR argument describes at what stage of
/*	the SMTP dialog the problem happened.
/*	The policy is: non-final server: log an informational record
/*	with the reason why the host is being skipped; final server:
/*	defer delivery of all remaining recipients.
/*	The session is marked as "do not cache".
/*	The result is non-zero.
/*
/*	Arguments:
/* .IP state
/*	SMTP client state per delivery request.
/* .IP resp
/*	Server response including reply code and text.
/* .IP recipient
/*	Undeliverable recipient address information.
/* .IP format
/*	Human-readable description of why mail is not deliverable.
/* DIAGNOSTICS
/*	Panic: unknown exception code.
/* SEE ALSO
/*	smtp_proto(3) smtp high-level protocol
/*	smtp_stream(3) smtp low-level protocol
/*	defer(3) basic message defer interface
/*	bounce(3) basic message bounce interface
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
#include <stdlib.h>			/* 44BSD stdarg.h uses abort() */
#include <stdarg.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <stringops.h>

/* Global library. */

#include <smtp_stream.h>
#include <deliver_request.h>
#include <deliver_completed.h>
#include <bounce.h>
#include <defer.h>
#include <mail_error.h>
#include <dsn_buf.h>
#include <dsn.h>

/* Application-specific. */

#include "smtp.h"

#define SMTP_THROTTLE	1
#define SMTP_NOTHROTTLE	0

/* smtp_check_code - check response code */

static void smtp_check_code(SMTP_SESSION *session, int code)
{

    /*
     * The intention of this code is to alert the postmaster when the local
     * Postfix SMTP client screws up, protocol wise. RFC 821 says that x0z
     * replies "refer to syntax errors, syntactically correct commands that
     * don't fit any functional category, and unimplemented or superfluous
     * commands". Unfortunately, this also triggers postmaster notices when
     * remote servers screw up, protocol wise. This is becoming a common
     * problem now that response codes are configured manually as part of
     * anti-UCE systems, by people who aren't aware of RFC details.
     */
    if (code < 400 || code > 599
	|| code == 555			/* RFC 1869, section 6.1. */
	|| (code >= 500 && code < 510))
	session->error_mask |= MAIL_ERROR_PROTOCOL;
}

/* smtp_bulk_fail - skip, defer or bounce recipients, maybe throttle queue */

static int smtp_bulk_fail(SMTP_STATE *state, DSN *dsn, int throttle_queue)
{
    DELIVER_REQUEST *request = state->request;
    SMTP_SESSION *session = state->session;
    RECIPIENT *rcpt;
    int     status;
    int     soft_error = (dsn->dtext[0] == '4');
    int     nrcpt;

    /*
     * Don't defer the recipients just yet when this error qualifies them for
     * delivery to a backup server. Just log something informative to show
     * why we're skipping this host.
     */
    if (soft_error && state->final_server == 0) {
	msg_info("%s: %s", request->queue_id, dsn->reason);
	for (nrcpt = 0; nrcpt < SMTP_RCPT_LEFT(state); nrcpt++) {
	    rcpt = request->rcpt_list.info + nrcpt;
	    if (SMTP_RCPT_ISMARKED(rcpt))
		continue;
	    SMTP_RCPT_KEEP(state, rcpt);
	}
    }

    /*
     * Defer or bounce all the remaining recipients, and delete them from the
     * delivery request. If a bounce fails, defer instead and do not qualify
     * the recipient for delivery to a backup server.
     */
    else {
	for (nrcpt = 0; nrcpt < SMTP_RCPT_LEFT(state); nrcpt++) {
	    rcpt = request->rcpt_list.info + nrcpt;
	    if (SMTP_RCPT_ISMARKED(rcpt))
		continue;
	    status = (soft_error ? defer_append : bounce_append)
		(DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
		 request->arrival_time, rcpt,
		 session ? session->namaddr : "none", dsn);
	    if (status == 0)
		deliver_completed(state->src, rcpt->offset);
	    SMTP_RCPT_DROP(state, rcpt);
	    state->status |= status;
	}
	if (throttle_queue && soft_error && request->hop_status == 0)
	    request->hop_status = DSN_COPY(dsn);
    }

    /*
     * Don't cache this session. We can't talk to this server.
     */
    if (throttle_queue && session)
	session->reuse_count = 0;

    return (-1);
}

/* smtp_sess_fail - skip site, defer or bounce all recipients */

int     smtp_sess_fail(SMTP_STATE *state, DSN_BUF *why)
{
    DSN     dsn;

    /*
     * We need to incur the expense of copying lots of strings into VSTRING
     * buffers when the error information is collected by a routine that
     * terminates BEFORE the error is reported. If no copies were made, the
     * information would not be frozen in time.
     */
    return (smtp_bulk_fail(state, DSN_FROM_DSN_BUF(&dsn, why), SMTP_THROTTLE));
}

/* vsmtp_fill_dsn - fill in temporary DSN structure */

static void vsmtp_fill_dsn(SMTP_STATE *state, DSN *dsn, const char *mta_name,
			           const char *status, const char *reply,
			           const char *format, va_list ap)
{

    /*
     * We can avoid the cost of copying lots of strings into VSTRING buffers
     * when the error information is collected by the routine that terminates
     * AFTER the error is reported. In this case, the information is already
     * frozen in time, so we don't need to make copies.
     */
    if (state->dsn_reason == 0)
	state->dsn_reason = vstring_alloc(100);
    else
	VSTRING_RESET(state->dsn_reason);
    if (mta_name && reply[0] != '4' && reply[0] != '5') {
	vstring_strcpy(state->dsn_reason, "Protocol error: ");
	mta_name = DSN_BY_LOCAL_MTA;
	status = "5.5.0";
	reply = "501 Protocol error in server reply";
    }
    vstring_vsprintf_append(state->dsn_reason, format, ap);
    SMTP_DSN_ASSIGN(dsn, mta_name, status, reply, STR(state->dsn_reason));
}

/* smtp_fill_dsn - fill in temporary DSN structure */

static void smtp_fill_dsn(SMTP_STATE *state, DSN *dsn, const char *mta_name,
			          const char *status, const char *reply,
			          const char *format,...)
{
    va_list ap;

    va_start(ap, format);
    vsmtp_fill_dsn(state, dsn, mta_name, status, reply, format, ap);
    va_end(ap);
}

/* smtp_site_fail - throttle this queue; skip, defer or bounce all recipients */

int     smtp_site_fail(SMTP_STATE *state, const char *mta_name, SMTP_RESP *resp,
		               const char *format,...)
{
    DSN     dsn;
    va_list ap;

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, &dsn, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Skip, defer or bounce recipients, and throttle this queue.
     */
    return (smtp_bulk_fail(state, &dsn, SMTP_THROTTLE));
}

/* smtp_mesg_fail - skip, defer or bounce all recipients; no queue throttle */

int     smtp_mesg_fail(SMTP_STATE *state, const char *mta_name, SMTP_RESP *resp,
		               const char *format,...)
{
    va_list ap;
    DSN     dsn;

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, &dsn, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Skip, defer or bounce recipients, but don't throttle this queue.
     */
    return (smtp_bulk_fail(state, &dsn, SMTP_NOTHROTTLE));
}

/* smtp_rcpt_fail - skip, defer, or bounce recipient */

void    smtp_rcpt_fail(SMTP_STATE *state, RECIPIENT *rcpt, const char *mta_name,
		               SMTP_RESP *resp, const char *format,...)
{
    DELIVER_REQUEST *request = state->request;
    SMTP_SESSION *session = state->session;
    DSN     dsn;
    int     status;
    int     soft_error;
    va_list ap;

    /*
     * Sanity check.
     */
    if (SMTP_RCPT_ISMARKED(rcpt))
	msg_panic("smtp_rcpt_fail: recipient <%s> is marked", rcpt->address);

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, &dsn, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);
    soft_error = dsn.dtext[0] == '4';

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Don't defer this recipient record just yet when this error qualifies
     * for trying other mail servers. Just log something informative to show
     * why we're skipping this recipient now.
     */
    if (soft_error && state->final_server == 0) {
	msg_info("%s: %s", request->queue_id, dsn.reason);
	SMTP_RCPT_KEEP(state, rcpt);
    }

    /*
     * Defer or bounce this recipient, and delete from the delivery request.
     * If the bounce fails, defer instead and do not qualify the recipient
     * for delivery to a backup server.
     * 
     * Note: we may still make an SMTP connection to deliver other recipients
     * that did qualify for delivery to a backup server.
     */
    else {
	status = (soft_error ? defer_append : bounce_append)
	    (DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
	     request->arrival_time, rcpt,
	     session ? session->namaddr : "none", &dsn);
	if (status == 0)
	    deliver_completed(state->src, rcpt->offset);
	SMTP_RCPT_DROP(state, rcpt);
	state->status |= status;
    }
}

/* smtp_stream_except - defer domain after I/O problem */

int     smtp_stream_except(SMTP_STATE *state, int code, const char *description)
{
    SMTP_SESSION *session = state->session;
    DSN     dsn;

    /*
     * Sanity check.
     */
    if (session == 0)
	msg_panic("smtp_stream_except: no session");

    /*
     * Initialize.
     */
    switch (code) {
    default:
	msg_panic("smtp_stream_except: unknown exception %d", code);
    case SMTP_ERR_EOF:
	smtp_fill_dsn(state, &dsn, DSN_BY_LOCAL_MTA,
		      "4.4.2", "421 lost connection",
		      "lost connection with %s while %s",
		      session->namaddr, description);
	break;
    case SMTP_ERR_TIME:
	smtp_fill_dsn(state, &dsn, DSN_BY_LOCAL_MTA,
		      "4.4.2", "426 conversation timed out",
		      "conversation with %s timed out while %s",
		      session->namaddr, description);
	break;
    }
    return (smtp_bulk_fail(state, &dsn, SMTP_THROTTLE));
}
