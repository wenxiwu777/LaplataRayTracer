#pragma once

#include "../stdafx.h"
#include <stdlib.h>
#include <vector>

#include "Console.h"
#include "MemBuffer.h"
#include "WindowSink.h"

#include "WorldObjects.h"
#include "ImageProc.h"
#include "ViewPlane.h"
#include "Camera.h"
#include "RayTracer.h"
#include "Texture.h"
#include "LightObject.h"
#include "Light.h"
#include "Volume.h"
#include "MeshObject.h"
#include "MovingObject.h"

using std::vector;

namespace LaplataRayTracer
{
	// All the objects' allocation and free are in This Class.
	class Scene
	{
	public:
		Scene()
			: mpSurface(nullptr), mpViewPlane(nullptr), mpCamera(nullptr),
				mpRenderWndSink(nullptr), mpRayTracer(nullptr), mpBackground(nullptr)
		{

		}
		virtual ~Scene() { Purge(); }

	public:
		inline void SetDrawingSurface(CDC *pDC) { mpSurface = pDC; }
		inline void SetConsoleHandle(OutputConsole *pConsle) { mpConsole = pConsle; }
		inline void SetRenderWndSink(IRenderWindowSink *pRenderWndSink) { mpRenderWndSink = pRenderWndSink; }

	public:
		inline void SetViewPlane(ViewPlane *pViewPlane) { mpViewPlane = pViewPlane; }
		inline void SetCamera(Camera *pCamear) { mpCamera = pCamear; }
		inline void SetRayTracer(RayTracer *pRayTracer) { mpRayTracer = pRayTracer; }
		inline void SetBackground(Texture *pTexture) { mpBackground = pTexture; }

	public:
		virtual void Setup() { }
		virtual void UpdateScene(float fElapse) { }
		virtual void BuildScene() { }

		virtual void RenderScene()
		{
			if (mvecObjects.size() == 0)
				return;

			// Setup rendering context & parameters.
			RTEnv env;
			env.mpvecHitableObjs = &mvecObjects;
			env.mpSceneLights = &mobjSceneLights;
			env.mpBackground = mpBackground;
			mpRayTracer->SetRTEnv(env);

			// Get a ray according to the current x, y offset of the view plane
			// Shoot the ray then use RayTracer object to trace it
			int w = mpViewPlane->Width();
			int h = mpViewPlane->Height();

			for (int row = 0; row < h; ++row)
			{
				for (int col = 0; col < w; ++col)
				{
					Color3f color(0.0f, 0.0f, 0.0f);
					for (int m = 0; m < 30; ++m)
					{
						for (int n = 0; n < 30; ++n)
						{
							float x = (col - 0.5f * w + (float)m/30);
							float y = (row - 0.5f * h + (float)n/30);

							Ray ray;
							if (mpCamera->GenerateRay(x, y, ray))
							{
								color += mpRayTracer->Run(ray, 0, 16);
							}
						}
					}
					color /= 900.0f;

					ImageProc::De_NAN(color);
					ImageProc::HDR_Operator_MaxToOne(color);

					int r = (int)(255.99f*color[0]);
					int g = (int)(255.99f*color[1]);
					int b = (int)(255.99f*color[2]);

					mpSurface->SetPixel(col, h - row - 1, RGB(r, g, b));
				}

				//
				if (mpRenderWndSink)
				{
					mpRenderWndSink->OnNotifyRenderProgress(row, h);
				}
			}
		}

		virtual void RenderText(const int x, const int y, std::string const& text, const unsigned long color = RGB(255, 255, 255))
		{
			if (mpSurface)
			{
				int text_len = strlen(text.c_str());
				mpSurface->SetBkColor(TRANSPARENT);
				mpSurface->SetTextColor(color);
				mpSurface->TextOut(x, y, text.c_str(), text_len);
			}
		}

		virtual void RenderImage(const int x, const int y, const int w, const int h, Texture *tex)
		{
			if (mpSurface)
			{
				for (int j = 0; j < h; ++j) {
					for (int i = 0; i < w; ++i) {
						Color3f texel_color = tex->GetTexelColor(i, j);
						int r = (int)(255.99f*texel_color[0]);
						int g = (int)(255.99f*texel_color[1]);
						int b = (int)(255.99f*texel_color[2]);
						mpSurface->SetPixel(x + i, y + j, RGB(r,g,b));
					}
				}
			}
		}

		virtual void Purge()
		{
			//
			ClearScene();

			//
			if (mpViewPlane) { delete mpViewPlane; mpViewPlane = nullptr; }
			if (mpCamera) { delete mpCamera; mpCamera = nullptr; }
			if (mpRayTracer) { delete mpRayTracer; mpRayTracer = nullptr; }
			if (mpBackground) { delete mpBackground; mpBackground = nullptr; }
		}

		virtual void ClearScene()
		{
			//
			for (int i = 0; i < mvecObjects.size(); ++i)
			{
				if (mvecObjects[i])
				{
					delete mvecObjects[i];
					mvecObjects[i] = nullptr;
				}
				mvecObjects.clear();
			}

			//
			mobjSceneLights.Purge();
		}

	protected:
		inline void showRenderingPercentage(float ration)
		{
			CString sRation;
			sRation.Format(_T("rendering: %.2f%%\n"), ration * 100);
			mpConsole->Write(sRation);
		}

	protected:
		CDC *	mpSurface;
		OutputConsole * mpConsole;
		IRenderWindowSink *mpRenderWndSink;

		SceneObjects	mvecObjects;
		SceneLights		mobjSceneLights;
		ViewPlane *		mpViewPlane;
		Camera	*		mpCamera;
		RayTracer *		mpRayTracer;
		Texture *		mpBackground;

	};

}
