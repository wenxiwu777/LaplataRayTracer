#pragma once

#include "./source/Scene.h"

using namespace LaplataRayTracer;

class CDemoScene : public Scene
{
public:
	CDemoScene()
	{
		mnPointLightX = 40;
		mnPointLightY = 60;
		mnPointLightZ = 430;
	}

	virtual ~CDemoScene()
	{

	}

public:
	inline void SetSurface(CDC *pDC) { Scene::SetDrawingSurface(pDC); }
	inline void SetConsole(OutputConsole *pConsle) { Scene::SetConsoleHandle(pConsle); }

public:
	virtual void Setup()
	{
		  mpRayTracer = new RayCastTracer;
		//	mpRayTracer = new AreaLightTracer;
		//	mpRayTracer = new WhittedTracer;
		//	mpRayTracer = new PathTracer;
		//	mpRayTracer = new PathTracer_PBRS;

		mpViewPlane = new ViewPlane;
		mpCamera = new PerspectiveCamera;

		//	MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(100);
		//	mpCamera = new ThinLenCamera;
		//	((ThinLenCamera *)mpCamera)->SetSampler(pMJSampler);
		//	((ThinLenCamera *)mpCamera)->SetFocusPlaneLen(35.0F);
		//	((ThinLenCamera *)mpCamera)->SetLenDiskRadius(0.5);

		//	mpCamera = new FishEyeCamera;
		//	((FishEyeCamera *)mpCamera)->SetPsiPhi(120);

		mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
	}

	virtual void UpdateScene(float fElapse)
	{
		//	mnPointLightY -= 1.0;
		//	mnPointLightZ -= 5.0;
		mnPointLightX -= 5.0F;

		ClearScene();
		BuildScene();
	}

	virtual void BuildScene()
	{
		//	buildup_test_hard_shadow_scene();
		//	buildup_test_ambient_occlusion_scene();
		//	BuildupMultiObjIntersectionTestScene();
		//	buildup_CornellBox();
		//	buildup_test_area_light_shading_scene();
		//	buildup_test_texutre_area_light_shading_scene();
		//	buildup_test_retangle_texture_scene();
		//	build_up_test_ply_model_scene();
		//	buildup_test_mesh_triangle_model_scene();
		//  buildup_test_motion_blur_scene();
		    buildup_test_other_geometry_objects();

	}

private:
	void buildup_CornellBox()
	{
		//
		//
		mpBackground = new ConstantTexture(Color3f(0.1F, 0.1F, 0.1F));

		mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
		mpCamera->SetEye(Vec3f(0, 40, 370));
		mpCamera->SetLookAt(Vec3f(0, 40, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(500);
		mpCamera->Update();

		//
		// back wall
		MaterialObject *pBackWall = new MaterialObject(new XYRect(Vec3f(-40, 0, 200), false, 80, 80),
			new MatteMaterial(Color3f(1.0F, 1.0F, 1.0F)));

		// left wall
		MaterialObject *pLeftWall = new MaterialObject(new YZRect(Vec3f(-40, 0, 200), false, 80, 80),
			new MatteMaterial(Color3f(0.1F, 0.9F, 0.1F)));

		// right wall
		MaterialObject *pRightWall = new MaterialObject(new YZRect(Vec3f(40, 0, 200), true, 80, 80),
			new MatteMaterial(Color3f(0.9F, 0.1F, 0.1F)));

		// cell wall
		MaterialObject *pCellWall = new MaterialObject(new XZRect(Vec3f(-40, 80, 200), true, 80, 80),
			new MatteMaterial(Color3f(1.0F, 1.0F, 1.0F)));

		// floor wall
		MaterialObject *pFloorWall = new MaterialObject(new XZRect(Vec3f(-40, 0, 200), false, 80, 80),
			new MatteMaterial(Color3f(1.0F, 1.0F, 1.0F)));

		//
		mvecObjects.push_back(pBackWall);
		mvecObjects.push_back(pLeftWall);
		mvecObjects.push_back(pRightWall);
		mvecObjects.push_back(pCellWall);
		mvecObjects.push_back(pFloorWall);

		//
		// lighting set-up to light the scene up
		AreaLightObject *pCellLight = new AreaLightObject(new XZRect(Vec3f(-20, 78, 210), true, 40, 40),
			new ConstantTexture(Color3f(3.0F, 3.0F, 3.0F)),
			new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f)));
		pCellLight->TurnON();

		mvecObjects.push_back(pCellLight);

		//
		// Add a glass material ball in the middle of the box.
		MaterialObject *pGlassBall = new MaterialObject(new SimpleSphere(Vec3f(20, 20, 240), 20),
			new GlassMaterial);

		mvecObjects.push_back(pGlassBall);

		//
//		ConstantMedium *pVolumeShpere = new ConstantMedium(pGlassBall, 0.05f, new IsotropicMaterial(new ConstantTexture(Color3f(0.2F,0.4F,0.9F))));
//		mvecObjects.push_back(pVolumeShpere);

		//
		CompoundObject *pLightList = new CompoundObject;
		pLightList->EnableAutoDelete(false);
		pLightList->AddObject(pCellLight);
		pLightList->AddObject(pGlassBall);
		((PathTracer_PBRS *)mpRayTracer)->BindLightList(pLightList);
	}

