/* ScummVM - Scumm Interpreter
 * Copyright (C) 2004-2005 The ScummVM project
 *
 * The ReInherit Engine is (C)2000-2003 by Daniel Balsom.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

// Scene management module private header file

#ifndef SAGA_SCENE_H
#define SAGA_SCENE_H

#include "saga/text.h"
#include "saga/list.h"

namespace Saga {

#define SCENE_DOORS_MAX 16

class ActionMap;
class ObjectMap;

enum SCENE_MODES {
	SCENE_MODE_INVALID,
	SCENE_MODE_NORMAL,
	SCENE_MODE_ISO
};


struct SCENE_BGINFO {
	int bg_x;
	int bg_y;
	int bg_w;
	int bg_h;
	int bg_p;
	byte *bg_buf;
	size_t bg_buflen;
};

struct SCENE_INFO {
	SCENE_BGINFO bg_info;
	TEXTLIST *text_list;
};

typedef int (SCENE_PROC) (int, SCENE_INFO *, void *);

enum SCENE_LOAD_FLAGS {
	BY_RESOURCE = 0,
	BY_SCENE,
	BY_DESC
};

enum SCENE_PROC_PARAMS {
	SCENE_BEGIN = 0,
	SCENE_END
};

// Resource type numbers
enum SAGA_RESOURCE_TYPES {
	SAGA_BG_IMAGE = 2,
	SAGA_BG_MASK = 3,
	SAGA_OBJECT_NAME_LIST = 5,
	SAGA_OBJECT_MAP = 6,
	SAGA_ACTION_MAP = 7,
	SAGA_ISO_TILESET = 8,
	SAGA_ISO_METAMAP = 9,
	SAGA_ISO_METATILESET = 10,
	SAGA_ENTRY = 12,
	SAGA_ANIM_1 = 14,
	SAGA_ANIM_2,
	SAGA_ANIM_3,
	SAGA_ANIM_4,
	SAGA_ANIM_5,
	SAGA_ANIM_6,
	SAGA_ANIM_7,
	SAGA_PAL_ANIM = 23,
	SAGA_FACES = 24
};

#define SAGA_RESLIST_ENTRY_LEN 4

struct SCENE_RESLIST {
	uint32 res_number;
	int res_type;
	byte *res_data;
	size_t res_data_len;
};

#define SAGA_SCENE_DESC_LEN 16

struct SCENE_DESC {
	int16 flags;
	int16 resListRN;
	int16 endSlope;
	int16 beginSlope;
	uint16 scriptNum;
	uint16 sceneScriptNum;
	uint16 startScriptNum;
	int16 musicRN;
	SCENE_RESLIST *resList;
	size_t resListCnt;
};

struct SCENE_IMAGE {
	int loaded;
	int w;
	int h;
	int p;
	byte *buf;
	size_t buf_len;
	byte *res_buf;
	size_t res_len;
	PALENTRY pal[256];
};

struct SCENE_ANIMINFO {
	int anim_res_number;
	int anim_handle;
};

typedef SortedList<SCENE_ANIMINFO> SceneAnimInfoList;

enum SCENE_FADE_TYPES {
	SCENE_NOFADE = 0,
	SCENE_FADE = 1,
	SCENE_FADE_NO_INTERFACE = 2
};

struct SCENE_QUEUE {
	uint32 scene_n;
	SCENE_DESC *scene_desc;
	int load_flag;
	SCENE_PROC *scene_proc;
	int scene_skiptarget;
	int fadeType;
};

typedef Common::List<SCENE_QUEUE> SceneQueueList;

///// IHNM-specific stuff
#define IHNM_PALFADE_TIME    1000
#define IHNM_INTRO_FRAMETIME 80
#define IHNM_DGLOGO_TIME     8000
#define IHNM_TITLE_TIME_GM   28750
#define IHNM_TITLE_TIME_FM   19500

///// ITE-specific stuff
#define INTRO_STRMAX 256
#define ITE_INTRO_FRAMETIME 90

#define INTRO_CAPTION_Y 170
#define VOICE_PAD 50
#define VOICE_LETTERLEN 90

#define PALETTE_FADE_DURATION 1000
#define DISSOLVE_DURATION 3000
#define LOGO_DISSOLVE_DURATION 1000

#define CREDIT_DURATION1 4000

enum INTRO_SCENE_DIALOGUE_INFO {
	INTRO_CAVE1_START = 0,
	INTRO_CAVE1_END = 4,

	INTRO_CAVE2_START = 4,
	INTRO_CAVE2_END = 7,

	INTRO_CAVE3_START = 7,
	INTRO_CAVE3_END = 10,

	INTRO_CAVE4_START = 10,
	INTRO_CAVE4_END = 14
};

struct INTRO_DIALOGUE {
	uint32 i_voice_rn;
	const char *i_cvar_name;
	char i_str[INTRO_STRMAX];
};

struct INTRO_CAPTION {
	int res_n;
	char *caption;
};

struct INTRO_CREDIT {
	int text_x;
	int text_y;
	int delta_time;
	int duration;
	const char *string;
	int font_id;
};


enum SceneFlags {
	kSceneFlagISO        = 1,
	kSceneFlagShowCursor = 2
};

class Scene {
 public:
	Scene(SagaEngine *vm);
	~Scene();

// Console functions
	void cmdSceneInfo();
	void cmdActionMapInfo();
	void cmdObjectMapInfo();

	void cmdSceneChange(int argc, const char **argv);

	int startScene();
	int nextScene();
	int skipScene();
	int endScene();
	int queueScene(SCENE_QUEUE *scene_queue);
	int draw(SURFACE *);
	int getMode();
	int getFlags() { return _desc.flags; }
	bool isInDemo() { return !_inGame; }
	
	void getBGMaskInfo(int &width, int &height, byte *&buffer, size_t &bufferLength);
	int isBGMaskPresent() {
		return _bgMask.loaded;
	}
	int getBGMaskType(const Point &testPoint);
	bool validBGMaskPoint(const Point &testPoint);
	bool canWalk(const Point &testPoint);
	bool offscreenPath(Point &testPoint);

	void setDoorState(int doorNumber, int doorState);
	int getDoorState(int doorNumber);
	void initDoorsState();

	int getBGInfo(SCENE_BGINFO *bginfo);
	int getBGPal(PALENTRY **pal);
	int getInfo(SCENE_INFO *si);
	void getSlopes(int &beginSlope, int &endSlope);

	int clearSceneQueue(void);
	int changeScene(int scene_num);

	bool initialized() { return _initialized; }


	int getSceneLUT(int num);
	int currentSceneNumber() { return _sceneNumber; }

 private:
	int loadScene(int scene, int load_flag, SCENE_PROC scene_proc, SCENE_DESC *, 
				  int fadeIn);
	int loadSceneDescriptor(uint32 res_number);
	int loadSceneResourceList(uint32 res_number);
	int processSceneResources();

 private:
	SagaEngine *_vm;
	bool _initialized;

	RSCFILE_CONTEXT *_sceneContext;
	int *_sceneLUT;
	int _sceneCount;
	int _sceneMax;
	SceneQueueList _sceneQueue;
	int _firstScene;
	bool _sceneLoaded;
	int _sceneMode;
	int _sceneNumber;
	int _sceneResNum;
	bool _inGame;
	bool _loadDesc;
	SCENE_DESC _desc;
	int _resListEntries;
	SCENE_RESLIST *_resList;
	int _animEntries;
	SceneAnimInfoList _animList;
	SCENE_PROC *_sceneProc;
	TEXTLIST *_textList;
	SCENE_IMAGE _bg;
	SCENE_IMAGE _bgMask;
	int _sceneDoors[SCENE_DOORS_MAX];

	static int SC_defaultScene(int param, SCENE_INFO *scene_info, void *refCon);
	int defaultScene(int param, SCENE_INFO *scene_info);

 public:
	ActionMap *_actionMap;
	ObjectMap *_objectMap;

 private:
	int IHNMStartProc();
	int ITEStartProc();

 public:
	static int SC_IHNMIntroMovieProc1(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_IHNMIntroMovieProc2(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_IHNMIntroMovieProc3(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_IHNMHateProc(int param, SCENE_INFO *scene_info, void *refCon);

 private:
	int IHNMIntroMovieProc1(int param, SCENE_INFO *scene_info);
	int IHNMIntroMovieProc2(int param, SCENE_INFO *scene_info);
	int IHNMIntroMovieProc3(int param, SCENE_INFO *scene_info);
	int IHNMHateProc(int param, SCENE_INFO *scene_info);

 public:
	int ITEIntroRegisterLang(void);
	static int SC_ITEIntroAnimProc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroCave1Proc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroCave2Proc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroCave3Proc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroCave4Proc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroValleyProc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroTreeHouseProc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroFairePathProc(int param, SCENE_INFO *scene_info, void *refCon);
	static int SC_ITEIntroFaireTentProc(int param, SCENE_INFO *scene_info, void *refCon);

 private:
	int ITEIntroAnimProc(int param, SCENE_INFO *scene_info);
	int ITEIntroCave1Proc(int param, SCENE_INFO *scene_info);
	int ITEIntroCave2Proc(int param, SCENE_INFO *scene_info);
	int ITEIntroCave3Proc(int param, SCENE_INFO *scene_info);
	int ITEIntroCave4Proc(int param, SCENE_INFO *scene_info);
	int ITEIntroValleyProc(int param, SCENE_INFO *scene_info);
	int ITEIntroTreeHouseProc(int param, SCENE_INFO *scene_info);
	int ITEIntroFairePathProc(int param, SCENE_INFO *scene_info);
	int ITEIntroFaireTentProc(int param, SCENE_INFO *scene_info);

};

} // End of namespace Saga

#endif
