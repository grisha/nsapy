
/**********************************************************
* NSAPy module.

Copyright 1997 Gregory Trubetskoy

Original concept and first code by Aaron Watters from
"Internet Programming with Python" by Aaron Watters, 
Guido Van Rossum and James C. Ahlstrom, ISBN 1-55851-484-8

Ported to Microsoft Windows NT and then later heavily
modified ( mostly cosmetic changes though ) by 
Gregory Trubetskoy <grisha@ispol.com>.

For a more detailed description of Netscape configuration,
see comments in nsapy.py. Below is only a brief outline.

  Netscape server configuration:

  In mime.types:

type=magnus-internal/X-python-e exts=pye,pyd

  In obj.conf:

# load the shared library ( _nsapy.dll )
#   use _nsapy30.dll for 3.0 servers
#   and _nsapy20.dll for 2.0 servers

Init funcs="nsapy_Init,nsapy_Service,nsapy_AuthTrans" fn="load-modules" \
    shlib="C:/python/lib/_nsapy30.dll"
Init fn="nsapy_Init" initstring="nsapy.init()" module="nsapy"

# ask the server to call our function to process PYthon files
# put this inside <Object name=default> ( or some other object )
Service fn="nsapy_Service" method="(GET|HEAD|POST)" \
    type="magnus-internal/X-python-e"

# ( optional ) Ask the server to call Python for custom authentication
# This goes inside <Object name=default> or some other object.
# "userdb" is a Python module name ( without .py ) that will be asked
# to do authentication.
#AuthTrans fn=basic-auth auth-type="basic" userdb=authtest userfn=nsapy_AuthTrans
#PathCheck fn=require-auth realm="hey python" auth-type="basic"

  How it works:
 
In brief, Netscape server will acquire a reference to a Python
object also known as CallBack object at server startup time
because the above "nsapy_Init" directive tells it to.

It will then call nsapy_Service fucntion in this DLL for every 
request that fits the MIME type. nsapy_Service function will in 
turn call "Service" method of the Python CallBack object.

The CallBack object can be any Python object, only requirement is
it must have the following methods

   1.  Service( pblockobject, sessionobject, requestobject )

        this method is called for every call to the C function
        nsapy_Service( pblockptr, sessionptr, requestptr )

        return value must be a Python string, one of
        "REQ_NOACTION", "REQ_PROCEED", "REQ_ABORTED", "REQ_EXIT"
        

   2.   Log( string )

        this method may be called a lot to log what happens if
        NSAPYDEBUG is defined

        return value doesn't matter

   3.   AuthTrans( pblockobject, sessionobject, requestobject )

        this method is called just like Service, only for 
        Authentication requests.

To find out what the above functions do in the provided CallBack,
look at its Python source in nsapy.py

*
***********************************************************/

/* Uncomment this for debugging */
#define NSAPYDEBUG 1

/* NSAPI headers */
#include "base/pblock.h"
#include "base/session.h"
#include "base/crit.h"
#include "base/systhr.h"
#include "frame/req.h"
#include "frame/protocol.h"
#include "frame/log.h"
#include "frame/http.h"

/* Python Headers */
#include "Python.h"
#include "pythonrun.h"

/* some forward declarations */
NSAPI_PUBLIC void initnsapi();


PyObject *NsapiModule = NULL;

/* The CallBack object */
static PyObject *obCallBack = NULL;

/* pointer to a global CRITICAL object for CriticalOnly processing */
static PyObject * obCrit = NULL;

/* 
 * These are Python equivalents of NSAPI
 * pblock, Session, Request, CRITICAL
 *
 */

typedef struct pblockobject {
    PyObject_VAR_HEAD
    pblock *pb;
} pblockobject;

typedef struct sessionobject {
    PyObject_VAR_HEAD
    Session *sn;
} sessionobject;

typedef struct requestobject {
    PyObject_VAR_HEAD
    Request *rq;
} requestobject;

typedef struct criticalobject {
    PyObject_VAR_HEAD
    CRITICAL crit;
} criticalobject;

/* type objects corresponding to the above object types */

static PyTypeObject pblockobjecttype;
static PyTypeObject requestobjecttype;
static PyTypeObject sessionobjecttype;
static PyTypeObject criticalobjecttype;

/* methods of pblocks */

static PyObject * Py_pblock2str( pblockobject *pbo, PyObject *args );
static PyObject * Py_nvinsert( pblockobject *pbo, PyObject *args );
static PyObject * Py_findval( pblockobject *pbo, PyObject *args );
static PyObject * Py_pblock_remove( pblockobject *pbo, PyObject *args );

static PyMethodDef Pypblockmethods[] = {
	{ "pblock2str",		(PyCFunction) Py_pblock2str,     1},
	{ "nvinsert",       (PyCFunction) Py_nvinsert,       1},
	{ "findval",        (PyCFunction) Py_findval,        1},
	{ "pblock_remove",  (PyCFunction) Py_pblock_remove,  1},
	{ NULL, NULL } /* sentinel */
};

/* methods for session */

static PyObject * Py_session_dns( sessionobject *sno, PyObject *args );
static PyObject * Py_net_write( sessionobject *sno, PyObject *args );
static PyObject * Py_sn_client( sessionobject *sno, PyObject *args );
static PyObject * Py_form_data(sessionobject *sno, PyObject *args );

