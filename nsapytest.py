
# An example of NSAPy

# June 11 1997 Gregory Trubetskoy <grisha@ispol.com>

"""

Minimally, all you need is:

    import nsapy
    class RequestHandler( nsapy.RequestHandler ):
	def Content( self ):
	    return 'Hello World!'

"""

import nsapy
import string

class RequestHandler( nsapy.RequestHandler ):

    def Content( self ):

	# uncomment this to see redirection!
	#self.redirect = "http://www.python.org/"

	s = """<html><head><title>NSAPY!</title></head>\n
	       <body bgcolor="#ffffc0">
	       <h1>Some things you can see in nsapy:</h1>
	    """

	s = s + '<h2> Parameter Block </h2>\n'
	s = s + '<blockquote>'

	s = s + pblock_table( self.pb )

	s = s + '</blockquote>'
	s = s + '<h2> Session Data </h2>\n'
	s = s + '<blockquote>'

	s = s + "<h3>DNS</h3>\n"
	s = s + "%s <br>\n" % self.sn.session_dns()

	s = s + "<h3>Client Data</h3>\n"
	s = s + pblock_table( self.sn.client() )

	s = s + '</blockquote>'
	s = s + '<h2> Request Data </h2>\n'
	s = s + '<blockquote>'

	s = s + "<h3> Parameter Block </h3>\n"
	s = s + pblock_table( self.rq.reqpb )

	s = s + "<h3> Request Headers </h3>\n"
	# NOTE! Netscape documentation sais you're
	# not supposed to access "headers" directly, 
	# instead, do something like ( to get "user-agent" )
	# >>> ua = self.rq.request_header("user-agent", self.sn)
	# ( P.S. but I don't care! )
	s = s + pblock_table( self.rq.headers )

	s = s + "<h3> Server Headers </h3>\n"
	s = s + pblock_table( self.rq.srvhdrs )

	s = s + "<h3> Request Variables </h3>\n"
	s = s + pblock_table( self.rq.vars )

	s = s + '</blockquote>'

	return s        

# convert a block to an HTML table
def pblock_table( pblock ):
    s = '\n<table border=1 cellpadding=3>\n'
    l = string.split( pblock.pblock2str(), '"' )
    for n in range( 1, len( l ), 2 ):
	s = s + '<tr><td align=center>%s</td><td align=center>%s</td></tr>\n' \
		% ( l[n-1][:-1], l[n] )
    s = s + '\n</table>\n'
    return s
