[English] installation manual:
Unpack the file DynItemInst_Ikarus.zip to <Gothic II installation path>/System/.
After unpacking there should have been created a subfolder called 
'DynItemInst_Ikarus' where you can find licence terms, this readme, the dll 
and a file called 'DynItemInstExternals.d'. The latter contains functions for
using this library in the daedalus scripting language. You can place the file
'DynItemInstExternals.d' in the scripting folder wherever you like. But keep 
attention that the file has to be parsed after the Ikarus package. I advice 
to parse it shortly after Ikarus or LeGo. In 'Startup.d' you have to initialize
the library after Ikarus or LeGo was initialized. For Initialization the library 
use this command: DII_Init();


[German] Installationsanweisung:
Die Datei DynItemInst_Ikarus.zip unter <Gothic II Pfad>/System/ entpacken. 
Es sollte ein Unterordner namens "DynItemInst_Ikarus" entpackt werden, 
in dem Lizenzbestimmungen, eine Readme, die DLL und eine Datei 
DynItemInstExternals.d enthalten sind. Letzteres enthält Funktionen, 
damit man die Bibliothek in daedalus verwenden kann. Die Datei 
DynItemInstExternals.d kann man hintun wohin man möchte. Wann man sie 
parst ist einem selbst überlassen, allerdings muss sie nach Ikarus 
geparst werden. Ich rate dazu, sie gleich nach Ikarus oder Lego zu parsen.
In der Startup muss die DLL noch initialisiert werden und zwar nachdem 
Ikarus oder Lego initialisiert wurden. Die Initialisierung erfolgt mit 
folgendem Aufruf: DII_Init();