static PyMethodDef Pysessionmethods[] = {
	{ "session_dns",     (PyCFunction) Py_session_dns,     1},
	{ "net_write",       (PyCFunction) Py_net_write,       1},
	{ "client",          (PyCFunction) Py_sn_client,       1},
	/*{ "net_read",        (PyCFunction)Py_net_read,        1},*/
	{ "form_data",            (PyCFunction) Py_form_data,     1}, 
	{ NULL, NULL } /* sentinel */
};

/* methods for request */

static PyObject * Py_request_header( requestobject *rqo, PyObject *args );
static PyObject * Py_log_warn( requestobject *rqo, PyObject *args );
static PyObject * Py_start_response( requestobject *rqo, PyObject *args );
static PyObject * Py_protocol_status( requestobject *rqo, PyObject *args );

static PyMethodDef Pyrequestmethods[] = {
	{ "request_header",		(PyCFunction) Py_request_header,     1},
	{ "log_error",          (PyCFunction) Py_log_warn,           1},
	{ "start_response",     (PyCFunction) Py_start_response,     1},
	{ "protocol_status",    (PyCFunction) Py_protocol_status,    1},
	{ NULL, NULL } /* sentinel */
};

/* nsapi MODULE methods */

static PyObject * SetCallBack( PyObject *self, PyObject *args );
static PyObject * Py_crit_enter( PyObject *self, PyObject *args );
static PyObject * Py_crit_exit( PyObject *self, PyObject *args );
static PyObject * Py_crit_init( PyObject *self, PyObject *args );

static struct PyMethodDef nsapi_module_methods[] = {
	{"SetCallBack",     (PyCFunction) SetCallBack,		1},
	{"crit_enter",		(PyCFunction) Py_crit_enter,	1},
	{"crit_exit",		(PyCFunction) Py_crit_exit,		1},
	{"crit_init",       (PyCFunction) Py_crit_init,     1},
	{NULL, NULL} /* sentinel */
};


/** 
 ** nsapy_log_error - calls NSAPI log_error. It prepends the thread id to
 ** the function name to make it easier to reolve those multithreaded problems
 **/
static int nsapy_log_error(int degree, char *func, Session *sn, Request *rq, char *fmt )
{
	char buff[1000];

	/* prepend a pointer for the current thread */
	sprintf( buff, "(thread %d) %s", ( int ) systhread_current(), func );

    /* call the Netscape log_error function, assume it succeeds */
    return log_error(degree, buff, sn, rq, fmt);

}

/**
 ** Log - This is optional logging, mostly for debug,
 ** normally disabled.
 **/

#ifdef NSAPYDEBUG

    static int Log( char * string )
    {

		/* This function never returns NULL. If there is no
		   callback object, it calls log_error */

		char buff[1000];
        PyObject *dummy;

        /* fail if no callback object is registered */
        if ( obCallBack ) 
		{

			/* prepend a pointer for the current thread */
			sprintf( buff, "(thread %d) %s", ( int ) systhread_current(), string );

			/* attempt to call obCallBack.Log( string ) */
			dummy = PyObject_CallMethod( obCallBack, "Log", "s", buff );

			if ( !dummy ) 
				nsapy_log_error(LOG_WARN, "couldn't Log() (obCallBack error)", NULL, NULL, string);
			else
		        /* discard result */
				Py_XDECREF( dummy );
		}

        return 1;
    }

#else /* #ifdef NSAPYDEBUG */

    static int Log( char * string ) { return 1; }

#endif /* #ifdef NSAPYDEBUG */


/** 
 ** InitAbort - instruct the server to abort initialization
 **/

static int InitAbort( pblock *pb, char *diagnostic )
{

    /* insert pair "error": diagnostic in parameter block */
    pblock_nvinsert("error", diagnostic, pb);

    /* return abort error code */
    return REQ_ABORTED;

}


/**
 ** nsapy_Init - The initialization function.
 **
 * 
 *  Initialize Python and get the callback object.
 *
 *  This function is called by the NS Server at initialization
 *  in order to initialize Python and obtain obCallBack.
 *
 *  The function expects two parameters in the pblock
 *
 *        "module"      -   the module name to be imported
 *        "initstring"  -   a string to be executed after that
 *
 *  "initstring" must return a Python object with all the methods
 *  descripbed above defined ( Service, Log, etc. )
 *
 *  This object is a.k.a. obCallBack.
 *
 *  *sn and *rq parameters are ignored.
 *
 */

