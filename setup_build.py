import subprocess
from platform import system

def ConfigureConan():
	subprocess.run(["conan", "profile", "detect"])

def RunConan(buildType):
	subprocess.run(["conan",  "install", ".", "--build=missing", f"--settings=build_type={buildType}"])

def RunPremake():
	match system():
		case "Linux":
			subprocess.run(["./premake5", "gmake2"])
		case "Windows":
			subprocess.run(["premake5", "vs2022"])
		case "Darwin":
			subprocess.run(["premake5", "xcode4"]) # Maybe?


if __name__ == "__main__" :
	#ConfigureConan()
	RunConan("Debug")
	RunConan("Release")
	RunPremake()