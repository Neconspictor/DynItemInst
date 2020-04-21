func void INIT_GLOBAL()
{
	// wird fuer jede Welt aufgerufen (vor INIT_<LevelName>)
	Game_InitGerman();
	
	//LeGo_Init(LeGo_All & ~LeGo_Bloodsplats);
	
	NEC_INIT_GLOBAL(); // after LeGo
	
	MAGICWEAPON_init(); // after NEC_INIT_GLOBAL();
};