import sys
import socket
import time
import os
import threading
import shutil
from xml.dom.minidom import parse, parseString, getDOMImplementation
import TestWebServer
import launcher
import SocketServer

class TestCase:
	def __init__(self,name=""):
		self.name = name

		
class TestSuite:
	def __init__(self,name="",jarPath=""):
		self.name = name
		self.jarPath = jarPath
		self.localDir = os.path.dirname(__file__)
		self.testcases = []
		
	def jarPath(self):
		return self.jarPath
		
	def addTestCase(self,testcase):
		self.testcases.append(testcase)
		
	def getTestCases(self):
		return self.testcases

	def setupEnv(self,sock):
		pass
		
	def teardownEnv(self,sock):
		pass
	
	def prerunSetup(self):
		pass
	
	def postrunSetup(self):
		pass
					
					

class SimpleServer(SocketServer.BaseRequestHandler):
	def setup(self):
		print "setup ", self.client_address
	def handle(self):
		print "client connected ", self.client_address
		while True:
			data = self.request.recv(1024)
			if not data:
				break
			print data
		self.request.close()

	def finish(self):
		print "finish"

class ServerRunner(threading.Thread):
	def __init__(self,parent = None):
		threading.Thread.__init__(self)
		self.exitEvent = threading.Event()
		self.server = None
		
	def run(self):
		self.server = SocketServer.ThreadingTCPServer(('localhost',8173),SimpleServer)
		while not self.exitEvent.isSet():
			self.server.handle_request()
		
	def stop(self):
		self.exitEvent.set()
		if self.server:
			self.server.socket.close()

class NetworkTestsuite(TestSuite):
	def __init__(self):
		TestSuite.__init__(self,"Network Tests","..\\NetworkTests\deployed\\NetworkTests.jar")
	
	def prerunSetup(self):
		print "starting simple server"
		self.server = ServerRunner()
		self.server.start()
	
	def postrunSetup(self):
		print "stopping simple server"
		self.server.stop()
		
	def setupEnv(self,sock):
		print "setupEnv"
		#self.sendCmd("net fail 231\r",sock)
		
	def teardownEnv(self,sock):
		print "tear down network tests"

	def checkResults(self):
		print "checking"
		shutil.copyfile("results.txt","results\\network-test.xml") 

	def sendCmd(self,cmd,sock):
		print cmd.replace("\r"," ")
		sock.sendall(cmd)
		data = sock.recv(50)
		print data
		time.sleep(0.5)	
		
class RMSTestsuit(TestSuite):
	def __init__(self):
		TestSuite.__init__(self,"RMS Tests","..\\RecordStore\deployed\\RecordStore.jar")
		
	def checkResults(self):
		print "copying file"
		shutil.copyfile("results.txt","results\\rms-test.xml") 

class FEPTestsuite(TestSuite):
	def __init__(self):
		TestSuite.__init__(self,"FEP Tests","..\\TestEditorTest\\deployed\\TestEditorTest.jar")
		test1 = HelloWorldEditTest()
		self.addTestCase(test1)
		test2 = NumberEditTest()
		self.addTestCase(test2)
		test3 = LongpressEditTest()
		self.addTestCase(test3)

	def setupEnv(self,sock):
		print "setupEnv"
		
	def teardownEnv(self,sock):
		self.sendCmd(2,sock) # exit

	def sendCmd(self,cmd,sock):
		msg = "cmd " + str(cmd) + "\r"
		print msg.replace("\r"," ")
		sock.sendall(msg)
		data = sock.recv(50)
		print data
		time.sleep(0.5)	
	
	def checkResults(self):
		testResults = getDOMImplementation()
		newdoc = testResults.createDocument(None, "testsuite", None)
		top_element = newdoc.documentElement
		top_element.setAttribute("name",self.name);
		
		res = "results.txt"
		print "results in " +res
		dom1 = parse(res)
		entries = dom1.getElementsByTagName("entry")
		index = 0
		fails = 0
		passes = 0
		for entry in entries:
			testcase = newdoc.createElement("testcase") 
			testcase.setAttribute("name",self.testcases[index].name) 
			if self.testcases[index].compareResult(entry.childNodes[0].data) :
				passes = passes + 1
			else:
				fails = fails + 1
				failure = newdoc.createElement("failure") 
				text = newdoc.createTextNode('Expected text not found ' + entry.childNodes[0].data)
				failure.appendChild(text)
				testcase.appendChild(failure)

			top_element.appendChild(testcase)
			index = index + 1
		dom1.unlink()
		top_element.setAttribute("tests",str(index));
		top_element.setAttribute("failures",str(fails));
		
		fp = open("results\\FEP-results.xml","w")
		newdoc.writexml(fp, "    ", "", "\n", "UTF-8")
		newdoc.unlink()
		fp.close()
	
class BlueWhaleTestCase(TestCase):
	def __init__(self,name=""):
		TestCase.__init__(self,name)

	def sendKey(self,key, delay = 0.1):
		msg = "key down " + hex(key) + "\r"
		print msg.replace("\r"," ")
		self.sock.sendall(msg)
		data = self.sock.recv(50)
		print data
		time.sleep(delay)
		msg = "key up " + hex(key) + "\r"
		print msg.replace("\r"," ")
		self.sock.sendall(msg)
		data = self.sock.recv(50)
		print data
		time.sleep(5*delay)
	
	def sendKeyDown(self,key, delay = 0.1):
		msg = "key down " + hex(key) + "\r"
		print msg.replace("\r"," ")
		self.sock.sendall(msg)
		data = self.sock.recv(50)
		print data
		time.sleep(delay)

	def sendKeyUp(self,key, delay = 0.1):
		msg = "key up " + hex(key) + "\r"
		print msg.replace("\r"," ")
		self.sock.sendall(msg)
		data = self.sock.recv(50)
		print data
		time.sleep(delay)
	

	def sendCmd(self,cmd):
		msg = "cmd " + str(cmd) + "\r"
		print msg.replace("\r"," ")
		self.sock.sendall(msg)
		data = self.sock.recv(50)
		print data
		time.sleep(0.5)	

