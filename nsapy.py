
"""

  Gregory Trubetskoy <grisha@ispol.com>

  This module is the interface between the Netscape server
  and Python.

  Original idea - Aaron Watters.

  ==============================================================

  NETSCAPE SERVER SETUP

    In mime.types:

  # both of these are python scripts. the ones ending with a "d"
  # are for debugging, any errors in the script will be sent back
  # to the browser. Take the "pyd" one out on production systems!!!

  type=magnus-internal/X-python-e exts=pye,pyd

    In obj.conf:

  # load the shared library ( _nsapy.dll )
  #   use _nsapy30.dll for 3.0 servers
  #   and _nsapy20.dll for 2.0 servers

  Init funcs="nsapy_Init,nsapy_Service,nsapy_AuthTrans" fn="load-modules" \
      shlib="C:/python/lib/_nsapy30.dll"

  # ask the server to call the module intialization routine at startup:
  # this is saying "import module 'module', then run 'initstring'"
  Init fn="nsapy_Init" initstring="nsapy.init()" module="nsapy"

  # Optional arguments:
  #
  # a. logname argument to nsapy_init() e.g.: 
  #  Init fn="nsapy_Init" initstring="nsapy.init(logname='/var/log/nsapy')" module="nsapy"
  # Use forward slashes, even on Windows! Every log entry is prepended 
  # with a thread id in parenthesis, which is very useful for debugging.
  # (Note: on Solaris, it seemed like Netscape creates the file as root, then
  # switches to nobody and can't write to the file anymore. It is better to
  # use an existing file with correct permissions)
  #
  # b. criticalonly to nsapy_Init() e.g.:
  #  Init fn="nsapy_Init" initstring="nsapy.init()" module="nsapy" criticalonly="xxx"
  # "xxx" can be anything, the actual value is ignored. This makes Nsapy
  # enter a critical section for every call into the Python interpreter.
  # See (4) below for some more details.

  # ask the server to call our function to process PYthon files
  # put this inside <Object name=default> ( or some other object )
  Service fn="nsapy_Service" method="(GET|HEAD|POST)" \
      type="magnus-internal/X-python-e"

  # ( optional ) Ask the server to call Python for custom authentication
  # This goes inside <Object name=default> or some other object.
  # "userdb" is a Python module name ( without .py ) that will be asked
  # to do authentication.
  AuthTrans fn=basic-auth auth-type="basic" userdb=authtest userfn=nsapy_AuthTrans
  PathCheck fn=require-auth realm="hey python" auth-type="basic"

  ==============================================================

  HOW THIS MODULE WORKS

  1. At server startup time, it is imported by httpd ( via
  a shared library that loads Python ), and then the init()
  function is called. The reference to a Python object
  returned by init() is stored in httpd and is used to service
  requests. This mechanism is probably of little interest to
  the developer, aside from the fact that the developer can
  design his own way of interfacing with the server by providing
  his own object as the return of the init() function.

  In fact, the function doesn't even have to be called init(), 
  and the module "nsapy" - you can create your own if you wish,
  though it is unlikely you'd ever want to.

  This line in obj.conf dictates the name of the module and the
  initialization function:

    Init fn="nsapy_Init" initstring="nsapy.init()" module="nsapy"

  You may want to use custom initialization functionality by either
  specifying a different function name or a different module.

  For example, something like this in myinit.py:

    def MyInit():
	import nsapy
	... do some initialization at server startup ...
	return nsapy.init()

  then the line in obj.conf would look like this:
    Init fn="nsapy_Init" initstring="myinit.MyInit()" module="myinit"

  I don't know why you'd want this though - the standard Python import
  mechanism is much better for things that only need to happen once.

  2. When an HTTP request comes in for a magnus-internal/X-python-e,
  the Service function of the callback object is called. ( the 
  "callcack" object is the object init() returned in step 1 above )

  The Service function will:

      get the module name from the URI, import that module, 
      instantiate the RequestHandler object and call its
      Handle() method passing it parameter block, session and
      request objects:

      For example, http://localhost/home/myscript.pye 
      will result in the equivalent of:

	>>> import myscript
	>>> hr = myscript.RequestHandler(pb, sn, rq)
	>>> hr.Handle()
      
      Handle() in turn calls the following methods in the
      following sequence:
	  Content()
	  Header()
	  Status()
	  Send()
      
      You can override any one of these to provide custom headers,
      alter the status, or sending ot the text.

      At the very least, you'll have to override Content().

  Here is a minimal module:

     import nsapy

     class RequestHandler( nsapy.RequestHandler ):
	 def Content( self ):
	     return "<HTML><H1>Hello World!</H1></HTML>"
      
  Here is a more elborate one:

      import nsapy

      class RequestHAndler( nsapy.RequestHandler ):
	  def Content( self ):
	      self.redirect = "http://www.python.org"
	      return "<HTML>Your browser doesn't understand redirects!'</HTML>"

  Here is how to get form data ( doesn't matter POST or GET ):

       --snip--
	method = self.rq.reqpb.findval('method')

	try:
	    if method == 'POST':
		fdlen = atoi(self.rq.request_header("content-length", self.sn))
		fd = cgi.parse_qs(self.sn.form_data(fdlen))
	    else:
		fd = cgi.parse_qs(self.rq.reqpb.findval('query'))
       --snip-- 

  Raise SERVER_RETURN with a pair (return_code, status) at any point.  
  If status is not None it will serve as the protocol_status, the return_code 
  will be used as the return code returned to the server-interface:

      # Can't find the file!
          raise SERVER_RETURN, (REQ_ABORTED, PROTOCOL_NOT_FOUND)

  or to simply give up (eg, if the response already started):
      raise SERVER_RETURN, (REQ_ABORTED, None)


  3. When the server wants to do authentication, ( see nsapimod.c )
  the AuthTrans function of the callback object is called.

  The AuthTrans function will:

      get the module name from the "userdb" parameter, import that module, 
      instantiate the AuthHandler object and call its
      Handle() method passing it parameter block, session and
      request objects:
      
      Handle() can return any of these:
	  REQ_NOACTION  - ask password again
	  REQ_ABORTED   - Server Error
	  REQ_PROCEED   - OK
      

  Here is a minimal module that lets grisha/mypassword in:

     import nsapy

     class AuthHandler( nsapy.AuthHandler ):
	 def Handle( self ):
	     if self.pb.findval( 'user' ) == 'grisha' and \
		self.pb.findval( 'pw' ) == 'mypassword':
		 return nsapy.REQ_PROCEED
	     else:
		 return nsapy.REQ_NOACTION

  4. Nsapy provides an interface to NSAPI critical-section processing.
  Look at http://developer.netscape.com/support/faqs/champions/nsapi.html#q16
  for more information.

  Critical-section processing is a way to deal with things that are not thread-
  safe. This is more reliable than using locks from thread.py.

  You can do something like this (mycrit is called a critical-section 
  variable):

         mycrit = nsapy.crit_init()

  then you can wrap thread unsafe areas with:

        crit_enter( mycrit )
           ....unsafe code....
        crit_exit( mycrit )

  You can have multiple critical-section variables.

  You can also pass a criticalonly parameter from the obj.conf file. In this case 
  *all* Python activity is considered critical-section. A global critical-section
  variable nsapy.CRITICAL is created at server start up. This variable can be
  used to manually exit the critical section in Python code that will take a long
  time to execute, e.g.:

        if nsapy.CRITICAL:
            nsapy.crit_exit( nsapy.CRITICAL )
        time.sleep( 2600 )  

  That's basically it...

"""

