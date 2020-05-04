import glob
import shutil
import pathlib
import traceback
import colorama
import os
import zipfile
import GothicVDFSExecuter
import sys
import argparse

# Same as add_hook(always=True)
import colored_traceback.always


def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate


class Copier:
    def __init__(self, neclibRoot : pathlib.Path):
        self.root = neclibRoot
        pass

    def getRoot(self) -> pathlib.Path:
        return self.root

    def createFolderPathChecked(self, subFolderPath : str) -> pathlib.Path:
        path = self.getRoot() / subFolderPath
        if (not path.is_dir()):
            raise FileNotFoundError("Expected a folder path: " + str(path))

        return path


    def createIfNotExist(self, subFolderPath : str) -> pathlib.Path:
        path = self.getRoot() / subFolderPath
        if (not path.exists()):
            os.makedirs(path)
            
        return path

    def copyFiles(self, source : pathlib.Path, dest : pathlib.Path, files : list):
        for file in files:
            shutil.copyfile(source / file, dest / file)
            pass
        pass

    def copyFolder(self, source : pathlib.Path, dest : pathlib.Path, recursive = True, pattern = "*"):
        files = source.glob(pattern)
        if (recursive):
            files = source.rglob(pattern)

        for file in files:
            destFile = dest / file.relative_to(source)
            if (file.is_dir() and recursive):
                destFile.mkdir()
            elif (not file.is_dir()):
                shutil.copyfile(file, destFile)
                pass
            

        pass

    def copyNeclib(self, dest : pathlib.Path) -> None:
        source = self.createFolderPathChecked("workspace/System/neclib")
        files = ["neclib.dll", "neclib.ini", "3RD PARTY LIBRARIES AND SOFTWARE.txt", "COPYING.LESSER.txt", "GPL.txt", "LICENCES.txt"]
        self.copyFiles(source, dest, files)
        pass

    def copySystemFiles(self, dest : pathlib.Path)->None: 
        # copy demo files 
        source = self.createFolderPathChecked("workspace/System")
        files = ["neclib-demo.ini", "neclib-demo.rtf"]
        self.copyFiles(source, dest, files)
        pass   
pass




def copyDemoSources(copier: Copier):

    #Anims
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Anims")
    files = ["HUM_BODY_NAKED0.ASC", "HUM_MAGPETRIFY_M01.ASC", "HumanS.mds"]
    copier.copyFiles(source, dest, files)

    #Anims/_compiled
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims/_compiled")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Anims/_compiled")
    files = ["HUMANS.MDH", "HUMANS.MSB"]
    copier.copyFiles(source, dest, files)
    copier.copyFolder(source, dest, False, "*TLK*")
    copier.copyFolder(source, dest, False, "HUMANS_LEVITATE*")

    #Anims/Levitation
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims/Levitation")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Anims/Levitation")
    copier.copyFolder(source, dest, True)

    #Content
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content")
    files = ["Gothic.src"]
    copier.copyFiles(source, dest, files)
    
    #Content/_Intern
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/_intern")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/_intern")
    copier.copyFolder(source, dest, True)

    #Content/AI/AI_Intern
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/AI/AI_Intern")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/AI/AI_Intern")
    files = ["Perception.d"]
    copier.copyFiles(source, dest, files)

    #Content/AI/Magic
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/AI/Magic")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/AI/Magic")
    files = ["Spell_ProcessMana.d", "Spell_ProcessMana_Release.d"]
    copier.copyFiles(source, dest, files)

    #Content/AI/Magic/Spells
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/AI/Magic/Spells")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/AI/Magic/Spells")
    files = ["Spell_Levitation.d", "Spell_Telekinesis.d"]
    copier.copyFiles(source, dest, files)

    #Content/Items
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/Items")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/Items")
    files = ["IT_Runen.d", "IT_Scrolls.d", "IT_Written.d"]
    copier.copyFiles(source, dest, files)

    #Content/LeGo
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/LeGo")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/LeGo")
    copier.copyFolder(source, dest, True)

    #Content/mod
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/mod")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/mod")
    copier.copyFolder(source, dest, True)

    #neclib
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/neclib")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/neclib")
    copier.copyFolder(source, dest)

    #Content/Story
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/Story")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/Story")
    files = ["Startup.d", "Text.d"]
    copier.copyFiles(source, dest, files)

    #Content/NPC
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/Story/NPC")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/Content/Story/NPC")
    files = ["PC_Hero.d"]
    copier.copyFiles(source, dest, files)

    #System
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/System/VisualFX")
    dest = copier.createIfNotExist("dist/source/demo/_work/Data/Scripts/System/VisualFX")
    files = ["VisualFxInst.d"]
    copier.copyFiles(source, dest, files)
    pass