NSAPI_PUBLIC int nsapy_Init(pblock *pb, Session *sn, Request *rq)
{

    char buff[1000];
    char *module, *initstring, *criticalonly;
	PyObject *d;

    /* get the parameters from the parameter block */

    module = pblock_findval("module", pb);
    initstring = pblock_findval("initstring", pb);
    criticalonly = pblock_findval("criticalonly", pb);

    if ( !module ) 
        return InitAbort( pb, "nsapy_Init: No module defined in pb" );

    if ( !initstring ) 
        return InitAbort(pb, "nsapy_Init: No initstring defined in pb");

    /* initialize Python */

    Py_Initialize();

    /*  Initialize nsapi 
        This makes an nsapi module available for import, but remember,
        YOU should use "import nsapy" not "nsapi". "nsapi" is for internal
        use only. ( The only time it's needed is to assign obCallBack )
    */

    initnsapi();

    /* If a criticalonly parameter was defined,
	   create a python object CRITICAL that can be used
	   to exit the critical section by passing it to 
	   crit_exit. 
	*/

	d = PyModule_GetDict( NsapiModule );
	if ( criticalonly )
	{
		obCrit =  Py_crit_init( NsapiModule, Py_None  );
		if ( !obCrit )
			return InitAbort( pb, "nsapy_Init: could not create CRITICAL variable" );
		Py_INCREF( obCrit );
		PyDict_SetItemString( d, "CRITICAL", obCrit );
	}
	else
	{
		Py_INCREF( Py_None );
		PyDict_SetItemString( d, "CRITICAL", Py_None );
		Py_INCREF( Py_None );
		obCrit = Py_None;
	}

    /* Now execute the equivalent of
        >>> import nsapi
        >>> import sys
        >>> import <module>
        >>> <initstring>
        in the __main__ module to start up Python.
    */

    PyRun_SimpleString( "import nsapi\n" );

    if ( PyErr_Occurred() )
        return InitAbort( pb, "nsapy_Init: could not import nsapi" );

    PyRun_SimpleString( "import sys\n" );

    if ( PyErr_Occurred() )
        return InitAbort( pb, "nsapy_Init: could not import sys" );

    sprintf( buff, "import %s\n", module);
    PyRun_SimpleString( buff );

    if ( PyErr_Occurred() ) 
    {
        sprintf( buff, "nsapy_Init: could not import %s", module );
        return InitAbort( pb, buff );
    }

    sprintf( buff, "%s\n", initstring );
    PyRun_SimpleString( buff );

    if ( PyErr_Occurred() )
    {
        sprintf( buff, "nsapy_Init: could not call %s", initstring );
        return InitAbort( pb, buff );
    }

    /* the "initstring" should execute something like
        >>> import nsapi
        >>> nsapi.SetCallBack( someobject )
        at this point obCallBack is a reference to someobject
    */

    if ( ! obCallBack ) 
    {
        sprintf( buff, "nsapy_Init: after %s no callback object found", initstring );
        return InitAbort( pb, buff );
    }


    /* Wow, this worked! */
    return REQ_PROCEED;
}

/**
 ** SetCallBack - assign a CallBack object
 **
 *  This function must be called from Python upon execution
 *  the initstring parameter, like this
 *
 *  >>> import nsapi
 *  >>> nsapi.SetCallBack( instance )
 *
 *  providing the instance as the CallBack object.
 */

static PyObject * SetCallBack( PyObject *self, PyObject *args )
{

    PyObject *callback;

    /* returning NULL means error, returning Py_None is good */

    /* see if CallBack is in *args */

    if ( ! PyArg_ParseTuple( args, "O", &callback ) ) 
        return NULL;

    /* dispose of the old call back object, if there was one */
  
    Py_XDECREF( obCallBack );
 
    /* store the object, incref */

    obCallBack = callback;  
    Py_INCREF( obCallBack );

    /* informative log message (callback) */

    Log( "SetCallBack: obCallBack Initialized" );

    /* return None */

    Py_INCREF(Py_None);
    return Py_None;
}

/* typetest macro */
#define is_criticalobject(o) ((o)->ob_type = &criticalobjecttype)


/**
 ** crit_enter - enter a critical section
 **
 *  
 *  Netscape will halt all other processing
 *  until crit_exit for this critical section
 *  variable.
 *
 */

static PyObject * Py_crit_enter( PyObject *self, PyObject *args )
{

	criticalobject * crit;
	char buff[1000];

    if ( ! PyArg_ParseTuple( args, "O", &crit ) ) 
        return NULL;


	if ( ! is_criticalobject( crit ) )
    {
        PyErr_SetString( PyExc_TypeError, 
            "argument to crit_enter must be a CRITICAL object");
        return NULL;
    }

	crit_enter( crit->crit );

	sprintf( buff, "crit_enter: entered critical section %d", ( int ) crit->crit );
    Log( buff );

    /* return None */

    Py_INCREF(Py_None);
    return Py_None;
}

/**
 ** crit_exit - exit a critical section
 **
 */

static PyObject * Py_crit_exit( PyObject *self, PyObject *args )
{

	criticalobject * crit;
	char buff[1000];

    if ( ! PyArg_ParseTuple( args, "O", &crit ) ) 
        return NULL;


	if ( ! is_criticalobject( crit ) )
    {
        PyErr_SetString( PyExc_TypeError, 
            "argument to crit_exit must be a CRITICAL object");
        return NULL;
    }

	/* we must log this *BEFORE* we exit critical-section */
	sprintf( buff, "crit_exit: exiting critical section %d", ( int ) crit->crit );
    Log( buff );

	crit_exit( crit->crit );

    /* return None */

    Py_INCREF(Py_None);
    return Py_None;
}


/**
 ** Py_crit_init
 **
 *  This routine creates a Python version of NSAPI CRITICAL object 
 *
 */
static PyObject * Py_crit_init( PyObject *self, PyObject *args )
{

	char buff[256];

    criticalobject * result;
	CRITICAL crit;

    result = PyMem_NEW( criticalobject, 1 );

    if ( ! result )
        return PyErr_NoMemory();

    result->ob_type = &criticalobjecttype;

	crit = crit_init();
    result ->crit = crit;

    _Py_NewReference( result );

	sprintf( buff, "Py_crit_init: critical-section variable %d allocated", ( int ) crit ); 
    Log( buff );

    return ( PyObject * ) result;
}

/*
 * critical_dealloc
 */

static void critical_dealloc( criticalobject *co )
{
	crit_terminate( co->crit );
    free( co );
}

/**
 ** make_pblockobject
 **
 *  This routine creates a Python pblockobject given an NSAPI
 *  pblock pointer.
 *
 */

