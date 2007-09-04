/**
 * @file rtems/monitor.h
 *
 * The RTEMS monitor task.
 */
 
/*
 *  $Id$
 */

#ifndef __MONITOR_H
#define __MONITOR_H

#include <rtems/error.h>		/* rtems_error() */

#ifdef __cplusplus
extern "C" {
#endif

/* Forward decls from symbols.h */
typedef struct _rtems_symbol_t rtems_symbol_t;
typedef struct _rtems_symbol_table_t rtems_symbol_table_t;

/*
 * Monitor types are derived from rtems object classes
 */

typedef enum {
    RTEMS_MONITOR_OBJECT_INVALID   =  OBJECTS_CLASSIC_NO_CLASS,
    RTEMS_MONITOR_OBJECT_TASK      =  OBJECTS_RTEMS_TASKS,
    RTEMS_MONITOR_OBJECT_EXTENSION =  OBJECTS_RTEMS_EXTENSIONS,
    RTEMS_MONITOR_OBJECT_QUEUE     =  OBJECTS_RTEMS_MESSAGE_QUEUES,
    RTEMS_MONITOR_OBJECT_SEMAPHORE =  OBJECTS_RTEMS_SEMAPHORES,
    RTEMS_MONITOR_OBJECT_PARTITION =  OBJECTS_RTEMS_PARTITIONS,
    RTEMS_MONITOR_OBJECT_REGION    =  OBJECTS_RTEMS_REGIONS,
    RTEMS_MONITOR_OBJECT_PORT      =  OBJECTS_RTEMS_PORTS,

    /* following monitor objects are not known to RTEMS, but
     * we like to have "types" for them anyway */

    RTEMS_MONITOR_OBJECT_DRIVER    =  OBJECTS_RTEMS_CLASSES_LAST+1,
    RTEMS_MONITOR_OBJECT_DNAME,
    RTEMS_MONITOR_OBJECT_CONFIG,
    RTEMS_MONITOR_OBJECT_INIT_TASK,
    RTEMS_MONITOR_OBJECT_MPCI,
    RTEMS_MONITOR_OBJECT_SYMBOL
} rtems_monitor_object_type_t;

/*
 * rtems_monitor_init() flags
 */

#define RTEMS_MONITOR_SUSPEND	0x0001		/* suspend monitor on startup */
#define RTEMS_MONITOR_GLOBAL    0x0002          /* monitor should be global */
#define RTEMS_MONITOR_NOTASK    0x0004 /* do not start monitor task */
#define RTEMS_MONITOR_NOSYMLOAD 0x0008 /* do not load symbols       */
#define RTEMS_MONITOR_WAITQUIT  0x0010 /* wait for monitor task to terminate */

/*
 * Public interfaces for RTEMS data structures monitor is aware of.
 * These are only used by the monitor.
 *
 * NOTE:
 *  All the canonical objects that correspond to RTEMS managed "objects"
 *  must have an identical first portion with 'id' and 'name' fields.
 *
 *  Others do not have that restriction, even tho we would like them to.
 *  This is because some of the canonical structures are almost too big
 *  for shared memory driver (eg: mpci) and we are nickel and diming it.
 */

/*
 * Type of a pointer that may be a symbol
 */

#define MONITOR_SYMBOL_LEN 20
typedef struct {
    char       name[MONITOR_SYMBOL_LEN];
    uint32_t   value;
    uint32_t   offset;
} rtems_monitor_symbol_t;

typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
} rtems_monitor_generic_t;

/*
 * Task
 */
typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
    Thread_Entry        entry;
    uint32_t            argument;
    void               *stack;
    uint32_t            stack_size;
    rtems_task_priority priority;
    States_Control      state;
    rtems_event_set     events;
    rtems_mode          modes;
    rtems_attribute     attributes;
    uint32_t            notepad[RTEMS_NUMBER_NOTEPADS];
    rtems_id            wait_id;
    uint32_t            wait_args;
} rtems_monitor_task_t;

/*
 * Init task
 */

typedef struct {
    rtems_id            id;		/* not really an id */
    rtems_name          name;
  /* end of common portion */
    rtems_monitor_symbol_t entry;
    uint32_t               argument;
    uint32_t               stack_size;
    rtems_task_priority    priority;
    rtems_mode             modes;
    rtems_attribute        attributes;
} rtems_monitor_init_task_t;


/*
 * Message queue
 */