def distNeclib(copier : Copier, version : str) -> None:

    print("Building neclib distribution...")

    # gothic-file-changes
    source = copier.createFolderPathChecked("gothic-file-changes")
    dest = copier.createIfNotExist("dist/source")
    copier.copyFolder(source, dest)

    # neclib _work sources
    source = copier.createFolderPathChecked("workspace/_work/Data/Scripts/Content/neclib")
    dest = copier.createIfNotExist("dist/source/_work/Data/Scripts/Content/neclib")
    copier.copyFolder(source, dest)

    #demo
    copyDemoSources(copier)


    #Levitation/Anims
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims")
    dest = copier.createIfNotExist("dist/source/Levitation/Anims")
    files = ["HUM_BODY_NAKED0.ASC"]
    copier.copyFiles(source, dest, files)

    #Levitation/Anims/_compiled
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims/_compiled")
    dest = copier.createIfNotExist("dist/source/Levitation/Anims/_compiled")
    copier.copyFolder(source, dest, False, "HUMANS_LEVITATE*")

    #Levitation/Anims/Levitation
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims/Levitation")
    dest = copier.createIfNotExist("dist/source/Levitation/Anims/Levitation")
    copier.copyFolder(source, dest)

    
    #System/neclib
    dest = copier.createIfNotExist("dist/source/System/neclib")
    copier.copyNeclib(dest)


    #Telekinesis/Anims/_compiled
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims/_compiled")
    dest = copier.createIfNotExist("dist/source/Telekinesis/Anims/_compiled")
    copier.copyFolder(source, dest, False, "*TLK*")

    #Telekinesis/Anims
    source = copier.createFolderPathChecked("workspace/_work/Data/Anims")
    dest = copier.createIfNotExist("dist/source/Telekinesis/Anims")
    files = ["HUM_MAGPETRIFY_M01.ASC"]
    copier.copyFiles(source, dest, files)


    # zip file
    distOut = copier.createFolderPathChecked("dist/")
    filename = "neclib-" + version + ".zip"
    zipFile = distOut / filename
    zipfolder(zipFile, distOut / "source")

    print("neclib distribution: " + str(zipFile))
    pass

def distDemo(copier : Copier, version : str) -> None:
    dest = copier.createIfNotExist("dist/demo-source/System") 
    copier.copySystemFiles(dest)

    dest = copier.createIfNotExist("dist/demo-source/System/neclib") 
    copier.copyNeclib(dest)

    #create build script
    workspaceFolder = copier.getRoot() / "workspace"
    demoModFile = workspaceFolder / "Data/modvdf/neclib-demo.mod"
    buildScript = pathlib.Path('neclib-demo.vm')
    buildDemoBuildScript(buildScript, workspaceFolder, demoModFile)

    #build demo file
    print("Building demo mod archive...")
    GothicVDFSExecuter.execute(buildScript)

    # copy demo mod to dist
    source = copier.createFolderPathChecked("workspace/Data/modvdf")
    dest = copier.createIfNotExist("dist/demo-source/Data/modvdf")
    files = ["neclib-demo.mod"]
    copier.copyFiles(source, dest, files)

    distOut = copier.createFolderPathChecked("dist/")
    filename = "neclib-demo-" + version + ".zip"
    zipFile = distOut / filename
    zipfolder(distOut / filename, distOut / "demo-source")

    print("neclib demo distribution: " + str(zipFile))

    pass


def zipfolder(outputPath : pathlib.Path, target_dir : pathlib.Path):            
    zipobj = zipfile.ZipFile(outputPath, 'w', zipfile.ZIP_DEFLATED)
    rootlen = len(str(target_dir.absolute())) + 1
    for base, dirs, files in os.walk(target_dir):
        for file in files:
            fn = os.path.join(base, file)
            zipobj.write(fn, fn[rootlen:])


def deleteFolderTreeNoException(folder : pathlib.Path):
    try:
        shutil.rmtree(folder)
    except:
        pass

def deleteFileNoException(file : pathlib.Path):
    try:
        os.remove(file)
    except:
        pass    



def clearSources(distFolder : pathlib.Path):
    deleteFolderTreeNoException(distFolder / "source")
    deleteFolderTreeNoException(distFolder / "demo-source")
    pass       

def clearDist(distFolder : pathlib.Path):
    print("Deleting intermediate files...")
    clearSources(distFolder)
    deleteFileNoException(distFolder / "neclib-demo-test.zip")
    deleteFileNoException(distFolder / "neclib-test.zip")
    pass            


def buildDemoBuildScript(destinationScriptFilePath : pathlib.Path, 
    workspaceFolder : pathlib.Path, 
    demoModFilePath : pathlib.Path):

    print("Buildong demo build script...")

    with open(str(destinationScriptFilePath.absolute()), "w") as file:

        filesRegex = [
            "_work\\Data\\Scripts\\_compiled\\VisualFX.dat -r",
            "_work\\Data\\Scripts\\_compiled\\Gothic.dat -r",
            "_work\\Data\\Anims\\_compiled\\*TLK* -r",
            "_work\\Data\\Anims\\_compiled\\HUMANS_LEVITATE* -r",
            "_work\\Data\\Anims\\_compiled\\HUMANS.MDH -r",
            "_work\\Data\\Anims\\_compiled\\HUMANS.MSB -r"]

        lines = [
            "[BEGINVDF]",
            "Comment=",
            "BaseDir=" + str(workspaceFolder.absolute()),
            "VDFName=" + str(demoModFilePath.absolute()),
            "[FILES]"]

        for regex in filesRegex:
            lines.append(str(pathlib.Path(regex)))

        lines.append("[EXCLUDE]")
        lines.append("_backup-2.6_de\\* -r") 

        lines += [    
            "[INCLUDE]",
            "[ENDVDF]"]

        for line in lines:
            file.write(line + '\n')

    pass



def parseArgs() -> argparse.Namespace:
    print("Parsing arguments...")
    parser = argparse.ArgumentParser(description='Build neclib distribution')
    parser.add_argument("-v", "--version", required=True)
    return parser.parse_args()


"""
Main
"""
args = parseArgs()
version = args.version

print("version = " + version)

root_dest = "./neclib-dist"

currentFolder = pathlib.Path(__file__).parent.absolute()
#workspaceFolder = pathlib.Path().absolute()
neclibRoot = currentFolder.parent.absolute()
copier = Copier(neclibRoot)
distFolder = neclibRoot / "dist"

clearDist(distFolder)
distNeclib(copier, version)
distDemo(copier, version)

print("Distribution building: Finished.")
#clearSources(distFolder)