static pblockobject * make_pblockobject( pblock * from_pb )
{
    pblockobject *result;

    result = PyMem_NEW( pblockobject, 1 );
    if ( ! result )
        return ( pblockobject * ) PyErr_NoMemory();

    result->pb = from_pb;
    result->ob_type = &pblockobjecttype;

    _Py_NewReference( result );
    return result;
}


/** 
 ** Object Methods for pblock
 **
 *  NOTE: FOR NOW ASSUME WE DON'T NEED THE pb_param STRUCTURE
 *      OR THE FUNCTIONS
 *  param_create, param_free, pblock_create, pblock_free,
 *  pblock_find (use pblock_findval instead), 
 *  pblock_pinsert (use pblock_nvinsert instead)
 *
 */

static void pblock_dealloc( pblockobject *op )
{  
    /* Note that this frees the Python object, but not
       the refrenece to a pblock that it holds. Httpd
       should take care of that.
    */
    free( op );
}

/*
 * pb.findval( string ) 
 */

static PyObject * Py_findval( pblockobject *pbo, PyObject *args )
{
    char *string, *value;

    if (! PyArg_ParseTuple( args, "s", &string ) )
        return NULL; /* error */

    /* find the value as a string */
    value = pblock_findval( string, pbo->pb );

    if ( ! value )
    {
        PyErr_SetString( PyExc_ValueError, "no such parameter" );
        return NULL;
    }

    return PyString_FromString( value );
}

/*
 * pb.nvinsert( namestring, valuestring )
 */

static PyObject * Py_nvinsert( pblockobject *pbo, PyObject *args )
{

  char *name, *value;

  if (! PyArg_ParseTuple(args, "ss", &name, &value) )
        return NULL; /* error */

  /* set the name/value, there doesn't seem to be any way to
     check for success or failure. */
  pblock_nvinsert( name, value, pbo->pb );

  Py_INCREF(Py_None);
  return Py_None;
}

/*
 * pb.pblock2str() 
 */

static PyObject * Py_pblock2str( pblockobject *pbo, PyObject *args )
{
    char *temp;
    PyObject *result;

    if ( ! PyArg_ParseTuple( args, "" ) )
        return NULL; /* error */

    /* get the result, have the server allocate space */
    temp = pblock_pblock2str( pbo->pb, NULL );

    /* convert it to a PythonString */
    result = PyString_FromString( temp ); 

    return result;
}

/* 
 * pb.pblock_remove( name )
 *
 */
static PyObject * Py_pblock_remove( pblockobject *pbo, PyObject *args )
{
  char *name;
  pb_param *temp;

  if (! PyArg_ParseTuple(args, "s", &name) ) {
    return NULL;  /* bad args */
  }

  /* imitated from nsapi/examples/session.c */
  temp = pblock_remove( name, pbo->pb );

  if ( temp )
        param_free( temp );

  Py_INCREF( Py_None );
  return Py_None;
}

/* standard getattr for pblocks */

static PyObject * pblock_getattr( PyObject *pbo, char *name )
{
    return Py_FindMethod( Pypblockmethods, pbo, name );
}


/* typetest macro */
#define is_sessionobject(op) ((op)->ob_type = &sessionobjecttype)

/**
 ** make_session_object
 **
 *  Given a NSAPI Session, create a Python sessionobject
 *
 */

static sessionobject * make_sessionobject( Session *from_sn )
{
    sessionobject *result;

    result = PyMem_NEW( sessionobject, 1 );
    if (! result )
        return ( sessionobject * ) PyErr_NoMemory();

    result->sn = from_sn;
    result->ob_type = &sessionobjecttype;
    _Py_NewReference( result );

    return result;
}

/** 
 ** Object Methods for Session
 **
 */

static void session_dealloc( sessionobject *op )
{
    /* Again, notice we let httpd do its freeing */
    free( op );
}

/*
 * sn.session_dns() 
 */
static PyObject * Py_session_dns( sessionobject *sno, PyObject *args )
{
    char *dns;

    if (! PyArg_ParseTuple(args, "") )
        return NULL; /* error */

    dns = session_dns( sno->sn );

    if ( dns )
        return PyString_FromString( dns );
    else 
    {
        Py_INCREF( Py_None );
        return Py_None;
    }
}

/*
 * sn.net_write(string)
 */

static PyObject * Py_net_write( sessionobject *sno, PyObject *args )
{
    int len;
    char *string;

    if (! PyArg_ParseTuple(args, "s#", &string, &len) )
        return NULL;  /* bad args */

    if ( net_write(sno->sn->csd, string, len) == IO_ERROR )
    {
        PyErr_SetString( PyExc_IOError, "net_write failed" );
        return NULL;
    }

    Py_INCREF( Py_None );
    return Py_None;
}


/* This is taken from an example that Netscape provides */

/*
   This function reads in data from the specified netbuf, discarding
   the header and then converting the rest into a string in query
   string format, up to len characters. qstr must already have been
   allocated. It returns the number of characters actually read.
   If the buffer is emptied early, a NULL is returned indicating
   the error. 
*/

int post2qstr(netbuf *buf, char* qstr, int clen)
{
    int i;                   /* index into qstr */
    int ichar;               /* char read in from netbuf */

    i = 0;
    ichar = !IO_EOF;

    /*
       Loop through reading a character and writing it to qstr, until
       either len characters have been read, there's no more input,
       or there's an IO error.
    */

     while (clen && ichar != IO_EOF)
     {
          ichar = netbuf_getc(buf);
          /* check for error in reading */
          if(ichar == IO_ERROR)
          {
               return 0;
          }
          qstr[i++] = ichar;
          clen--;
     }
     qstr[i] = '\0';

     return(i);
}


