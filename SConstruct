import glob, os, sys

targetPlatform = ARGUMENTS.get('platform', 'TARGET_LINUX32')
targetMode = ARGUMENTS.get('mode', 'TARGET_RELEASE')
if targetMode == 'release':
	targetMode = 'TARGET_RELEASE'
elif targetMode == 'debug':
	targetMode = 'TARGET_DEBUG'
elif targetMode != 'TARGET_DEBUG' and targetMode != 'TARGET_RELEASE':
	print 'Invalid mode: ' + targetMode
	sys.exit(1)
targetCharSize = ARGUMENTS.get('charsize', 'TARGET_ASCII8')

# "Declarations"

# output_path = 'BUILDRESULTS/' + targetPlatform + targetMode + targetCharSize

folderIgnoreList = ['.svn', '.git']

#####################################
# Generic functions to build things #
#####################################

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
	cppFlags = []
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
		cppFlags = ['-g']
	else:
		compOutName = 'release/'+name
		cppFlags = ['-O2']
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)

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
	cppFlags = []
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
		cppFlags = ['-g']
	else:
		compOutName = 'release/'+name
		cppFlags = ['-O2']
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)

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
	cppFlags = []
	
	# Output path and name
	if targetMode == 'TARGET_DEBUG':
		compOutName = 'debug/'+name
		cppFlags = ['-g']
	else:
		compOutName = 'release/'+name
		cppFlags = ['-O2']
	
	srcList = generateSourceIncDirLists(pathList, externalIncludeDirList)

	target = Program(
		compOutName,
		srcList,
		CPPPATH=externalIncludeDirList,
		CPPFLAGS=cppFlags,
		CPPDEFINES=[targetPlatform, targetMode, targetCharSize],
		LIBS=externalLibList,
		LIBPATH='debug' if targetMode == 'TARGET_DEBUG' else 'release',
		LINKFLAGS=linkFlagList
	)
	
	return target

#####################
# Build main things #
#####################

irclib = buildLibrary(
	'firc',
	['core/modules', 'anppfindep.git', 'anpcommon.git'],
	['anpbase.git/anp_basecode']
)
devapp_console = buildProgram(
	'firc_devapp_console',
	['frontend_console_cpp/modules'],
	[
		'anpbase.git/anp_basecode',
		'core/modules/frontend_interface',
		'core/modules/network',
		'core/modules/plugin_interface',
		'core/modules/pluginmanager',
		'anpcommon.git/anp_workerthreads',
		'anpcommon.git/anp_threadsafequeue',
		'anppfindep.git/anp_threading',
		'core/modules/cache',
		'anpcommon.git/log',
		'core/modules/tcpconnection',
		'core/modules/networkfactory',
		'anpcommon.git/eventdispatcher'
	],
	['firc', 'pthread', 'dl', 'pcrecpp'],
	['-rdynamic']
)

# Plugins
pluginTest1 = buildSharedLibrary(
	'pluginTest1',
	['plugindev/pluginTest1/modules'],
	[
		'anpbase.git/anp_basecode',
		'core/modules/plugin_interface',
		'core/modules/network',
		'anpcommon.git/eventdispatcher',
		'core/modules/networkfactory'
	],
	[])

# Depends(pluginTest1, [irclib])
Depends(irclib, [pluginTest1])
Depends(devapp_console, [irclib])