typedef struct {
    rtems_id            id;
    rtems_name          name;
  /* end of common portion */
    rtems_attribute     attributes;
    uint32_t            number_of_pending_messages;
    uint32_t            maximum_pending_messages;
    uint32_t            maximum_message_size;
} rtems_monitor_queue_t;

/*
 * Extension
 */
typedef struct {
    rtems_id                 id;
    rtems_name               name;
  /* end of common portion */
    rtems_monitor_symbol_t  e_create;
    rtems_monitor_symbol_t  e_start;
    rtems_monitor_symbol_t  e_restart;
    rtems_monitor_symbol_t  e_delete;
    rtems_monitor_symbol_t  e_tswitch;
    rtems_monitor_symbol_t  e_begin;
    rtems_monitor_symbol_t  e_exitted;
    rtems_monitor_symbol_t  e_fatal;
} rtems_monitor_extension_t;

/*
 * Device driver
 */

typedef struct {
    rtems_id            id;		   /* not really an id (should be tho) */
    rtems_name          name;              /* ditto */
  /* end of common portion */
    rtems_monitor_symbol_t initialization; /* initialization procedure */
    rtems_monitor_symbol_t open;           /* open request procedure */
    rtems_monitor_symbol_t close;          /* close request procedure */
    rtems_monitor_symbol_t read;           /* read request procedure */
    rtems_monitor_symbol_t write;          /* write request procedure */
    rtems_monitor_symbol_t control;        /* special functions procedure */
} rtems_monitor_driver_t;

/*
 * System config
 */

typedef struct {
    void               *work_space_start;
    uint32_t            work_space_size;
    uint32_t            maximum_tasks;
    uint32_t            maximum_timers;
    uint32_t            maximum_semaphores;
    uint32_t            maximum_message_queues;
    uint32_t            maximum_partitions;
    uint32_t            maximum_regions;
    uint32_t            maximum_ports;
    uint32_t            maximum_periods;
    uint32_t            maximum_extensions;
    uint32_t            microseconds_per_tick;
    uint32_t            ticks_per_timeslice;
    uint32_t            number_of_initialization_tasks;
} rtems_monitor_config_t;

/*
 * MPCI config
 */

#if defined(RTEMS_MULTIPROCESSING)
typedef struct {
    uint32_t    node;                   /* local node number */
    uint32_t    maximum_nodes;          /* maximum # nodes in system */
    uint32_t    maximum_global_objects; /* maximum # global objects */
    uint32_t    maximum_proxies;        /* maximum # proxies */

    uint32_t                 default_timeout;        /* in ticks */
    uint32_t                 maximum_packet_size;
    rtems_monitor_symbol_t   initialization;
    rtems_monitor_symbol_t   get_packet;
    rtems_monitor_symbol_t   return_packet;
    rtems_monitor_symbol_t   send_packet;
    rtems_monitor_symbol_t   receive_packet;
} rtems_monitor_mpci_t;
#endif

/*
 * The generic canonical information union
 */

typedef union {
    rtems_monitor_generic_t    generic;
    rtems_monitor_task_t       task;
    rtems_monitor_queue_t      queue;
    rtems_monitor_extension_t  extension;
    rtems_monitor_driver_t     driver;
    rtems_monitor_config_t     config;
#if defined(RTEMS_MULTIPROCESSING)
    rtems_monitor_mpci_t       mpci;
#endif
    rtems_monitor_init_task_t  itask;
} rtems_monitor_union_t;

/*
 * Support for talking to other monitors
 */

/*
 * Names of other monitors
 */

#define RTEMS_MONITOR_NAME        (rtems_build_name('R', 'M', 'O', 'N'))
#define RTEMS_MONITOR_SERVER_NAME (rtems_build_name('R', 'M', 'S', 'V'))
#define RTEMS_MONITOR_QUEUE_NAME  (rtems_build_name('R', 'M', 'S', 'Q'))
#define RTEMS_MONITOR_RESPONSE_QUEUE_NAME (rtems_build_name('R', 'M', 'R', 'Q'))

#define RTEMS_MONITOR_SERVER_RESPONSE    0x0001
#define RTEMS_MONITOR_SERVER_CANONICAL   0x0002

typedef struct
{
    uint32_t    command;
    rtems_id    return_id;
    uint32_t    argument0;
    uint32_t    argument1;
    uint32_t    argument2;
    uint32_t    argument3;
    uint32_t    argument4;
    uint32_t    argument5;
} rtems_monitor_server_request_t;

typedef struct
{
    uint32_t    command;
    uint32_t    result0;
    uint32_t    result1;
    rtems_monitor_union_t payload;
} rtems_monitor_server_response_t;