/* 
 * sn.form_data(int)
 */

static PyObject * Py_form_data(sessionobject *sno, PyObject *args )
{
    int clen;
    PyObject *result;
    char *qstr;              /* actual query string to work on */
    int postlen;             /* number of chars read from net_buf */

    result = NULL;
    postlen = 0;

    /* get length */
    if (! PyArg_ParseTuple(args, "i", &clen) )
        return NULL;

    if ( clen <= 0 )
    {
        PyErr_SetString( PyExc_ValueError, 
            "sn.form_data must have positive integer parameter" );
        return NULL;
    }

    /* allocate space to put the query string after we read it in */

    result = PyString_FromStringAndSize( (char *) NULL, clen );
    if ( result == NULL )
        return NULL;

    qstr = PyString_AS_STRING( (PyStringObject *) result);

    /* call the function above */
    postlen = post2qstr( sno->sn->inbuf, qstr, clen );

    if ( postlen != clen )
        if ( _PyString_Resize( &result, postlen ) < 0 )
            return NULL;

    return result;
}

/* 
 * sn.net_read(int)
 */
/*
   This function is commented and omitted, because The Netscape
   documentation didn't make it clear how to read additional information
   from the session without incurring the possibility of blocking
   and without violating the Server assumptions about the behavior
   of the connection (whatever they may be).  If you want to be trusting
   you can modify the function given below to use netbuf_getc(...) to
   get one character at a time, but if you geuss the number of characters
   in the buffer incorrectly the function may block, so we omit it.

static PyObject *
Py_net_read( sno, args )
     sessionobject *sno;
     PyObject *args;
{
  int test, len, realsize;
  PyObject *result;
  char *charbuffer;
  result = NULL;
  realsize = 0;
  if (!Log("sn.net_read(...)\n")) {
    return NULL;
  }
  test = PyArg_ParseTuple(args, "i", &len);
  if (!test) {
    return NULL;
  }
  if (len<=0) {
    PyErr_SetString(PyExc_ValueError, 
            "sn.net_read must have positive integer parameter");
    return NULL;
  }
//  printf("s.nr: getting string\n");
  result = PyString_FromStringAndSize((char *)NULL, len);
  if (result == NULL) {
    return NULL;
  }
//  printf("s.nr: getting internal string %d\n", len);
  charbuffer = PyString_AS_STRING( (PyStringObject *) result);
//  printf("s.nr: reading\n");
//  *** this doesn't work on our system (bombs) ***
  realsize = net_read(sno->sn->csd, charbuffer, len, 1);
//  printf("s.nr: done reading %d\n", realsize);
  if (realsize != len) {
//    printf("s.nr: resizing string\n");
    test = _PyString_Resize(&result, realsize);
    if (test<0) {
      return NULL;
    }
  }
//  printf("s.nr: exiting normally\n");
  return result;
}
*/

/* 
 *  sn.client()
 */

static PyObject * Py_sn_client( sessionobject *sno, PyObject *args )
{

    if (! PyArg_ParseTuple(args, "") )
        return NULL; /* error */

    return (PyObject *) make_pblockobject( sno->sn->client );
}

/* 
 *standard getattr for sessions 
 */

static PyObject * session_getattr( PyObject *pbo, char *name )
{
    return Py_FindMethod( Pysessionmethods, pbo, name );
}


/**
 ** make_requestobject
 **
 *  Given a Request, make a Python requestobject
 */

static requestobject * make_requestobject( Request *from_rq )
{
    requestobject *result;

    result = PyMem_NEW( requestobject, 1 );

    if (! result )
        return ( requestobject * ) PyErr_NoMemory();

    result->rq = from_rq;
    result->ob_type = &requestobjecttype;

    _Py_NewReference( result );
    return result;
}

/*
 * request_dealloc
 */

static void request_dealloc( requestobject *op )
{
    free( op );
}

/* 
 * rq.request_header(namestring, sn)
 */

static PyObject * Py_request_header( requestobject *rqo, PyObject *args )
{

    char *namestring, *value;
    sessionobject *sno;

    if (! PyArg_ParseTuple(args, "sO", &namestring, &sno) )
        return NULL; /* error */

    /* check that sno is a sessionobject */
    if ( ! is_sessionobject( sno ) )
    {
        PyErr_SetString( PyExc_TypeError, 
            "second arg to request_header must be session object");
        return NULL;
    }

    /* call the underlying request_header function */
    if ( request_header( namestring, &value, sno->sn, rqo->rq ) 
            == REQ_ABORTED)
    {
        PyErr_SetString( PyExc_SystemError, "request_header signalled REQ_ABORTED" );
        return NULL;
    }

    /* if the value is NULL, return None */
    if ( ! value )
    {
        Py_INCREF( Py_None );
        return Py_None;
    }

    /* otherwise return the value string as a Python string */
    return PyString_FromString( value );
}

/*
 *  rq.start_response(sn)
 *
   Signal the beginning of a response to the Server.
   The sessionobject sn should be the session associated with
   this request!

   IF THIS FUNCTION RAISES KeyboardInterrupt THE ERROR MUST BE CAUGHT
   AT THE TOP LEVEL AND THE VALUE "REQ_PROCEED" RETURNED FROM
   PYTHON, WITHOUT ANY OTHER ACTIONS TAKEN.
*/
static PyObject * Py_start_response( requestobject *rqo, PyObject *args )
{

    sessionobject *sno;

    if (! PyArg_ParseTuple(args, "O", &sno) )
        return NULL; /* error */

    if ( !is_sessionobject(sno) )
    {
        PyErr_SetString( PyExc_TypeError, "arg of start_response must be session object");
        return NULL;
    }

    /* raise SystemExit if REQ_NOACTION was returned */
    if ( protocol_start_response(sno->sn, rqo->rq) == REQ_NOACTION )
    {
        PyErr_SetString( PyExc_KeyboardInterrupt,
            "protocol_start_response returned REQ_NOACTION");
        return NULL;
    }

    Py_INCREF( Py_None );
    return Py_None;
}

