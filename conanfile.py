from conan import ConanFile

class BlackTekServerSDK(ConanFile):
    settings = ("os", "compiler", "build_type", "arch")
    generators = ("PremakeDeps")

    def requirements(self):
        self.requires("cryptopp/8.9.0")
        self.requires("fmt/10.2.1")
        self.requires("mariadb-connector-c/3.3.3")
        self.requires("pugixml/1.14")
        self.requires("boost/1.84.0")
        self.requires("luajit/2.1.0-beta3")

    def build_requirements(self):
        self.tool_requires("cmake/[>=3.25]")

    def layout(self):
        self.folders.generators = "./packages"