#pragma once

#include "Common.h"

#ifdef PLATFORM_WIN
#include "../stdafx.h"
#endif // PLATFORM_WIN

#ifdef PLATFORM_MACOSX
#include "../ext/img/tgaimage.h"
#endif // PLATFORM_MACOSX

#include <stdlib.h>
#include <vector>

#ifdef PLATFORM_WIN
#include "Console.h"
#include "MemBuffer.h"
#endif // PLATFORM_WIN

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
#include "Instance.h"
#include "SceneEnvrionment.h"

using std::vector;

namespace LaplataRayTracer
{
	// All the objects' allocation and free are in This Class.
	class Scene
	{
	public:
		Scene()
		    : mpSurface(nullptr), mpViewPlane(nullptr), mpCamera(nullptr),
		    mpRenderWndSink(nullptr), mpViewSampler(nullptr), mpRayTracer(nullptr), mpBackground(nullptr)
		{

		}
		virtual ~Scene() { Purge(); }

	public:
#ifdef PLATFORM_WIN
		inline void SetDrawingSurface(CDC *pDC) { mpSurface = pDC; }
		inline void SetConsoleHandle(OutputConsole *pConsle) { mpConsole = pConsle; }
#endif // PLATFORM_WIN
		inline void SetRenderWndSink(IRenderWindowSink *pRenderWndSink) { mpRenderWndSink = pRenderWndSink; }

	public:
		inline void SetViewSampler(SamplerBase *pSampler) { mpViewSampler = pSampler; }
		inline void SetViewPlane(ViewPlane *pViewPlane) { mpViewPlane = pViewPlane; }
		inline void SetCamera(Camera *pCamear) { mpCamera = pCamear; }
		inline void SetRayTracer(RayTracer *pRayTracer) { mpRayTracer = pRayTracer; }
        inline void SetBackground(WorldEnvironment *pSceneEnv) { mpBackground = pSceneEnv; }

	public:
		virtual void Setup(int w = 400, int h = 400)
        {
#ifdef PLATFORM_MACOSX
		    mpSurface = new TGAImage(w, h, TGAImage::RGB);
#endif // PLATFORM_MACOSX
        }
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

			bool enableZoom = mpCamera->IsZoomMode();
			float zoomFactor = mpCamera->GetZoomFactor();

			for (int row = 0; row < h; ++row)
			{
				for (int col = 0; col < w; ++col)
				{
					Color3f color(0.0f, 0.0f, 0.0f);
                    /*int sampler_count = mpViewSampler->GetSampleCount();
					for (int c = 0; c < sampler_count; ++c)
					{
						float x;
						float y;

						Point2f sp = mpViewSampler->SampleFromUnitSquare();

						if (enableZoom)
						{
							x = zoomFactor * (col - 0.5f * w + sp.X());
							y = zoomFactor * (row - 0.5f * h + sp.Y());
						}
						else
						{
							x = (col - 0.5f * w + sp.X());
							y = (row - 0.5f * h + sp.Y());
						}

						Ray ray;
						if (mpCamera->GenerateRay(x, y, ray))
                        {
							color += mpRayTracer->Run(ray, 0, 10);
						}
					}
                    color /= (float)sampler_count;*/
                    int N = 30;
                    for (int m = 0; m < N; ++m)
                    {
                        for (int n = 0; n < N; ++n)
                        {
                            float x;
                            float y;

                            if (enableZoom)
                            {
                                x = zoomFactor * (col - 0.5f * w + (float)m / N);
                                y = zoomFactor * (row - 0.5f * h + (float)n / N);
                            }
                            else
                            {
                                x = (col - 0.5f * w + (float)m / N);
                                y = (row - 0.5f * h + (float)n / N);
                            }

                            Ray ray;
                            if (mpCamera->GenerateRay(x, y, ray))
                            {
                                color += mpRayTracer->Run(ray, 0, 10);
                            }
                        }
                    }
                    color /= (N*N);
					
//                    float x = (col - 0.5f * w);
//                    float y = (row - 0.5f * h);
//                    Ray ray;
//                    if (mpCamera->GenerateRay(x, y, ray))
//                    {
//                        color = mpRayTracer->Run(ray, 0, 10);
//                    }

                    // Postphone processing
					ImageProc::De_NAN(color);
#ifdef GAMMA_CORRECTION
                    ImageProc::GammaCorrection(color, 0.4f);
#else
                    ImageProc::HDR_Operator_MaxToOne(color);
#endif // GAMMA_CORRECTION

					int r = (int)(255.99f*color[0]);
					int g = (int)(255.99f*color[1]);
					int b = (int)(255.99f*color[2]);

#ifdef PLATFORM_WIN
					mpSurface->SetPixel(col, h - row - 1, RGB(r, g, b));
#endif // PLATFORM_WIN

#ifdef PLATFORM_MACOSX
                    TGAColor pixel_color(r, g, b);
                    mpSurface->set(col, h - row - 1, pixel_color);
#endif // PLATFORM_MACOSX
				}

				//
				if (mpRenderWndSink)
				{
					mpRenderWndSink->OnNotifyRenderProgress(row, h);
				}
			}
		}