/* 
 *  rq.protocol_status(sn, string)
 *
    Signal the protocol status.  sn must be the session object for this
    session.  string should be one of the PROTOCOL_* responses listed
    in the source below.
 */

static PyObject * Py_protocol_status( requestobject *rqo, PyObject *args )
{

    sessionobject *sno;
    char *responsestr;
    int response;

    if (! PyArg_ParseTuple(args, "Os", &sno, &responsestr ))
        return NULL; /* error */

    if (! is_sessionobject(sno))
    {
        PyErr_SetString( PyExc_TypeError,
            "arg 2 of start_response must be session object");
        return NULL;
    }

    /* translate response string to a status code, default to PROTOCOL_FORBIDDEN. */
    response = PROTOCOL_FORBIDDEN;
    if ( strcmp( responsestr, "PROTOCOL_OK")==0)
        response = PROTOCOL_OK;
    else if ( strcmp( responsestr, "PROTOCOL_NO_RESPONSE")==0)
        response = PROTOCOL_NO_RESPONSE;
    else if ( strcmp( responsestr, "PROTOCOL_REDIRECT")==0)
        response = PROTOCOL_REDIRECT;
    else if ( strcmp( responsestr, "PROTOCOL_NOT_MODIFIED")==0)
        response = PROTOCOL_NOT_MODIFIED;
    else if ( strcmp( responsestr, "PROTOCOL_BAD_REQUEST")==0)
        response = PROTOCOL_BAD_REQUEST;
    else if ( strcmp(responsestr, "PROTOCOL_UNAUTHORIZED")==0)
        response = PROTOCOL_UNAUTHORIZED;
    else if ( strcmp( responsestr, "PROTOCOL_FORBIDDEN")==0)
        response = PROTOCOL_FORBIDDEN;
    else if ( strcmp( responsestr, "PROTOCOL_NOT_FOUND")==0)
        response = PROTOCOL_NOT_FOUND;
    else if ( strcmp( responsestr, "PROTOCOL_PROXY_UNAUTHORIZED")==0)
        response = PROTOCOL_PROXY_UNAUTHORIZED;
    else if ( strcmp( responsestr, "PROTOCOL_SERVER_ERROR")==0)
        response = PROTOCOL_SERVER_ERROR;
    else if ( strcmp( responsestr, "PROTOCOL_NOT_IMPLEMENTED")==0)
        response = PROTOCOL_NOT_IMPLEMENTED;

  /* call the underlying nsapi function */
  protocol_status( sno->sn, rqo->rq, response, NULL );

  Py_XINCREF( Py_None );
  return Py_None;
}

/*
 *  rq.nsapy_log_error(function_string, message_string, sn)
 *
   Log an error message to the server log file.
   This implementation always uses the severity LOG_WARN.

*/
static PyObject * Py_log_warn( requestobject *rqo, PyObject *args )
{
    char *message;
    char *function;
    sessionobject *sno;


    if (! PyArg_ParseTuple(args, "ssO", &function, &message, &sno)) 
        return NULL; /* error */

    if (!is_sessionobject(sno))
    {
        PyErr_SetString( PyExc_TypeError, "third arg of log_error must be session object");
        return NULL;
    }

    /* call the underlying log_error function, assume it succeeds */
    nsapy_log_error(LOG_WARN, function, sno->sn, rqo->rq, message);

    Py_INCREF(Py_None);
    return Py_None;
}
  


/* 
 * ALMOST standard getattr for sessions.
 *
   Allow methods defined above as well as

     rq.reqpb, rq.srvhdrs, rq.vars

   which all return pblockobjects based on the members of the
   request structure.

   NOTE: The os, statpath, finfo, loadhdrs members are ignored
         because they are not documented.
*/

static PyObject * request_getattr(requestobject *rqo, char *name)
{

    /*  If the name is "reqpb", "srvhdrs" or "vars" return
        the appropriate pblock in Python object form.
        Note that the headers member should only be read through
        the official request_header interface, as per documentation.
    */

    pblock *member;

    member = NULL;

    /* check for member names reqpb, srvhdrs, vars */
    if ( strcmp(name, "reqpb") == 0 )
        member = rqo->rq->reqpb;
    else if ( strcmp(name, "srvhdrs") == 0 )
        member = rqo->rq->srvhdrs;
    else if ( strcmp(name, "vars") == 0 )
        member = rqo->rq->vars;
    else if ( strcmp(name, "headers") == 0 )
        member = rqo->rq->headers;

    /* if member is set, return it translated as a pblockobject */
    if ( member )
        return (PyObject *) make_pblockobject(member);

    /* otherwise look for a standard method */
    return Py_FindMethod( Pyrequestmethods, (PyObject *) rqo, name );
}