extern rtems_id  rtems_monitor_task_id;

extern uint32_t   rtems_monitor_node;	       /* our node number */
extern uint32_t   rtems_monitor_default_node;  /* current default for commands */

/*
 * Monitor command function and table entry
 */

typedef struct rtems_monitor_command_entry_s rtems_monitor_command_entry_t;
typedef union _rtems_monitor_command_arg_t   rtems_monitor_command_arg_t;

typedef void ( *rtems_monitor_command_function_t )(
                 int         argc,
                 char      **argv,
                 rtems_monitor_command_arg_t *command_arg,
                 boolean     verbose
             );

union _rtems_monitor_command_arg_t {
  rtems_monitor_object_type_t 	monitor_object;
  rtems_status_code		status_code;
  rtems_symbol_table_t 		**symbol_table;
  rtems_monitor_command_entry_t *monitor_command_entry;
};

struct rtems_monitor_command_entry_s {
    char        *command;      /* command name */
    char        *usage;        /* usage string for the command */
    uint32_t     arguments_required;    /* # of required args */
    rtems_monitor_command_function_t command_function;
                               /* Some argument for the command */
    rtems_monitor_command_arg_t   command_arg;
    rtems_monitor_command_entry_t *next;
};


typedef void *(*rtems_monitor_object_next_fn)(void *, void *, rtems_id *);
typedef void (*rtems_monitor_object_canonical_fn)(void *, void *);
typedef void (*rtems_monitor_object_dump_header_fn)(boolean);
typedef void (*rtems_monitor_object_dump_fn)(void *, boolean);

typedef struct {
    rtems_monitor_object_type_t         type;
    void                               *object_information;
    int                                 size;	/* of canonical object */
    rtems_monitor_object_next_fn        next;
    rtems_monitor_object_canonical_fn   canonical;
    rtems_monitor_object_dump_header_fn dump_header;
    rtems_monitor_object_dump_fn        dump;
} rtems_monitor_object_info_t;


/* monitor.c */
void    rtems_monitor_kill(void);
void    rtems_monitor_init(uint32_t  );
void    rtems_monitor_wakeup(void);
void    rtems_monitor_pause_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);
void    rtems_monitor_fatal_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);
void    rtems_monitor_continue_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);
void    rtems_monitor_debugger_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);
void    rtems_monitor_node_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);
void    rtems_monitor_symbols_loadup(void);
int     rtems_monitor_insert_cmd(rtems_monitor_command_entry_t *);
int     rtems_monitor_erase_cmd(rtems_monitor_command_entry_t *);

void    rtems_monitor_task(rtems_task_argument);

/* server.c */
void    rtems_monitor_server_kill(void);
rtems_status_code rtems_monitor_server_request(uint32_t  , rtems_monitor_server_request_t *, rtems_monitor_server_response_t *);
void    rtems_monitor_server_task(rtems_task_argument);
void    rtems_monitor_server_init(uint32_t  );

/* command.c */
int     rtems_monitor_make_argv(char *, int *, char **);
int     rtems_monitor_command_read(char *, int *, char **);
rtems_monitor_command_entry_t *rtems_monitor_command_lookup(
    rtems_monitor_command_entry_t * table, int argc, char **argv);
void    rtems_monitor_command_usage(rtems_monitor_command_entry_t *, char *);
void    rtems_monitor_help_cmd(int, char **, rtems_monitor_command_arg_t *, boolean);

/* prmisc.c */
void       rtems_monitor_separator(void);
uint32_t   rtems_monitor_pad(uint32_t   dest_col, uint32_t   curr_col);
int        rtems_monitor_dump_char(char   ch);
int        rtems_monitor_dump_decimal(uint32_t   num);
int        rtems_monitor_dump_hex(uint32_t   num);
int        rtems_monitor_dump_id(rtems_id id);
int        rtems_monitor_dump_name(rtems_name name);
int        rtems_monitor_dump_priority(rtems_task_priority priority);
int        rtems_monitor_dump_state(States_Control state);
int        rtems_monitor_dump_modes(rtems_mode modes);
int        rtems_monitor_dump_attributes(rtems_attribute attributes);
int        rtems_monitor_dump_events(rtems_event_set events);
int        rtems_monitor_dump_notepad(uint32_t   *notepad);