import sys
import string
import traceback
import time

SERVER_RETURN = "SERVER_RETURN"

REQ_PROCEED = "REQ_PROCEED"   
REQ_ABORTED = "REQ_ABORTED"   
REQ_NOACTION = "REQ_NOACTION" 
REQ_EXIT = "REQ_EXIT"         

# Response status codes for use with rq.protocol_status(sn, *)
PROTOCOL_OK = "PROTOCOL_OK" 
PROTOCOL_REDIRECT = "PROTOCOL_REDIRECT" 
PROTOCOL_NOT_MODIFIED = "PROTOCOL_NOT_MODIFIED" 
PROTOCOL_BAD_REQUEST = "PROTOCOL_BAD_REQUEST" 
PROTOCOL_UNAUTHORIZED = "PROTOCOL_UNAUTHORIZED" 
PROTOCOL_FORBIDDEN = "PROTOCOL_FORBIDDEN" 
PROTOCOL_NOT_FOUND = "PROTOCOL_NOT_FOUND" 
PROTOCOL_SERVER_ERROR = "PROTOCOL_SERVER_ERROR" 
PROTOCOL_NOT_IMPLEMENTED = "PROTOCOL_NOT_IMPLEMENTED" 


class nsCallBack:
    """
    A generic NSAPI callback object.
    """

    def __init__( self ):
	""" 
	Constructor.
	if self.debug is not 0, the Python error
	output will be sent to the browser - very
	useful for debugging.
	"""

	# don't change this here
	self.debug = 0


    def Service(self, pb, sn, rq):
	""" 
	This method is envoked by nsapi module for
	each request. The return value is one of the
	REQ constants above. REQ_PROCEED means OK.
	"""

	(self.pb, self.sn, self.rq) = (pb, sn, rq)

	# be pessimistic
	result = REQ_ABORTED

	try:
	    handler = self.get_request_handler()
	    result = handler.Handle()

	except SERVER_RETURN, value:
	    # SERVER_RETURN indicates a non-local abort from below
	    # with value as (result, status) or (result, None)
	    try:
		(result, status) = value
		if status:
		    rq.protocol_status(sn, status)
	    except:
		pass

	except KeyboardInterrupt:
	    # This is a special case, meaning 
	    # protocol_start_response() C function returned
	    # REQ_NOACTION, so we play along, it's OK.
	    result = REQ_PROCEED

	except:
	    # Any other rerror
	    if self.debug :
		result = self.ReportError(sys.exc_type, sys.exc_value, sys.exc_traceback)
	    else:
		result = REQ_ABORTED

	# lest we waste memory, always clear traceback
	sys.last_traceback = None

	return result

    def AuthTrans(self, pb, sn, rq):
	""" 
	This method is envoked by nsapi module for
	each *AuthTrans* request.

	self.pb.findval( "user" ) is login
	self.pb.findval( "pw" ) is password

	REQ_PROCEED means OK
	REQ_NOACTION means Ask Again
	REQ_ABORTED means Server Error

	"userdb" is the module name to do the authorization

	"""

	(self.pb, self.sn, self.rq) = (pb, sn, rq)

	# be pessimistic
	result = REQ_ABORTED

	# get URI
	try:
	    module_name = self.pb.findval( "userdb" )
	except:
	    raise ValueError, "Failed to locate auth module name"

	# debugging?
	if module_name[-5:] == 'DEBUG':
	    self.debug = 1
	    module_name = module_name[:-5]
	else:
	    self.debug = 0

	# try to import the module
	try:
	    module = __import__(module_name)
	    # if module extension ends with DEBUG reload it
	    if self.debug :
		module = reload( module )
	    # get the Handler class
	    handler = module.AuthHandler( self.pb, self.sn, self.rq )
	    result = handler.Handle()
	except:
	    pass

	# lest we waste memory, always clear traceback
	sys.last_traceback = None

	return result

    def Log(self, str):

        log( str )


    def get_request_handler( self ):
	""" 
	Get the module and the object to handle the request. 
	This function is called by Service(). The module is 
	extracted from the URI.

	Depending on the last letter of the extension
	self.debug is set:
	   .pyd   -   debugging ON
	   .pye   -   debugging OFF
	   
	When envoked with .pyd, the module is *reloaded*
	for every request, otherwise, it follows normal
	Python behaviour  - "import" loads a module only once.
	"""

	# get URI
	try:
	    uri = self.rq.reqpb.findval( "uri" )
	except:
	    raise ValueError, "Failed to locate URI in rq.reqpb"

	# debugging?
	if uri[-1:] == 'd' :
	    self.debug = 1
	else:
	    self.debug = 0

	# find the module name by getting the string between the
	# last slash and the last dot.

	slash = string.rfind( uri, "/" )
	dot = string.rfind( uri, "." )
	module_name = uri[ slash + 1 : dot ]

	# try to import the module

	try:
	    module = __import__(module_name)
	    # if module extension ends with a d reload it
	    if self.debug :
		module = reload( module )
	    # get the Handler class
	    Class = module.RequestHandler

	except (ImportError, AttributeError, SyntaxError):
	    if self.debug :
		# pass it on
		raise sys.exc_type, sys.exc_value
	    else:
		# show and HTTP error
		raise SERVER_RETURN, (REQ_ABORTED, PROTOCOL_FORBIDDEN)

	# construct and return an instance of the handler class
	result = Class( self.pb, self.sn, self.rq )

	return result

    def ReportError(self, etype, evalue, etb):
	""" 
	This function is only used when debugging is on.
	It sends the output similar to what you'd see
	when using Python interactively to the browser
	"""
	(sn, rq) = (self.sn, self.rq)

	srvhdrs = self.rq.srvhdrs

	# replace magnus-internal/X-python-e with text/html
	srvhdrs.pblock_remove("content-type")
	srvhdrs.nvinsert("content-type", "text/html")

	rq.protocol_status(sn, PROTOCOL_OK)
	rq.start_response(sn)

	text = "<HTML><H1>A Python Error Happened:</H1><PRE>"
	for e in traceback.format_exception( etype, evalue, etb ):
	    text = text + e + '\n'
	text = text + "</PRE></HTML>"

	sn.net_write( text )

	return "REQ_PROCEED"

