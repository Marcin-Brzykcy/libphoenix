/*
 * Phoenix-RTOS
 *
 * posixsrv_private.h
 *
 * Copyright 2018 Phoenix Systems
 * Author: Jan Sikorski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef POSIXSRV_PRIVATE_H
#define POSIXSRV_PRIVATE_H

#include <sys/ioctl.h>
#include <termios.h>
#include <posix/utils.h>

#define PIPE_BUFSZ 0x1000


typedef struct request_t {
	struct request_t *next, *prev;

	unsigned int rid;
	msg_t msg;
} request_t;


struct _object_t;


typedef request_t *(handler_t)(struct _object_t *, request_t *);


typedef struct {
	union {
		struct {
			handler_t *open, *close, *read, *write, *truncate, *devctl,
				*create, *destroy, *setattr, *getattr,
				*lookup, *link, *unlink, *readdir;
			void (*release)(struct _object_t *);
		};
		handler_t *handlers[mtCount + 1];
	};
} operations_t;


typedef struct _object_t {
	idnode_t linkage;
	operations_t *operations;
	int refs, destroy;
} object_t;


static inline int rq_sz(request_t *r)
{
	return (r->msg.type == mtWrite) ? r->msg.i.size : r->msg.o.size;
}


static inline void *rq_buf(request_t *r)
{
	return (r->msg.type == mtWrite) ? r->msg.i.data : r->msg.o.data;
}


extern void rq_wakeup(request_t *r, int retval);


extern int rq_id(request_t *r);


extern unsigned srv_port(void);


extern int object_link(object_t *o, char *path);


static inline int object_id(object_t *o)
{
	return o->linkage.id;
}


extern void object_destroy(object_t *o);


extern object_t *object_get(int id);


extern void object_put(object_t *o);


extern int object_create(object_t *o, operations_t *ops);


extern int pipe_create(int type, int *id, unsigned open);


extern int pipe_init(void);


extern int pty_init(void);


#endif