class BlueWhaleTextEditTest(BlueWhaleTestCase):
	def __init__(self,name="",expected=""):
		BlueWhaleTestCase.__init__(self,name)
		self.expected = expected
	
	def compareResult(self,result):
		print "compareResult " + result + " " + self.expected
		if result == self.expected:
			print "test ok"
			return True
		else:
			print "test fail"
			return False

class HelloWorldEditTest(BlueWhaleTextEditTest):
	def __init__(self):
		BlueWhaleTextEditTest.__init__(self,"Hello, World","hello, World.")
		
	def runTest(self, sock):
		print "running " + self.name
		self.sock = sock
		self.sendCmd(1) # clear
		self.doHelloWorld()
		self.sendCmd(0) # capture
		
	def doHelloWorld(self):
		self.sendKey(0x34) # h
		self.sendKey(0x33) # e
		self.sendKey(0x35) # l
		self.sendKey(0x35) # l
		self.sendKey(0x36) # 0
		self.sendKey(0x31) # .
		self.sendKey(0x85) # *
		
		self.sendKey(0x30)

		self.sendKey(0x7f) # #
		self.sendKey(0x39) # w
		self.sendKey(0x36) # o
		self.sendKey(0x37) # r
		self.sendKey(0x35) # l
		self.sendKey(0x33) # d
		self.sendKey(0x31) # .

		self.sendKey(0x0f)

class NumberEditTest(BlueWhaleTextEditTest):
	def __init__(self):
		BlueWhaleTextEditTest.__init__(self,"Number edit","1234")
		
	def runTest(self, sock):
		print "running " + self.name
		self.sock = sock
		self.sendCmd(1) # clear
		self.doNumber()
		self.sendCmd(0) # capture
		

	def doNumber(self):
		self.sendKeyDown(0x7f) # #
		time.sleep(1)
		self.sendKeyUp(0x7f) # #
		self.sendKey(0x31) # 1
		self.sendKey(0x32) # 2
		self.sendKey(0x33) # 3
		self.sendKey(0x34) # 4
		self.sendKeyDown(0x7f) # #
		time.sleep(1)
		self.sendKeyUp(0x7f) # #

class LongpressEditTest(BlueWhaleTextEditTest):
	def __init__(self):
		BlueWhaleTextEditTest.__init__(self,"Long key press test","a2")
		
	def runTest(self, sock):
		print "running " + self.name
		self.sock = sock
		self.sendCmd(1) # clear
		self.doLong()
		self.sendCmd(0) # capture
		

	def doLong(self):
		self.sendKey(0x32) # 2
		self.sendKeyDown(0x32) # #
		time.sleep(1)
		self.sendKeyUp(0x32) # #
			
		
class TestManager:
	
	def __init__(self):
		self.fileServedEvent = threading.Event()
		self.resultEvent = threading.Event()
		self.testSuites = []

		self.webServer = TestWebServer.TestWebServerRunner(self)

		#networkTests = NetworkTestsuite()
		#self.testSuites.append(networkTests)
		
		fepTests = FEPTestsuite()
		self.testSuites.append(fepTests)
		rmsTests  = RMSTestsuit()
		self.testSuites.append(rmsTests)
		
	def fileUploaded(self):
		self.resultEvent.set()
	
	def fileServed(self):
		self.fileServedEvent.set()
	
	def doTest(self):
		emulator = None
		try:
			self.webServer.startup()
			for suite in self.testSuites:
				try:
					try:
						os.remove("results.txt")
					except:
						print "ignoring deletion of non-existant file"
					print "about to run ", suite.name
					suite.prerunSetup()
					self.webServer.serveFile(suite.jarPath)
					self.fileServedEvent.clear()
					emulator = self.launchEmulator()
					self.fileServedEvent.wait(60) # emulator can take a long time to start first time
					
					if self.fileServedEvent.isSet(): # emulator started and requested file
						time.sleep(1)
						self.connect()
						suite.setupEnv(self.sock)
						for test in suite.getTestCases():
							print "run testcase " + test.name
							test.runTest(self.sock)
						suite.teardownEnv(self.sock)
						self.resultEvent.wait()
						suite.checkResults()
						self.resultEvent.clear()
						self.sock.close()
				except KeyboardInterrupt:
					print "User wants to quit script"
					raise
				except:
					print "Exception: ", sys.exc_info()
				finally:
					print "running finally block, shutting down emulator"
					if emulator:
						exitCode = launcher.waitForExit(emulator,10000)
						print "Emulator exit code ", exitCode
					suite.postrunSetup()
					

		except KeyboardInterrupt:
			print "outer exception"
		finally:
			print "outer finally block, stopping web server"
			self.webServer.stop()
			
		
	def launchEmulator(self):
		path=r'\Symbian\9.1\S60_3rd\Epoc32\release\winscw\udeb\bluewhaleplatform.exe'
		print "launch ", path
		child = launcher.run(path)
		return child

	def connect(self):
		HOST = '127.0.0.1'
		PORT = 8182

		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.connect((HOST, PORT))
		data = self.sock.recv(1024)
		print data
	

theTest = TestManager()
theTest.doTest()
#gen = theTest.filesToServe()
#for f in gen:
#	print f
