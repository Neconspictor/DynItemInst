Dynamic Item Instances - Version 1.01

[Beschreibung]
Dynamic Item Instances (DynItemInst) ist eine in C++ geschriebene Laufzeitbibliothek, 
die es ermöglicht, dass man in Gothic 2 DNdR zur Laufzeit Instanzen von Items
generieren kann. Damit ist es z.B. problemlos möglich Features wie 
'Waffen schärfen' oder 'Rüstungen verbessern' zu implementieren, ohne dass man die 
benötigten Item Instanzen vorm Parsen der Skripte erstellt haben muss.

[Description]
Dynamic Item Instances (DynItemInst) is a runtime library written in c++ which 
allows to create item instances at runtime for the game Gothic 2 TNotR.
Therefore it is easily possible to implement features such as 'sharp weapons' 
or 'improve armour' without the need of pre-generating the additional 
item instances.


[German] 

Installationsanweisung:

Die zip Datei unter <Gothic II Pfad>/System/ entpacken. 
Es sollte ein Unterordner namens "DynItemInst_Ikarus" entpackt werden, 
in dem Lizenzbestimmungen, diese Readme, die DLL und die Dateien 
DynItemInstExternals.d und DynItemInstClasses.d enthalten sind. 
Die letzten beiden genannten Dateien müssen in den Script ordner.
Wann man sie parst ist einem selbst überlassen, allerdings muss 
DynItemInstClasses.d vor DynItemInstExternals.d geparst werden.
Letztere muss zudem nach Ikarus geparst werden. 
In der Startup muss die DLL noch initialisiert werden und zwar nachdem 
Ikarus oder Lego initialisiert wurden. Die Initialisierung erfolgt mit 
folgendem Aufruf: DII_Init();


Neue Klassen:

In der Datei DynItemInstClasses.d wurde die Klasse DII_USER_DATA definiert.
Für jede dynamische Instanz wird ein zusätzlicher Speicher angelegt, damit
man einer Instanz zusätzliche Eigenschaften vergegeben kann. Im Moment handelt
es sich um ein 100 elementiges integer array.


Neue Funktionen:

Die Bibliothek besitzt Funktionen, die über daedalus mit ein paar Hilfsfunktionen 
benutzt werden können um die Funktionalität der Bibliothek innerhalb Daedalus 
zu verwenden. Definiert sind diese Funktionen in DynItemInstExternals.d:

// *****************************************************************	
// Lädt und initialisiert die Bibliothek DynItemInst_Ikarus.DLL			
// *****************************************************************
FUNC void DII_Init();

// ***************************************************
// Erstellt ein neues item anhand einer instance id.
// Das item wird allerdings nicht in der Welt eingefügt.
// Diese Funktion ist praktisch wenn man ein temporäres 
// Item braucht.
// ***************************************************
FUNC C_ITEM DII_CreateNewItem (VAR INT n0); 


// ***************************************************
// Erstellt eine neue Iteminstanz und gibt dieser
// Instanz die gleichen Werte wie das angegebene Item
// hat.
// ***************************************************
FUNC INT DII_CreateNewInstance (VAR C_ITEM n0);


// ********************************************************
// Überprüft, ob ein gegebenes item eine dynamische Instanz
// hat oder nicht. Falls ja, wird TRUE (1) ansonsten 
// FALSE (0) zurückgegeben.
// ********************************************************
FUNC INT DII_IsDynamic(VAR C_ITEM n0);


// ***************************************************
// Löscht ein item und entfernt es aus der Welt.
// ***************************************************
FUNC VOID DII_DeleteItem (VAR C_ITEM n0);

// ***************************************************
// Liefert direkten Zugriff auf den zusätzlichen 
// Speicher einer dynamischen Instanz. Wird keine 
// dynamische Instanz angegeben, wird ein eines 
// DII_USER_DATA object zurückgegben.
// ***************************************************
FUNC DII_USER_DATA DII_GetUserData (VAR INT n0);

// *****************************************************************		
// Liefert die Versionsnummer der DynItemInst_Ikarus Bibliothek.
// Wichtig: Die Versionsnummer ist ein float, der in einem 
// Integer zurückgegeben wird. Man sollte daher diese Funktion z.B. 
// zusammen mit dem float-Packet von Ikarus verwenden.		
// *****************************************************************
FUNC INT DII_GetLibVersion();


Beispielaufrufe: 

var c_item item;
var int instance;
var int test;
var DII_UserData userData;
var int libVersion;

