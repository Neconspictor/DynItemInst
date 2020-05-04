# edits the version of the library
# automatically changes all occurences in files using the library version

import pathlib
import re
import sys
import functools 
import argparse


class VersionNumber:
    integral : int
    fractionalNumerator : int
    fractionalDenominator : int

    def __init__(self, versionTokens : list):
        size = len(versionTokens)
        self.integral = 0
        self.fractionalNumerator = 0
        self.fractionalDenominator = 1
        if (size > 0):
            self.integral = int(versionTokens[0])

        if (size > 1):
            numeratorStr = functools.reduce(lambda a,b : a+b, versionTokens[1:])
            self.fractionalNumerator = int(numeratorStr) 
            self.fractionalDenominator = pow(10, len(numeratorStr))

    def makeFloat(self) -> float:
        return float(self.integral) + float(self.fractionalNumerator) / float(self.fractionalDenominator)       


def processNeclibGeneralD(neclibGeneralDFile : pathlib.Path, versionNumber : VersionNumber):

    with open(neclibGeneralDFile) as f:
        content = f.readlines()

    beginVersionMark = str("BeginVersionMark").upper()
    endVersionMark = str("EndVersionMark").upper()

    beginVersionMarkIndices = [x for x in range(len(content)) if beginVersionMark in content[x].upper()]
    endVersionMarkIndices = [x for x in range(len(content)) if endVersionMark in content[x].upper()]

    assert  len(beginVersionMarkIndices) == 1, "expected exact one line with" + beginVersionMark
    assert  len(endVersionMarkIndices) == 1, "expected exact one line with" + endVersionMark

    beginVersionMarkIndex = beginVersionMarkIndices[0]
    endVersionMarkIndex = endVersionMarkIndices[0]

    assert endVersionMarkIndex - beginVersionMarkIndex == 3, "Expected two lines between " + beginVersionMark + " and " + endVersionMark

    #write version
    content[beginVersionMarkIndex + 1] = "    integral = mkf(%i);\n" % versionNumber.integral
    content[beginVersionMarkIndex + 2] = "    fraction = divf(%i, %i);\n" % (versionNumber.fractionalNumerator, versionNumber.fractionalDenominator)

    #update file
    with open(neclibGeneralDFile, "w") as f:
        f.writelines(content)



def processGeneralCpp(neclibGeneralCppFile : pathlib.Path, versionNumber : VersionNumber):

    decimalVersion = versionNumber.makeFloat()

    with open(neclibGeneralCppFile) as f:
        content = f.readlines()


    libVersionStatement = "const float General::LIB_VERSION ="

    libVersionIndices = [x for x in range(len(content)) if libVersionStatement in content[x]]  

    assert  len(libVersionIndices) == 1, "expected exact one line with" + libVersionStatement

    index = libVersionIndices[0]

    content[index] =   "const float General::LIB_VERSION = %sf;\n" % str(decimalVersion)

    #update file
    with open(neclibGeneralCppFile, "w") as f:
        f.writelines(content)

    pass


def processDemoIni(demoIniFile : pathlib.Path, versionNumber : VersionNumber):

    decimalVersion = versionNumber.makeFloat()

    with open(demoIniFile) as f:
        content = f.readlines()


    versionStatement = "Version="

    versionIndices = [x for x in range(len(content)) if versionStatement in content[x]]  

    assert  len(versionIndices) > 0, "expected minimal one line with" + versionStatement

    index = versionIndices[0]

    content[index] = "Version=%s\n" % str(decimalVersion)

    #update file
    with open(demoIniFile, "w") as f:
        f.writelines(content)

    pass


def execute(version : str):

    validVersionRegex = r"^[0-9]+([\.0-9])*$"

    if (not re.match(validVersionRegex, version)):
        print("This version is not valid: " + version)
        print("version has to match this regex: " + validVersionRegex)
        return False


    print("version = " + version)
    
    versionTokens = re.split(r"\.", version)

    # Note: validVersionRegex assures that there is at least one digit.
    # Thus, versionTokens won't be empty

    versionNumber = VersionNumber(versionTokens)

    print("integral = " + str(versionNumber.integral))
    print("fractionalNumerator = " + str(versionNumber.fractionalNumerator))
    print("fractionalDenominator = " + str(versionNumber.fractionalDenominator))
    print("decimal version = " + str(versionNumber.makeFloat()))

    neclibRoot = pathlib.Path(__file__).parent.absolute()

    neclibGeneralDFile = neclibRoot / "workspace/_work/data/Scripts/Content/neclib/General.d"
    neclibGeneralCppFile = neclibRoot / "Src/General.cpp"
    demoIniFile = neclibRoot / "workspace/System/neclib-demo.ini"


    print("processing " + str(neclibGeneralDFile))
    processNeclibGeneralD(neclibGeneralDFile, versionNumber)
    
    print("processing " + str(neclibGeneralCppFile))
    processGeneralCpp(neclibGeneralCppFile, versionNumber)
    
    print("processing " + str(demoIniFile))
    processDemoIni(demoIniFile, versionNumber)

    print("Finished updating library version in source files.")

    return True


def parseArgs() -> argparse.Namespace:
    print("Parsing arguments...")
    parser = argparse.ArgumentParser(description='Updating library version for source files')
    parser.add_argument("-v", "--version", required=True)
    return parser.parse_args()


args = parseArgs()
execute(args.version)