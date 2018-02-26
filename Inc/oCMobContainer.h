/*////////////////////////////////////////////////////////////////////////////

This file is part of DynItemInst.

Copyright © 2015 David Goeth

All Rights reserved.

THE WORK (AS DEFINED BELOW) IS PROVIDED
UNDER THE TERMS OF THIS CREATIVE COMMONS
PUBLIC LICENSE ("CCPL" OR "LICENSE").
THE WORK IS PROTECTED BY COPYRIGHT AND/OR
OTHER APPLICABLE LAW. ANY USE OF THE WORK
OTHER THAN AS AUTHORIZED UNDER THIS LICENSE
OR COPYRIGHT LAW IS PROHIBITED.

BY EXERCISING ANY RIGHTS TO THE WORK PROVIDED
HERE, YOU ACCEPT AND AGREE TO BE BOUND BY THE
TERMS OF THIS LICENSE. TO THE EXTENT THIS
LICENSE MAY BE CONSIDERED TO BE A CONTRACT,
THE LICENSOR GRANTS YOU THE RIGHTS CONTAINED
HERE IN CONSIDERATION OF YOUR ACCEPTANCE OF
SUCH TERMS AND CONDITIONS.

Full license at http://creativecommons.org/licenses/by-nc/3.0/legalcode

/////////////////////////////////////////////////////////////////////////////**/

#ifndef __OCMOBCONTAINER_H___
#define __OCMOBCONTAINER_H___

#include "api/g2/ztypes.h"
#include "api/g2/macros.h"
#include "api/g2/ocitemExtended.h"

/**
 * Represents the gothic 2 oCMobContainer equivalent. A classic oCMobContainer is a chest.
 * Attention: This class uses all class members of the oCMobContainer class defined in the
 * file oCMob.d from the Ikarus script package (v. 1.2.0).
 * My heartfelt thanks go to Sektenspinner for permitting me to use it!
 */
class oCMobContainer
{
public:
    //zCVob {
      //zCObject {
      int    _vtbl;
      int    _zCObject_refCtr;
      int    _zCObject_hashIndex;
      int    _zCObject_hashNext;
      zSTRING _zCObject_objectName;
      //}
      int    _zCVob_globalVobTreeNode;
      int    _zCVob_lastTimeDrawn;
      int    _zCVob_lastTimeCollected;
      int    _zCVob_vobLeafList_array;
      int    _zCVob_vobLeafList_numAlloc;
      int    _zCVob_vobLeafList_numInArray;
      int    _zCVob_trafoObjToWorld[16];
      int    _zCVob_bbox3D_mins[3];
      int    _zCVob_bbox3D_maxs[3];
      int    _zCVob_bsphere3D_center[3];
      int    _zCVob_bsphere3D_radius;
      int    _zCVob_touchVobList_array;
      int    _zCVob_touchVobList_numAlloc;
      int    _zCVob_touchVobList_numInArray;
      int    _zCVob_type;
      int    _zCVob_groundShadowSizePacked;
      int    _zCVob_homeWorld;
      int    _zCVob_groundPoly;
      int    _zCVob_callback_ai;
      int    _zCVob_trafo;
      int    _zCVob_visual;
      int    _zCVob_visualAlpha;
      int    _zCVob_m_fVobFarClipZScale;
      int    _zCVob_m_AniMode;
      int    _zCVob_m_aniModeStrength;
      int    _zCVob_m_zBias;
      int    _zCVob_rigidBody;
      int    _zCVob_lightColorStat;
      int    _zCVob_lightColorDyn;
      int    _zCVob_lightDirectionStat[3];
      int    _zCVob_vobPresetName;
      int    _zCVob_eventManager;
      int    _zCVob_nextOnTimer;
      int    _zCVob_bitfield[5];
      int    _zCVob_m_poCollisionObjectClass;
      int    _zCVob_m_poCollisionObject;
    
    //oCMob
      zSTRING _oCMob_name;               
      int    _oCMob_bitfield;
      zSTRING _oCMob_visualDestroyed;    
      zSTRING _oCMob_ownerStr;           
      zSTRING _oCMob_ownerGuildStr;      
      int    _oCMob_owner;              
      int    _oCMob_ownerGuild;         
      int    _oCMob_focusNameIndex;     
      int    _oCMob_ignoreVobList_data; 
      int    _oCMob_ignoreVobList_next; 
      
    //oCMobInter:
      int _oCMobInter_optimalPosList_data;
      int _oCMobInter_optimalPosList_next;
      zSTRING _oCMobInter_triggerTarget; 
      zSTRING _oCMobInter_useWithItem;       
      zSTRING _oCMobInter_sceme;         
      zSTRING _oCMobInter_conditionFunc; 
      zSTRING _oCMobInter_onStateFuncName;   
      int _oCMobInter_state;                
      int _oCMobInter_state_num;            
      int _oCMobInter_state_target;     
      int _oCMobInter_rewind;               
      int _oCMobInter_mobStateAni;      
      int _oCMobInter_npcStateAni;      
      int _oCMobInter_bitfield;
      int _oCMobInter_tmpStateChanged;
      int _oCMobInter_Direction;        
      int _oCMobInter_onInterruptReturnToSlotPos;   
      int _oCMobInter_startPos[3];              
      int _oCMobInter_aniCombHeight;                
      int _oCMobInter_inUseVob;                 
      int _oCMobInter_timerEnd;                 
    
    //oCMobLockable  
      int _oCMobLockable_bitfield;
      zSTRING _oCMobLockable_keyInstance;
      zSTRING _oCMobLockable_pickLockStr;
    
    //oCMobContainer
    zSTRING contains;                //zSTRING //Scriptinstanzen der Items. Nicht aktuelle gehalten! Original Spacereinstellung!
    
    int items;                      //oCItemContainer*
    //zCListSort<oCItem>    containList; //sollte die richtigen Daten beinhalten
      int containList_compareFunc;          //int (*Compare)(oCItem *ele1,oCItem *ele2);
      int containList_data;                 //oCItem*           
      int containList_next;                 //zCListSort<oCItem>*

	  /**
	   * Inserts the given item into this container.
	   * \param item The item to insert
	   */
	  void Insert(oCItem* item) 
	  {
		XCALL(0x00725FC0);
	  };

	  /**
	   * Removes the given item from this container.
	   * \param item The item to remove
	   */
	  void Remove(oCItem* item)
	  {
		XCALL(0x00725FF0);
	  }
};

#endif __OCMOBCONTAINER_H___