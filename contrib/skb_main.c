/*
 * Copyright (c) 2007, 2008, 2009, 2011, ETH Zurich.
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached LICENSE file.
 * If you do not find this file, copies can be found by writing to:
 * ETH Zurich D-INFK, Haldeneggsteig 4, CH-8092 Zurich. Attn: Systems Group.
 */

/*This file has been taken from barrelfish project and was used for debugging reasons. For skb per se it is NOT required.*/

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <eclipse.h>
#include <skb_debug.h>

#include <readline/readline.h>

#define ECLIPSE_DIR "/usr/local/eclipse"

#define RESULT_BUF_SIZE 1024

int skb_init(void);
void execute_string(char *string);

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *rl_gets()
{
	/* If the buffer has already been allocated, return the memory
	   to the free pool. */
	if (line_read) {
		free(line_read);
		line_read = (char *)NULL;
	}

	/* Get a line from the user. */
	line_read = readline("");

	/* If the line has any text in it, save it on the history. */
	if (line_read && *line_read)
		add_history(line_read);

	return (line_read);
}

#if 0

/*
int main(int argc, char **argv)
{

	char *str;
	char input;

	chdir(ECLIPSE_DIR);

	// now set the right values for the eclipse-clp engine
	ec_set_option_int(EC_OPTION_IO, MEMORY_IO);
	ec_set_option_ptr(EC_OPTION_ECLIPSEDIR, ECLIPSE_DIR);
	ec_set_option_long(EC_OPTION_GLOBALSIZE, MEMORY_SIZE);
	ec_set_option_int(EC_OPTION_IO, MEMORY_IO);

	// ec_.m.vm_flags |= 8;
	SKB_DEBUG("before ec init\n");

	int n = ec_init();
	if (n != 0) {
		SKB_DEBUG("\nskb_main: ec_init() failed. Return code = %d\n",
			  n);
	} else {
		SKB_DEBUG("\nskb_main: ec_init() succeeded.\n");
	}
	execute_string("set_flag(print_depth,100).");

//XXX vb not clear why if is neeeded here at all
	//if(disp_get_core_id() == 0) {
	//debug_printf("oct_server_init\n");
	//execute_string("set_flag(gc, off).");
	//execute_string("set_flag(gc_policy, fixed).");
	//execute_string("set_flat(gc_interval, 536870912)."); // 512 mb
	//execute_string("set_flag(gc_interval_dict, 10000).");
	//execute_string("set_flag(enable_interrupts, off).");
	//execute_string("set_flag(debug_compile, off).");
	//execute_string("set_flag(debugging, nodebug).");
	//bench_init();

	// octopus related stuff
	execute_string("compile_term(capital(london,england)).");
	execute_string("compile_term(capital(paris,france)).");
//        execute_string("end_of_file.");
//        dident e = ec_did("eclipse", 0);
	//ec_external(ec_did("notify_client", 2), p_notify_client, e);

//XXX vb I included icparc solver with regex functions, yet not sure it's needed
//        ec_external(ec_did("match", 3), (int (*)()) ec_regmatch, e);
//        ec_external(ec_did("split", 4), (int (*)()) ec_regsplit, e);
	// end

//    }

	// SKB Hardware related
	//    execute_string("[queries].");
	// execute_string("get_local_affinity(1,B,L),write(output,[B,L]).");
	// execute_string("lib(branch_and_bound).");
	// execute_string("minimize(member(X,[4,1,2]),X),write(output,X).");

	while (1) {
		puts("f for facts mode, q for query.\n");
		scanf(" %c", &input);

		if (input == 'f') {
			//facts 
			printf("enter stop to end facts sequence\n");
			while (1) {
				str = rl_gets();
				if (!strcmp(str, "stop"))
					break;
				printf("string len %d\n", strlen(str));
				skb_add_fact(str);
			}

		} else if (input == 'q') {
			//querie
			printf("enter empty string to end queries sequence\n");
			while (1) {
				str = rl_gets();
				if (!strcmp(str, "stop"))
					break;
				execute_query(str, NULL);
			}

		} else {
			printf("wrong input: %c\n", input);
			continue;
		}
		//execute_query(str, NULL);

		//execute_string_v(str, NULL);

	}
}

*/
#endif


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

void execute_string(char *string)
{
	char buf[RESULT_BUF_SIZE];
	int n;

	ec_post_string(string);
	int res = 7;		//means that we have to flush the output.
	while (res == 7) {
		res = ec_resume();
		SKB_DEBUG("\nres = %d\n", res);

		//give back the result and the error messages.
		//in case there is no message, a '.' is still returned
		n = ec_queue_read(1, buf, RESULT_BUF_SIZE);
		if ((n >= 0) && (n < RESULT_BUF_SIZE)) {
			buf[n] = 0;
		}
		SKB_DEBUG("eclipse returned: %s with length %d.\n", buf, n);

		n = ec_queue_read(2, buf, RESULT_BUF_SIZE);
		if ((n >= 0) && (n < RESULT_BUF_SIZE)) {
			buf[n] = 0;
		}
		SKB_DEBUG("eclipse error returned: %s with length %d.\n", buf,
			  n);
	}
}
