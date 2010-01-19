import os
import cherrypy
from cherrypy.lib import static
import threading

tutconf = os.path.join(os.path.dirname(__file__), 'server.conf')
localDir = os.path.dirname(__file__)
absDir = os.path.join(os.getcwd(), localDir)

class TestWebServer:

	def __init__(self,parent=None):
		self.parent = parent
		self.fileToServer = None
	
	def index(self):
		if self.fileToServer:
			print "serving " + self.fileToServer
			filepath = os.path.join(absDir,self.fileToServer)
			self.fileToServer = None
			self.parent.fileServed()
			return static.serve_file(filepath, "application/x-download", "attachment",'test.jar')
		else:
			raise cherrypy.HTTPError(500, message="End of tests")

	index.exposed = True

	def reset(self):
		return "OK"

	reset.exposed = True

	def upload(self, myFile=None):
		print "getting a file"
		data = myFile.file.read()
		newFile = open("results.txt",'w')
		newFile.write(data)
		newFile.close()
		self.parent.fileUploaded();
		
	upload.exposed = True

	def serveFile(self,file):
		self.fileToServer = file


class TestWebServerRunner(threading.Thread):
	def __init__(self,parent = None):
		threading.Thread.__init__(self)
		self.startEvent = threading.Event()
		self.parent = parent
		self.server = TestWebServer(self.parent)

	def startup(self):
		self.startEvent.clear()
		self.start()
		self.startEvent.wait()
		
	def run(self):
		print "Starting BluewhalePlatform test server..."
		cherrypy.config.update(tutconf)
		cherrypy.tree.mount(self.server, '/',tutconf)
		cherrypy.server.quickstart()
		self.startEvent.set()
		cherrypy.engine.start()
		cherrypy.engine.block()
	
	def stop(self):
		cherrypy.engine.stop()
		cherrypy.server.stop()

	def serveFile(self,file):
		self.server.serveFile(file)
