/*
	LINKS:

	--- Instancing ---
	http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter03.html	// it's rather about DirectX 9 than 11. Not sure if this fits me right.

	--- Sprite Atlas ---
	http://dallinwellington.com/rendering/sprite-atlas-rendering-with-dx-hlsl/

*/

#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "__d3dClass.h"
#include "__cameraClass.h"
#include "__modelClass.h"
#include "__colorShaderClass.h"
#include "__textureShaderClass.h"
#include "__lightShaderClass.h"
#include "__lightClass.h"
#include "__bitmapClass.h"
#include "__highPrecTimer.h"
#include "__textOutClass.h"
#include "__threadPool.h"
#include "__SpriteInstanced.h"
#include "__bitmapClassInstancing.h"
#include "__textureShaderClassInstancing.h"

#include <string>

#include "Game.h"

// ---------------------------------------------------------------------------------------
#if defined fullScreen
const bool FULL_SCREEN    = true;
const int  windowedWidth  = 0;
const int  windowedHeight = 0;
#else
const bool FULL_SCREEN    = false;
const int  windowedWidth  = 800;
const int  windowedHeight = 600;
#endif

const bool	VSYNC_ENABLED = false;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 0.1f;
// ---------------------------------------------------------------------------------------



// Предварительное объявление дружественного класса
class Game;



class GraphicsClass {

 // Объявляем класс Game как дружественный, чтобы просто передавать в него указатель на GraphicsClass и иметь возможность рендерить игровые объекты
 friend class Game;

 public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass &);
   ~GraphicsClass();

	bool Initialize(int, int, HighPrecisionTimer *, HWND);
	void Shutdown();
	bool Frame(const int &, const int &, const float &);

	inline void logMsg(const std::string &, bool = false, char * = "___msgLog.log");

    bool Render(const float &, const float &, const int &, const int &, const keysPressed *, const bool & = false);

 private:
    bool Render2d(const float &, const float &, const int &, const int &, const keysPressed *, const bool & = false);
    bool Render3d(const float &, const float &, const int &, const int &, const keysPressed *, const bool & = false);

 private:

    // Поскольку мы рендерим кадры 1 раз в 20 мс, то не будем каждый раз создавать переменные и матроицы для рендеринга, а зададим их один раз и навсегда:
    bool		 result;
	D3DXMATRIX	 matrixView, matrixProjection, matrixWorldX, matrixWorldY, matrixWorldZ, matrixOrthographic, matrixTranslation, matrixScaling;

    int scrWidth, scrHeight, scrHalfWidth, scrHalfHeight;

    char                            *msg;                   // текстовое сообщение, которое можно вывести на экран

    d3dClass				        *m_d3d;
    CameraClass			            *m_Camera;
    ModelClass				        *m_Model;

    TextureShaderClass		        *m_TextureShader;       // Классы для шейдеров
    TextureShaderClass_Instancing   *m_TextureShaderIns;
    TextureShaderClass_Instancing   *m_TextureShaderInsArr;
    LightShaderClass		        *m_LightShader;
//    ColorShaderClass		        *m_ColorShader;

    LightClass				        *m_Light;
    BitmapClass			            *m_Bitmap_Tree;		    // BitmapClass object
	TextOutClass			        *m_TextOut;             // TextClass object
	BitmapClass_Instancing	        *m_BitmapIns;
};

#endif