/* nsapi MODULE INITIALIZATION FUNCTION */
NSAPI_PUBLIC void initnsapi()
{

    PyTypeObject pot = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "nsapi_pblock",
        sizeof(pblockobject),
        0,
        (destructor)pblock_dealloc,      /*tp_dealloc*/
        0,                               /*tp_print*/
        (getattrfunc)pblock_getattr,     /*tp_getattr*/
        0,                               /*tp_setattr*/
        0,                               /*tp_compare*/
        0,                               /*tp_repr*/
        0,                               /*tp_as_number*/
        0,                               /*tp_as_sequence*/
        0,                               /*tp_as_mapping*/
        0,                               /*tp_hash*/
    };

    PyTypeObject sot = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "nsapi_session",
        sizeof(sessionobject),
        0,
        (destructor)session_dealloc,     /*tp_dealloc*/
        0,                               /*tp_print*/
        (getattrfunc)session_getattr,    /*tp_getattr*/
        0,                               /*tp_setattr*/
        0,                               /*tp_compare*/
        0,                               /*tp_repr*/
        0,                               /*tp_as_number*/
        0,                               /*tp_as_sequence*/
        0,                               /*tp_as_mapping*/
        0,                               /*tp_hash*/
    };


    PyTypeObject rot = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "nsapi_request",
        sizeof(requestobject),
        0,
        (destructor)request_dealloc,     /*tp_dealloc*/
        0,                               /*tp_print*/
        (getattrfunc)request_getattr,    /*tp_getattr*/
        0,                               /*tp_setattr*/
        0,                               /*tp_compare*/
        0,                               /*tp_repr*/
        0,                               /*tp_as_number*/
        0,                               /*tp_as_sequence*/
        0,                               /*tp_as_mapping*/
        0,                               /*tp_hash*/
    };


    PyTypeObject cot = {
        PyObject_HEAD_INIT(&PyType_Type)
        0,
        "CRITICAL",
        sizeof(criticalobject),
        0,
        (destructor)critical_dealloc,    /*tp_dealloc*/
        0,                               /*tp_print*/
        0,								 /*tp_getattr*/
        0,                               /*tp_setattr*/
        0,                               /*tp_compare*/
        0,                               /*tp_repr*/
        0,                               /*tp_as_number*/
        0,                               /*tp_as_sequence*/
        0,                               /*tp_as_mapping*/
        0,                               /*tp_hash*/
    };

    pblockobjecttype = pot;
    sessionobjecttype = sot;
    requestobjecttype = rot;
    criticalobjecttype = cot;

    NsapiModule = Py_InitModule("nsapi", nsapi_module_methods);
}


/**
 ** nsapy_Service
 **
 *
 *  This is the workhorse of this module :)
 *
 *  The primary purpose of this function is to invoke the Python
 *   function
 *      >>> resultobject = obCallBack.nsapy_Service(pb, sn, rq)
 *  and interpret the PyString resultobject as one of the standard NSAPI
 *  application return codes.  An error, or an unrecognized return
 *  code will return REQ_ABORTED.
 */

NSAPI_PUBLIC int nsapy_Service(pblock *pb, Session *sn, Request *rq)
{
	char buff[1000];
    pblockobject *pbo;
    sessionobject *sno;
    requestobject *rqo;
    PyObject *resultobject;
    char *resultstring;
    int result;

    /* pessimistic */
    result = REQ_ABORTED;

	if ( obCrit != Py_None )
	{
		crit_enter( ( ( criticalobject * ) obCrit )->crit);
		sprintf( buff, "nsapy_Service: entered critical section %d", ( int ) ( ( criticalobject * ) obCrit )->crit );
		Log( buff );
	}
    
	/* initialize pointers to NULL */
    pbo = NULL;
    sno = NULL;
    rqo = NULL;
    resultobject = NULL;

    /* we must have a callback object to succeed! */
    if ( !obCallBack ) 
        nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "no callback object registered");
    else
    {
        pbo = make_pblockobject( pb );
        if (!pbo) 
            nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "couldn't make pno");
        else
        {
            sno = make_sessionobject(sn);
            if (!sno)
            {
                nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "couldn't make pno");
                Log("nsapy_Service: make_sessionobject failed");
            }
            else
            {
                rqo = make_requestobject(rq);
                if (!rqo)
                {
                    nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "couldn't make rqo");
                    Log("nsapy_Service: make_requestobject failed");

                }
                else
                {

                    /* 
                     INVOKE THE PYTHON initstringBACK:
                     deliver the inputs to the underlying callback object.

                     This is the C equivalent of
                       >>> resultobject = obCallBack.Service(pbo, sno, rqo)
                    */
                    resultobject = PyObject_CallMethod( obCallBack, "Service", "OOO",
                            (PyObject *)pbo, (PyObject *)sno, (PyObject *)rqo);

                    if (!resultobject) 
                    {
                        nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "couldn't call down to Service");
                        Log("nsapy_Service: Service() failed");

                    }
                    else
                    {
                        /* Attempt to analyse the result as a string indicating which
                         result to return */
                        if (!PyString_Check(resultobject)) 
                        {
                            nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "Service returned non-string");
                            Log("nsapy_Service: Service() result not string");
                        }
                        else
                        {
                            resultstring = PyString_AS_STRING( (PyStringObject *) resultobject);
                            if (strcmp(resultstring, "REQ_NOACTION")==0) 
                            {
                                Log("nsapy_Service: Service() returns REQ_NOACTION");
                                result = REQ_NOACTION;
                            }
                            else if (strcmp(resultstring, "REQ_PROCEED")==0) 
                            {
                                Log("nsapy_Service: Service() returns REQ_PROCEED");
                                result = REQ_PROCEED;
                            }
                            else if (strcmp(resultstring, "REQ_EXIT")==0) {
                                Log("nsapy_Service: Service() returns REQ_EXIT");
                                result = REQ_EXIT;
                            }
                            else
							{
							    sprintf( buff, "nsapy_Service: Service() returns %s, defaults to REQ_ABORTED", resultstring );
								Log( buff );
                                result = REQ_ABORTED;
							}
                        }
                    }
                }
            }
        }
    }
  if (result == REQ_ABORTED) 
  {
        nsapy_log_error(LOG_WARN, "nsapy_Service", sn, rq, "REQ_ABORTED");
		Log("nsapy_Service: Request Aborted (REQ_ABORTED)");
  }

  /* dispose of object wrappers and method result */
 
  Py_XDECREF(pbo);
  Py_XDECREF(sno);
  Py_XDECREF(rqo);
  Py_XDECREF(resultobject);

  if ( obCrit != Py_None )
  {
	sprintf( buff, "nsapy_Service: exiting critical section %d", ( int ) ( ( criticalobject * ) obCrit )->crit );
	Log( buff );
	crit_exit( ( ( criticalobject * ) obCrit )->crit );
  }

  /* return the translated result (or default result) to the Server. */
  return result;
}