def log( s, kind='info:' ):
    """
    This adds a timestamp and writes to the log file
    """

    if logfile:
        t = time.strftime( '[%d/%b/%Y:%H:%M:%S]', time.localtime( time.time() ) )
        f = open( logfile, 'a' )
        f.write( '%s %s %s\n' % ( t, kind, str( s ) ) )
        f.close()

def init( logname=None ):
    """ 
        This function is called by the server at startup time

        If you want logging, give a full path to the
        logfile.
    """

    global logfile
    logfile = logname

    # create a callback object
    obCallBack = nsCallBack( )

    import nsapi

    # "give it back" to nsapi
    nsapi.SetCallBack( obCallBack )

    # provide an interface to crit_* functions
    global crit_init, crit_enter, crit_exit, CRITICAL
    crit_init, crit_enter, crit_exit, CRITICAL = \
      nsapi.crit_init, nsapi.crit_enter, nsapi.crit_exit, nsapi.CRITICAL

class RequestHandler:
    """
    A superclass that may be used to create RequestHandlers
    in other modules, for use with this module.
    """

    def __init__( self, pb, sn, rq ):
	( self.pb, self.sn, self.rq ) = ( pb, sn, rq )
	# default content-type
	self.content_type = 'text/html'
	# no redirect
	self.redirect = ''
	
    def Send( self, content ):

	self.rq.start_response( self.sn )
	self.sn.net_write( str( content ) )

    def Header( self ):
	""" 
	This prepares the headers
	"""
	srvhdrs = self.rq.srvhdrs
	
	# content-type
	srvhdrs.pblock_remove("content-type")
	srvhdrs.nvinsert("content-type", self.content_type)

	# for redirects, add Location header
	if self.redirect:
	    srvhdrs.nvinsert("Location", self.redirect)

	# add a silly header, for fun.
	srvhdrs.nvinsert("x-grok-this", "Python-psychobabble")


    def Status( self ):
	""" 
	The status is set here.
	"""
	if self.redirect:
	    self.rq.protocol_status( self.sn, PROTOCOL_REDIRECT )
	else:
	    self.rq.protocol_status( self.sn, PROTOCOL_OK )


    def Handle( self ):
	"""
	This method handles the request. Although, you may be 
	tempted to override this method, you should concider 
	overriding Content() first, it may be all you need.
	"""
	try:
	    content = self.Content()
	    self.Header()
	    self.Status()
	    self.Send( content )
	except:
	    # debugging ?
	    uri = self.rq.reqpb.findval("uri")
	    if uri[-1:] == 'd':
		raise sys.exc_type, sys.exc_value
	    return REQ_ABORTED
	
	return REQ_PROCEED

    def Content( self ):
       """
       For testing and reference
       """
       return "Welcome to Nsapy!"



class AuthHandler( RequestHandler ):

    def Handle( self ):

	return REQ_PROCEED