libVersion = DII_GetLibVersion();		// Hole die Versionsnummer der Bibliothek.
printf(libVersion);				// Gebe die Versionsnummer in float Darstellung auf dem Bildschirm aus.
item = DII_CreateNewItem(ItMw_1h_vlk_dagger); 	// item ist nun ein Dolch
item.value = 666;				// item hat einen Wert von 666 Gold;
instance = DII_CreateNewInstance(item);		// Eine neue instance wurde erstellt und in instance gespeichert.
userData = DII_GetUserData(instance);		// Hohle den zusätzlichen Speicher der neuen Instanz.
userData[0] = -3;				// Setze das erste Element auf -3.
userData = DII_GetUserData(instance);
test = userData[0];				// test hat jetzt den Wert -3.
CreateInvItem(hero, instance, 20);		// Der Held bekommt 20 Dolche, die jeweils einen Wert von 666 Gold haben.
test = DII_IsDynamic(item);    			// test sollte TRUE sein.
DII_DeleteItem(item);				// unser temporäres item wird nicht mehr benötigt.




[English] 

installation manual:

Unpack the file zip file to <Gothic II installation path>/System/.
After unpacking there should have been created a subfolder called 
'DynItemInst_Ikarus' where you can find licence terms, this readme, the dll 
and two files called 'DynItemInstExternals.d' and 'DynItemInstClasses.d'. 
The latter mentioned files have to be placed in yout script folder.
Keep attention that 'DynItemInstClasses.d' has to be parsed before 
'DynItemInstExternals.d' and the latter has to be parsed after the Ikarus package, too. 
In 'Startup.d' you have to initialize the library after Ikarus or LeGo was initialized. 
For Initialization the library use this command: DII_Init();


New Classes:
The file DynItemInstClasses.d defines the class DII_USER_DATA.
For each dynamic instance additional memory will be allocated, so that
you can give an instance additional properties. At the moment this class
consists of an array of one hundred integers. 


New functions:

The library has functions which can be accessed over the daedalus scripting language 
over helper functions. Therfore you can use the functionality of this library 
within the daedalus.
This functions are defined in 'DynItemInstExternals.d':

// *****************************************************************	
// Loads and inits the library DynItemInst_Ikarus.DLL			
// *****************************************************************
FUNC void DII_Init();

// ************************************************************			
// Creates a new item. The item will have the instance id n0.
// This function is practiable if you need a temporary item.				
// ************************************************************
FUNC C_ITEM DII_CreateNewItem (VAR INT n0); 


// *********************************************************************				
// Creates a new dynamic item instance (dii) on the base of the item n0.
// Objects having this new dii will have identic properties like n0.			
// *********************************************************************
FUNC INT DII_CreateNewInstance (VAR C_ITEM n0);


// ************************************************************		
// Checks whether a given item n0 has a dynamic item instance.	
// If the item is dynamic, TRUE (1) otherwise FALSE (0) will
// be returned.		
// ************************************************************
FUNC INT DII_IsDynamic(VAR C_ITEM n0);


// ***************************************************************
// Deletes a provided item and removes it from the current world.			
// ***************************************************************
FUNC VOID DII_DeleteItem (VAR C_ITEM n0);

// **********************************************************************
// Provides access to the user data of a given dynamic item instance 
// which has the instance id n0. If the given instance id isn't dynamic,
// a new DII_USER_DATA object will be returned.			
// **********************************************************************
FUNC DII_USER_DATA DII_GetUserData (VAR INT n0);

// *****************************************************************		
// Provides the version number of the DynItemInst_Ikarus library.
// Important: The version number is a float digit, that is returned 
// as an integer. Therefore you should use this function along with
// e.g. the float package of Ikarus.	
// *****************************************************************
FUNC INT DII_GetLibVersion();

Example calls:

var c_item item;
var int instance;
var int test;
var DII_UserData userData;
var int libVersion;

libVersion = DII_GetLibVersion();		// Get the library version.
printf(libVersion);				// prints the gotten library version in float notation.
item = DII_CreateNewItem(ItMw_1h_vlk_dagger); 	// item is now a dagger
item.value = 666;				// item has a value of 666 gold pieces.
instance = DII_CreateNewInstance(item);		// A new instance was created and its id is stored in instance.
userData = DII_GetUserData(instance);		// Get the additional memory of the new instance.
userData[0] = -3;				// Set the value of the first element to -3.
userData = DII_GetUserData(instance);
test = userData[0];				// test has now a value of -3.
CreateInvItem(hero, instance, 20);		// The hero gets 20 daggers, which each has a value of 666 gold pieces.
test = DII_IsDynamic(item);    			// test should be TRUE.
DII_DeleteItem(item);				// our temporary item isn't needed anymore.