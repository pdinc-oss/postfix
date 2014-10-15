    static const CONFIG_STR_TABLE lmtp_str_table[] = {
	VAR_NOTIFY_CLASSES, DEF_NOTIFY_CLASSES, &var_notify_classes, 0, 0,
	VAR_BESTMX_TRANSP, DEF_BESTMX_TRANSP, &var_bestmx_transp, 0, 0,
	VAR_ERROR_RCPT, DEF_ERROR_RCPT, &var_error_rcpt, 1, 0,
	VAR_LMTP_SASL_PASSWD, DEF_LMTP_SASL_PASSWD, &var_smtp_sasl_passwd, 0, 0,
	VAR_LMTP_SASL_OPTS, DEF_LMTP_SASL_OPTS, &var_smtp_sasl_opts, 0, 0,
	VAR_LMTP_SASL_PATH, DEF_LMTP_SASL_PATH, &var_smtp_sasl_path, 0, 0,
#ifdef USE_TLS
	VAR_LMTP_SASL_TLS_OPTS, DEF_LMTP_SASL_TLS_OPTS, &var_smtp_sasl_tls_opts, 0, 0,
	VAR_LMTP_SASL_TLSV_OPTS, DEF_LMTP_SASL_TLSV_OPTS, &var_smtp_sasl_tlsv_opts, 0, 0,
	VAR_LMTP_TLS_CERT_FILE, DEF_LMTP_TLS_CERT_FILE, &var_smtp_tls_cert_file, 0, 0,
	VAR_LMTP_TLS_KEY_FILE, DEF_LMTP_TLS_KEY_FILE, &var_smtp_tls_key_file, 0, 0,
	VAR_LMTP_TLS_DCERT_FILE, DEF_LMTP_TLS_DCERT_FILE, &var_smtp_tls_dcert_file, 0, 0,
	VAR_LMTP_TLS_DKEY_FILE, DEF_LMTP_TLS_DKEY_FILE, &var_smtp_tls_dkey_file, 0, 0,
	VAR_LMTP_TLS_CA_FILE, DEF_LMTP_TLS_CA_FILE, &var_smtp_tls_CAfile, 0, 0,
	VAR_LMTP_TLS_CA_PATH, DEF_LMTP_TLS_CA_PATH, &var_smtp_tls_CApath, 0, 0,
	VAR_LMTP_TLS_MAND_CIPH, DEF_LMTP_TLS_MAND_CIPH, &var_smtp_tls_mand_ciph, 1, 0,
	VAR_LMTP_TLS_EXCL_CIPH, DEF_LMTP_TLS_EXCL_CIPH, &var_smtp_tls_excl_ciph, 0, 0,
	VAR_LMTP_TLS_MAND_EXCL, DEF_LMTP_TLS_MAND_EXCL, &var_smtp_tls_mand_excl, 0, 0,
	VAR_LMTP_TLS_MAND_PROTO, DEF_LMTP_TLS_MAND_PROTO, &var_smtp_tls_mand_proto, 0, 0,
	VAR_LMTP_TLS_VFY_CMATCH, DEF_LMTP_TLS_VFY_CMATCH, &var_smtp_tls_vfy_cmatch, 1, 0,
	VAR_LMTP_TLS_SEC_CMATCH, DEF_LMTP_TLS_SEC_CMATCH, &var_smtp_tls_sec_cmatch, 1, 0,
	VAR_LMTP_TLS_FPT_CMATCH, DEF_LMTP_TLS_FPT_CMATCH, &var_smtp_tls_fpt_cmatch, 0, 0,
	VAR_LMTP_TLS_FPT_DGST, DEF_LMTP_TLS_FPT_DGST, &var_smtp_tls_fpt_dgst, 1, 0,
	VAR_LMTP_TLS_TAFILE, DEF_LMTP_TLS_TAFILE, &var_smtp_tls_tafile, 0, 0,
	VAR_LMTP_TLS_PROTO, DEF_LMTP_TLS_PROTO, &var_smtp_tls_proto, 0, 0,
	VAR_LMTP_TLS_CIPH, DEF_LMTP_TLS_CIPH, &var_smtp_tls_ciph, 1, 0,
	VAR_LMTP_TLS_ECCERT_FILE, DEF_LMTP_TLS_ECCERT_FILE, &var_smtp_tls_eccert_file, 0, 0,
	VAR_LMTP_TLS_ECKEY_FILE, DEF_LMTP_TLS_ECKEY_FILE, &var_smtp_tls_eckey_file, 0, 0,
	VAR_LMTP_TLS_LOGLEVEL, DEF_LMTP_TLS_LOGLEVEL, &var_smtp_tls_loglevel, 0, 0,
#endif
	VAR_LMTP_SASL_MECHS, DEF_LMTP_SASL_MECHS, &var_smtp_sasl_mechs, 0, 0,
	VAR_LMTP_SASL_TYPE, DEF_LMTP_SASL_TYPE, &var_smtp_sasl_type, 1, 0,
	VAR_LMTP_BIND_ADDR, DEF_LMTP_BIND_ADDR, &var_smtp_bind_addr, 0, 0,
	VAR_LMTP_BIND_ADDR6, DEF_LMTP_BIND_ADDR6, &var_smtp_bind_addr6, 0, 0,
	VAR_LMTP_HELO_NAME, DEF_LMTP_HELO_NAME, &var_smtp_helo_name, 1, 0,
	VAR_LMTP_HOST_LOOKUP, DEF_LMTP_HOST_LOOKUP, &var_smtp_host_lookup, 1, 0,
	VAR_LMTP_DNS_SUPPORT, DEF_LMTP_DNS_SUPPORT, &var_smtp_dns_support, 0, 0,
	VAR_LMTP_CACHE_DEST, DEF_LMTP_CACHE_DEST, &var_smtp_cache_dest, 0, 0,
	VAR_SCACHE_SERVICE, DEF_SCACHE_SERVICE, &var_scache_service, 1, 0,
	VAR_LMTP_EHLO_DIS_WORDS, DEF_LMTP_EHLO_DIS_WORDS, &var_smtp_ehlo_dis_words, 0, 0,
	VAR_LMTP_EHLO_DIS_MAPS, DEF_LMTP_EHLO_DIS_MAPS, &var_smtp_ehlo_dis_maps, 0, 0,
	VAR_LMTP_TLS_PER_SITE, DEF_LMTP_TLS_PER_SITE, &var_smtp_tls_per_site, 0, 0,
	VAR_LMTP_TLS_LEVEL, DEF_LMTP_TLS_LEVEL, &var_smtp_tls_level, 0, 0,
	VAR_LMTP_TLS_POLICY, DEF_LMTP_TLS_POLICY, &var_smtp_tls_policy, 0, 0,
	VAR_PROP_EXTENSION, DEF_PROP_EXTENSION, &var_prop_extension, 0, 0,
	VAR_LMTP_GENERIC_MAPS, DEF_LMTP_GENERIC_MAPS, &var_smtp_generic_maps, 0, 0,
	VAR_LMTP_TCP_PORT, DEF_LMTP_TCP_PORT, &var_lmtp_tcp_port, 0, 0,
	VAR_LMTP_PIX_BUG_WORDS, DEF_LMTP_PIX_BUG_WORDS, &var_smtp_pix_bug_words, 0, 0,
	VAR_LMTP_PIX_BUG_MAPS, DEF_LMTP_PIX_BUG_MAPS, &var_smtp_pix_bug_maps, 0, 0,
	VAR_LMTP_SASL_AUTH_CACHE_NAME, DEF_LMTP_SASL_AUTH_CACHE_NAME, &var_smtp_sasl_auth_cache_name, 0, 0,
	VAR_CYRUS_CONF_PATH, DEF_CYRUS_CONF_PATH, &var_cyrus_conf_path, 0, 0,
	VAR_LMTP_HEAD_CHKS, DEF_LMTP_HEAD_CHKS, &var_smtp_head_chks, 0, 0,
	VAR_LMTP_MIME_CHKS, DEF_LMTP_MIME_CHKS, &var_smtp_mime_chks, 0, 0,
	VAR_LMTP_NEST_CHKS, DEF_LMTP_NEST_CHKS, &var_smtp_nest_chks, 0, 0,
	VAR_LMTP_BODY_CHKS, DEF_LMTP_BODY_CHKS, &var_smtp_body_chks, 0, 0,
	VAR_LMTP_RESP_FILTER, DEF_LMTP_RESP_FILTER, &var_smtp_resp_filter, 0, 0,
	VAR_LMTP_ADDR_PREF, DEF_LMTP_ADDR_PREF, &var_smtp_addr_pref, 1, 0,
	VAR_LMTP_DNS_RES_OPT, DEF_LMTP_DNS_RES_OPT, &var_smtp_dns_res_opt, 0, 0,
	VAR_LMTP_DSN_FILTER, DEF_LMTP_DSN_FILTER, &var_smtp_dsn_filter, 0, 0,
	0,
    };
    static const CONFIG_TIME_TABLE lmtp_time_table[] = {
	VAR_LMTP_CONN_TMOUT, DEF_LMTP_CONN_TMOUT, &var_smtp_conn_tmout, 0, 0,
	VAR_LMTP_HELO_TMOUT, DEF_LMTP_HELO_TMOUT, &var_smtp_helo_tmout, 1, 0,
	VAR_LMTP_XFWD_TMOUT, DEF_LMTP_XFWD_TMOUT, &var_smtp_xfwd_tmout, 1, 0,
	VAR_LMTP_MAIL_TMOUT, DEF_LMTP_MAIL_TMOUT, &var_smtp_mail_tmout, 1, 0,
	VAR_LMTP_RCPT_TMOUT, DEF_LMTP_RCPT_TMOUT, &var_smtp_rcpt_tmout, 1, 0,
	VAR_LMTP_DATA0_TMOUT, DEF_LMTP_DATA0_TMOUT, &var_smtp_data0_tmout, 1, 0,
	VAR_LMTP_DATA1_TMOUT, DEF_LMTP_DATA1_TMOUT, &var_smtp_data1_tmout, 1, 0,
	VAR_LMTP_DATA2_TMOUT, DEF_LMTP_DATA2_TMOUT, &var_smtp_data2_tmout, 1, 0,
	VAR_LMTP_RSET_TMOUT, DEF_LMTP_RSET_TMOUT, &var_smtp_rset_tmout, 1, 0,
	VAR_LMTP_QUIT_TMOUT, DEF_LMTP_QUIT_TMOUT, &var_smtp_quit_tmout, 1, 0,
	VAR_LMTP_PIX_THRESH, DEF_LMTP_PIX_THRESH, &var_smtp_pix_thresh, 0, 0,
	VAR_LMTP_PIX_DELAY, DEF_LMTP_PIX_DELAY, &var_smtp_pix_delay, 1, 0,
	VAR_QUEUE_RUN_DELAY, DEF_QUEUE_RUN_DELAY, &var_queue_run_delay, 1, 0,
	VAR_MIN_BACKOFF_TIME, DEF_MIN_BACKOFF_TIME, &var_min_backoff_time, 1, 0,
	VAR_LMTP_CACHE_CONNT, DEF_LMTP_CACHE_CONNT, &var_smtp_cache_conn, 1, 0,
	VAR_LMTP_REUSE_TIME, DEF_LMTP_REUSE_TIME, &var_smtp_reuse_time, 1, 0,
#ifdef USE_TLS
	VAR_LMTP_STARTTLS_TMOUT, DEF_LMTP_STARTTLS_TMOUT, &var_smtp_starttls_tmout, 1, 0,
#endif
	VAR_SCACHE_PROTO_TMOUT, DEF_SCACHE_PROTO_TMOUT, &var_scache_proto_tmout, 1, 0,
	VAR_LMTP_SASL_AUTH_CACHE_TIME, DEF_LMTP_SASL_AUTH_CACHE_TIME, &var_smtp_sasl_auth_cache_time, 0, 0,
	0,
    };
    static const CONFIG_INT_TABLE lmtp_int_table[] = {
	VAR_LMTP_LINE_LIMIT, DEF_LMTP_LINE_LIMIT, &var_smtp_line_limit, 0, 0,
	VAR_LMTP_MXADDR_LIMIT, DEF_LMTP_MXADDR_LIMIT, &var_smtp_mxaddr_limit, 0, 0,
	VAR_LMTP_MXSESS_LIMIT, DEF_LMTP_MXSESS_LIMIT, &var_smtp_mxsess_limit, 0, 0,
	VAR_LMTP_REUSE_COUNT, DEF_LMTP_REUSE_COUNT, &var_smtp_reuse_count, 0, 0,
#ifdef USE_TLS
	VAR_LMTP_TLS_SCERT_VD, DEF_LMTP_TLS_SCERT_VD, &var_smtp_tls_scert_vd, 0, 0,
#endif
	0,
    };
    static const CONFIG_BOOL_TABLE lmtp_bool_table[] = {
	VAR_LMTP_SKIP_5XX, DEF_LMTP_SKIP_5XX, &var_smtp_skip_5xx_greeting,
	VAR_LMTP_SKIP_QUIT_RESP, DEF_LMTP_SKIP_QUIT_RESP, &var_skip_quit_resp,
	VAR_LMTP_SASL_ENABLE, DEF_LMTP_SASL_ENABLE, &var_smtp_sasl_enable,
	VAR_LMTP_RAND_ADDR, DEF_LMTP_RAND_ADDR, &var_smtp_rand_addr,
	VAR_LMTP_QUOTE_821_ENV, DEF_LMTP_QUOTE_821_ENV, &var_smtp_quote_821_env,
	VAR_LMTP_DEFER_MXADDR, DEF_LMTP_DEFER_MXADDR, &var_smtp_defer_mxaddr,
	VAR_LMTP_SEND_XFORWARD, DEF_LMTP_SEND_XFORWARD, &var_smtp_send_xforward,
	VAR_LMTP_CACHE_DEMAND, DEF_LMTP_CACHE_DEMAND, &var_smtp_cache_demand,
	VAR_LMTP_USE_TLS, DEF_LMTP_USE_TLS, &var_smtp_use_tls,
	VAR_LMTP_ENFORCE_TLS, DEF_LMTP_ENFORCE_TLS, &var_smtp_enforce_tls,
#ifdef USE_TLS
	VAR_LMTP_TLS_ENFORCE_PN, DEF_LMTP_TLS_ENFORCE_PN, &var_smtp_tls_enforce_peername,
	VAR_LMTP_TLS_NOTEOFFER, DEF_LMTP_TLS_NOTEOFFER, &var_smtp_tls_note_starttls_offer,
	VAR_LMTP_TLS_BLK_EARLY_MAIL_REPLY, DEF_LMTP_TLS_BLK_EARLY_MAIL_REPLY, &var_smtp_tls_blk_early_mail_reply,
	VAR_LMTP_TLS_FORCE_TLSA, DEF_LMTP_TLS_FORCE_TLSA, &var_smtp_tls_force_tlsa,
#endif
	VAR_LMTP_SENDER_AUTH, DEF_LMTP_SENDER_AUTH, &var_smtp_sender_auth,
	VAR_LMTP_CNAME_OVERR, DEF_LMTP_CNAME_OVERR, &var_smtp_cname_overr,
	VAR_LMTP_SASL_AUTH_SOFT_BOUNCE, DEF_LMTP_SASL_AUTH_SOFT_BOUNCE, &var_smtp_sasl_auth_soft_bounce,
	VAR_LMTP_ASSUME_FINAL, DEF_LMTP_ASSUME_FINAL, &var_lmtp_assume_final,
	VAR_LMTP_REC_DEADLINE, DEF_LMTP_REC_DEADLINE, &var_smtp_rec_deadline,
	VAR_LMTP_DUMMY_MAIL_AUTH, DEF_LMTP_DUMMY_MAIL_AUTH, &var_smtp_dummy_mail_auth,
	0,
    };