/**
 ** nsapy_AuthTrans
 **
 *  Authorization works the same way as nsapy_Service, only calls
 *  AuthTrans function of the callback object
 *
 */

NSAPI_PUBLIC int nsapy_AuthTrans(pblock *pb, Session *sn, Request *rq)
{
	char buff[1000];
    pblockobject *pbo;
    sessionobject *sno;
    requestobject *rqo;
    PyObject *resultobject;
    char *resultstring;
    int result;

    /* pessimistic */
    result = REQ_ABORTED;


if ( obCrit != Py_None )
{
	crit_enter( ( ( criticalobject * ) obCrit )->crit );
	sprintf( buff, "nsapy_AuthTrans: entered critical section %d", ( int ) ( ( criticalobject * ) obCrit )->crit );
	Log( buff );
}

    /* initialize pointers to NULL */
    pbo = NULL;
    sno = NULL;
    rqo = NULL;
    resultobject = NULL;

    if ( !obCallBack ) 
        nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "no callback object registered");
    else
    {
        pbo = make_pblockobject( pb );
        if (!pbo) 
            nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "couldn't make pno");
        else
        {
            sno = make_sessionobject(sn);
            if (!sno)
            {
                nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "couldn't make pno");
                Log("nsapy_AuthTrans: make_sessionobject failed");
            }
            else
            {
                rqo = make_requestobject(rq);
                if (!rqo)
                {
                    nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "couldn't make rqo");
                    Log("nsapy_AuthTrans: make_requestobject failed");

                }
                else
                {

                    /* 
                     deliver the inputs to the underlying callback object.

                     This is the C equivalent of
                       >>> resultobject = obCallBack.AuthTrans(pbo, sno, rqo)
                    */
                    resultobject = PyObject_CallMethod( obCallBack, "AuthTrans", "OOO",
                            (PyObject *)pbo, (PyObject *)sno, (PyObject *)rqo);

                    if (!resultobject) 
                    {
                        nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "couldn't call down to AuthTrans");
                        Log("nsapy_AuthTrans: AuthTrans() failed");

                    }
                    else
                    {
                        /* Attempt to analyse the result as a string indicating which
                         result to return */
                        if (!PyString_Check(resultobject)) 
                        {
                            nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "AuthTrans() returned non-string");
                            Log("nsapy_AuthTrans: AuthTrans() result not string");
                        }
                        else
                        {
                            resultstring = PyString_AS_STRING( (PyStringObject *) resultobject);
                            if (strcmp(resultstring, "REQ_NOACTION")==0) 
                            {
                                Log("nsapy_AuthTrans: AuthTrans() returns REQ_NOACTION");
                                result = REQ_NOACTION;
                            }
                            else if (strcmp(resultstring, "REQ_PROCEED")==0) 
                            {
                                Log("nsapy_AuthTrans: AuthTrans() returns REQ_PROCEED");
                                result = REQ_PROCEED;
                            }
                            else if (strcmp(resultstring, "REQ_EXIT")==0) 
							{
                                Log("nsapy_AuthTrans: AuthTrans() returns REQ_EXIT");
                                result = REQ_EXIT;
                            }
                            else
							{
                                result = REQ_ABORTED;
							    sprintf( buff, "nsapy_AuthTrans: AuthTrans() returns %s, defaults to REQ_ABORTED", resultstring );
								Log( buff );
							}
                        }
                    }
                }
            }
        }
    }
  if (result == REQ_ABORTED) 
  {
        nsapy_log_error(LOG_WARN, "nsapy_AuthTrans", sn, rq, "REQ_ABORTED");
        Log("nsapy_AuthTrans: Request Aborted (REQ_ABORTED)");
  }

  /* dispose of object wrappers and method result */
 
  Py_XDECREF(pbo);
  Py_XDECREF(sno);
  Py_XDECREF(rqo);
  Py_XDECREF(resultobject);

if ( obCrit != Py_None )
{
	sprintf( buff, "nsapy_AuthTrans: exiting critical section %d", ( int ) ( ( criticalobject * ) obCrit )->crit );
	Log( buff );
	crit_exit( ( ( criticalobject * ) obCrit )->crit );
}

	/* return the translated result (or default result) to the Server. */
  return result;
}


/* stub required by Python, give the interpreter a name to call itself */
char * getprogramname()
{
    return "PythonNSAPI";
}

/* stub required by Python, give the Python Path (hard coded here) */
char * getpythonpath()
{
    return "/usr/local/lib/python";
}