	void BuildupMultiObjIntersectionTestScene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(0.1F, 0.1F, 0.1F));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		//
		SimpleSphere *pShpere = new SimpleSphere(Vec3f(-45, 45, 40), 50);
		pShpere->SetColor(Color3f(1, 0, 0));
		mvecObjects.push_back(pShpere);

		SimpleBox *pBox = new SimpleBox(20, 90, -101, 100, -100, 20);
		pBox->SetColor(Color3f(0, 1, 1));
		mvecObjects.push_back(pBox);

		SimpleTriangle *pTri = new SimpleTriangle(Vec3f(-110, -85, 80), Vec3f(120, 10, 20), Vec3f(-40, 50, -30));
		pTri->SetColor(Color3f(0, 0, 1));
		mvecObjects.push_back(pTri);

		SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -101, 0), Vec3f(0, 1, 0));
		pSimplePlane->SetColor(Color3f(1, 1, 0));
		mvecObjects.push_back(pSimplePlane);
	}

	void buildup_checker_texture_test_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1.0, 1.0, 1.0));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		PlaneCheckerTexture *pCheckerTexture = new PlaneCheckerTexture;
		pCheckerTexture->SetCheckSize(50.0f);
		pCheckerTexture->SetLineWidth(0.0f);
		TextureOnlyMaterial *pTexMaterial = new TextureOnlyMaterial(pCheckerTexture);
		SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -100, 0), Vec3f(0, 1, 0));
		MaterialObject *pPlane = new MaterialObject(pSimplePlane, pTexMaterial);
		mvecObjects.push_back(pPlane);

		SimpleSphere *pSimpleShpere = new SimpleSphere(Vec3f(40.0f, -50.0f, 220.0f), 50.0f);
		GlassMaterial *pGlassMaterial = new GlassMaterial;
		MaterialObject *pShphere = new MaterialObject(pSimpleShpere, pGlassMaterial);
		mvecObjects.push_back(pShphere);
	}

	void buildup_sphere_texture()
	{
		//
		mpBackground = new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		ImageTexture *pImageTex = new ImageTexture;
		bool bLoaded = pImageTex->LoadJPGFromFile("D:/workspace/计算机图形学/Ray Tracing/Ray-Traced-Images/Figure29.12(a).jpg");
		if (bLoaded)
		{
			ImageTextureMapping *pShpereMapping = new ShpereicalTextureMapping;
			pImageTex->SetInverseMapping(pShpereMapping);
			TextureOnlyMaterial *pImageTexMaterial = new TextureOnlyMaterial(pImageTex);
			SimpleSphere *pSimpleShpere = new SimpleSphere(Vec3f(0.0f, 0.0f, 220.0f), 100.0f);
			MaterialObject *pShpere = new MaterialObject(pSimpleShpere, pImageTexMaterial);
			mvecObjects.push_back(pShpere);
		}
		else
		{
			delete pImageTex;
			pImageTex = nullptr;
		}
	}

	void buildup_volume_test_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1.0F, 1.0F, 1.0F));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
