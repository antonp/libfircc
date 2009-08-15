# TODO:
# 1) Set target parameters through command line

import glob, os

targetPlatform = 'TARGET_UBUNTU64'
targetMode = 'TARGET_DEBUG'
targetCharSize = 'TARGET_ASCII8'

# "Declarations"

# output_path = 'BUILDRESULTS/' + targetPlatform + targetMode + targetCharSize

folderIgnoreList = ['.svn']

###################################
# Generic functions to build crap #
###################################
def buildLibrary(name, path, externalIncludeDirList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		targetMode == 'release/'+name
	
	modulesBasePath = path+'/modules'
	srcList = []
	
	for folder in os.listdir(modulesBasePath):
		if folder not in folderIgnoreList:
			modulePath = os.path.join(modulesBasePath, folder)
			if ( os.path.isdir(modulePath) ):
				print 'Added '+modulePath+' to the list of modules.'
				externalIncludeDirList += [modulePath]
				moduleSrcPath = os.path.join(modulePath, 'src') + '/*.cpp'
				srcList += Split(glob.glob(moduleSrcPath))

	cppFlags = ['-g']

	target = StaticLibrary(
		compOutName,
		srcList,
		CPPPATH=externalIncludeDirList,
		CPPFLAGS=cppFlags,
		CPPDEFINES=[targetPlatform, targetMode, targetCharSize]
	)
	
	return target
	
def buildSharedLibrary(name, path, externalIncludeDirList, externalLibList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		targetMode == 'release/'+name
	
	modulesBasePath = path+'/modules'
	srcList = []
	
	for folder in os.listdir(modulesBasePath):
		if folder not in folderIgnoreList:
			modulePath = os.path.join(modulesBasePath, folder)
			if ( os.path.isdir(modulePath) ):
				print 'Added '+modulePath+' to the list of modules.'
				externalIncludeDirList += [modulePath]
				moduleSrcPath = os.path.join(modulePath, 'src') + '/*.cpp'
				srcList += Split(glob.glob(moduleSrcPath))

	cppFlags = ['-g']

	target = SharedLibrary(
		compOutName,
		srcList,
		CPPPATH=externalIncludeDirList,
		CPPFLAGS=cppFlags,
		CPPDEFINES=[targetPlatform, targetMode, targetCharSize],
		LIBS=externalLibList,
		LIBPATH=('debug' if (targetMode == 'TARGET_DEBUG') else 'release')
	)
	
	return target

def buildProgram(name, path, externalIncludeDirList, externalLibList, linkFlagList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		targetMode == 'release/'+name
	
	modulesBasePath = path+'/modules'
	srcList = []
	
	for folder in os.listdir(modulesBasePath):
		if folder not in folderIgnoreList:
			modulePath = os.path.join(modulesBasePath, folder)
			if ( os.path.isdir(modulePath) ):
				print 'Added '+modulePath+' to the list of modules.'
				moduleSrcPath = os.path.join(modulePath, 'src') + '/*.cpp'
				srcList += Split(glob.glob(moduleSrcPath))

	cppFlags = ['-g']

	target = Program(
		compOutName,
		srcList,
		CPPPATH=externalIncludeDirList,
		CPPFLAGS=cppFlags,
		CPPDEFINES=[targetPlatform, targetMode, targetCharSize],
		LIBS=externalLibList,
		LIBPATH=('debug' if (targetMode == 'TARGET_DEBUG') else 'release'),
#		LINKFLAGS=['-E', '-rdynamic', '--export-dynamic']
		LINKFLAGS=linkFlagList
	)
	
	return target

###################
# Build main crap #
###################

core = buildLibrary(
	'core',
	'core',
	['basecode']
)
frontend_console = buildProgram(
	'frontend_console',
	'frontend_console',
	['basecode', 'core/modules/plugin_interface'],
	['core', 'pthread', 'dl'],
	['-rdynamic']
)

# Plugins
pluginTest1 = buildSharedLibrary('pluginTest1', 'plugindev/pluginTest1', ['basecode', 'core/modules/plugin_interface'], [])
pluginTest2 = buildSharedLibrary('pluginTest2', 'plugindev/pluginTest2', ['basecode', 'core/modules/plugin_interface'], [])

Depends(pluginTest1, [core])
Depends(frontend_console, [core])