/* object.c */
rtems_id   rtems_monitor_id_fixup(rtems_id, uint32_t  , rtems_monitor_object_type_t);
rtems_id   rtems_monitor_object_canonical_get(rtems_monitor_object_type_t, rtems_id, void *, size_t *size_p);
rtems_id   rtems_monitor_object_canonical_next(rtems_monitor_object_info_t *, rtems_id, void *);
void      *rtems_monitor_object_next(void *, void *, rtems_id, rtems_id *);
rtems_id   rtems_monitor_object_canonical(rtems_id, void *);
void       rtems_monitor_object_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);

/* manager.c */
void      *rtems_monitor_manager_next(void *, void *, rtems_id *);

/* config.c */
void       rtems_monitor_config_canonical(rtems_monitor_config_t *, void *);
void      *rtems_monitor_config_next(void *, rtems_monitor_config_t *, rtems_id *);
void       rtems_monitor_config_dump_header(boolean);
int        rtems_monitor_config_dump(rtems_monitor_config_t *, boolean verbose);

/* mpci.c */
#if defined(RTEMS_MULTIPROCESSING)
void       rtems_monitor_mpci_canonical(rtems_monitor_mpci_t *, void *);
void      *rtems_monitor_mpci_next(void *, rtems_monitor_mpci_t *, rtems_id *);
void       rtems_monitor_mpci_dump_header(boolean);
void       rtems_monitor_mpci_dump(rtems_monitor_mpci_t *, boolean verbose);
#endif

/* itask.c */
void       rtems_monitor_init_task_canonical(rtems_monitor_init_task_t *, void *);
void      *rtems_monitor_init_task_next(void *, rtems_monitor_init_task_t *, rtems_id *);
void       rtems_monitor_init_task_dump_header(boolean);
void       rtems_monitor_init_task_dump(rtems_monitor_init_task_t *, boolean verbose);

/* extension.c */
void       rtems_monitor_extension_canonical(rtems_monitor_extension_t *, void *);
void       rtems_monitor_extension_dump_header(boolean verbose);
void       rtems_monitor_extension_dump(rtems_monitor_extension_t *, boolean);

/* task.c */
void    rtems_monitor_task_canonical(rtems_monitor_task_t *, void *);
void    rtems_monitor_task_dump_header(boolean verbose);
void    rtems_monitor_task_dump(rtems_monitor_task_t *, boolean);

/* queue.c */
void    rtems_monitor_queue_canonical(rtems_monitor_queue_t *, void *);
void    rtems_monitor_queue_dump_header(boolean verbose);
void    rtems_monitor_queue_dump(rtems_monitor_queue_t *, boolean);

/* driver.c */
void    *rtems_monitor_driver_next(void *, rtems_monitor_driver_t *, rtems_id *);
void     rtems_monitor_driver_canonical(rtems_monitor_driver_t *, void *);
void     rtems_monitor_driver_dump_header(boolean);
void     rtems_monitor_driver_dump(rtems_monitor_driver_t *, boolean);

/* symbols.c */
rtems_symbol_table_t *rtems_symbol_table_create();
void                  rtems_symbol_table_destroy(rtems_symbol_table_t *table);

rtems_symbol_t *rtems_symbol_create(rtems_symbol_table_t *, char *, uint32_t  );
rtems_symbol_t *rtems_symbol_value_lookup(rtems_symbol_table_t *, uint32_t  );
const rtems_symbol_t *rtems_symbol_value_lookup_exact(rtems_symbol_table_t *, uint32_t  );
rtems_symbol_t *rtems_symbol_name_lookup(rtems_symbol_table_t *, char *);
void   *rtems_monitor_symbol_next(void *object_info, rtems_monitor_symbol_t *, rtems_id *);
void    rtems_monitor_symbol_canonical(rtems_monitor_symbol_t *, rtems_symbol_t *);
void    rtems_monitor_symbol_canonical_by_name(rtems_monitor_symbol_t *, char *);
void    rtems_monitor_symbol_canonical_by_value(rtems_monitor_symbol_t *, void *);
uint32_t   rtems_monitor_symbol_dump(rtems_monitor_symbol_t *, boolean);
void    rtems_monitor_symbol_cmd(int, char **, rtems_monitor_command_arg_t*, boolean);


/* mon-object.c */
rtems_monitor_object_info_t *rtems_monitor_object_lookup(
  rtems_monitor_object_type_t type
);

/* shared data */
extern rtems_symbol_table_t *rtems_monitor_symbols;

/* FIXME: This should not be here */
extern rtems_monitor_command_entry_t rtems_monitor_commands[];

#define MONITOR_WAKEUP_EVENT   RTEMS_EVENT_0

#ifdef __cplusplus
}
#endif

#endif  /* ! __MONITOR_H */