		virtual void RenderText(const int x, const int y, std::string const& text, int r, int g, int b)
		{
#ifdef PLATFORM_WIN
			if (mpSurface)
			{
				int text_len = strlen(text.c_str());
				mpSurface->SetBkColor(TRANSPARENT);
				mpSurface->SetTextColor(RGB(255, 255, 255));
				mpSurface->TextOut(x, y, text.c_str(), text_len);
			}
#endif // PLATFORM_WIN
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
#ifdef PLATFORM_WIN
						mpSurface->SetPixel(x + i, y + j, RGB(r,g,b));
#endif // PLATFORM_WIN

#ifdef PLATFORM_MACOSX
                        TGAColor pixel_color(r, g, b);
                        mpSurface->set(x + i, y + j, pixel_color);
#endif // PLATFORM_MACOSX
					}
				}
			}
		}

		virtual void SaveScene(const char *resultPath)
        {
#ifdef PLATFORM_MACOSX
            mpSurface->write_tga_file(resultPath);
#endif // PLATFORM_MACOSX

        }

		virtual void Purge()
		{
			//
			ClearScene();

			//
			if (mpViewSampler) { delete mpViewSampler; mpViewSampler = nullptr; }
			if (mpViewPlane) { delete mpViewPlane; mpViewPlane = nullptr; }
			if (mpCamera) { delete mpCamera; mpCamera = nullptr; }
			if (mpRayTracer) { delete mpRayTracer; mpRayTracer = nullptr; }
			if (mpBackground) { delete mpBackground; mpBackground = nullptr; }

#ifdef PLATFORM_MACOSX
			if (mpSurface) { delete mpSurface; mpSurface = nullptr; }
#endif // PLATFORM_MACOSX
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

//	protected:
//		inline void showRenderingPercentage(float ration)
//		{
//			CString sRation;
//			sRation.Format(_T("rendering: %.2f%%\n"), ration * 100);
//			mpConsole->Write(sRation);
//		}

	protected:
#ifdef PLATFORM_WIN
		CDC *	mpSurface;
		OutputConsole * mpConsole;
#endif // PLATFORM_WIN

#ifdef PLATFORM_MACOSX
		TGAImage *mpSurface;
#endif // PLATFORM_MACOSX

		IRenderWindowSink *mpRenderWndSink;

		SamplerBase *   mpViewSampler;

		SceneObjects	mvecObjects;
		SceneLights		mobjSceneLights;

		ViewPlane *		mpViewPlane;
		Camera	*		mpCamera;
		RayTracer *		mpRayTracer;

        WorldEnvironment *mpBackground;

	};

}
