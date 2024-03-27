import subprocess

def ConfigureConan():
	subprocess.run(["conan", "profile", "detect"])

def RunConan(buildType):
	subprocess.run(["conan",  "install", ".", "--build=missing", f"--settings=build_type={buildType}"])

def RunPremake(target):
	subprocess.run(["premake5", target])

if __name__ == "__main__" :
	#ConfigureConan()
	RunConan("Debug")
	RunConan("Release")
	RunPremake("vs2022")