#
# A python script to correct and configure Symbian SDK installations
#
# All SDKs found under "symbianDir" are modified
# The script takes no parameters but "symbianDir" below can be modified if the SDKs have been installed to a non-default location
#
# Patches are:
#   cl_bpabi.pm		Changed to prevent ARM build failure when lots of object files are linked
#   cl_gcce.pm		Changed to allow static libs to build
#   gcce.mak 		Changed to allow static libs to build
#
# The original files are backed up to ".bak" files, except gcce.mak (doing so causes the builds to fail)
# Repeated running of the script doesn't overwrite the original backed up files
#
# The script also configures the UIQ SDK to get emulator internet access via the PC's internet connection
# (only works for ethernet connections)
#


import os
import sys
import shutil

symbianDir = "C:\\Symbian"

def correctFile(aFileName, aOrigLines, aNewLines, aBackup=1):
	if not os.path.exists(aFileName):
		print "Can't find " + aFileName + " - skipping"
	else:
		print "Correcting " + aFileName
		f = open(aFileName, "r")
		lines = f.readlines()
		correctionMade = 0
		for i in range(len(lines)):
			line = lines[i]
			if line in aOrigLines:
				lines[i] = aNewLines[aOrigLines.index(line)]
				if (aBackup == 1) and not os.path.exists(aFileName + ".bak"):
					shutil.copyfile(aFileName, aFileName + ".bak")
				f2 = open(aFileName, "w")
				f2.writelines(lines)
				f2.close()
				correctionMade = 1
		f.close()
		if correctionMade == 0:
			print "Couldn't find lines to change - no correction made"


if not os.path.exists(symbianDir):
	print "Can't find " + symbianDir + " directory"
else:
	for version in os.listdir(symbianDir):
		if os.path.isdir(symbianDir + "\\" + version):
			print "\nCorrecting " + version 	

			epocRoot = symbianDir + "\\" + version + "\\"
			subDirs = os.listdir(epocRoot)
			if len(subDirs) == 1:
				epocRoot = epocRoot + subDirs[0] + "\\"

			cl_bpabi = epocRoot + "epoc32\\tools\\cl_bpabi.pm"
			origLines = ["\t\t\t\t\"\\t\\t\\$(OBJECTS$Bld) \\\\\\n\"\n"]
			newLines = ["\t\t\t\t\"\\t\\t\\\"\\$(EPOCBLD$Bld)\\\\*.o\\\" \\\\\\n\"\n"]
			correctFile(cl_bpabi, origLines, newLines)

			cl_gcce = epocRoot + "epoc32\\tools\\cl_gcce.pm"
			origLines = ["\t'ARCHIVER'=>' arm-none-symbianelf-ar ',\n"]
			newLines = ["\t'AR'=>' arm-none-symbianelf-ar ',\n"]
			correctFile(cl_gcce, origLines, newLines)

			gcce = epocRoot + "epoc32\\tools\\compilation_config\\gcce.mk"
			origLines = ["AR=arm-none-symbianelf-ar\n"]
			newLines = ["ARCHIVER=arm-none-symbianelf-ar\n"]
			correctFile(gcce, origLines, newLines, 0)	# don't create .bak here - stops builds working

			if (version == "UIQ3SDK"):
				print "Setting up ethernet connection" 

				ethernetced = epocRoot + "epoc32\\winscw\\c\\ethernetced.xml"
				origLines = ["        <DialogPref>DONOTPROMPT</DialogPref>\n", "        <DialogPref>PROMPT</DialogPref>\n"]
				newLines = ["        <DialogPref>PROMPT</DialogPref>\n", "        <DialogPref>DONOTPROMPT</DialogPref>\n"]
				correctFile(ethernetced, origLines, newLines)

				os.environ["EPOCROOT"] = epocRoot[2:]
				os.chdir(epocRoot + "epoc32\\data")		
				os.system("perl ..\\tools\\configchange.pl --config ethernetWithCommDB --kernel EKA1 --target winscw --variant udeb")

				os.system("devices -setdefault @UIQ3:com.symbian.UIQ")
				os.system("uiqenv -eth -dhcp -mod_mac")
