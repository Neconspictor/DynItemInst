func int C_IsTakenItemMyPossession (var C_NPC slf, var C_NPC oth, var C_ITEM itm)
{
	/*
	
	...
	
	// ------ wenn item in meinem Portalraum (oder Raum von befreundeter Gilde) genommen wurde ------
	if (C_NpcIsBotheredByPlayerRoomGuild(self))
	|| (Wld_GetPlayerPortalGuild() == GIL_PUBLIC)
	{
		return TRUE;
	};
	*/


	// Check room of item
	// created by mud-freak: https://forum.worldofplayers.de/forum/threads/1555267-RELEASE-neclib?p=26389325&viewfull=1#post26389325
	ExternalAcceptVobs();
    portalguild = Npc_GetPortalGuild(itm);
    ExternalDenyVobs();

    if ((portalguild > GIL_NONE)
    && ((slf.guild == portalguild) || (Wld_GetGuildAttitude(slf.guild, portalguild) == ATT_FRIENDLY)))
    || (portalguild == GIL_PUBLIC) {
        return TRUE;
    };
	
	/*
	 ...
	*/
};
