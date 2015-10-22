[German]

Installationsanleitung:

Die Datei DynItemInst_G2Ext.zip unter <Gothic II Pfad>/System/ entpacken. 
Es sollte ein Unterordner entpackt werden, die Lizenzbestimmungen, diese Readme 
mit nützlichen Informationen, die DLL enthält und eine Datei namens DynItemInstClasses.d.
Letzeres muss in den Script ordner verschoben werden und sollte möglichst früh 
geparst werden (z.B. direkt nach _INTERN\CLASSES.d).
In der *.ini der mod muss 
der Abschnitt [G2Ext] erstellt werden (sofern nicht vorhanden) und dann folgende 
Zeile eingefügt werden:
DLL = DynItemInst_G2Ext/DynItemInst_G2Ext.dll


Neue Klassen:

In der Datei DynItemInstClasses.d wurde die Klasse DII_USER_DATA definiert.
Für jede dynamische Instanz wird ein zusätzlicher Speicher angelegt, damit
man einer Instanz zusätzliche Eigenschaften vergegeben kann. Im Moment handelt
es sich um ein 100 elementiges integer array.


Neue Externals:

Die Bibliothek erstellt neue externe Funktionen, die benutzt werden können um die
Funktionalität der Bibliothek innerhalb Daedalus zu verwenden. Diese Funktionen 
müssen nicht in Externals.d eingetragen werden:

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


Beispielaufrufe: 

var c_item item;
var int instance;
var int test;
var DII_UserData userData;
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

Installation manual:

Unpack the file DynItemInst_G2Ext.zip to <Gothic II installation path>/System/.
After unpacking there should have been created a subfolder called 
'DynItemInst_G2Ext' where you can find licence terms, this readme, 
the dll and a file named DynItemInstClasses.d. The latter has to be 
moved to your script folder and should be parsed early (e.g. direct 
after _INTERN\CLASSES.d).
In the *.ini file of your mod you have to create a new paragraph called 
[G2Ext] (if not there yet) und than paste the following line:
DLL = DynItemInst_G2Ext/DynItemInst_G2Ext.dll


New Classes:
The file DynItemInstClasses.d defines the class DII_USER_DATA.
For each dynamic instance additional memory will be allocated, so that
you can give an instance additional properties. At the moment this class
consists of an array of one hundred integers. 


New external functions:

The library creates new external functions which exists that you can use 
the functionality of this library within the daedalus scripting language.
This functions haven't to be entered in the file Externals.d:

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


Example calls:

var c_item item;
var int instance;
var int test;
var DII_UserData userData;
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