// 		SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -100, 0), Vec3f(0, 1, 0));
// 		MatteMaterial *pPlaneMatte = new MatteMaterial(Color3f(0.2F, 0.4F, 0.9F));
// 		MaterialObject *pPlane = new MaterialObject(pSimplePlane, pPlaneMatte);
// 		mvecObjects.push_back(pPlane);

		//
		SimpleSphere *pSimpleShpere = new SimpleSphere(Vec3f(0.0f, 0.0f, 220.0f), 100.0f);
		GlassMaterial *pShpereGlass = new GlassMaterial;
		MaterialObject *pShere = new MaterialObject(pSimpleShpere, pShpereGlass);
		//	mvecObjects.push_back(pShere);

		ConstantMedium *pVolumeShpere = new ConstantMedium(pSimpleShpere, 0.001f, new IsotropicMaterial(new ConstantTexture(Color3f(1.0F, 0.0f, 0.0f))));
		mvecObjects.push_back(pVolumeShpere);
	}

	void buildup_matte_material_with_texture_test_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1.0F, 1.0F, 1.0F));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -100, 0), Vec3f(0, 1, 0));
		MatteMaterial *pPlaneMatte = new MatteMaterial(new CheckerTexture(new ConstantTexture(Color3f(1.0F, 1.0F, 1.0F)), new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f))));
		MaterialObject *pPlane = new MaterialObject(pSimplePlane, pPlaneMatte);
		mvecObjects.push_back(pPlane);

		SimpleSphere *pSimpleShpere = new SimpleSphere(Vec3f(0.0f, 0.0f, 220.0f), 100.0f);
		MaterialObject *pShere = new MaterialObject(pSimpleShpere, new MatteMaterial(new CheckerTexture(new ConstantTexture(Color3f(1.0F, 1.0F, 1.0F)), new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f)))));
		mvecObjects.push_back(pShere);
	}

	void buildup_test_retangle_texture_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1.0F, 1.0F, 1.0F));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		ImageTexture *pImage = new ImageTexture;
		bool bLoaded = pImage->LoadJPGFromFile("D:/workspace/计算机图形学/Ray Tracing/f16-9c.jpg");
		if (bLoaded)
		{
			YZRect *pSimpleYZRect = new YZRect(Vec3f(-50.0f, -50.0f, 300.0f), false, 60, 100);
			EmissiveTextureMaterial *pYZRectMat = new EmissiveTextureMaterial;
			pYZRectMat->SetRadiance(1.0F);
			pYZRectMat->SetEmissiveColor(pImage);
			MaterialObject *pYZRect = new MaterialObject(pSimpleYZRect, pYZRectMat);
			mvecObjects.push_back(pYZRect);
		}
	}

	void buildup_test_scene_of_phong_shading_model_with_lights()
	{
		//
		mpBackground = new ConstantTexture(Color3f(0.2, 0.1, 0.1));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(420);
		mpCamera->Update();

		//
		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.25F);
		pPhongMaterial->SetKd(0.75F);
		pPhongMaterial->SetCd2(1.0F, 1.0F, 1.0F);
		pPhongMaterial->SetKs(0.25F);
		pPhongMaterial->SetCs2(1.0F, 1.0F, 1.0F);
		pPhongMaterial->SetEXP(150);
		SimpleSphere *pSphere = new SimpleSphere(Vec3f(0, 0, 400), 20);
		MaterialObject *pMaterialSphere = new MaterialObject(pSphere, pPhongMaterial);
		mvecObjects.push_back(pMaterialSphere);

		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.3F);
		pAmbientLight->SetLightColor(Color3f(Color3f(0.2, 0.1, 0.1)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		DirectionLight *pDirectionLight0 = new DirectionLight;
		pDirectionLight0->SetScaleRadiance(2.0F);
		pDirectionLight0->SetDirection(Vec3f(1, 0, 0));
		pDirectionLight0->SetLightColor(Color3f(1.0F, 0.0f, 0.0f));
		mobjSceneLights.AddLight(pDirectionLight0);

		DirectionLight *pDirectionLight1 = new DirectionLight;
		pDirectionLight1->SetScaleRadiance(2.0F);
		pDirectionLight1->SetDirection(Vec3f(0, 1, 0));
		pDirectionLight1->SetLightColor(Color3f(0.0f, 1.0F, 0.0f));
		mobjSceneLights.AddLight(pDirectionLight1);

		DirectionLight *pDirectionLight2 = new DirectionLight;
		pDirectionLight2->SetScaleRadiance(2.0F);
		pDirectionLight2->SetDirection(Vec3f(0, 0, 1));
		pDirectionLight2->SetLightColor(Color3f(0.0f, 0.0f, 1.0F));
		mobjSceneLights.AddLight(pDirectionLight2);
		/*
			//
			PhongMaterial *pPhongMaterial = new PhongMaterial;
			pPhongMaterial->SetKa(0.4F);
			pPhongMaterial->SetKd(0.75F);
			pPhongMaterial->SetCd2(1.0F, 0.5F, 1.0F);
			pPhongMaterial->SetKs(0.25F);
			pPhongMaterial->SetCs2(1.0F, 1.0F, 1.0F);
			pPhongMaterial->SetEXP(4);
			SimpleBox *pBox = new SimpleBox(-20, 20, -30, 30, 380, 400);
			MaterialObject *pMaterialSphere = new MaterialObject(pBox, pPhongMaterial);
			mvecObjects.push_back(pMaterialSphere);

			//
			AmbientLight *pAmbientLight = new AmbientLight;
			pAmbientLight->SetScaleRadiance(0.0f);
			pAmbientLight->SetLightColor(Color3f(Color3f(0.0f, 0.0f, 0.0f)));
			mobjSceneLights.SetAmbientLight(pAmbientLight);

			PointLight *pPointLight3 = new PointLight;
			pPointLight3->SetScaleRadiance(2.0F);
			pPointLight3->SetPosition(Vec3f(40, 40, 530));
			pPointLight3->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
			mobjSceneLights.AddLight(pPointLight3);
		*/
	}

	void buildup_test_hard_shadow_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(0.2, 0.1, 0.1));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(320);
		mpCamera->Update();

		//
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, -20.0f, 500), Vec3f(0, 1, 0));

		Matte2Material *pMatteMaterial = new Matte2Material;
		pMatteMaterial->SetKa(0.25F);
		pMatteMaterial->SetKd(0.75F);
		pMatteMaterial->SetCd2(0.6F, 0.7F, 0.8F);

		MaterialObject *pPlaneObj = new MaterialObject(pPlane, pMatteMaterial);
		mvecObjects.push_back(pPlaneObj);

		//
		SimpleSphere *pShpere = new SimpleSphere(Vec3f(-20, 20, 420), 20);

		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.25);
		pPhongMaterial->SetKd(0.75);
		pPhongMaterial->SetCd2(0.9, 0.3, 0.0f);
		pPhongMaterial->SetKs(0.75F);
		pPhongMaterial->SetCs2(0.9, 0.3, 0.0f);
		pPhongMaterial->SetEXP(100);

		MaterialObject *pShpereObj = new MaterialObject(pShpere, pPhongMaterial);
		mvecObjects.push_back(pShpereObj);

		//
		PhongMaterial *pPhongMaterial2 = new PhongMaterial;
		pPhongMaterial2->SetKa(0.25F);
		pPhongMaterial2->SetKd(0.75F);
		pPhongMaterial2->SetCd2(1.0F, 1.0F, 1.0F);
		pPhongMaterial2->SetKs(0.75F);
		pPhongMaterial2->SetCs2(1.0F, 1.0F, 1.0F);
		pPhongMaterial2->SetEXP(100);
		SimpleBox *pBox = new SimpleBox(0, 20, -20, 0, 420, 450);
		MaterialObject *pMaterialBox = new MaterialObject(pBox, pPhongMaterial2);
		mvecObjects.push_back(pMaterialBox);

		//
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.3F);
		pAmbientLight->SetLightColor(Color3f(Color3f(0.2, 0.1, 0.1)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		// 		DirectionLight *pPointLight = new DirectionLight;
		// 		pPointLight->EnableShadow(false);
		// 		pPointLight->SetScaleRadiance(2.0F);
		// 		pPointLight->SetDirection(Vec3f(40, 60, 430));
		// 		pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		// 		mobjSceneLights.AddLight(pPointLight);

		PointLight *pPointLight2 = new PointLight;
		pPointLight2->SetScaleRadiance(2.0F);
		pPointLight2->SetPosition(Vec3f(40, 60, 430));
		pPointLight2->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		mobjSceneLights.AddLight(pPointLight2);
	}

	void buildup_test_ambient_occlusion_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1, 1, 1));

		mpCamera->SetEye(Vec3f(0, 0, 500));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(320);
		mpCamera->Update();

		//
		//
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, -20.0f, 500), Vec3f(0, 1, 0));

		Matte2Material *pMatteMaterial = new Matte2Material;
		pMatteMaterial->SetKa(0.75F);
		pMatteMaterial->SetKd(0.75F);
		pMatteMaterial->SetCd2(1, 1, 1);

		MaterialObject *pPlaneObj = new MaterialObject(pPlane, pMatteMaterial);
		mvecObjects.push_back(pPlaneObj);

		//
		SimpleSphere *pShpere = new SimpleSphere(Vec3f(0, 0, 420), 20);

		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.75);
		pPhongMaterial->SetKd(0.75);
		pPhongMaterial->SetCd2(0.9, 0.8, 0.0f);
		pPhongMaterial->SetKs(0.75F);
		pPhongMaterial->SetCs2(0.9, 0.8, 0.0f);
		pPhongMaterial->SetEXP(100);

		MaterialObject *pShpereObj = new MaterialObject(pShpere, pPhongMaterial);
		mvecObjects.push_back(pShpereObj);

		//
		AmbientOcclusionLight *pAmbientOccLight = new AmbientOcclusionLight;
		pAmbientOccLight->SetScaleRadiance(1);
		pAmbientOccLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
		pAmbientOccLight->SetFraction(0.3F);

		MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(100);
		pAmbientOccLight->SetSampler(pMJSampler, 2.0F);

		mobjSceneLights.SetAmbientLight(pAmbientOccLight);

		//
	//	DirectionLight *pDirectionLight = new DirectionLight;
	//	pDirectionLight->EnableShadow(false);
	//	pDirectionLight->SetScaleRadiance(2.0F);
	//	pDirectionLight->SetDirection(Vec3f(1, 1, 0));
	//	pDirectionLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
	//	mobjSceneLights.AddLight(pDirectionLight);
	}

	void buildup_test_area_light_shading_scene()
	{
		/*		//
				mpBackground = new ConstantTexture(Color3f(0.2, 0.1, 0.2));

				mpCamera->SetEye(Vec3f(-20, 10, 500));
				mpCamera->SetLookAt(Vec3f(0, 0, 0));
				mpCamera->SetUpVector(Vec3f(0, 1, 0));
				((PerspectiveCamera *)mpCamera)->SetDistance(320);
				mpCamera->Update();

				//
				// Add a play ground(plane)
				SimplePlane *pGroundPlane = new SimplePlane(Vec3f(0, -10, 0), Vec3f(0, 1, 0));

				Matte2Material *pGroundMaterial = new Matte2Material;
				pGroundMaterial->SetKa(0.25F);
				pGroundMaterial->SetKd(0.75F);
				pGroundMaterial->SetCd2(1, 1, 1);

				MaterialObject *pPlaneMaterialObj = new MaterialObject(pGroundPlane, pGroundMaterial);

				mvecObjects.push_back(pPlaneMaterialObj);

				// Add tres boxes
				SimpleBox *pBoxA = new SimpleBox(-5, 5, -10, 10, 455, 465);
				SimpleBox *pBoxB = new SimpleBox(-20, -10, -10, 10, 445, 455);
				SimpleBox *pBoxC = new SimpleBox(10, 20, -10, 10, 445, 455);

				Matte2Material *pBoxMaterial = new Matte2Material;
				pBoxMaterial->SetKa(0.25F);
				pBoxMaterial->SetKd(0.75F);
				pBoxMaterial->SetCd2(1, 1, 1);

				MaterialObject *pBoxAObject = new MaterialObject(pBoxA, pBoxMaterial, true, false);
				MaterialObject *pBoxBObject = new MaterialObject(pBoxB, pBoxMaterial, true, false);
				MaterialObject *pBoxCObject = new MaterialObject(pBoxC, pBoxMaterial);

				mvecObjects.push_back(pBoxAObject);
				mvecObjects.push_back(pBoxBObject);
				mvecObjects.push_back(pBoxCObject);

				// Set up the ambient light
				AmbientLight *pAmbientLight = new AmbientLight;
				pAmbientLight->SetScaleRadiance(1.0F);
				pAmbientLight->SetLightColor2(0.8, 0.9, 0.0f);
				mobjSceneLights.SetAmbientLight(pAmbientLight);

				// Set up const area light
				// First, add the object that represents shape of the light
			//	SimpleRectangle *pRectLight = new SimpleRectangle(Vec3f(-30, 15, 450), Vec3f(1, -1, -1), Vec3f(20, 0, 0), Vec3f(0, 0, 20));
				YZRect *pRectLight = new YZRect(Vec3f(-30, 0, 410), false, 50, 50);

				Emissive2Material *pLightEmissiveMaterial = new Emissive2Material;
				pLightEmissiveMaterial->SetRadiance(1.0F);
				pLightEmissiveMaterial->SetEmissiveColor(Color3f(1.0F, 1.0F, 1.0F));

				MaterialObject *pRectLightMaterialObj = new MaterialObject(pRectLight, pLightEmissiveMaterial);

				mvecObjects.push_back(pRectLightMaterialObj);

				//
				AreaConstLight *pAreaConstLight = new AreaConstLight;
				pAreaConstLight->SetLightShapeObject(pRectLight);
				pAreaConstLight->SetScaleRadiance(1.0F);
				pAreaConstLight->SetLightColor2(1.0F, 1.0F, 1.0F);

				mobjSceneLights.AddLight(pAreaConstLight);
		*/

		//
		mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

		//		mpCamera->SetEye(Vec3f(-2, 1, 2));
		mpCamera->SetEye(Vec3f(-20, 10, 20));
		//		mpCamera->SetLookAt(Vec3f(0, 0.2, 0));
		mpCamera->SetLookAt(Vec3f(0, 2, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(1080);
		mpCamera->Update();

		//
		// Add a play ground(plane)
		SimplePlane *pGroundPlane = new SimplePlane(Vec3f(0, 0, 0), Vec3f(0, 1, 0));

		Matte2Material *pGroundMaterial = new Matte2Material;
		pGroundMaterial->SetKa(0.1F);
		pGroundMaterial->SetKd(0.9F);
		pGroundMaterial->SetCd2(1, 1, 1);

		MaterialObject *pPlaneMaterialObj = new MaterialObject(pGroundPlane, pGroundMaterial);

		mvecObjects.push_back(pPlaneMaterialObj);

		// Add tres boxes
		float box_width = 1.0; 		// x dimension
		float box_depth = 1.0; 		// z dimension
		float box_height = 4.5; 	// y dimension
		float gap = 3.0;

		SimpleBox *pBoxA = new SimpleBox(-1.5 * gap - 2.0 * box_width, -1.5 * gap - box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxB = new SimpleBox(-0.5 * gap - box_width, -0.5 * gap,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxC = new SimpleBox(0.5 * gap, 0.5 * gap + box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxD = new SimpleBox(1.5 * gap + box_width, 1.5 * gap + 2.0 * box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		Matte2Material *pBoxMaterial = new Matte2Material;
		pBoxMaterial->SetKa(0.25F);
		pBoxMaterial->SetKd(0.75F);
		pBoxMaterial->SetCd2(0.4, 0.7, 0.4);

		MaterialObject *pBoxAObject = new MaterialObject(pBoxA, pBoxMaterial, true, false);
		MaterialObject *pBoxBObject = new MaterialObject(pBoxB, pBoxMaterial, true, false);
		MaterialObject *pBoxCObject = new MaterialObject(pBoxC, pBoxMaterial, true, false);
		MaterialObject *pBoxDObject = new MaterialObject(pBoxD, pBoxMaterial);

		mvecObjects.push_back(pBoxAObject);
		mvecObjects.push_back(pBoxBObject);
		mvecObjects.push_back(pBoxCObject);
		mvecObjects.push_back(pBoxDObject);

		// Set up the ambient light
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(1.0F);
		pAmbientLight->SetLightColor2(0.8, 0.9, 0.0f);
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		// Set up const area light
		float width = 4.0;
		float height = 4.0;
		Vec3f center(0.0f, 7.0, -7.0);
		Vec3f p0(-0.5 * width, center.Y() - 0.5 * height, center.Z());
		Vec3f a(width, 0.0f, 0.0f);
		Vec3f b(0.0f, height, 0.0f);
		// First, add the object that represents shape of the light
	//	SimpleRectangle *pRectLight = new SimpleRectangle(p0, Vec3f(0, 0, 1), a, b);
		XYRect *pRectLight = new XYRect(p0, false, width, height);

		Emissive2Material *pLightEmissiveMaterial = new Emissive2Material;
		pLightEmissiveMaterial->SetRadiance(40.0f);
		pLightEmissiveMaterial->SetEmissiveColor(Color3f(1.0F, 1.0F, 1.0F));

		MaterialObject *pRectLightMaterialObj = new MaterialObject(pRectLight, pLightEmissiveMaterial);

		mvecObjects.push_back(pRectLightMaterialObj);

		//
		AreaConstLight *pAreaConstLight = new AreaConstLight;
		pAreaConstLight->SetLightShapeObject(pRectLight);
		pAreaConstLight->SetScaleRadiance(40.0f);
		pAreaConstLight->SetLightColor2(1.0F, 1.0F, 1.0F);

		mobjSceneLights.AddLight(pAreaConstLight);

	}

	void buildup_test_texutre_area_light_shading_scene()
	{
		mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

		mpCamera->SetEye(Vec3f(-20, 10, 30));
		mpCamera->SetLookAt(Vec3f(0, 2, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(1080);
		mpCamera->Update();

		//
		// Add a play ground(plane)
		SimplePlane *pGroundPlane = new SimplePlane(Vec3f(0, 0, 0), Vec3f(0, 1, 0));

		Matte2Material *pGroundMaterial = new Matte2Material;
		pGroundMaterial->SetKa(0.1F);
		pGroundMaterial->SetKd(0.9F);
		pGroundMaterial->SetCd2(1, 1, 1);

		MaterialObject *pPlaneMaterialObj = new MaterialObject(pGroundPlane, pGroundMaterial);

		mvecObjects.push_back(pPlaneMaterialObj);

		// Add tres boxes
		float box_width = 1.0; 		// x dimension
		float box_depth = 1.0; 		// z dimension
		float box_height = 4.5; 		// y dimension
		float gap = 3.0;

		SimpleBox *pBoxA = new SimpleBox(-1.5 * gap - 2.0 * box_width, -1.5 * gap - box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxB = new SimpleBox(-0.5 * gap - box_width, -0.5 * gap,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxC = new SimpleBox(0.5 * gap, 0.5 * gap + box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		SimpleBox *pBoxD = new SimpleBox(1.5 * gap + box_width, 1.5 * gap + 2.0 * box_width,
			0.0f, box_height, -0.5 * box_depth, 0.5 * box_depth);

		Matte2Material *pBoxMaterial = new Matte2Material;
		pBoxMaterial->SetKa(0.25F);
		pBoxMaterial->SetKd(0.75F);
		pBoxMaterial->SetCd2(0.4, 0.7, 0.4);

		MaterialObject *pBoxAObject = new MaterialObject(pBoxA, pBoxMaterial, true, false);
		MaterialObject *pBoxBObject = new MaterialObject(pBoxB, pBoxMaterial, true, false);
		MaterialObject *pBoxCObject = new MaterialObject(pBoxC, pBoxMaterial, true, false);
		MaterialObject *pBoxDObject = new MaterialObject(pBoxD, pBoxMaterial);

		mvecObjects.push_back(pBoxAObject);
		mvecObjects.push_back(pBoxBObject);
		mvecObjects.push_back(pBoxCObject);
		mvecObjects.push_back(pBoxDObject);

		// Set up the ambient light
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(1.0F);
		pAmbientLight->SetLightColor2(0.8, 0.9, 0.0f);
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		// Set up const area light
		float width = 4.0;
		float height = 4.0;
		Vec3f center(0.0f, 7.0, -7.0);
		Vec3f p0(-0.5 * width, center.Y() - 0.5 * height, center.Z());
		Vec3f a(width, 0.0f, 0.0f);
		Vec3f b(0.0f, height, 0.0f);
		// First, add the object that represents shape of the light
		//	SimpleRectangle *pRectLight = new SimpleRectangle(p0, Vec3f(0, 0, 1), a, b);
		XYRect *pRectLight = new XYRect(p0, false, width, height);

		//
		PlaneCheckerTexture *texLightChecker = new PlaneCheckerTexture(
			EPlaneOritention::PALNE_XOY,
			1, 0.0f, Color3f(1, 1, 1), Color3f(0, 0, 0), Color3f(1, 1, 1));

		EmissiveTextureMaterial *pLightEmissiveMaterial = new EmissiveTextureMaterial;
		pLightEmissiveMaterial->SetRadiance(40.0f);
		pLightEmissiveMaterial->SetEmissiveColor(texLightChecker);

		MaterialObject *pRectLightMaterialObj = new MaterialObject(pRectLight, pLightEmissiveMaterial);

		mvecObjects.push_back(pRectLightMaterialObj);

		//
		AreaTextureLight *pAreaTextureLight = new AreaTextureLight;
		pAreaTextureLight->SetLightShapeObject(pRectLight);
		pAreaTextureLight->SetScaleRadiance(40.0f);
		pAreaTextureLight->SetLightColor(texLightChecker);

		mobjSceneLights.AddLight(pAreaTextureLight);
	}

	void build_up_test_ply_model_scene()
	{
		//
		mpBackground = new ConstantTexture(Color3f(1, 1, 1));

		//
		mpCamera->SetEye(Vec3f(-0.3, 0.1, 0.7)); // the perspective for dragon model
	//	mpCamera->SetEye(Vec3f(3, 0, 30));		// For hand model
	//	mpCamera->SetLookAt(Vec3f(3, 0.1, 0));	// For hand model
		mpCamera->SetLookAt(Vec3f(0, 0.1, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(1080);
		mpCamera->Update();

		//
		Matte2Material *pMatteMaterial = new Matte2Material;
		pMatteMaterial->SetKa(0.25F);
		pMatteMaterial->SetKd(0.75F);
		pMatteMaterial->SetCd2(1, 1, 1);

		ReflectiveMaterial *pReflectiveMaterial = new ReflectiveMaterial;
		pReflectiveMaterial->SetKa(0.2);
		pReflectiveMaterial->SetKd(0.75);
		pReflectiveMaterial->SetCd3(0.5);
		pReflectiveMaterial->SetKs(0.0f);
		//	pReflectiveMaterial->SetEXP(20);
		pReflectiveMaterial->SetKr(0.5);
		pReflectiveMaterial->SetCr2(0.8, 1.0, 0.8);

		//
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, 0.052f, 0), Vec3f(0, 1, 0));
		MaterialObject *pPlaneMaterialObj = new MaterialObject(pPlane, pMatteMaterial, true, true);

		mvecObjects.push_back(pPlaneMaterialObj);

		//
		RegularGridMeshObject *pPLY_Horse2K = new RegularGridMeshObject;
		pPLY_Horse2K->SetMaterial(pMatteMaterial);
		//	pPLY_Horse2K->ReverseMeshNormals();
		pPLY_Horse2K->EnableAcceleration(true);
		pPLY_Horse2K->LoadFromPLYFile("D:/workspace/计算机图形学/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/dragon.ply", EMeshType::SMOOTH_SHADING);
		pPLY_Horse2K->BuildupAccelerationStructure();

		mvecObjects.push_back(pPLY_Horse2K);

		//
		AmbientOcclusionLight *pAmbientOccLight = new AmbientOcclusionLight;
		pAmbientOccLight->SetScaleRadiance(1.0F);
		pAmbientOccLight->SetLightColor2(0.2, 0.3, 0.2);
		pAmbientOccLight->SetFraction(0.3F);

		MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(100);
		pAmbientOccLight->SetSampler(pMJSampler, 2);

		mobjSceneLights.SetAmbientLight(pAmbientOccLight);

		//
		PointLight *pPointLight2 = new PointLight;
		//	pPointLight2->EnableShadow(false);
		pPointLight2->SetScaleRadiance(2.0F);
		pPointLight2->SetPosition(Vec3f(4, 100, 2));
		pPointLight2->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		mobjSceneLights.AddLight(pPointLight2);

	}

	void buildup_test_mesh_triangle_model_scene()
	{
		// Backgound setting
		mpBackground = new ConstantTexture(Color3f(1, 1, 1));

		// Camera setting
		mpCamera->SetEye(Vec3f(0, 0, 2));
		//	mpCamera->SetEye(Vec3f(-6, 10, 131));
		//	mpCamera->SetEye(Vec3f(0, 0, 200));
		mpCamera->SetLookAt(Vec3f(-0.009f, 0.11, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(500);
		mpCamera->Update();

		// Material setting
		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.2);
		pPhongMaterial->SetKd(0.95);
		pPhongMaterial->SetCd2(1, 1, 1);   // orange
		pPhongMaterial->SetKs(0.5);
		pPhongMaterial->SetEXP(300.0f);
		pPhongMaterial->SetCs2(1, 1, 1);   // orange

// 		ReflectiveMaterial *pReflectiveMaterial = new ReflectiveMaterial;
// 		pReflectiveMaterial->SetKa(0.2);
// 		pReflectiveMaterial->SetKd(0.75);
// 		pReflectiveMaterial->SetCd3(0.5);
// 		pReflectiveMaterial->SetKs(0.0f);
// 	//	pReflectiveMaterial->SetEXP(20);
// 		pReflectiveMaterial->SetKr(0.5);
// 		pReflectiveMaterial->SetCr2(0.8, 1.0, 0.8);

		// Material Objects setting6 tgrg  
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, -0.30, 0), Vec3f(0, 1, 0));
		//	SimplePlane *pPlane = new SimplePlane(Vec3f(0, 0.55, 0), Vec3f(0, 1, 0));
		MaterialObject *pPlaneMaterialObj = new MaterialObject(pPlane, pPhongMaterial);
		mvecObjects.push_back(pPlaneMaterialObj);

		RegularGridMeshObject *pPLY_TriangleMesh = new RegularGridMeshObject;
		pPLY_TriangleMesh->SetMaterial(pPhongMaterial);
		//	pPLY_TriangleMesh->ReverseMeshNormals();
		pPLY_TriangleMesh->EnableAcceleration(true);
		pPLY_TriangleMesh->LoadFromPLYFile("D:/workspace/计算机图形学/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/Horse97K.ply", EMeshType::SMOOTH_SHADING);
		pPLY_TriangleMesh->BuildupAccelerationStructure();

		mvecObjects.push_back(pPLY_TriangleMesh);

		// Samplers setting
		MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(1);

		// Lighting setting
		AmbientOcclusionLight *pAmbientOcclusion = new AmbientOcclusionLight;
		pAmbientOcclusion->SetScaleRadiance(1.0F);
		pAmbientOcclusion->SetLightColor2(1.0F, 1.0F, 1.0F);
		pAmbientOcclusion->SetFraction(0.25);
		pAmbientOcclusion->SetSampler(pMJSampler, 2.0F);
		mobjSceneLights.SetAmbientLight(pAmbientOcclusion);

		// 		DirectionLight *pDirectionLight = new DirectionLight;
		// 		pDirectionLight->SetScaleRadiance(1.0F);
		// 		pDirectionLight->SetDirection(Vec3f(0.5, 1, 0.75));
		// 		pDirectionLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		// 		mobjSceneLights.AddLight(pDirectionLight);

				// ========= Test the spot light ==========
		SpotLight *pSpotLight = new SpotLight;
		pSpotLight->SetLightPos(Vec3f(2.6, 5, 3.7));
		pSpotLight->SetLightDirection(Vec3f(-0.5, -1, -0.75));
		pSpotLight->SetUmbra(5);
		pSpotLight->SetPenumbra(10);
		pSpotLight->SetLightIntensity(100.0f);
		mobjSceneLights.AddLight(pSpotLight);
		// ========================================

		// ========= Test the projective texture spot light ==========
// 		PlaneCheckerTexture *pTex = new PlaneCheckerTexture(EPlaneOritention::PLANE_XOZ,
// 			0.1, 0.0f, Color3f(1, 1, 1), Color3f(0, 0, 0), Color3f(1, 1, 1));
// 
// 		ProjectiveTextureSpotLight *pProjectiveTextureSpotLight = new ProjectiveTextureSpotLight;
// 		pProjectiveTextureSpotLight->SetLightPos(Vec3f(2.6, 5, 3.7));
// 		pProjectiveTextureSpotLight->SetLightDirection(Vec3f(-0.5, -1, -0.75));
// 		pProjectiveTextureSpotLight->SetLightViewAngle(10.0f);
// 		pProjectiveTextureSpotLight->SetLightIntensity(5.0);
// 		pProjectiveTextureSpotLight->SetProjectiveTexture(pTex);
// 		mobjSceneLights.AddLight(pProjectiveTextureSpotLight);
		// ===========================================================

		// ========= Test the projective texture area light ==========
// 		PlaneCheckerTexture *pTex = new PlaneCheckerTexture(EPlaneOritention::PLANE_XOZ,
// 			0.2, 0.0f, Color3f(1, 1, 1), Color3f(0, 0, 0), Color3f(1, 1, 1));

// 		ImageTexture *pImage = new ImageTexture;
// 		bool bLoaded = pImage->LoadJPGFromFile("D:/workspace/计算机图形学/Ray Tracing/f16-9c.jpg");
// 		
// 		if (bLoaded)
// 		{
// 			ProjectiveTextureAreaLight *pProjectiveTextureAreaLight = new ProjectiveTextureAreaLight;
// 			pProjectiveTextureAreaLight->SetLightPos(Vec3f(2.6, 5, 3.7));
// 			pProjectiveTextureAreaLight->SetLightDirection(Vec3f(-0.5, -1, -3.75));
// 			pProjectiveTextureAreaLight->SetHorizontalViewAngle(90.0f);
// 			pProjectiveTextureAreaLight->SetVerticalViewAngle(90.0f);
// 			pProjectiveTextureAreaLight->SetNearDistance(1.0);
// 			pProjectiveTextureAreaLight->SetFarDistance(1000.0f);
// 			pProjectiveTextureAreaLight->SetLightIntensity(5.0);
// 			pProjectiveTextureAreaLight->SetProjectiveTexture(pImage);
// 			mobjSceneLights.AddLight(pProjectiveTextureAreaLight);
// 		}
		// ===========================================================

// 		PointLight *pPointLight3 = new PointLight;
// 		pPointLight3->SetScaleRadiance(1.0F);
// 		pPointLight3->SetPosition(Vec3f(0.5, 1, 0.75));
// 		pPointLight3->SetLightColor(Color3f(1.0F, 0.0f, 0.0f));
// 		mobjSceneLights.AddLight(pPointLight3);

	}

	void buildup_test_motion_blur_scene()
	{
		mpBackground = new SimpleSkyTexture;

		mpCamera->SetExpTime(0, 2);
		mpCamera->SetEye(Vec3f(0, 0, 200.0f));
		mpCamera->SetLookAt(Vec3f(0, 0, 0.0f));
		mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
		mpCamera->SetDistance(500.0f);
		mpCamera->Update();

		//		SimpleSphere *pTestShpere = new SimpleSphere(Vec3f(0, 0, 0), 20.0f);

		// 		NormalMaterial *pNormMaterial = new NormalMaterial;
		// 		MaterialObject *pMatShpere = new MaterialObject(pTestShpere, pNormMaterial);

		MovingShpere *pMovingShpere = new MovingShpere(Vec3f(20, 0, 0), 20.0f, Vec3f(15, 0, 0), 0, 0.5);
		pMovingShpere->SetColor(Color3f(0, 0, 1));
		mvecObjects.push_back(pMovingShpere);

		MovingShpere *pMovingShpere2 = new MovingShpere(Vec3f(-40, 0, 0), 20.0f, Vec3f(-40, 5, 0), 0, 1);
		pMovingShpere2->SetColor(Color3f(1, 0, 0));
		mvecObjects.push_back(pMovingShpere2);

		//		mvecObjects.push_back(pMatShpere);

	}

	void buildup_test_other_geometry_objects()
	{
		mpBackground = new ConstantTexture(Color3f(0.4, 0.6, 0.8));

		mpCamera->SetExpTime(0, 2);
	//	mpCamera->SetEye(Vec3f(5, 25, 20));		// For torus
	//	mpCamera->SetEye(Vec3f(235, 25, 600));		// For teardrop, parallel rays may cause problem for solving quratic in y direction
		mpCamera->SetEye(Vec3f(10, 5, 6));
		mpCamera->SetLookAt(Vec3f(-5, 0, 0));
		mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
	//	mpCamera->SetDistance(1500.0f);			// For torus
		mpCamera->SetDistance(500.0f);
		mpCamera->Update();

//		SimpleCylinder *pCylinder = new SimpleCylinder(Vec3f(0, 0, 0), 20, 0, 60);
// 		MaterialObject *pMatNormCylinder = new MaterialObject(pCylinder, new NormalMaterial);
// 		mvecObjects.push_back(pMatNormCylinder);

// 		SimpleCone *pCone = new SimpleCone(Vec3f(20.0f, 0.0f, 0.0f), 20, 0, 60);
// 		MaterialObject *pMatNormalCone = new MaterialObject(pCone, new NormalMaterial);
// 		mvecObjects.push_back(pMatNormalCone);

//		SimpleTorus *pTorus = new SimpleTorus(Vec3f(0,0,0), 2, 0.5);
// 		MaterialObject *pMatNormalTorus = new MaterialObject(pTorus, new NormalMaterial);
//		mvecObjects.push_back(pMatNormalTorus);

//		SimpleEllipsoid *pEllipsoid = new SimpleEllipsoid;
//		MaterialObject *pMatNormalEllipsoid = new MaterialObject(pEllipsoid, new NormalMaterial);
//		mvecObjects.push_back(pMatNormalEllipsoid);

//		SimpleQuadratic *pQuadratic = new SimpleQuadratic(Vec3f(0,0,0), 30, 20, 10, -1, 0, 30);
//		MaterialObject *pMatNormalQuadratic = new MaterialObject(pQuadratic, new NormalMaterial);
//		mvecObjects.push_back(pMatNormalQuadratic);

// 		SimpleParaboloid *pParaboloid = new SimpleParaboloid;
// 		MaterialObject *pMatParaboloid = new MaterialObject(pParaboloid, new NormalMaterial);
//		mvecObjects.push_back(pMatParaboloid);

//   		SimpleHyperboloid *pHyperboloid = new SimpleHyperboloid(Vec3f(0.0f, 0.0f, 0.0f), 70, 52, 50, 50);
//   		MaterialObject *pMatParaHyperboloid = new MaterialObject(pHyperboloid, new NormalMaterial);
//   		mvecObjects.push_back(pMatParaHyperboloid);

// 		Teardrop *pTeardrop = new Teardrop(Vec3f(-13.0f, 13.0f, 0), 2, 2, 2);
// 		MaterialObject *pMatParaTeardrop = new MaterialObject(pTeardrop, new NormalMaterial);
// 		mvecObjects.push_back(pMatParaTeardrop);

// 		SimpleShpere2 *pShere2 = new SimpleShpere2;
// 		mvecObjects.push_back(pShere2);

		JoinBlendCylinder *pJoinBlendCylinder = new JoinBlendCylinder;
		MaterialObject *pMatJoinBlendCylinder = new MaterialObject(pJoinBlendCylinder, new NormalMaterial);
		mvecObjects.push_back(pMatJoinBlendCylinder);

// 		int num = 0;
// 		float roots[4] = { 0.0f };
// 		RTMath::SolveQuaraticEquation(1.0, 0.0, 0.0, 0.0, -16, num, roots);
// 		char szBuf[32] = { 0 };
// 		sprintf_s(szBuf, 32, "num: %d, %.2f %.2f\n", num, roots[0], roots[1]);
// 		g_Console.Write(szBuf);

	}

private:
	float mnPointLightX;
	float mnPointLightY;
	float mnPointLightZ;
};
