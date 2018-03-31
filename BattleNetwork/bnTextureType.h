#pragma once
/** 
 * @warning These have to be in order.
 * @see TextureResourceManager
*/
enum TextureType
{
	//Tiles
	TILE_BLUE_NORMAL,
	TILE_BLUE_CRACKED,
	TILE_BLUE_BROKEN,
	TILE_BLUE_EMPTY,
	TILE_RED_NORMAL,
	TILE_RED_CRACKED,
	TILE_RED_BROKEN,
	TILE_RED_EMPTY,
	//Navis
	NAVI_MEGAMAN_MOVE,
	NAVI_MEGAMAN_HIT,
	NAVI_MEGAMAN_SHOOT,
	//Mobs
	MOB_MOVE,
	MOB_EXPLOSION,
	MOB_METTAUR_IDLE,
	MOB_METTAUR_HIT,
	MOB_METTAUR_ATTACK,
	MOB_PROGSMAN_IDLE,
	MOB_PROGSMAN_MOVE,
	MOB_PROGSMAN_SHOOT,
	MOB_PROGSMAN_HIT,
	MOB_PROGSMAN_PUNCH,
	//Spells
	SPELL_BULLET_HIT,
	SPELL_BUSTER_CHARGE,
	  //SPELL_CHARGED_BULLET_HIT,
	SPELL_WAVE,
	//Background/Foreground
	TEXTURE_TYPE_SIZE
};