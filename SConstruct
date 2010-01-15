# TODO:
# 1) Set target parameters through command line

import glob, os

targetPlatform = 'TARGET_UBUNTU64'
targetMode = 'TARGET_DEBUG'
targetCharSize = 'TARGET_ASCII8'

# "Declarations"

# output_path = 'BUILDRESULTS/' + targetPlatform + targetMode + targetCharSize

folderIgnoreList = ['.svn', '.git']

###################################
# Generic functions to build crap #
###################################

def generateSourceIncDirLists(pathList, incDirList):
	srcList = []

	for path in pathList:
		modulesBasePath = path
	
		for folder in os.listdir(modulesBasePath):
			if folder not in folderIgnoreList:
				modulePath = os.path.join(modulesBasePath, folder)
				if ( os.path.isdir(modulePath) ):
					print 'Added '+modulePath+' to the list of modules.'
					incDirList += [modulePath]
					moduleSrcPath = os.path.join(modulePath, 'src') + '/*.cpp'
					srcList += Split(glob.glob(moduleSrcPath))
					if modulePath == 'frontend_console_cpp/modules':
						print 'srcList for frontend_console_cpp: '
						print srcList
						print moduleSrcPath
	return srcList

def buildLibrary(name, pathList, externalIncludeDirList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		compOutName = 'release/'+name
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)

	cppFlags = ['-g']

	target = StaticLibrary(
		compOutName,
		srcList,
		CPPPATH=externalIncludeDirList,
		CPPFLAGS=cppFlags,
		CPPDEFINES=[targetPlatform, targetMode, targetCharSize]
	)
	
	return target
	
def buildSharedLibrary(name, pathList, externalIncludeDirList, externalLibList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		targetMode == 'release/'+name
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)

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

def buildProgram(name, pathList, externalIncludeDirList, externalLibList, linkFlagList):
	compOutName = name
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
	else:
		targetMode == 'release/'+name
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)
	
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
	['core/modules', 'anppfindep.git', 'anpcommon.git'],
	['anpbase.git/anp_basecode']
)
frontend_console_cpp = buildProgram(
	'frontend_console_cpp',
	['frontend_console_cpp/modules'],
	['anpbase.git/anp_basecode', 'core/modules/frontend_interface', 'core/modules/networkmanager', 'core/modules/plugin_interface', 'core/modules/pluginmanager', 'core/modules/anp_workerthreads', 'core/modules/anp_threading', 'core/modules/cache'],
	['core', 'pthread', 'dl'],
	['-rdynamic']
)

# frontend_console_c = buildProgram(
#	'frontend_console_c',
#	'frontend_console_c',
#	['basecode', 'core/modules/plugin_interface'],
#	['core', 'pthread', 'dl'],
#	['-rdynamic']
# )

test_threading = buildProgram(
	'test_threading',
	['test_threading'],
	['anpbase.git/anp_basecode', 'core/modules/anp_threading', 'core/modules/anp_workerthreads'],
	['core', 'pthread', 'dl'], # core because anp_workerthreads is compiled into it...
	['-rdynamic'] #beacuse of dynamic linking
)

# Plugins
pluginTest1 = buildSharedLibrary('pluginTest1', ['plugindev/pluginTest1'], ['anpbase.git/anp_basecode', 'core/modules/plugin_interface'], [])
pluginTest2 = buildSharedLibrary('pluginTest2', ['plugindev/pluginTest2'], ['anpbase.git/anp_basecode', 'core/modules/plugin_interface'], [])

Depends(pluginTest1, [core])
# Depends(frontend_console_c, [core])
Depends(frontend_console_cpp, [core])
