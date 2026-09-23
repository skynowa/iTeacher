#!/usr/bin/env python3

#--------------------------------------------------------------------------------------------------
# \file  Deploy.py
# \brief
#--------------------------------------------------------------------------------------------------


import shutil
import os
from pathlib import Path
#--------------------------------------------------------------------------------------------------
if (__name__ == "__main__"):
	projects   = ["iTeacher", "Trainer", "SentenceBuilder"]
	projectDir = Path(__file__).resolve().parent
	destDir    = "/home/skynowa/Dropbox/Soft/iTeacher"

	if (not os.path.exists(destDir)):
		print("Error: destDir not exists {}. Exit.".format(destDir))
		exit(1)

	print("Deploy - start")

	for it_project in projects:
		binSrc  = projectDir / it_project / "build" / it_project
		if (not os.path.exists(binSrc)):
			print("Error: binSrc not exists {}. Skip.".format(binSrc))
			continue

		binDest = destDir + "/" + it_project

		print("- {} -> {} ...".format(binSrc, destDir))

		try:
			shutil.copy(binSrc, destDir)
		except IOError as e:
			print("  {}. Skip.".format(e))
			continue

	print("Deploy - OK")

#--------------------------------------------------------------------------------------------------
