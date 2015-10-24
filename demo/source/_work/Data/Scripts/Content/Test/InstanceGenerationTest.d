Func void InstanceCreationTest() {
	
	VAR C_ITEM testItem;
	VAR C_ITEM testItem2;
	var int instanceId;
	var DII_USER_DATA userData;
	var int libVersion;
	
	libVersion = DII_GetLibVersion();
	Print(ConcatStrings("Library version: ", toStringf(libVersion)));
	
	testItem = DII_CreateNewItem(ItMw_1h_vlk_dagger);
	testItem2 = DII_CreateNewItem(ItMw_1h_vlk_dagger);
	
	testItem.name = "Hello";
	testItem.description = "Test";
	testItem.damageTotal = 500;
	testItem.Text[0] = "Yeah!";
	testItem.count[2] = testItem.damageTotal;
	
	instanceId = DII_CreateNewInstance(testItem);
	userData = DII_GetUserData(instanceId);
	userData.data[0] = 100;
	userData.data[99] = -2;
	userData = DII_GetUserData(instanceId);

	PrintDebug(ConcatStrings("userData.data[0] = ", IntToString(userData.data[0])));
	PrintDebug(ConcatStrings("userData.data[99] = ", IntToString(userData.data[99])));
	CreateInvItems(hero, instanceId, 30);
	
	testItem2 = DII_CreateNewItem(ItMw_1h_vlk_dagger);
	testItem = DII_CreateNewItem(instanceId);

	var int result;
	result = DII_IsDynamic(testItem2);
	PrintDebug(ConcatStrings("Has testItem2 a dynamic instance id? : ", IntToString(result)));
	result = DII_IsDynamic(testItem);
	PrintDebug(ConcatStrings("Has testItem a dynamic instance id? : ", IntToString(result)));

	result = Hlp_GetInstanceId(testItem);
	PrintDebug(ConcatStrings("testItem has id : ", IntToString(result)));
	DII_DeleteItem(testItem);
	DII_DeleteItem(testItem2);
	
};


Func void KeyPressureAction () {
if (MEM_KeyPressed(KEY_t) == KEY_PRESSED){InstanceCreationTest();};
};


func void Script_per_frame ()
{
    FF_ApplyOnceExt(KeyPressureAction, 300, -1);
};