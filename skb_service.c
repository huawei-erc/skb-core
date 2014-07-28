/**
 * \file
 * \brief Server part of the SKB
 *
 * This file exports the SKB functions
 */

/*
 * Copyright (c) 2007, 2008, 2009, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

//turn on if you want to see everything which is executed on the SKB.
//this helps to create a (data) file which can be used to play on linux
//a sed script can filter these lines by checking the prefix

//#define PRINT_SKB_FILE

#define SKB_FILE_OUTPUT_PREFIX "SKB_FILE:"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//XXX vb
#include <stdarg.h>
#include <stdbool.h>

//#include <barrelfish/barrelfish.h>
//#include <barrelfish/nameservice_client.h>
#include <eclipse.h>
//#include <skb_server.h>
#include <skb_debug.h>
//#include <include/queue.h>

#include <skb.h>

int skb_init(void)
{
	int n;

	ec_set_option_int(EC_OPTION_IO, MEMORY_IO);
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, ECLIPSE_DIR);
	ec_set_option_long(EC_OPTION_GLOBALSIZE, MEMORY_SIZE);
	ec_set_option_int(EC_OPTION_IO, MEMORY_IO);

	SKB_DEBUG("\ninitialize eclipse\n");
	n = ec_init();

	if (n != 0) {
		SKB_DEBUG("\nskb_main: ec_init() failed.");
	}
	return (n);
}

/*Simple notation of the skb request. We assume all the Vars used in the request
to be in form Xn, n belongs to [0..N]
This way, we avoid putting explicit write() requests and parsing the output afterwards.
This helper function is not in use at the moment.
*/
errval_t execute_string_v(char *query, char *output)
{
	int i = 0, size;
	char *s, str[256];
	pword res;
	pword pw1, pw2, pw3;

	ec_ref Vars = ec_ref_create(ec_nil());

	if (ec_exec_string(query, Vars) == PSUCCEED) {
		printf("exec string OK!\n");
	} else {
		printf("exec string faiile!\n");
		return -1;
	}

	for (i = 0; i < 3; i++) {
		res = ec_ref_get(Vars);
		sprintf(str, "X%d", i);
		printf("looking for %s\n", str);
		if (ec_var_lookup(Vars, str, &res) == PSUCCEED) {
			ec_get_string(res, &s);
			printf("Result: %s = %s\n", str, s);
		}
	}
}

errval_t skb_add_fact_my(char *fmt, ...)
{
	static char buffer[BUFFER_SIZE];
	static char str[BUFFER_SIZE];
	static struct skb_query_state st;
	errval_t err;

	va_list va_l;
	va_start(va_l, fmt);
	vsnprintf(buffer, BUFFER_SIZE, fmt, va_l);
	buffer[BUFFER_SIZE - 1] = 0;
	int len = strlen(buffer);

	char *dot = strrchr(buffer, '.');
	if (dot != 0) {
		*dot = 0;
	}

	sprintf(str, "assert(%s).", buffer);
	SKB_DEBUG("skb_add_fact(): %s\n", str);

	err = execute_query(str, &st);

	va_end(va_l);
	return err;
}

errval_t execute_query(char *query, struct skb_query_state * st)
{
	int res;

	static struct skb_query_state sst;
	char str[256];
	char *s;

	if (st == NULL)
		st = &sst;	//XXX if we are not interested in output and/or error states 

	ec_ref Start = ec_ref_create_newvar();

	st->exec_res = PFLUSHIO;
	st->output_length = 0;
	st->error_output_length = 0;

	/* Processing */
	ec_post_string(query);

	while (st->exec_res == PFLUSHIO) {
		st->exec_res = ec_resume1(Start);

		res = 0;
		do {
			res =
			    ec_queue_read(1,
					  st->output_buffer + st->output_length,
					  BUFFER_SIZE - res);
			st->output_length += res;
		}
		while ((res != 0) && (st->output_length < BUFFER_SIZE));
		st->output_buffer[st->output_length] = 0;

		res = 0;
		do {
			res =
			    ec_queue_read(2,
					  st->error_buffer +
					  st->error_output_length,
					  BUFFER_SIZE - res);
			st->error_output_length += res;
		}
		while ((res != 0) && (st->error_output_length < BUFFER_SIZE));

		st->error_buffer[st->error_output_length] = 0;
	}

	if (st->exec_res == PSUCCEED) {
		ec_cut_to_chp(Start);
		ec_resume();
	}

	printf("skb output was: %s\n", st->output_buffer);
	printf("skb error  was: %s\n", st->error_buffer);
	printf("skb exec res: %d\n", st->exec_res);

	ec_ref_destroy(Start);

	return st->exec_res;
}

