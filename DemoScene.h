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

	virtual ~CDemoScene() {

	}

#ifdef PLATFORM_WIN
public:
	inline void SetSurface(CDC *pDC) { Scene::SetDrawingSurface(pDC); }
	inline void SetConsole(OutputConsole *pConsle) { Scene::SetConsoleHandle(pConsle); }
#endif // PLATFORM_WIN

public:
	virtual void Setup(int w = 400, int h = 400)
	{
	    Scene::Setup(w, h);

//		mpViewler = new RegularSampler(100);
		mpViewSampler = new MultiJitteredSampler(16);
//		mpViewSampler->SetSetCount(1);

        //  mpRayTracer = new RayCastTracer;
        //	mpRayTracer = new AreaLightTracer;
		//	mpRayTracer = new WhittedTracer;
	 	//    mpRayTracer = new GlobalTracer;
            mpRayTracer = new PathTracer;
        //    mpRayTracer = new PathTracer_PBRS;

		mpViewPlane = new ViewPlane(w, h);
		mpCamera = new PerspectiveCamera;
		//mpCamera = new PanoramicCamera;

		//	MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(100);
		//	mpCamera = new ThinLenCamera;
		//	((ThinLenCamera *)mpCamera)->SetSampler(pMJSampler);
		//	((ThinLenCamera *)mpCamera)->SetFocusPlaneLen(35.0F);
		//	((ThinLenCamera *)mpCamera)->SetLenDiskRadius(0.5);

		//	mpCamera = new FishEyeCamera;
		//	((FishEyeCamera *)mpCamera)->SetPsiPhi(120);

		mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
    //    mpCamera->EnableZoomFactor(true);
    //    mpCamera->SetZoomFactor(0.05f);
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
        //    BuildupMultiObjIntersectionTestScene();
        //  buildup_test_scene_of_phong_shading_model_with_lights();
        //    buildup_CornellBox();
		//	buildup_test_area_light_shading_scene();
		//	buildup_test_texutre_area_light_shading_scene();
		//	buildup_test_retangle_texture_scene();
        //    buildup_sphere_texture();
        //    build_up_test_ply_model_scene();
		//	buildup_checker_texture_test_scene();
		//	buildup_test_mesh_triangle_model_scene();
		//  buildup_test_motion_blur_scene();
		//  buildup_test_other_geometry_objects();
        //  buildup_test_geometry_tessellation_shpere_scene();
        //    buildup_test_part_geometric_object_scene();
        //    buildup_test_light_prob_env_test_scene();
        //    buildup_test_noise_based_texture_scene();
        //     buildup_test_texture_mapping_scene();
        //     buildup_test_caustic_by_global_tracer_scene();
        //     buildup_test_caustic_by_path_tracer_scene();
        //    buildup_test_dielectric_material_scene();
        //    buildup_test_dielectric_material_scene2();
        //buildup_test_glossy_reflective_scene();
        //    buildup_CornellBox_scene2();
		//	buildup_test_geometric_object_plus();
		//	 buildup_test_microfacet_brdf_scene();
        //    buildup_test_microfacet_brdf_scene2();
        //    buildup_test_obj_model_scene();
		//    buildup_test_inter_grid_shadow_scne();
        //  builup_shelf_test_scene();
            buildup_random_sphere_scene();

	}

private:
	void buildup_CornellBox()
	{
        //
        //
        mpBackground = new ConstantTexture(Color3f(0.9F, 0.9F, 0.9F));

        mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
        mpCamera->SetEye(Vec3f(1, 41, 370));
        mpCamera->SetLookAt(Vec3f(0, 40, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(500);
        mpCamera->Update();

        //
        // back wall
        MaterialObject *pBackWall = new MaterialObject(new XYRect(Vec3f(-40, 0, 200), false, 80, 80),
                                                       new MatteMaterial(Color3f(0.73, 0.73, 0.73)));

        // left wall
        MaterialObject *pLeftWall = new MaterialObject(new YZRect(Vec3f(-40, 0, 200), false, 80, 80),
                                                       new MatteMaterial(Color3f(0.12, 0.45, 0.15)));

        // right wall
        MaterialObject *pRightWall = new MaterialObject(new YZRect(Vec3f(40, 0, 200), true, 80, 80),
                                                        new MatteMaterial(Color3f(0.65, 0.05, 0.05)));

        // cell wall
        MaterialObject *pCellWall = new MaterialObject(new XZRect(Vec3f(-40, 80, 200), true, 80, 80),
                                                       new MatteMaterial(Color3f(0.73, 0.73, 0.73)));

        // floor wall
        MaterialObject *pFloorWall = new MaterialObject(new XZRect(Vec3f(-40, 0, 200), false, 80, 80),
                                                        new MatteMaterial(Color3f(0.73, 0.73, 0.73)));

        //
        mvecObjects.push_back(pBackWall);
        mvecObjects.push_back(pLeftWall);
        mvecObjects.push_back(pRightWall);
        mvecObjects.push_back(pCellWall);
        mvecObjects.push_back(pFloorWall);

        //
        // lighting set-up to light the scene up
        AreaLightObject *pCellLight = new AreaLightObject(new XZRect(Vec3f(-10, 79.9, 228), true, 20, 20),
                                                          new ConstantTexture(Color3f(25, 25, 25)),
                                                          new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f)));
        pCellLight->TurnON();

        mvecObjects.push_back(pCellLight);

        //
        // Add a glass material ball in the middle of the box.
        MaterialObject *pGlassBall = new MaterialObject(new SimpleSphere(Vec3f(14, 15, 245), 10),
                                                        new GlassMaterial(1.5, Color3f(1,1,1), Color3f(1, 1, 1)));
//        MaterialObject *pMetalBall = new MaterialObject(new SimpleSphere(Vec3f(10, 50, 240), 5),
//                                                        new MirrorMaterial(Color3f(0.7,0.7,0.8), 0.6));
//        mvecObjects.push_back(pMetalBall);

        mvecObjects.push_back(pGlassBall);

        //
//		ConstantMedium *pVolumeShpere = new ConstantMedium(pGlassBall, 0.05f, new IsotropicMaterial(new ConstantTexture(Color3f(0.2F,0.4F,0.9F))));
//		mvecObjects.push_back(pVolumeShpere);

        SimpleBox *box1 = new SimpleBox(0, 20, 0, 55, 0, 20);
        MaterialObject *box_mat1 = new MaterialObject(box1, new MatteMaterial(Color3f(0.5,0.5,0.5)));
        Instance *inst_box1 = new Instance(box_mat1);
        inst_box1->RotateY(60);
        inst_box1->Translate(-28, 0, 240);
        mvecObjects.push_back(inst_box1);

//        SimpleBox *box2 = new SimpleBox(0, 20, 0, 40, 0, 20);
//        MaterialObject *box_mat2 = new MaterialObject(box2, new MatteMaterial(Color3f(0.5,0.5,0.5)));
//        Instance *inst_box2 = new Instance(box_mat2);
////		inst_box2->RotateY(-30);
//        inst_box2->Translate(10, 0, 230);
//        mvecObjects.push_back(inst_box2);

//        RegularGridMeshObject *model = new RegularGridMeshObject;
//        model->SetMaterial(new GlassMaterial(1.5, WHITE,WHITE));
////		model->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
//        model->EnableAcceleration(true);
//        model->LoadFromFile("/Users/wuwenxi/code/git-project/monte-carlo-ray-tracer-master/scenes/data/bunny.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ);
//        model->BuildupAccelerationStructure();
//        Instance *inst_model = new Instance(model);
//        inst_model->Scale(30, 30, 30);
//        inst_model->RotateY(40);
//        inst_model->Translate(2, 10, 260);
//        mvecObjects.push_back(inst_model);

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
		bool bLoaded = pImageTex->LoadJPGFromFile("/Users/wuwenxi/workspace/CG/RayTracing/laplataraytracer/cmake-build-release-1/sky_img.jpg");
		if (bLoaded)
		{
			ImageTextureMapping *pShpereMapping = new ShpereicalTextureMapping;
			pImageTex->SetMappingMethod(pShpereMapping);
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
		bool bLoaded = pImage->LoadJPGFromFile("D:/workspace/�����ͼ��ѧ/Ray Tracing/f16-9c.jpg");
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

		mpCamera->SetEye(Vec3f(0, 0, 20));
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

        //mpCamera->SetEye(Vec3f(-2, 1, 2));
        mpCamera->SetEye(Vec3f(-20, 10, 20));
        //mpCamera->SetLookAt(Vec3f(0, 0.2, 0));
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
        //XYRect *pRectLight = new XYRect(p0, false, width, height);
		SimpleDisk *pDiskLight = new SimpleDisk(p0, Vec3f(0, 0, 1), 2);

        Emissive2Material *pLightEmissiveMaterial = new Emissive2Material;
        pLightEmissiveMaterial->SetRadiance(40.0f);
        pLightEmissiveMaterial->SetEmissiveColor(Color3f(1.0F, 1.0F, 1.0F));

        //MaterialObject *pRectLightMaterialObj = new MaterialObject(pRectLight, pLightEmissiveMaterial);
		MaterialObject *pRectLightMaterialObj = new MaterialObject(pDiskLight, pLightEmissiveMaterial);

        mvecObjects.push_back(pRectLightMaterialObj);

        //
        AreaConstLight *pAreaConstLight = new AreaConstLight;
        //pAreaConstLight->SetLightShapeObject(pRectLight);
		pAreaConstLight->SetLightShapeObject(pDiskLight);
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
		mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

		//
		mpCamera->SetEye(Vec3f(-0.3, 0.1, 2.7)); // the perspective for dragon model
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
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, -0.022f, 0), Vec3f(0, 1, 0));
		MaterialObject *pPlaneMaterialObj = new MaterialObject(pPlane, pMatteMaterial, true, true);

		mvecObjects.push_back(pPlaneMaterialObj);

		//
        DielectricMaterial *transparentMat = new DielectricMaterial;
        transparentMat->SetKa(0.2);
        transparentMat->SetKd(0);
        transparentMat->SetCd2(1, 1, 1);   // orange
        transparentMat->SetKs(0);
        transparentMat->SetEXP(2000.0f);
        transparentMat->SetCs2(1, 1, 1);   // orange
        transparentMat->SetEtaIn(1.6);
        transparentMat->SetEtaOut(1.0);
		transparentMat->SetColorFilter_In(1,1,0);
		transparentMat->SetColorFilter_Out(1,1,1);

		//
		RegularGridMeshObject *pPLY_Horse2K = new RegularGridMeshObject;
		pPLY_Horse2K->SetMaterial(pMatteMaterial);
        //pPLY_Horse2K->ReverseMeshNormals();
		//	pPLY_Horse2K->ReverseMeshNormals();
		pPLY_Horse2K->EnableAcceleration(true);
        //pPLY_Horse2K->LoadFromFile("/Users/wuwenxi/workspace/CG/Models_Materials/bunny/reconstruction/bun_zipper.ply", EMeshType::SMOOTH_SHADING, EModelType::MODEL_PLY);
		pPLY_Horse2K->LoadFromFile("/Users/wuwenxi/workspace/CG/Models_Materials/material-testball/models/Mesh000.ply", EMeshType::SMOOTH_SHADING, EModelType::MODEL_PLY, true);
		//pPLY_Horse2K->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/Horse97K.ply", EMeshType::SMOOTH_SHADING);
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
        PointLight *pPointLight1 = new PointLight;
        pPointLight1->SetScaleRadiance(4.5F);
        pPointLight1->SetPosition(Vec3f(30, 50, 10));
        pPointLight1->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight1);

		PointLight *pPointLight2 = new PointLight;
		//	pPointLight2->EnableShadow(false);
		//pPointLight2->SetScaleRadiance(2.0F);
		//pPointLight2->SetPosition(Vec3f(4, 100, 2));
        pPointLight2->SetScaleRadiance(4.5F);
        pPointLight2->SetPosition(Vec3f(-30, 50, 10));
		pPointLight2->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		//mobjSceneLights.AddLight(pPointLight2);

	}

	void buildup_test_mesh_triangle_model_scene()
	{
		// Backgound setting
		mpBackground = new ConstantTexture(Color3f(0, 0, 0));

		// Camera setting
		mpCamera->SetEye(Vec3f(-6, 5, 21));
		//	mpCamera->SetEye(Vec3f(-6, 5, 21));
		mpCamera->SetLookAt(Vec3f(-0.009, 0.11, 0));
		mpCamera->SetUpVector(Vec3f(0, 1, 0));
		((PerspectiveCamera *)mpCamera)->SetDistance(37500);
		mpCamera->Update();

		// Material setting
		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.2);
		pPhongMaterial->SetKd(0.95);
		pPhongMaterial->SetCd2(1, 0.6, 0);   // orange
		pPhongMaterial->SetKs(0.9);
		pPhongMaterial->SetEXP(100);
		pPhongMaterial->SetCs2(1, 0.6, 0);   // orange

		PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
		chekcer3d->SetCheckSize(0.05f);
		chekcer3d->SetAlternateColor(Color3f(0.75, 0.75, 0.75), Color3f(1, 1, 1));

		Matte2Material *pMatMaterial = new Matte2Material;
		pMatMaterial->SetKa(0.45);
		pMatMaterial->SetKd(0.95);
		pMatMaterial->SetCd(Color3f(1, 1, 1), chekcer3d);

		ReflectiveMaterial *pReflectiveMaterial = new ReflectiveMaterial;
		pReflectiveMaterial->SetKa(0.2);
		pReflectiveMaterial->SetKd(0.75);
		pReflectiveMaterial->SetCd3(0.5);
		pReflectiveMaterial->SetKs(0.0);
		pReflectiveMaterial->SetEXP(20);
		pReflectiveMaterial->SetKr(0.5);
		pReflectiveMaterial->SetCr2(0.8, 1.0, 0.8);

		// Material Objects setting6 tgrg  
		//	SimplePlane *pPlane = new SimplePlane(Vec3d(0, 0.00, 0), Vec3d(0, 1, 0));
		//SimplePlane *pPlane = new SimplePlane(Vec3f(0, 0.055, 0), Vec3f(0, 1, 0));
		SimplePlane *pPlane = new SimplePlane(Vec3f(0, -0.015, 0), Vec3f(0, 1, 0));
		MaterialObject *pPlaneMaterialObj = new MaterialObject(pPlane, pMatMaterial);
		mvecObjects.push_back(pPlaneMaterialObj);

		ImageTexture *tex = new ImageTexture;
		tex->LoadJPGFromFile("D:/workspace/CG/Ray Tracing/TextureImages/TextureFiles/jpg/GreenAndYellow.jpg");

		TextureTransform *tex_trans = new TextureTransform(tex);
		tex_trans->Scale(0.1, 0.1, 0.1);
		tex_trans->RotateX(-60);

		Matte2Material *pMatMaterial2 = new Matte2Material;
		pMatMaterial2->SetKa(0.45);
		pMatMaterial2->SetKd(0.95);
		pMatMaterial2->SetCd(Color3f(1, 1, 1), tex);

		//////////////////////////////////////////////////////////////////
		OrenNayar *diffuse_oren_naya = new OrenNayar;
		diffuse_oren_naya->SetSigma(new FixedFloatTexture(80));
		diffuse_oren_naya->SetR(new ConstantTexture(Color3f(0.9, 0.9, 0.3)));
//		diffuse_oren_naya->SetR(new ConstantTexture(Color3f(1, 1, 1)));
		Matte2Material *pOrenNayaMatte = new Matte2Material;
		pOrenNayaMatte->SetDiffuseBRDF(diffuse_oren_naya);

		Lambertian *diffuse_ambient = new Lambertian;
		diffuse_ambient->SetKa(0.3);
		diffuse_ambient->SetCd2(0.2, 0.2, 0.2);
//		MicrofacetSpecular *microfacet_specular = new MicrofacetSpecular(0.001, 1, 0.25);
//		MicrofacetSpecular *microfacet_specular = new MicrofacetSpecular(0.001, 1, 0.35, Color3f(0.9, 0.9, 0.9));
		MicrofacetSpecular *microfacet_specular = new MicrofacetSpecular(0.001, 1, 0.25, Color3f(0.03, 0.03, 0.01));
		PlasticMaterial *plastic_material = new PlasticMaterial;
		plastic_material->SetAmbientBRDF(diffuse_ambient);
		plastic_material->SetDiffuseBRDF(diffuse_oren_naya);
		plastic_material->SetGlossyBRDF(microfacet_specular);
//		pReflectiveMaterial->SetAmbientBRDF(diffuse_ambient);
//		pReflectiveMaterial->SetDiffuseBRDF(diffuse_oren_naya);
//		pReflectiveMaterial->SetGlossyBRDF(microfacet_specular);

		DielectricMaterial *transparentMat = new DielectricMaterial;
		transparentMat->SetKa(0.2);
		transparentMat->SetKd(0.9);
		transparentMat->SetCd2(1, 1, 1);   // orange
		transparentMat->SetKs(0.8);
		transparentMat->SetEXP(100);
		transparentMat->SetCs2(1, 1, 1);   // orange
		transparentMat->SetEtaIn(1.5);
		transparentMat->SetEtaOut(1.0);
		transparentMat->SetColorFilter_In(1, 1, 0);
		transparentMat->SetColorFilter_Out(1, 1, 0);

//		transparentMat->SetAmbientBRDF(diffuse_ambient);
//		transparentMat->SetDiffuseBRDF(diffuse_oren_naya);
//		transparentMat->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////
		MultiJitteredSampler *pMJSamplerBRDF = new MultiJitteredSampler(100);
		pMJSamplerBRDF->MapSamplesToHemiShpere(80);
		microfacet_specular->SetSampler(pMJSamplerBRDF);

		GlossyReflectiveMaterial *pGlossyReflection = new GlossyReflectiveMaterial;
		pGlossyReflection->SetAmbientBRDF(diffuse_ambient);
		pGlossyReflection->SetDiffuseBRDF(diffuse_oren_naya);
		pGlossyReflection->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////
		Glass2Material *glass_material = new Glass2Material(2.5, WHITE, Color3f(0.8, 0.8, 0.8));
		//////////////////////////////////////////////////////////////////

		RegularGridMeshObject *pPLY_TriangleMesh = new RegularGridMeshObject;
		pPLY_TriangleMesh->SetMaterial(pMatMaterial2);
//		pPLY_TriangleMesh->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
		pPLY_TriangleMesh->EnableAcceleration(true);
//		pPLY_TriangleMesh->LoadFromFile("D:/git_projects/PBRTv3/pbrt-v3/scenes/material-testball/models/Mesh002.ply", EMeshType::SMOOTH_UV_SHADING, EModelType::MODEL_PLY, true);
//		pPLY_TriangleMesh->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/Horse97K.ply", EMeshType::SMOOTH_SHADING);
//		pPLY_TriangleMesh->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/TwoTriangles.ply", EMeshType::SMOOTH_SHADING);
		pPLY_TriangleMesh->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/material-testball/models/Mesh002.obj", EMeshType::SMOOTH_UV_SHADING, EModelType::MODEL_OBJ);
		pPLY_TriangleMesh->BuildupAccelerationStructure();
		Instance *dragon_inst = new Instance(pPLY_TriangleMesh);
		dragon_inst->Scale(0.05, 0.05, 0.05);
		dragon_inst->RotateX(60);
		dragon_inst->Translate(0.0, 0.08, 0);
		mvecObjects.push_back(dragon_inst);

		/////////////////////////// The following code sets up a mesh grid manually ///////////////////
/*		int meshID = ResourcePool::Instance()->AllocMesh();
		MeshDesc *mesh = ResourcePool::Instance()->QueryMesh(meshID);
		mesh->mesh_face_count = 2;
		mesh->mesh_vertex_count = 4;
		mesh->mesh_support_uv = 0;
		mesh->mesh_vertices.push_back(Point3f(-1.0, 0.0, -1.0));
		mesh->mesh_vertices.push_back(Point3f(-1.0, -1.0, 1.0));
		mesh->mesh_vertices.push_back(Point3f(1.0, 0.0, 1.0));
		mesh->mesh_vertices.push_back(Point3f(1.0, -1.0, -1.0));
		TriFace face0 = { 0, 1, 2 };
		TriFace face1 = { 0, 2, 3 };
		mesh->mesh_face_datas.push_back(face0);
		mesh->mesh_face_datas.push_back(face1);
		vector<int> v0_faces;
		v0_faces.push_back(0);
		v0_faces.push_back(1);
		vector<int> v1_faces;
		v1_faces.push_back(0);
		vector<int> v2_faces;
		v2_faces.push_back(0);
		v2_faces.push_back(1);
		vector<int> v3_faces;
		v3_faces.push_back(1);
		mesh->mesh_vertex_faces.push_back(v0_faces);
		mesh->mesh_vertex_faces.push_back(v1_faces);
		mesh->mesh_vertex_faces.push_back(v2_faces);
		mesh->mesh_vertex_faces.push_back(v3_faces);
		mesh->mesh_texU.push_back(0);
		mesh->mesh_texU.push_back(0.25);
		mesh->mesh_texU.push_back(0.5);
		mesh->mesh_texU.push_back(0.75);
		mesh->mesh_texV.push_back(0);
		mesh->mesh_texV.push_back(0.25);
		mesh->mesh_texV.push_back(0.5);
		mesh->mesh_texV.push_back(0.75);

		RegularGridMeshObject *mesh_grid = new RegularGridMeshObject;
		mesh_grid->SetMaterial(pMatMaterial2);
		mesh_grid->LoadFromMeshDesc(meshID, EMeshType::SMOOTH_UV_SHADING);
		mesh_grid->BuildupAccelerationStructure();
		Instance *mesh_inst = new Instance(mesh_grid);
		mesh_inst->Scale(0.05, 0.05, 0.05);
		mesh_inst->Translate(0, 0.1, 0);
		mesh_inst->RotateY(-60);
		mvecObjects.push_back(mesh_inst);*/
		/////////////////////////////////////////////////////////////////////////////////////

		// Samplers setting
		MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(1);
		pMJSampler->MapSamplesToHemiShpere(2.0F);

		// Lighting setting
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.8F);
		pAmbientLight->SetLightColor(Color3f(Color3f(0.9, 0.9, 0.9)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

//		AmbientOcclusionLight *pAmbientOcclusion = new AmbientOcclusionLight;
//		pAmbientOcclusion->SetScaleRadiance(1.0F);
//		pAmbientOcclusion->SetLightColor2(1.0F, 1.0F, 1.0F);
//		pAmbientOcclusion->SetFraction(0.25F);
//		pAmbientOcclusion->SetSampler(pMJSampler, 2);
//		mobjSceneLights.SetAmbientLight(pAmbientOcclusion);

		DirectionLight *pDirectionLight = new DirectionLight;
		pDirectionLight->SetScaleRadiance(1.0F);
		pDirectionLight->SetDirection(Vec3f(0.5, 1, 0.75));
		pDirectionLight->SetLightColor(Color3f(1.0F, 0.0F, 0.0F));
		//mobjSceneLights.AddLight(pDirectionLight);

		PointLight *pPointLight = new PointLight;
		pPointLight->SetScaleRadiance(2.0F);
		pPointLight->SetPosition(Vec3f(-2, 6, 2));
		pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		mobjSceneLights.AddLight(pPointLight);

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
	//	mpBackground = new ConstantTexture(Color3f(0.4, 0.6, 0.8));
        mpBackground = new SimpleSkyTexture;

		mpCamera->SetExpTime(0, 2);
	//	mpCamera->SetEye(Vec3f(5, 25, 20));		// For torus
	//	mpCamera->SetEye(Vec3f(235, 25, 600));		// For teardrop, parallel rays may cause problem for solving quratic in y direction
	//	mpCamera->SetEye(Vec3f(10, 5, 6));
	//	mpCamera->SetLookAt(Vec3f(-5, 0, 0));
    //    mpCamera->SetEye(Vec3f(0, 50, 100));
        mpCamera->SetEye(Vec3f(0, 100, 200));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
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

//		JoinBlendCylinder *pJoinBlendCylinder = new JoinBlendCylinder;
//		MaterialObject *pMatJoinBlendCylinder = new MaterialObject(pJoinBlendCylinder, new NormalMaterial);
//		mvecObjects.push_back(pMatJoinBlendCylinder);

        // Because in my render the unit of coordinate is different from <<Ray tracing in one weekedn>>,
        // so almost everything need to be enlarged, that's why it looks like some bug
        // in comparision to the result image on LibingZeng's blog.
        float controlPoints[6][2] = {
                {-10.0,  50.0}, {20.0,  40.0}, {20.0,  10.0},
                {-5,  10.0}, {15, -30.0}, {30.0,  0.0}
        };
        float controlPoints2[6][2] = {
                {40.0,  40.0}, {20.0,  40.0}, {20.0,  10.0},
                {-5,  10}, {15, -30}, {30.0,  0.0}
        };
        RotationalSweeping *pRotationalSweeping = new RotationalSweeping(Vec3f(30.0f, 3.0f, 0.0f), Color3f(1.0f, 0.0f, 0.0f), controlPoints);
        //MaterialObject *pMatRotationalSweeping = new MaterialObject(pRotationalSweeping, new MatteMaterial(new ConstantTexture(Color3f(0.4f, 0.5f, 0.6f))));
        MaterialObject *pMatRotationalSweeping = new MaterialObject(pRotationalSweeping, new GlassMaterial(4.5, Color3f(1,1,1), Color3f(1, 1, 1)));
        //MaterialObject *pMatRotationalSweeping = new MaterialObject(pRotationalSweeping, new NormalMaterial);
        mvecObjects.push_back(pMatRotationalSweeping);

//        SimpleTorus *pTorus = new SimpleTorus(Vec3f(0,0,0), 20, 5);
//        MaterialObject *pMatNormalTorus = new MaterialObject(pTorus, new MatteMaterial(new ConstantTexture(Color3f(0.4f, 0.5f, 0.6f))));
//        mvecObjects.push_back(pMatNormalTorus);

//        RotationalSweeping *pRotationalSweeping2 = new RotationalSweeping(Vec3f(-30.0f, 0.0f, 0.0f), Color3f(1.0f, 0.0f, 0.0f), controlPoints2);
//        MaterialObject *pMatRotationalSweeping2 = new MaterialObject(pRotationalSweeping2, new MatteMaterial(new ConstantTexture(Color3f(0.4f, 0.5f, 0.6f))));
//        mvecObjects.push_back(pMatRotationalSweeping2);

//        SolidCylinder *pSolidCylinder0 = new SolidCylinder(Vec3f(50, 0, 0), 20, 0, 60, Color3f(1.0, 0.0, 0.0));
//        MaterialObject *pMatSolidCylinder0 = new MaterialObject(pSolidCylinder0, new MatteMaterial(Color3f(0,0,1)));
//        mvecObjects.push_back(pMatSolidCylinder0);
//
//        SolidCylinder *pSolidCylinder = new SolidCylinder(Vec3f(0, 0, 0), 20, 0, 60, Color3f(1.0, 0.0, 0.0));
//        MaterialObject *pMatSolidCylinder = new MaterialObject(pSolidCylinder, new GlassMaterial(4.5, Color3f(1,1,1), Color3f(1,1,1), Color3f(1,1,1)));
//        mvecObjects.push_back(pMatSolidCylinder);
//
//        SolidCylinder *pSolidCylinder2 = new SolidCylinder(Vec3f(-50, 0, 0), 20, 0, 60, Color3f(1.0, 0.0, 0.0));
//        MaterialObject *pMatSolidCylinder2 = new MaterialObject(pSolidCylinder2, new MatteMaterial(Color3f(1,0,0)));
//        mvecObjects.push_back(pMatSolidCylinder2);

//        SimpleSphere *pShpere1 = new SimpleSphere(Vec3f(-40, 0, 0), 20);
//        MaterialObject *pMatShpere = new MaterialObject(pShpere1, new MatteMaterial(Color3f(1,0,0)));
//        mvecObjects.push_back(pMatShpere);
//
//        SimpleSphere *pShpere2 = new SimpleSphere(Vec3f(10, 0, 0), 20);
//        MaterialObject *pMatShpere2 = new MaterialObject(pShpere2, new GlassMaterial(4.5, Color3f(1,1,1)));
//        mvecObjects.push_back(pMatShpere2);
//
//        SimpleSphere *pShpere3 = new SimpleSphere(Vec3f(60, 0, 0), 20);
//        MaterialObject *pMatShpere3 = new MaterialObject(pShpere3, new MatteMaterial(Color3f(0,0,1)));
//        mvecObjects.push_back(pMatShpere3);

//
        SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -30, 0), Vec3f(0, 1, 0));
        MaterialObject *pMatPlane = new MaterialObject(pSimplePlane, new MirrorMaterial(Color3f(0,1,0.5)));
        mvecObjects.push_back(pMatPlane);

// 		int num = 0;
// 		float roots[4] = { 0.0f };
// 		RTMath::SolveQuaraticEquation(1.0, 0.0, 0.0, 0.0, -16, num, roots);
// 		char szBuf[32] = { 0 };
// 		sprintf_s(szBuf, 32, "num: %d, %.2f %.2f\n", num, roots[0], roots[1]);
// 		g_Console.Write(szBuf);

//        float ee6[7] = {1.0,  3.0,  -5.0, -15.0,  4.0,  12.0,  0.0};
//        float roots[6];
//        int roots_num = RTMath::SolveOne6th_equation(ee6, -5.5, 5.5, roots);
//        int ii = 0;
//
//        for (int i = 0; i < roots_num; ++i) {
//            std::cout << roots[i] << std::endl;
//        }

	}

	void buildup_test_geometry_tessellation_shpere_scene() {
	    //
        mpBackground = new ConstantTexture(Color3f(0.8, 0.7, 0.9));

        //
        mpCamera->SetExpTime(0, 2);
        mpCamera->SetEye(Vec3f(0, 10, 15));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
        mpCamera->SetDistance(500.0f);
        mpCamera->Update();

        //
        Matte2Material *pTessellationMaterial = new Matte2Material;
        pTessellationMaterial->SetKa(0.25F);
        pTessellationMaterial->SetKd(0.75F);
        pTessellationMaterial->SetCd2(0.4, 0.7, 0.4);

        PhongMaterial *pPhongMaterial = new PhongMaterial;
        pPhongMaterial->SetKa(0.75);
        pPhongMaterial->SetKd(0.75);
        pPhongMaterial->SetCd2(0.9, 0.8, 0.0f);
        pPhongMaterial->SetKs(0.75F);
        pPhongMaterial->SetCs2(0.9, 0.8, 0.0f);
        pPhongMaterial->SetEXP(100);

        RegularGridMeshObject *tessellationShpere = new RegularGridMeshObject;
        tessellationShpere->SetMaterial(pPhongMaterial);
        tessellationShpere->EnableAutoDelete(false);
//        tessellationShpere->TessellateFlatShpere(Vec3f(0.0f, 0.0f, 0.0f), 4, 4);
//        float controlPoints[6][2] = {
//                {-10.0,  50.0}, {20.0,  40.0}, {20.0,  10.0},
//                {-5,  10.0}, {15, -30.0}, {30.0,  0.0}
//        };
        float controlPoints[6][2] = {
                {-1.0,  5.0}, {2.0,  4.0}, {2.0,  1.0},
                {-0.5,  1.0}, {1.5, -3.0}, {3.0,  0.0}
        };
        tessellationShpere->TessellateFlatRotaionalSweeping(Vec3f(3.0f, 5.0f, 0.0f), 200, 50, controlPoints);
        tessellationShpere->EnableAcceleration(true);
        tessellationShpere->BuildupAccelerationStructure();
        mvecObjects.push_back(tessellationShpere);

        SimpleSphere *pShpere = new SimpleSphere(Vec3f(-2,0,0), 2);
        MaterialObject *pShpereMat = new MaterialObject(pShpere, pPhongMaterial);
        mvecObjects.push_back(pShpereMat);

        //
        Matte2Material *pPlaneMaterial = new Matte2Material;
        pPlaneMaterial->SetKa(0.95F);
        pPlaneMaterial->SetKd(0.95F);
        pPlaneMaterial->SetCd2(0.8, 0.9, 0.9);

        SimplePlane *pSimplePlane = new SimplePlane(Vec3f(0, -2, 0), Vec3f(0, 1, 0));
        MaterialObject *pMatPlane = new MaterialObject(pSimplePlane, pPlaneMaterial);
        mvecObjects.push_back(pMatPlane);

        //
//        MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(1);
//        AmbientOcclusionLight *pAmbientOcclusion = new AmbientOcclusionLight;
//        pAmbientOcclusion->SetScaleRadiance(1.0F);
//        pAmbientOcclusion->SetLightColor2(1.0F, 1.0F, 1.0F);
//        pAmbientOcclusion->SetFraction(0.25);
//        pAmbientOcclusion->SetSampler(pMJSampler, 2.0F);
//        mobjSceneLights.SetAmbientLight(pAmbientOcclusion);

        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.3F);
        pAmbientLight->SetLightColor(Color3f(Color3f(0.2, 0.1, 0.1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        //
        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(2.0F);
        pPointLight->SetPosition(Vec3f(0, 20, 0));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);
	}

	void buildup_test_part_geometric_object_scene() {
        mpBackground = new ConstantTexture(Color3f(0.4, 0.5, 0.4));

        mpCamera->SetExpTime(0, 2);
//        mpCamera->SetEye(Vec3f(20, 20, 30)); // good
        mpCamera->SetEye(Vec3f(0, 120, 140));
        //mpCamera->SetEye(Vec3f(0, 20, 10));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
        mpCamera->SetDistance(1500.0f);
        mpCamera->Update();

        //
        Matte2Material *pMatMaterial = new Matte2Material;
        pMatMaterial->EnableSelfShadow(false);
        pMatMaterial->SetKa(0.95);
        pMatMaterial->SetKd(0.95);
        pMatMaterial->SetCd2(1, 1, 0);

        //
//        SimpleTorus *pTorus = new SimpleTorus(Vec3f(0,0,0), 2, 1);
//        pTorus->SetPartParams(true, 0.0f, 270.0f, 90.0f, 360.0f);
// 		MaterialObject *pMatTorus = new MaterialObject(pTorus, pMatMaterial);
//		mvecObjects.push_back(pMatTorus);

//        SimpleSphere *pShpere = new SimpleSphere(Vec3f(-2,0,0), 2);
//        pShpere->SetPartParams(true, 0, 360, 90, 180);
//        MaterialObject *pShpereMat = new MaterialObject(pShpere, pMatMaterial);
//        mvecObjects.push_back(pShpereMat);

//        float bottom 	= -1.0;
//        float top 		=  1.0;
//        float radius	=  7.0;
//        SimpleCylinder *pCylinder = new SimpleCylinder(Vec3f(0,0,0), radius, bottom, top);
//        pCylinder->SetPartParams(true, 0, 180);
//        MaterialObject *pMatCylinder = new MaterialObject(pCylinder, pMatMaterial);
//        mvecObjects.push_back(pMatCylinder);

//        SimpleCone *pCone = new SimpleCone(Vec3f(0,-10,0), 6, -10, 10);
//        pCone->SetPartParams(true, 0, 270, 12);
//        MaterialObject *pMatCone = new MaterialObject(pCone, pMatMaterial);
//        mvecObjects.push_back(pMatCone);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.4);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
 //       pPointLight->EnableShadow(false);
        pPointLight->SetScaleRadiance(2.0F);
        pPointLight->SetPosition(Vec3f(60, 60, 30));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);
	}

	void buildup_test_light_prob_env_test_scene() {
        mpBackground = new SimpleSkyTexture;

        mpCamera->SetExpTime(0, 2);
        mpCamera->SetEye(Vec3f(0, 0, 30));

        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
        mpCamera->SetDistance(1500.0f);
        mpCamera->Update();

        //
        ImageTexture *pTex = new ImageTexture;
        if (pTex->LoadPNGFromFile("/Users/wuwenxi/workspace/CG/RayTracing/laplataraytracer/cmake-build-release-1/sky_image2.png")) {
            pTex->SetMappingMethod(new LightProbMapping);
            EmissiveTextureMaterial *pEmissive = new EmissiveTextureMaterial(1.0f, pTex);

            //
            SimpleSphere *pShpere = new SimpleSphere(Vec3f(0,0,0), 1);
            MaterialObject *pShpereMat = new MaterialObject(pShpere, pEmissive);
            mvecObjects.push_back(pShpereMat);

        } else {
            std::cout << "failed to load ppm file." << std::endl;
        }

	}

	void buildup_test_noise_based_texture_scene() {
        //mpBackground = new SimpleSkyTexture;
        mpBackground = new ConstantTexture(Color3f(0.5,0.5,0.5));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(0, 0, 10));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(420);
        mpCamera->Update();

        //
        CubicNoise *pNoise = new CubicNoise;
        pNoise->SetOctavesNum(6);
        pNoise->SetGainFactor(0.5f);
        pNoise->SetLacunarityFactor(2.0f);

//        FBMTexture *pFBmTex = new FBMTexture;
//        pFBmTex->SetColor(WHITE);
//        pFBmTex->SetMinMaxRange(0.0f, 1.0f);
//        pFBmTex->SetLatticeNoise(pNoise);

        RampFBmTexture *pFBmTex = new RampFBmTexture;

        ImageTexture *pRampTex = new ImageTexture;
        pRampTex->LoadJPGFromFile("/Users/wuwenxi/workspace/CG/RayTracing/laplataraytracer/cmake-build-release-1/ramptex.jpg");

        pFBmTex->SetRampTexture(pRampTex);
        pFBmTex->SetPerturbence(0);
        pFBmTex->SetLatticeNoise(pNoise);

        TextureTransform *pTransformTex = new TextureTransform(pFBmTex);
        pTransformTex->RotateX(-60);
        pTransformTex->RotateY(-60);

        WrappedFBmTexture *pWrapped = new WrappedFBmTexture;
        pWrapped->SetExpNum(8);
        pWrapped->SetLatticeNoise(pNoise);
        pWrapped->SetColor(Color3f(0.7, 1, 0.5));

        Matte2Material *pMat = new Matte2Material;
        pMat->SetKa(0.25);
        pMat->SetKd(0.85);
        pMat->SetCd(Color3f(1,1,1), pTransformTex);

        /////
        PhongMaterial *pPhongMaterial = new PhongMaterial;
        pPhongMaterial->SetKa(0.25F);
        pPhongMaterial->SetKd(0.75F);
        pPhongMaterial->SetCd2(1.0F, 1.0F, 1.0F);
        pPhongMaterial->SetKs(0.25F);
        pPhongMaterial->SetCs2(1.0F, 1.0F, 1.0F);
        pPhongMaterial->SetEXP(150);
        TextureLambertian *pTexLambertian = new TextureLambertian;
        pTexLambertian->SetKa(0.25);
        pTexLambertian->SetKd(0.75);
        pTexLambertian->SetCd(pTransformTex);
        pPhongMaterial->SetDiffuseBRDF(pTexLambertian);
        ////

        SimpleSphere *pSphere = new SimpleSphere(Vec3f(0,0,0), 3.0f);
        MaterialObject *pMatObj = new MaterialObject(pSphere, pPhongMaterial);
 //       mvecObjects.push_back(pMatObj);

        SimpleBox *pBox = new SimpleBox(20, 90, -101, 100, -100, 20);
        pBox->SetColor(Color3f(0, 1, 1));
        MaterialObject *pMatObj2 = new MaterialObject(pBox, pPhongMaterial);

        Instance *pInstShpere = new Instance(pMatObj);
        pInstShpere->RotateX(-60);
        pInstShpere->RotateY(-60);
        mvecObjects.push_back(pInstShpere);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.4);
        pAmbientLight->SetLightColor(Color3f(Color3f(0.8, 0.9, 0.6)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(2.5F);
        pPointLight->SetPosition(Vec3f(10, 10, 20));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);

	}

	void buildup_test_texture_mapping_scene() {
        mpBackground = new SimpleSkyTexture;

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(0, 0, 20));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(420);
        mpCamera->Update();

        //
        ImageTexture *pTex = new ImageTexture;
        //pTex->LoadJPGFromFile("/Users/wuwenxi/workspace/CG/RayTracing/laplataraytracer/cmake-build-debug/cylinder_tex.jpg");
		pTex->LoadJPGFromFile("D:/workspace/CG/Ray Tracing/TextureImages/TextureFiles/jpg/BilliardBall.jpg");
        pTex->SetMappingMethod(new SphericalMapping);

        TextureTransform *pTexTrans = new TextureTransform(pTex);
        pTexTrans->Scale(5,5,5);
		pTexTrans->RotateY(300);

        //
        EmissiveTextureMaterial *pMat = new EmissiveTextureMaterial(1.0f, pTexTrans);

        //
        //SimpleCylinder *pObj = new SimpleCylinder(Vec3f(0,0,0), 40, 0, 80, Color3f(1,0,0));
        //SimpleDisk *pObj = new SimpleDisk(Vec3f(0,0,0), Vec3f(0, 0, 1), 30);
        SimpleSphere *pObj = new SimpleSphere(Vec3f(0,0,0), 1);
        MaterialObject *pMatObj = new MaterialObject(pObj, pMat);
		//mvecObjects.push_back(pMatObj);
        Instance *pInst = new Instance(pMatObj);
        pInst->Scale(5,5,5);
		pInst->RotateY(300);
        mvecObjects.push_back(pInst);

	}

	void buildup_test_caustic_by_global_tracer_scene() {
	    //
        mpBackground = new ConstantTexture(Color3f(0,0,0));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(5, 10, 20));
        mpCamera->SetLookAt(Vec3f(-1, 1, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(500);
        mpCamera->Update();

        //
        Emissive2Material *pEmissive = new Emissive2Material;
        pEmissive->SetRadiance(30.0f);
        pEmissive->SetEmissiveColor(Color3f(0.75, 1, 0.75));

        ReflectiveMaterial *pReflective = new ReflectiveMaterial;
        pReflective->SetKa(0.7);
        pReflective->SetKd(0.4);
        pReflective->SetKs(0.7);
        pReflective->SetEXP(1);
        pReflective->SetKr(0.95);
        pReflective->SetCr2(1.0, 1.0, 0.0);

        GlossyReflectiveMaterial *pPhongMaterial = new GlossyReflectiveMaterial;
        pPhongMaterial->SetKa(0.25F);
        pPhongMaterial->SetKd(0.75F);
        pPhongMaterial->SetCd2(1.0F, 1.0F, 1.0F);
        pPhongMaterial->SetKs(0.95F);
        pPhongMaterial->SetCs2(1.0F, 1.0F, 0.0F);
        pPhongMaterial->SetEXP(0.2);
        MultiJitteredSampler *pSampler = new MultiJitteredSampler(100);
        pSampler->MapSamplesToHemiShpere(0.02);
        pPhongMaterial->SetSampler(pSampler);

        Matte2Material *pMatMaterial = new Matte2Material;
        pMatMaterial->SetKa(0);
        pMatMaterial->SetKd(0.75);
        pMatMaterial->SetCd(WHITE);
        MultiJitteredSampler *pSampler2 = new MultiJitteredSampler(100);
        pSampler2->MapSamplesToHemiShpere(0.02);
        pMatMaterial->SetSampler(pSampler2);

        //
        SimpleSphere *pShpere = new SimpleSphere(Vec3f(-2, 7, 6), 2);
        pShpere->SetColor(Color3f(1,1,1));
        MaterialObject *pMatSphere = new MaterialObject(pShpere, pEmissive);
        mvecObjects.push_back(pMatSphere);

        float bottom 	= -1.0;
        float top 		=  1.0;
        float radius	=  7.0;
        SimpleCylinder *pCylinder = new SimpleCylinder(Vec3f(0,0,0), radius, bottom, top);
        pCylinder->SetColor(0,0,0);
        MaterialObject *pMatCylinder = new MaterialObject(pCylinder, pReflective);
        mvecObjects.push_back(pMatCylinder);

        SimplePlane *pPlane = new SimplePlane(Vec3f(0, -1, 0), Vec3f(0, 1, 0));
        pPlane->SetColor(Color3f(0,0,0));
        MaterialObject *pPlaneMat = new MaterialObject(pPlane, pMatMaterial);
        mvecObjects.push_back(pPlaneMat);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.0);
        pAmbientLight->SetLightColor(Color3f(Color3f(0.8, 0.9, 0.6)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        AreaConstLight *pAreaConstLight = new AreaConstLight;
        pAreaConstLight->SetLightShapeObject(pShpere);
        pAreaConstLight->SetScaleRadiance(40.0f);
        pAreaConstLight->SetLightColor2(1.0F, 0.0F, 0.0F);

        mobjSceneLights.AddLight(pAreaConstLight);
	}

    void buildup_test_caustic_by_path_tracer_scene() {
        //
        mpBackground = new ConstantTexture(Color3f(0,0,0));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(5, 10, 20));
        mpCamera->SetLookAt(Vec3f(-1, 1, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(500);
        mpCamera->Update();

        //
        EmissiveMaterial *pEmissive = new EmissiveMaterial(new ConstantTexture(Color3f(10.75, 12, 10.75)));

        MirrorMaterial *pReflective = new MirrorMaterial(Color3f(1.0, 1.0, 0.0));

        MatteMaterial *pMatMaterial = new MatteMaterial(Color3f(1.0, 1.0, 1.0));

        //
        SimpleSphere *pShpere = new SimpleSphere(Vec3f(-2, 7, 6), 2);
        pShpere->SetColor(Color3f(1,1,1));
        MaterialObject *pMatSphere = new MaterialObject(pShpere, pEmissive);
        mvecObjects.push_back(pMatSphere);

        float bottom 	= -1.0;
        float top 		=  1.0;
        float radius	=  7.0;
        SimpleCylinder *pCylinder = new SimpleCylinder(Vec3f(0,0,0), radius, bottom, top);
        pCylinder->SetColor(0,0,0);
        MaterialObject *pMatCylinder = new MaterialObject(pCylinder, pReflective);
        mvecObjects.push_back(pMatCylinder);

//        Instance *ptr_inst = new Instance(pMatCylinder);
//        ptr_inst->RotateZ(60);
//        ptr_inst->Translate(0, 8, 0);
//        mvecObjects.push_back(ptr_inst);

        SimplePlane *pPlane = new SimplePlane(Vec3f(0, -1, 0), Vec3f(0, 1, 0));
        pPlane->SetColor(Color3f(0,0,0));
        MaterialObject *pPlaneMat = new MaterialObject(pPlane, pMatMaterial);
        mvecObjects.push_back(pPlaneMat);
    }

	void buildup_test_dielectric_material_scene() {
        mpBackground = new ConstantTexture(Color3f(1,1,1));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(0, 0, 200));
        mpCamera->SetLookAt(Vec3f(-0.4, 0.6, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(15000);
        mpCamera->Update();

        //
        DielectricMaterial *transparentMat = new DielectricMaterial;
        transparentMat->SetKa(0);
        transparentMat->SetKd(0);
//        transparentMat->SetCd2(1,1,1);
        transparentMat->SetKs(0.2);   // orange
        transparentMat->SetEXP(2000.0f);
//        transparentMat->SetCs2(1, 1, 1);   // orange
        transparentMat->SetEtaIn(1.5);
        transparentMat->SetEtaOut(1.0);
        transparentMat->SetColorFilter_In(0.65,0.45,0);
        transparentMat->SetColorFilter_Out(1,1,1);
        transparentMat->EnableSelfShadow(false);

        //
        RegularGridMeshObject *model = new RegularGridMeshObject;
//        model->SetMaterial(new GlassMaterial(2.5, Color3f(1,0.8,0), Color3f(1,1,1)));
        model->SetMaterial(transparentMat);
        model->ReverseMeshNormals();
        model->EnableAcceleration(true);
 //       model->LoadFromFile("/Users/wuwenxi/workspace/CG/Models_Materials/bunny/reconstruction/bun_zipper_res2.ply", EMeshType::SMOOTH_SHADING);
		model->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/Bunny4K.ply", EMeshType::SMOOTH_SHADING);
        model->BuildupAccelerationStructure();

    //    mvecObjects.push_back(model);
        Instance* model_inst = new Instance(model);
        model_inst->Scale(20, 20, 20);
        model_inst->Translate(0, -1.5, 0.0);
        mvecObjects.push_back(model_inst);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.25);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

	}

	void buildup_test_dielectric_material_scene2() {
	    //
        mpBackground = new ConstantTexture(Color3f(0.0, 0.3, 0.25));
        //mpBackground = new ConstantTexture(Color3f(1, 1, 1));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(-8, 5.5, 40));
        mpCamera->SetLookAt(Vec3f(1, 4, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(2400);
        mpCamera->Update();

        //
        PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
        chekcer3d->SetCheckSize(4);
        chekcer3d->SetAlternateColor(Color3f(0.75,0.75,0.75), Color3f(1,1,1));

        //
        DielectricMaterial *transparentMat = new DielectricMaterial;
        transparentMat->SetKa(0.8);
        transparentMat->SetKd(1);
//        transparentMat->SetCd(Color3f(1,1,1));
        transparentMat->SetKs(0.2);   // orange
//        transparentMat->SetCs(Color3f(1,1,1));
        transparentMat->SetEXP(2000.0f);
        transparentMat->SetEtaIn(0.75);
        transparentMat->SetEtaOut(1.0);
        transparentMat->SetColorFilter_In(1,1,1);
        transparentMat->SetColorFilter_Out(1,1,1);

        ReflectiveMaterial *pReflective = new ReflectiveMaterial;
        pReflective->SetKa(0.3);
        pReflective->SetKd(0.3);
        pReflective->SetCd(Color3f(1,0,0));
        pReflective->SetKs(0.2);
        pReflective->SetCs2(1,1,1);
        pReflective->SetEXP(2000);
        pReflective->SetKr(0.25);
        pReflective->SetCr2(1.0, 1.0, 0.0);

        Matte2Material *pMatMaterial = new Matte2Material;
        pMatMaterial->SetKa(0.5);
        pMatMaterial->SetKd(0.35);
        pMatMaterial->SetCd(WHITE, chekcer3d);

        //
        SimpleSphere *pShpere = new SimpleSphere(Vec3f(0, 4.5, 0), 3);
        pShpere->SetColor(Color3f(0,0,0));
 //       MaterialObject *pMatSphere = new MaterialObject(pShpere, new GlassMaterial(2.5, Color3f(1,1,1), Color3f(1,1,1)));
        MaterialObject *pMatSphere = new MaterialObject(pShpere, transparentMat);
        mvecObjects.push_back(pMatSphere);

        SimpleSphere *pShpere2 = new SimpleSphere(Vec3f(4, 4, -6), 3);
        pShpere2->SetColor(Color3f(0,0,0));
//        MaterialObject *pMatSphere2 = new MaterialObject(pShpere2, new MirrorMaterial(Color3f(1,1,0)));
        MaterialObject *pMatSphere2 = new MaterialObject(pShpere2, pReflective);
        mvecObjects.push_back(pMatSphere2);

        Vec3f p0(-20, 0.0f, -100);
        Vec3f a(0, 0, 120);
        Vec3f b(40, 0, 0);
        SimpleRectangle *rect = new SimpleRectangle(p0, a, b);
//        MaterialObject *pMatRect = new MaterialObject(rect, new MatteMaterial(chekcer3d));
        MaterialObject *pMatRect = new MaterialObject(rect, pMatMaterial);
        mvecObjects.push_back(pMatRect);

//        SimplePlane *plane = new SimplePlane(Vec3f(0, -1, 0), Vec3f(0,1,1));
//        MaterialObject *pPlaneMatRect = new MaterialObject(plane, pMatMaterial);
//        mvecObjects.push_back(pPlaneMatRect);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.25);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(4.5F);
        pPointLight->SetPosition(Vec3f(40, 50, 0));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
//		pPointLight->EnableDistanceAttenuation(true, 2.0f);
        mobjSceneLights.AddLight(pPointLight);

        PointLight *pPointLight2 = new PointLight;
        pPointLight2->SetScaleRadiance(4.5F);
        pPointLight2->SetPosition(Vec3f(-10, 20, 10));
        pPointLight2->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
//		pPointLight2->EnableDistanceAttenuation(true, 2.0f);
        mobjSceneLights.AddLight(pPointLight2);

        DirectionLight *pDirectionLight0 = new DirectionLight;
        pDirectionLight0->SetScaleRadiance(4.5F);
        pDirectionLight0->SetDirection(Vec3f(-1, 0, 0));
        pDirectionLight0->SetLightColor(Color3f(1.0F, 1.0f, 1.0f));
        mobjSceneLights.AddLight(pDirectionLight0);
	}

	void buildup_test_glossy_reflective_scene() {
        //
        mpBackground = new ConstantTexture(Color3f(0,0,0));

        mpCamera->SetExpTime(0, 2);

        mpCamera->SetEye(Vec3f(5, 10, 20));
        mpCamera->SetLookAt(Vec3f(-1, 1, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(500);
        mpCamera->Update();

        //
        GlossyReflectiveMaterial *pPhongMaterial = new GlossyReflectiveMaterial;
        pPhongMaterial->SetKa(0.25F);
        pPhongMaterial->SetKd(0.75F);
        pPhongMaterial->SetCd2(1.0F, 0.30F, 0.20F);
        pPhongMaterial->SetKs(0.95F);
        pPhongMaterial->SetCs2(1.0F, 0.30F, 0.20F);
        pPhongMaterial->SetEXP(150);
        MultiJitteredSampler *pSampler = new MultiJitteredSampler(100);
        pSampler->MapSamplesToHemiShpere(0.2);
        pPhongMaterial->SetSampler(pSampler);

        //
        SimpleSphere *pShpere = new SimpleSphere(Vec3f(-2, 7, 6), 2);
        pShpere->SetColor(Color3f(1,1,1));
        MaterialObject *pMatSphere = new MaterialObject(pShpere, pPhongMaterial);
        mvecObjects.push_back(pMatSphere);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.0);
        pAmbientLight->SetLightColor(Color3f(Color3f(0.8, 0.9, 0.6)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(4.5F);
        pPointLight->SetPosition(Vec3f(30, 50, 10));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);

    }

    void buildup_CornellBox_scene2()
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
		Matte2Material *pMatBox = new Matte2Material;
		pMatBox->SetKa(1);
		pMatBox->SetKd(1);
		pMatBox->SetCd(Color3f(0, 0, 1), new ConstantTexture(Color3f(0, 0, 1)));

		Matte2Material *pMatBox2 = new Matte2Material;
		pMatBox2->SetKa(1);
		pMatBox2->SetKd(1);
		pMatBox2->SetCd(Color3f(1,1,0), new ConstantTexture(Color3f(1, 1, 0)));

        Matte2Material *pMatMaterial = new Matte2Material;
        pMatMaterial->SetKa(0.2);
        pMatMaterial->SetKd(0.25);
        pMatMaterial->SetCd(WHITE, new ConstantTexture(Color3f(1, 0, 0)));

        Matte2Material *pMatMaterial2 = new Matte2Material;
        pMatMaterial2->SetKa(0.2);
        pMatMaterial2->SetKd(0.25);
        pMatMaterial2->SetCd(WHITE, new ConstantTexture(Color3f(1, 0, 1)));

        Matte2Material *pMatMaterial3 = new Matte2Material;
        pMatMaterial3->SetKa(0.2);
        pMatMaterial3->SetKd(0.25);
        pMatMaterial3->SetCd(WHITE, new ConstantTexture(Color3f(1, 1, 0)));

        Matte2Material *pMatMaterial4 = new Matte2Material;
        pMatMaterial4->SetKa(0.2);
        pMatMaterial4->SetKd(0.25);
        pMatMaterial4->SetCd(BLACK, new ConstantTexture(Color3f(0, 0, 0)));

        Matte2Material *pMatMaterial5 = new Matte2Material;
        pMatMaterial5->SetKa(0.2);
        pMatMaterial5->SetKd(0.25);
        pMatMaterial5->SetCd(WHITE, new ConstantTexture(Color3f(0, 1, 0)));

        MultiJitteredSampler *pSampler = new MultiJitteredSampler(100);
        pSampler->MapSamplesToHemiShpere(0.02);
        pMatMaterial->SetSampler(pSampler);
        pMatMaterial2->SetSampler(pSampler);
        pMatMaterial3->SetSampler(pSampler);
        pMatMaterial4->SetSampler(pSampler);
        pMatMaterial5->SetSampler(pSampler);

        DielectricMaterial *transparentMat = new DielectricMaterial;
        transparentMat->SetKa(0.4);
        transparentMat->SetKd(0.4);
        transparentMat->SetCd2(1,1,1);
        transparentMat->SetKs(0.6);
        transparentMat->SetCs2(1,1,1);
        transparentMat->SetEXP(2000.0f);
        transparentMat->SetEtaIn(1.6);
        transparentMat->SetEtaOut(1.0);
        transparentMat->SetColorFilter_In(1,1,1);
        transparentMat->SetColorFilter_Out(1,1,1);
//        MultiJitteredSampler *pSampler2 = new MultiJitteredSampler(100);
//        pSampler2->MapSamplesToHemiShpere(0.02);
//        transparentMat->SetSampler(pSampler2);

        ReflectiveMaterial *pReflective = new ReflectiveMaterial;
        pReflective->SetKa(0.3);
        pReflective->SetKd(0.3);
        pReflective->SetCd(Color3f(1,0,0));
        pReflective->SetKs(0.2);
        pReflective->SetCs2(1,1,1);
        pReflective->SetEXP(2000);
        pReflective->SetKr(0.25);
        pReflective->SetCr2(1.0, 1.0, 1.0);

        Emissive2Material *pEmissive = new Emissive2Material;
        pEmissive->SetRadiance(7.8);
        pEmissive->SetEmissiveColor(Color3f(1, 1, 1));

        //
        XYRect *p0 = new XYRect(Vec3f(-40, 0, 200), false, 80, 80);
//        p0->SetColor(Color3f(0,0,0));
        MaterialObject *pBackWall = new MaterialObject(p0, pMatMaterial);
        mvecObjects.push_back(pBackWall);

        YZRect *p1 = new YZRect(Vec3f(-40, 0, 200), false, 80, 80);
 //       p1->SetColor(Color3f(0,0,0));
        MaterialObject *pLeftWall = new MaterialObject(p1, pMatMaterial2);
		mvecObjects.push_back(pLeftWall);

        YZRect *p2 = new YZRect(Vec3f(40, 0, 200), true, 80, 80);
  //      p2->SetColor(Color3f(0,0,0));
        MaterialObject *pRightWall = new MaterialObject(p2, pMatMaterial3);
        mvecObjects.push_back(pRightWall);

        XZRect *p3 = new XZRect(Vec3f(-40, 80, 200), true, 80, 80);
  //      p3->SetColor(Color3f(0,0,0));
        MaterialObject *pCellWall = new MaterialObject(p3, pMatMaterial4);
        mvecObjects.push_back(pCellWall);

        XZRect *p4 = new XZRect(Vec3f(-40, 0, 200), false, 80, 80);
 //       p4->SetColor(Color3f(0,0,0));
        MaterialObject *pFloorWall = new MaterialObject(p4, pMatMaterial5);
        mvecObjects.push_back(pFloorWall);

        //
        // Add a glass material ball in the middle of the box.
        SimpleSphere *pSphere = new SimpleSphere(Vec3f(0, 0, 0), 10);
//        pSphere->SetColor(Color3f(0,0,0));
//        MaterialObject *pGlassBall = new MaterialObject(pSphere, transparentMat);
//        mvecObjects.push_back(pGlassBall);
		MaterialObject *sphere_mat = new MaterialObject(pSphere, pMatBox);
		Instance *sphere_inst = new Instance(sphere_mat);
		sphere_inst->RotateY(60);
		sphere_inst->Translate(20, 40, 240);

		SolidCylinder *pCylinder = new SolidCylinder(Vec3f(0, 0, 0), 10, 0, 40, RED);
		ThickRing *pThickRing = new ThickRing(Vec3f(0, 0, 0), 10, 13, 0, 10, RED);

        //
        XZRect *p5 = new XZRect(Vec3f(-25, 78, 205), true, 50, 50);
 //       p5->SetColor(Color3f(0,0,0));
        MaterialObject *pCellLight = new MaterialObject(p5, pEmissive);
        mvecObjects.push_back(pCellLight);

//		MetalMaterial *metal_material = new MetalMaterial;
//		metal_material->SetSpecular(0.9, WHITE);
		Glass2Material *glass_material = new Glass2Material;

		//
		SimpleBox *box1 = new SimpleBox(0, 20, 0, 55, 0, 20);
		MaterialObject *box_mat1 = new MaterialObject(box1, pMatBox);
		Instance *inst_box1 = new Instance(box_mat1);
		inst_box1->RotateY(60);		// 先旋转，再平移，<<3D游戏大师>>最初的教导不能忘!!
		inst_box1->Translate(-28, 0, 250);
//		mvecObjects.push_back(inst_box1);

		SimpleBox *box2 = new SimpleBox(0, 20, 0, 40, 0, 20);
		MaterialObject *box_mat2 = new MaterialObject(box2, pMatBox2);
		Instance *inst_box2 = new Instance(box_mat2);
//		inst_box2->RotateY(-30);		// 先旋转，再平移，<<3D游戏大师>>最初的教导不能忘!!
		inst_box2->Translate(10, 0, 230);
//		mvecObjects.push_back(inst_box2);

		SimpleSphere *rand_sphere = new SimpleSphere(Vec3f(-25, 40, 210), 6);
		SimpleSphere *rand_sphere2 = new SimpleSphere(Vec3f(-15, 40, 220), 6);
		SimpleSphere *rand_sphere3 = new SimpleSphere(Vec3f(-30, 40, 230), 6);
		SimpleSphere *rand_sphere4 = new SimpleSphere(Vec3f(25, 40, 240), 6);
		SimpleSphere *rand_sphere5 = new SimpleSphere(Vec3f(30, 40, 250), 6);
		SimpleSphere *rand_sphere6 = new SimpleSphere(Vec3f(0, 40, 210), 6);

		MaterialObject *mat_1 = new MaterialObject(rand_sphere, pMatMaterial);
		MaterialObject *mat_2 = new MaterialObject(rand_sphere2, pMatMaterial, true, false);
		MaterialObject *mat_3 = new MaterialObject(rand_sphere3, pMatMaterial, true, false);
		MaterialObject *mat_4 = new MaterialObject(rand_sphere4, pMatMaterial, true, false);
		MaterialObject *mat_5 = new MaterialObject(rand_sphere5, pMatMaterial, true, false);
		MaterialObject *mat_6 = new MaterialObject(rand_sphere6, pMatMaterial, true, false);

//		RegularGridMeshObject *all_objs = new RegularGridMeshObject;
//		all_objs->AddObject(pBackWall);
//		all_objs->AddObject(pLeftWall);
//		all_objs->AddObject(pRightWall);
//		all_objs->AddObject(pCellWall);
//		all_objs->AddObject(pCellLight);
//		all_objs->AddObject(pFloorWall);
//		all_objs->AddObject(inst_box1);
//		all_objs->AddObject(inst_box2);
//		all_objs->AddObject(pCellLight);
//		all_objs->AddObject(sphere_inst);
//		all_objs->AddObject(mat_1);
//		all_objs->AddObject(mat_2);
//		all_objs->AddObject(mat_3);
//		all_objs->AddObject(mat_4);
//		all_objs->AddObject(mat_5);
//		all_objs->AddObject(mat_6);
//		all_objs->EnableAcceleration(false);
//		all_objs->BuildupAccelerationStructure();
//		mvecObjects.push_back(all_objs);


//		Instance *inst_all = new Instance(all_objs);
//		inst_all->RotateX(30);
//		mvecObjects.push_back(inst_all);

		MetalMaterial *metal = new MetalMaterial;
//		metal->SetDiffuse(1, new ConstantTexture(0.9, 0.9, 0.9));
		metal->SetGlossy(1, 0.9, 0.90, Color3f(0.9, 0.9, 0.9));
//		metal->SetSpecular(0.9, Color3f(0.9, 0.9, 0.9));
		metal->SetTransparent(1.5, Color3f(0.9, 0.9, 0.9), Color3f(0.6, 0.6, 0.6));

		RegularGridMeshObject *model = new RegularGridMeshObject;
		model->SetMaterial(metal);
//		model->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
		model->EnableAcceleration(true);
		model->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/dragon.ply", EMeshType::SMOOTH_SHADING);
		model->BuildupAccelerationStructure();
		Instance *inst_model = new Instance(model);
//		inst_model->Scale(300, 300, 300); // For stanford bunny10k and dragon
		inst_model->Scale(300, 300, 300);
		inst_model->RotateY(40);
		inst_model->Translate(2, 10, 260);
		mvecObjects.push_back(inst_model);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.8);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

//		PointLight *pPointLight = new PointLight;
//		pPointLight->SetScaleRadiance(20.0F);
//		pPointLight->SetPosition(Vec3f(0, 80, 230));
//		pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
//		mobjSceneLights.AddLight(pPointLight);

		AreaConstLight *pAreaConstLight1 = new AreaConstLight;
		pAreaConstLight1->SetLightShapeObject(p1);
		pAreaConstLight1->SetScaleRadiance(10.0f);
		pAreaConstLight1->SetLightColor2(1.0F, 1.0F, 1.0F);
//		mobjSceneLights.AddLight(pAreaConstLight1);

		AreaConstLight *pAreaConstLight2 = new AreaConstLight;
		pAreaConstLight2->SetLightShapeObject(p2);
		pAreaConstLight2->SetScaleRadiance(10.0f);
		pAreaConstLight2->SetLightColor2(1.0F, 1.0F, 1.0F);
//		mobjSceneLights.AddLight(pAreaConstLight2);

		AreaConstLight *pAreaConstLight = new AreaConstLight;
		pAreaConstLight->SetLightShapeObject(p5);
		pAreaConstLight->SetScaleRadiance(10.0f);
		pAreaConstLight->SetLightColor2(1.0F, 1.0F, 1.0F);
		mobjSceneLights.AddLight(pAreaConstLight);
    }

	void buildup_test_geometric_object_plus() {
		mpBackground = new ConstantTexture(Color3f(0.4, 0.5, 0.4));

		mpCamera->SetExpTime(0, 2);
		mpCamera->SetEye(Vec3f(20, 20, 30)); // good
		//mpCamera->SetEye(Vec3f(0, 0, 40));
		//mpCamera->SetEye(Vec3f(0, 20, 10));
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
		mpCamera->SetDistance(1500.0f);
		mpCamera->Update();

		//
		PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
		chekcer3d->SetCheckSize(4.0f);
		chekcer3d->SetAlternateColor(Color3f(0.75, 0.75, 0.75), Color3f(1, 1, 1));

		//
		Matte2Material *pMatBowlMaterial = new Matte2Material;
		//pMatMaterial->EnableSelfShadow(false);
		pMatBowlMaterial->SetKa(0.95);
		pMatBowlMaterial->SetKd(0.95);
		pMatBowlMaterial->SetCd2(0.0, 0.3, 0.9);

		Matte2Material *pMatThickRingMaterial = new Matte2Material;
		//pMatThickRingMaterial->EnableSelfShadow(false);
		pMatThickRingMaterial->SetKa(0.95);
		pMatThickRingMaterial->SetKd(0.95);
		pMatThickRingMaterial->SetCd2(0.9, 0.3, 0.0);

		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.25F);
		pPhongMaterial->SetKd(0.75F);
		pPhongMaterial->SetCd2(1.0F, 1.0F, 1.0F);
		pPhongMaterial->SetKs(0.25F);
		pPhongMaterial->SetCs2(1.0F, 1.0F, 1.0F);
		pPhongMaterial->SetEXP(150);

		ReflectiveMaterial *pReflective = new ReflectiveMaterial;
		pReflective->SetKa(0.3);
		pReflective->SetKd(0.3);
		pReflective->SetCd(Color3f(1, 0, 0));
		pReflective->SetKs(0.2);
		pReflective->SetCs2(1, 1, 1);
		pReflective->SetEXP(2000);
		pReflective->SetKr(0.25);
		pReflective->SetCr2(1.0, 1.0, 1.0);

		Matte2Material *pMatPlane = new Matte2Material;
		pMatPlane->SetKa(0.9);
		pMatPlane->SetKd(0.5);
		pMatPlane->SetCd(WHITE, chekcer3d);

		//
 		FlatRimmedBowl *pFlatRimmedBowl = new FlatRimmedBowl(Vec3f(0,0,0), 4.9, 5, Color3f(0,0.3,0.9), 0, 360, 45, 180);
 		MaterialObject *pMat = new MaterialObject(pFlatRimmedBowl, pMatBowlMaterial);
 		//mvecObjects.push_back(pMat);
		//pFlatRimmedBowl->SetMaterial(pMatBowlMaterial, true, pPhongMaterial, true, pMatBowlMaterial, false);
		//pFlatRimmedBowl->SetPartMaerial(FlatRimmedBowl::INNER, pMatBowlMaterial, true);
		//pFlatRimmedBowl->SetPartMaerial(FlatRimmedBowl::OUTER, pPhongMaterial, true);
		//pFlatRimmedBowl->SetPartMaerial(FlatRimmedBowl::TOP, pMatBowlMaterial, false);
		//mvecObjects.push_back(pFlatRimmedBowl);

		Instance *bowl_inst = new Instance(pMat);
		bowl_inst->Scale(0.5, 0.5, 0.5);
//		bowl_inst->RotateX(45);
//		bowl_inst->RotateZ(45);
//		bowl_inst->Translate(0, 5, 0);
		mvecObjects.push_back(bowl_inst);

//      SolidCylinder *pSolidCylinder = new SolidCylinder(Vec3f(0, 0, 0), 2, 0, 5, Color3f(1.0, 0.0, 0.0));
//      MaterialObject *pMatSolidCylinder = new MaterialObject(pSolidCylinder, pMatBowlMaterial);
//      mvecObjects.push_back(pMatSolidCylinder);
//		pSolidCylinder->SetMaterial(pReflective, true, pMatBowlMaterial, true, pMatBowlMaterial);
//		mvecObjects.push_back(pSolidCylinder);

		ThickRing *pThickRing = new ThickRing;
		MaterialObject *pMatThickRing = new MaterialObject(pThickRing, pMatThickRingMaterial);
//		mvecObjects.push_back(pMatThickRing);
//		pThickRing->SetMaterial(pMatThickRingMaterial, true, pMatThickRingMaterial, false, pPhongMaterial, true, pPhongMaterial, false);
//		Instance *thick_ring_inst = new Instance(pThickRing);
//		thick_ring_inst->Translate(5, 5, 0);
//		thick_ring_inst->RotateX(45);
//		mvecObjects.push_back(thick_ring_inst);

//		FrustumCone *pFrustumCone = new FrustumCone(Vec3f(0,0,0), 1.8, 0, 6, 3.6, Color3f(0.9f, 0.8f, 0.0f));
//		MaterialObject *pMatFrustumCone = new MaterialObject(pFrustumCone, pPhongMaterial);
//		//mvecObjects.push_back(pMatFrustumCone);
//		Instance *frustum_cone_inst = new Instance(pMatFrustumCone);
//		frustum_cone_inst->RotateX(-90);
//		mvecObjects.push_back(frustum_cone_inst);

//		SolidCone *pFrustumCone = new SolidCone(Vec3f(0, 0, 0), 1.8, 0, 6, Color3f(0.9f, 0.8f, 0.0f));
//		MaterialObject *pMatSolidCone = new MaterialObject(pFrustumCone, pPhongMaterial);
//		//mvecObjects.push_back(pMatSolidCone);
//		Instance *solid_cone_inst = new Instance(pMatSolidCone);
//		solid_cone_inst->RotateX(-90);
//		mvecObjects.push_back(solid_cone_inst);

//		ThickTrimedCone *pThickCone = new ThickTrimedCone;
//		MaterialObject *pMatThickCone = new MaterialObject(pThickCone, pMatThickRingMaterial);
//		mvecObjects.push_back(pMatThickCone);
//		Instance *thick_trimed_cone_inst = new Instance(pMatThickCone);
//		thick_trimed_cone_inst->RotateX(-90);
//		mvecObjects.push_back(thick_trimed_cone_inst);

		//
		Vec3f p0(-20, -5.0f, -100);
		Vec3f a(0, 0, 120);
		Vec3f b(40, 0, 0);
		SimpleRectangle *rect = new SimpleRectangle(p0, a, b);
		MaterialObject *pMatRect = new MaterialObject(rect, pMatPlane);
		//MaterialObject *pMatRect = new MaterialObject(rect, pMatMaterial);
		mvecObjects.push_back(pMatRect);

		//
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.4);
		pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		PointLight *pPointLight = new PointLight;
		pPointLight->SetScaleRadiance(2.0F);
		pPointLight->SetPosition(Vec3f(60, 60, 30));
		pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		mobjSceneLights.AddLight(pPointLight);
	}

	void buildup_test_microfacet_brdf_scene() {
		mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

		mpCamera->SetExpTime(0, 2);
		mpCamera->SetEye(Vec3f(20, 20, 30)); // good
		mpCamera->SetLookAt(Vec3f(0, 0, 0));
		mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
		mpCamera->SetDistance(1500.0f);
		mpCamera->Update();

		//
		PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
		chekcer3d->SetCheckSize(4.0f);
		chekcer3d->SetAlternateColor(Color3f(0.75, 0.75, 0.75), Color3f(1, 1, 1));

		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.4F);
		pPhongMaterial->SetKd(0.75F);
		pPhongMaterial->SetCd2(0.9, 0.3, 0.0F);
		pPhongMaterial->SetKs(0.25F);
		pPhongMaterial->SetCs2(0.9, 0.3, 0.0F);
		pPhongMaterial->SetEXP(150);

		//////////////////////////////////////////////////////////////////
		OrenNayar *diffuse_oren_naya = new OrenNayar;
		diffuse_oren_naya->SetSigma(new FixedFloatTexture(80));
		//		diffuse_oren_naya->SetR(new ConstantTexture(Color3f(0.9, 0.9, 0.0)));
		diffuse_oren_naya->SetR(new ConstantTexture(Color3f(191 / 255.0, 173 / 255.0, 111 / 255.0)));
		Matte2Material *pOrenNayaMatte = new Matte2Material;
		pOrenNayaMatte->SetDiffuseBRDF(diffuse_oren_naya);

		Lambertian *diffuse_ambient = new Lambertian;
		diffuse_ambient->SetKa(0.3);
		diffuse_ambient->SetCd2(0.1, 0.1, 0.1);
		MicrofacetSpecular *microfacet_specular = new MicrofacetSpecular(0.001, 1, 0.25, Color3f(191 / 255.0, 173 / 255.0, 111 / 255.0));
		PlasticMaterial *plastic_material = new PlasticMaterial;
		plastic_material->SetAmbientBRDF(diffuse_ambient);
		plastic_material->SetDiffuseBRDF(diffuse_oren_naya);
		plastic_material->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////
		Matte2Material *pMatPlane = new Matte2Material;
		pMatPlane->SetKa(0.9);
		pMatPlane->SetKd(0.5);
		pMatPlane->SetCd(WHITE, chekcer3d);
		//////////////////////////////////////////////////////////////////
		ReflectiveMaterial *pReflectiveMaterial = new ReflectiveMaterial;
		pReflectiveMaterial->SetKa(0.2);
		pReflectiveMaterial->SetKd(0.75);
		pReflectiveMaterial->SetCd3(0);
		pReflectiveMaterial->SetKs(0.0);
		pReflectiveMaterial->SetEXP(20);
		pReflectiveMaterial->SetCs3(0);
		pReflectiveMaterial->SetKr(1);
		pReflectiveMaterial->SetCr2(0.8, 1.0, 0.8);
//		pReflectiveMaterial->SetAmbientBRDF(diffuse_ambient);
//		pReflectiveMaterial->SetDiffuseBRDF(diffuse_oren_naya);
//		pReflectiveMaterial->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////
		DielectricMaterial *transparentMat = new DielectricMaterial;
		transparentMat->SetKa(0.2);
		transparentMat->SetKd(0.7);
		transparentMat->SetCd2(1, 1, 1);   // orange
		transparentMat->SetKs(1);
		transparentMat->SetEXP(200.0f);
		transparentMat->SetCs2(1, 1, 1);   // orange
		transparentMat->SetEtaIn(1.6);
		transparentMat->SetEtaOut(1.0);
		transparentMat->SetColorFilter_In(1, 1, 1);
		transparentMat->SetColorFilter_Out(1, 1, 1);
//		transparentMat->SetAmbientBRDF(diffuse_ambient);
//		transparentMat->SetDiffuseBRDF(diffuse_oren_naya);
//		transparentMat->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////
		MultiJitteredSampler *pMJSampler = new MultiJitteredSampler(100);
		pMJSampler->MapSamplesToHemiShpere(120);
		microfacet_specular->SetSampler(pMJSampler);

		GlossyReflectiveMaterial *pGlossyReflection = new GlossyReflectiveMaterial;
		pGlossyReflection->SetAmbientBRDF(diffuse_ambient);
		pGlossyReflection->SetDiffuseBRDF(diffuse_oren_naya);
		pGlossyReflection->SetGlossyBRDF(microfacet_specular);
		//////////////////////////////////////////////////////////////////

		//
		SimpleSphere *pTestBall = new SimpleSphere(Vec3f(0, 2, 0), 2.5f);
		MaterialObject *pMatBall = new MaterialObject(pTestBall, pGlossyReflection);
		mvecObjects.push_back(pMatBall);

		//
		Vec3f p0(-20, -5.0f, -100);
		Vec3f a(0, 0, 120);
		Vec3f b(40, 0, 0);
		SimpleRectangle *rect = new SimpleRectangle(p0, a, b);
		MaterialObject *pMatRect = new MaterialObject(rect, pMatPlane);
		//MaterialObject *pMatRect = new MaterialObject(rect, pMatMaterial);
		mvecObjects.push_back(pMatRect);

		//
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.4);
		pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		PointLight *pPointLight = new PointLight;
		pPointLight->SetScaleRadiance(2.0F);
		pPointLight->SetPosition(Vec3f(-20, 60, 20));
		pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
		mobjSceneLights.AddLight(pPointLight);

	}

	void buildup_test_microfacet_brdf_scene2() {
        mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

        mpCamera->SetExpTime(0, 2);
        mpCamera->SetEye(Vec3f(20, 20, 30)); // good
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
        mpCamera->SetDistance(1500.0f);
        mpCamera->Update();

        //
        PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
        chekcer3d->SetCheckSize(4.0f);
        chekcer3d->SetAlternateColor(Color3f(0.75, 0.75, 0.75), Color3f(1, 1, 1));

        //////////////////////////////////////////////////////////////////
        OrenNayar *diffuse_oren_naya = new OrenNayar;
        diffuse_oren_naya->SetSigma(new FixedFloatTexture(80));
//        diffuse_oren_naya->SetR(new ConstantTexture(Color3f(191 / 255.0, 173 / 255.0, 111 / 255.0)));
        diffuse_oren_naya->SetR(new ConstantTexture(Color3f(0.01,0.01,0.01)));
        Matte2Material *pOrenNayaMatte = new Matte2Material;
        pOrenNayaMatte->SetDiffuseBRDF(diffuse_oren_naya);

        Lambertian *diffuse_ambient = new Lambertian;
        diffuse_ambient->SetKa(0.0);
        diffuse_ambient->SetCd2(0.01, 0.01, 0.01);

		PhongMaterial *pPhongMaterial = new PhongMaterial;
		pPhongMaterial->SetKa(0.1F);
		pPhongMaterial->SetKd(0.75F);
		pPhongMaterial->SetCd2(0.9, 0.2, 0.0F);
		pPhongMaterial->SetKs(1);
		pPhongMaterial->SetCs2(0.9, 0.2, 0.0F);
		pPhongMaterial->SetEXP(150);

        //
 //       float uRough = TrowbridgeReitzDistribution::RoughnessToAlpha(0.01f);
 //       float vRough = TrowbridgeReitzDistribution::RoughnessToAlpha(0.01f);
 //       TrowbridgeReitzDistribution *trow_bridge = new TrowbridgeReitzDistribution(uRough, vRough);
 //       Fresnel *fresnel_conductor = new FresnelConductor(Color3f(1,1,1),
 //               Color3f(1, 1, 1), Color3f(1, 1, 1));
 //       MicrofacetReflection *microfacet_reflection = new MicrofacetReflection(Color3f(1,1,1), trow_bridge, fresnel_conductor);

		MetalMaterial *metal_material = new MetalMaterial;
//		metal_material->SetDiffuse(0.8, new ConstantTexture(Color3f(0.9, 0.9, 0.0)));
//		metal_material->SetSpecular(0.8, WHITE);
//		metal_material->SetTransparent(1.5, Color3f(0.8, 0.8, 0.8), WHITE);
//		metal_material->SetGlossy(0.01, 0.9, 0.25, Color3f(0.9,0.9,0.9), 100, 180);

		FresnelBlend *fresnel_blend = new FresnelBlend(Color3f(0.843117, 0.059106, 0.000849), Color3f(0.067215, 0.067215, 0.067215), 0.01, 1.0f);
		MultiJitteredSampler *pMJSamplerBRDF = new MultiJitteredSampler(100);
		pMJSamplerBRDF->MapSamplesToHemiShpere(180);
		fresnel_blend->SetSampler(pMJSamplerBRDF);
        //////////////////////////////////////////////////////////////////
        Matte2Material *pMatPlane = new Matte2Material;
        pMatPlane->SetKa(0.9);
        pMatPlane->SetKd(0.5);
        pMatPlane->SetCd(WHITE, chekcer3d);

        GlossyReflectiveMaterial *pGlossyReflection = new GlossyReflectiveMaterial;
        pGlossyReflection->SetAmbientBRDF(diffuse_ambient);
        pGlossyReflection->SetDiffuseBRDF(diffuse_oren_naya);
        pGlossyReflection->SetGlossyBRDF(fresnel_blend);
        //////////////////////////////////////////////////////////////////
		Glass2Material *glass_material = new Glass2Material;

		SubstrateMaterial *sub_material = new SubstrateMaterial(WHITE, WHITE, 0.025);

        //
        SimpleSphere *pTestBall = new SimpleSphere(Vec3f(0, 2, 0), 2.5f);
        MaterialObject *pMatBall = new MaterialObject(pTestBall, metal_material);
        mvecObjects.push_back(pMatBall);

        //
        Vec3f p0(-20, -5.0f, -100);
        Vec3f a(0, 0, 120);
        Vec3f b(40, 0, 0);
        SimpleRectangle *rect = new SimpleRectangle(p0, a, b);
        MaterialObject *pMatRect = new MaterialObject(rect, pMatPlane);
        //MaterialObject *pMatRect = new MaterialObject(rect, pMatMaterial);
        mvecObjects.push_back(pMatRect);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.4);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(2.0F);
        pPointLight->SetPosition(Vec3f(-20, 60, 20));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);
	}

	void buildup_test_obj_model_scene() {
        mpBackground = new ConstantTexture(Color3f(0.5, 0.5, 0.5));

        mpCamera->SetExpTime(0, 2);
        mpCamera->SetEye(Vec3f(20, 20, 30)); // good
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0.0f, 1.0f, 0.0f));
        mpCamera->SetDistance(1500.0f);
        mpCamera->Update();

        //
        PlaneCheckerTexture *chekcer3d = new PlaneCheckerTexture;
        chekcer3d->SetCheckSize(4.0f);
        chekcer3d->SetAlternateColor(Color3f(0.75, 0.75, 0.75), Color3f(1, 1, 1));

        //////////////////////////////////////////////////////////////////
        OrenNayar *diffuse_oren_naya = new OrenNayar;
        diffuse_oren_naya->SetSigma(new FixedFloatTexture(80));
        diffuse_oren_naya->SetR(new ConstantTexture(Color3f(0.9, 0.9, 0.0)));
//        diffuse_oren_naya->SetR(new ConstantTexture(Color3f(0.9,0.8,0.3)));
        Matte2Material *pOrenNayaMatte = new Matte2Material;
        pOrenNayaMatte->SetDiffuseBRDF(diffuse_oren_naya);

        Lambertian *diffuse_ambient = new Lambertian;
        diffuse_ambient->SetKa(0.0);
        diffuse_ambient->SetCd2(0.9, 0.9, 0.01);

		Matte2Material *matte_material = new Matte2Material;
		matte_material->SetKa(0.9);
		matte_material->SetCd2(0.9, 0.9, 0.8);
		matte_material->SetKd(1);

        MicrofacetSpecular *microfacet_specular = new MicrofacetSpecular(0.001, 1, 0.25, Color3f(0.9, 0.9, 0.9));
        MultiJitteredSampler *pMJSamplerBRDF = new MultiJitteredSampler(100);
        pMJSamplerBRDF->MapSamplesToHemiShpere(180);
        microfacet_specular->SetSampler(pMJSamplerBRDF);
        //////////////////////////////////////////////////////////////////
        Matte2Material *pMatPlane = new Matte2Material;
        pMatPlane->SetKa(0.9);
        pMatPlane->SetKd(1);
        pMatPlane->SetCd(WHITE, chekcer3d);

		FresnelBlend *fresnel_blend = new FresnelBlend(Color3f(0.843117, 0.059106, 0.000849), Color3f(0.067215, 0.067215, 0.067215), 0.01, 0.3);
		MultiJitteredSampler *pMJSamplerBRDF2 = new MultiJitteredSampler(100);
		pMJSamplerBRDF2->MapSamplesToHemiShpere(180);
		fresnel_blend->SetSampler(pMJSamplerBRDF2);

		matte_material->SetSampler(pMJSamplerBRDF);

        GlossyReflectiveMaterial *pGlossyReflection = new GlossyReflectiveMaterial;
        pGlossyReflection->SetAmbientBRDF(diffuse_ambient);
        pGlossyReflection->SetDiffuseBRDF(diffuse_oren_naya);
        pGlossyReflection->SetGlossyBRDF(microfacet_specular);
        //////////////////////////////////////////////////////////////////
        // For other materials testing.
        DielectricMaterial *transparentMat = new DielectricMaterial;
        transparentMat->SetKa(0.2);
        transparentMat->SetKd(0.01);
        transparentMat->SetCd2(1, 1, 1);   // orange
        transparentMat->SetKs(0.01);
        transparentMat->SetEXP(200.0f);
        transparentMat->SetCs2(1, 1, 1);   // orange
        transparentMat->SetEtaIn(1.6);
        transparentMat->SetEtaOut(1.0);
        transparentMat->SetColorFilter_In(1, 1, 1);
        transparentMat->SetColorFilter_Out(1, 1, 1);

        ReflectiveMaterial *pReflectiveMaterial = new ReflectiveMaterial;
        pReflectiveMaterial->SetKa(0);
        pReflectiveMaterial->SetKd(0);
        pReflectiveMaterial->SetCd3(1);
        pReflectiveMaterial->SetKs(0);
        pReflectiveMaterial->SetEXP(20);
        pReflectiveMaterial->SetCs3(1);
        pReflectiveMaterial->SetKr(1);
        pReflectiveMaterial->SetCr2(0.9, 0.9, 0.9);
 //       pReflectiveMaterial->SetAmbientBRDF(diffuse_ambient);
//        pReflectiveMaterial->SetDiffuseBRDF(diffuse_oren_naya);
//        pReflectiveMaterial->SetGlossyBRDF(microfacet_specular);

        PhongMaterial *pPhongMaterial = new PhongMaterial;
        pPhongMaterial->SetKa(0.75);
        pPhongMaterial->SetKd(0.75);
        pPhongMaterial->SetCd2(0.9, 0.9, 0.0f);
        pPhongMaterial->SetKs(0.95F);
        pPhongMaterial->SetCs2(0.9, 0.9, 0.0f);
        pPhongMaterial->SetEXP(1380);

        GlossyReflectiveMaterial *pGlossyMaterial = new GlossyReflectiveMaterial;
        pGlossyMaterial->SetKa(0.25F);
        pGlossyMaterial->SetKd(0.75F);
        pGlossyMaterial->SetCd2(1.0F, 1.0F, 0.0F);
        pGlossyMaterial->SetKs(0.95F);
        pGlossyMaterial->SetCs2(1.0F, 1.0F, 0.0F);
        pGlossyMaterial->SetEXP(180);
        MultiJitteredSampler *pSampler = new MultiJitteredSampler(100);
        pSampler->MapSamplesToHemiShpere(1800);
        pGlossyMaterial->SetSampler(pSampler);

        ////////// The New Metal Material /////////////
		ImageTexture *tex = new ImageTexture;
		tex->LoadJPGFromFile("D:/workspace/CG/Ray Tracing/obj_models/dining-room/textures/Teacup.png");
        MetalMaterial *metal_material = new MetalMaterial;
//		metal_material->SetDiffuse(0.7, tex);
//		metal_material->SetSpecular(0.6, Color3f(0.9, 0.9, 0.9));
//		metal_material->SetTransparent(1.5, WHITE, WHITE);
		metal_material->SetGlossy(0.01, 0.9, 0.35, Color3f(0.9, 0.9, 0.4), 100, 200);
//		metal_material->SetGlossy(0.01, 0.6, 0.35, Color3f(0.1, 0.1, 0.1), 100, 2);
//		metal_material->SetGlossy(17, 1, 0.75, RED);

        Glass2Material *glass_material = new Glass2Material;
//        glass_material->SetGlossy(0.001, 1, 0.6, WHITE);
        glass_material->SetKa(0.01);
        glass_material->SetKd(0.01);
        glass_material->SetCd2(0.9, 0.9, 0.9f);
        glass_material->SetKs(0.01);
        glass_material->SetCs2(0.9, 0.9, 0.9f);
        glass_material->SetEXP(180);

		SubstrateMaterial *sub_material = new SubstrateMaterial;

//        SimpleSphere *pTestBall = new SimpleSphere(Vec3f(2.5, 4, 6), 1);
//        MaterialObject *pMatBall = new MaterialObject(pTestBall, matte_material);
//        mvecObjects.push_back(pMatBall);

//		SimpleSphere *pTestBall2 = new SimpleSphere(Vec3f(0, 0, 0), 4);
//		MaterialObject *pMatBall2 = new MaterialObject(pTestBall2, glass_material);
//		mvecObjects.push_back(pMatBall2);
/*
        RegularGridMeshObject *obj_test_model0 = new RegularGridMeshObject;
        obj_test_model0->SetMaterial(matte_material);
//		obj_test_model->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
        obj_test_model0->EnableAcceleration(true);
        obj_test_model0->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/material-testball/models/Mesh000.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ);
        obj_test_model0->BuildupAccelerationStructure();
        Instance *obj_model_inst0 = new Instance(obj_test_model0);
        obj_model_inst0->Scale(2.5, 2.5, 2.5);
 //       mvecObjects.push_back(obj_model_inst0);

        RegularGridMeshObject *obj_test_model1 = new RegularGridMeshObject;
        obj_test_model1->SetMaterial(glass_material);
//		obj_test_model->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
       obj_test_model1->EnableAcceleration(true);
        obj_test_model1->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/material-testball/models/Mesh001.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ);
        obj_test_model1->BuildupAccelerationStructure();
        Instance *obj_model_inst1 = new Instance(obj_test_model1);
		obj_model_inst1->Translate(-0.1, -0.58, 0);
        obj_model_inst1->Scale(2.5, 2.5, 2.5);
  //      mvecObjects.push_back(obj_model_inst1);

        RegularGridMeshObject *obj_test_model2 = new RegularGridMeshObject;
        obj_test_model2->SetMaterial(metal_material);
//		obj_test_model2->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
        obj_test_model2->EnableAcceleration(true);
//		obj_test_model2->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/Bunny10K.ply", EMeshType::SMOOTH_SHADING, EModelType::MODEL_PLY, false);
//		obj_test_model2->LoadFromFile("D:/git_projects/PBRTv3/pbrt-v3/scenes/material-testball/models/Mesh000.ply", EMeshType::SMOOTH_SHADING, EModelType::MODEL_PLY, true);
		obj_test_model2->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/material-testball/models/Mesh002.obj", EMeshType::SMOOTH_UV_SHADING, EModelType::MODEL_OBJ, false);
//        obj_test_model2->LoadFromFile("/Users/wuwenxi/workspace/CG/Models_Materials/material-testball/models/Mesh000.ply", EMeshType::SMOOTH_SHADING, EModelType::MODEL_PLY, true);
//        obj_test_model2->LoadFromFile("/Users/wuwenxi/workspace/CG/Models_Materials/material-testball 2/models/Mesh002.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ);
        obj_test_model2->BuildupAccelerationStructure();
        Instance *obj_model_inst2 = new Instance(obj_test_model2);
		obj_model_inst2->Translate(0.0, 0.45, 0);
		obj_model_inst2->Scale(2.5, 2.5, 2.5);
//        mvecObjects.push_back(obj_model_inst2);*/
/*
		SubstrateMaterial *sub_material2 = new SubstrateMaterial(
			Color3f(0.900000, 0.900000, 0.900000), Color3f(0.400000, 0.400000, 0.400000), 0.01, 0.1, 100, 2000);
		sub_material2->SetDiffuseEx(tex);

		RegularGridMeshObject *obj_test_model_teapot = new RegularGridMeshObject;
		obj_test_model_teapot->SetMaterial(sub_material2);
		obj_test_model_teapot->EnableAcceleration(true);
		obj_test_model_teapot->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/teapot/models/Mesh001.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ, false);
		obj_test_model_teapot->BuildupAccelerationStructure();
		Instance *obj_model_teapot_inst = new Instance(obj_test_model_teapot);
		obj_model_teapot_inst->Translate(0, -0.5, 0);
		obj_model_teapot_inst->RotateY(-60);
		mvecObjects.push_back(obj_model_teapot_inst);

		RegularGridMeshObject *obj_test_model_teapot_lid = new RegularGridMeshObject;
		obj_test_model_teapot_lid->SetMaterial(sub_material2);
		obj_test_model_teapot_lid->EnableAcceleration(true);
		obj_test_model_teapot_lid->LoadFromFile("D:/workspace/CG/Ray Tracing/obj_models/teapot/models/Mesh000.obj", EMeshType::SMOOTH_SHADING, EModelType::MODEL_OBJ, false);
		obj_test_model_teapot_lid->BuildupAccelerationStructure();
		Instance *obj_model_teapot_lid_inst = new Instance(obj_test_model_teapot_lid);
		obj_model_teapot_lid_inst->Translate(0, -0.5, 0);
		obj_model_teapot_lid_inst->RotateY(-60);
		mvecObjects.push_back(obj_model_teapot_lid_inst);
*/

		MetalMaterial *metal = new MetalMaterial;
		//		metal->SetDiffuse(1, new ConstantTexture(0.9, 0.9, 0.9));
		metal->SetGlossy(1, 0.5, 0.55, Color3f(0.9, 0.9, 0.9));
		//		metal->SetSpecular(0.9, Color3f(0.9, 0.9, 0.9));
				metal->SetTransparent(1.5, Color3f(0.6, 0.6, 0.6), Color3f(0.6, 0.6, 0.6));

		RegularGridMeshObject *model = new RegularGridMeshObject;
		model->SetMaterial(metal);
		//		model->ReverseMeshNormals(); ////////// IMPORTANT SETTING!!!!!!
		model->EnableAcceleration(true);
		model->LoadFromFile("D:/workspace/CG/Ray Tracing/59739329PLYFiles.bak/PLYFiles.bak/ply/dragon.ply", EMeshType::SMOOTH_SHADING);
		model->BuildupAccelerationStructure();
		Instance *inst_model = new Instance(model);
		//		inst_model->Scale(300, 300, 300); // For stanford bunny10k and dragon
		inst_model->Scale(40, 40, 40);
		inst_model->RotateY(40);
		inst_model->Translate(0, -5, 0);
		mvecObjects.push_back(inst_model);

        //
        Vec3f p0(-20, -5.0f, -100);
        Vec3f a(0, 0, 120);
        Vec3f b(40, 0, 0);
        SimpleRectangle *rect = new SimpleRectangle(p0, a, b);
		pMatPlane->SetSampler(pSampler);
        MaterialObject *pMatRect = new MaterialObject(rect, pMatPlane);
        //MaterialObject *pMatRect = new MaterialObject(rect, pMatMaterial);
        mvecObjects.push_back(pMatRect);

        //
        AmbientLight *pAmbientLight = new AmbientLight;
        pAmbientLight->SetScaleRadiance(0.4);
        pAmbientLight->SetLightColor(Color3f(Color3f(1, 1, 1)));
        mobjSceneLights.SetAmbientLight(pAmbientLight);

        PointLight *pPointLight = new PointLight;
        pPointLight->SetScaleRadiance(1.0F);
        pPointLight->SetPosition(Vec3f(20, 40, 14));
        pPointLight->SetLightColor(Color3f(1.0F, 1.0F, 1.0F));
        mobjSceneLights.AddLight(pPointLight);

		//
		ImageTexture *pTex = new ImageTexture;
		pTex->LoadJPGFromFile("D:/workspace/CG/Ray Tracing/TextureImages/TextureFiles/jpg/CloudsHighRes.jpg");
		pTex->SetMappingMethod(new LightProbMapping);
		EnvrionmentTextureLight *pEnvLight = new EnvrionmentTextureLight(0.2, pTex);
		MultiJitteredSampler *pEnvLightSampler = new MultiJitteredSampler(100);
		pEnvLightSampler->MapSamplesToHemiShpere(4000);
		pEnvLight->SetSampler(pEnvLightSampler);
//		pEnvLight->EnableShadow(false);
//		mobjSceneLights.AddLight(pEnvLight);

//		DirectionLight *pDirectionLight = new DirectionLight(Vec3f(0, 1, 0), 1.0f, Color3f(1, 1, 1));
//		mobjSceneLights.AddLight(pDirectionLight);
	}

	void buildup_test_inter_grid_shadow_scne() {
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
		Matte2Material *pMatBox = new Matte2Material;
		pMatBox->SetKa(1);
		pMatBox->SetKd(1);
		pMatBox->SetCd(Color3f(1, 1, 1), new ConstantTexture(Color3f(1, 1, 1)));

		Emissive2Material *pEmissive = new Emissive2Material;
		pEmissive->SetRadiance(7.8);
		pEmissive->SetEmissiveColor(Color3f(1, 1, 1));

		//
//		XZRect *p3 = new XZRect(Vec3f(-40, 80, 200), true, 80, 80);
//		//      p3->SetColor(Color3f(0,0,0));
//		MaterialObject *pCellWall = new MaterialObject(p3, pMatMaterial4);
//		mvecObjects.push_back(pCellWall);

		XZRect *p4 = new XZRect(Vec3f(-40, 0, 200), false, 80, 80);
		//       p4->SetColor(Color3f(0,0,0));
		MaterialObject *pFloorWall = new MaterialObject(p4, pMatBox);
//		mvecObjects.push_back(pFloorWall);

		//
		// Add a glass material ball in the middle of the box.
		SimpleSphere *pSphere = new SimpleSphere(Vec3f(0, 40, 235), 20);
		MaterialObject *sphere_mat = new MaterialObject(pSphere, pMatBox);
//		Instance *sphere_inst = new Instance(sphere_mat);
//		sphere_inst->RotateY(60);
//		sphere_inst->Translate(20, 40, 240);
//		mvecObjects.push_back(sphere_mat);

		//
		XZRect *p5 = new XZRect(Vec3f(-20, 78, 210), true, 40, 40);
		MaterialObject *pCellLight = new MaterialObject(p5, pEmissive);
//		mvecObjects.push_back(pCellLight);

		RegularGridMeshObject *all_objs = new RegularGridMeshObject;
		all_objs->AddObject(pCellLight);
		all_objs->AddObject(pFloorWall);
		all_objs->AddObject(sphere_mat);
		all_objs->EnableAcceleration(false);
		mvecObjects.push_back(all_objs);

		//
		AmbientLight *pAmbientLight = new AmbientLight;
		pAmbientLight->SetScaleRadiance(0.4);
		pAmbientLight->SetLightColor(Color3f(Color3f(0.8, 0.9, 0.6)));
		mobjSceneLights.SetAmbientLight(pAmbientLight);

		AreaConstLight *pAreaConstLight = new AreaConstLight;
		pAreaConstLight->SetLightShapeObject(p5);
		pAreaConstLight->SetScaleRadiance(10.0f);
		pAreaConstLight->SetLightColor2(1.0F, 1.0F, 1.0F);
		mobjSceneLights.AddLight(pAreaConstLight);
	}

    void builup_shelf_test_scene() {
        mpBackground = new ConstantTexture(Color3f(0.1F, 0.1F, 0.1F));

        mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
        mpCamera->SetEye(Vec3f(0, 40, 370));
        mpCamera->SetLookAt(Vec3f(0, 40, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        ((PerspectiveCamera *)mpCamera)->SetDistance(500);
        mpCamera->Update();

        MaterialObject *pGlassBall = new MaterialObject(new SimpleSphere(Vec3f(20, 50, 240), 10),
                                                        new GlassMaterial(2.5, Color3f(1,1,1), Color3f(1, 1, 1)));
        mvecObjects.push_back(pGlassBall);

        MaterialObject *pMetalBall = new MaterialObject(new SimpleSphere(Vec3f(-10, 50, 250), 10),
                                                                new MirrorMaterial(Color3f(0.7,0.7,0.8), 0.6));
        mvecObjects.push_back(pMetalBall);

        AreaLightObject *pCellLight = new AreaLightObject(new XZRect(Vec3f(-30, 78, 210), true, 70, 70),
                                                          new ConstantTexture(Color3f(3.0F, 3.0F, 3.0F)),
                                                          new ConstantTexture(Color3f(0.0f, 0.0f, 0.0f)));
        pCellLight->TurnON();

        mvecObjects.push_back(pCellLight);
    }

    void buildup_random_sphere_scene() {
        //
        //
        mpBackground = new SimpleSkyTexture;

        mpCamera->SetViewPlane(mpViewPlane->Width(), mpViewPlane->Height());
        mpCamera->SetEye(Vec3f(0, 3.3, 10));
        mpCamera->SetLookAt(Vec3f(0, 0, 0));
        mpCamera->SetUpVector(Vec3f(0, 1, 0));
        mpCamera->SetExpTime(0, 2);
        ((PerspectiveCamera *)mpCamera)->SetDistance(400);
        mpCamera->Update();

        //
        RegularGridMeshObject *grid = new RegularGridMeshObject;

        MaterialObject *sphere0 = new MaterialObject(new SimpleSphere(Vec3f(0, -1000, 0), 1000),
                                                     new MatteMaterial(Color3f(0.8,0.8,0.8)));
//        mvecObjects.push_back(sphere0);
        grid->AddObject(sphere0);


        int i = 1;
        for (int a = -4; a < 4; a++) {
            for (int b = -4; b < 4; b++) {
                float choose_mat = Random::drand48();
                Vec3f center(a+0.9*Random::drand48(), 0.2, b+0.9*Random::drand48());
                Vec3f temp = center - Vec3f(4,0.2,0);
                if (temp.Length() > 0.9) {
                    if (choose_mat < 0.8) {

                        if (Random::drand48() > 0.5) {
                            MaterialObject *temp = new MaterialObject(
                                        new MovingShpere(center, 0.2, Vec3f(center.X()+0.1, center.Y(), center.Z()), 0, 1),
                            new MatteMaterial(Color3f(Random::drand48(),
                                                      Random::drand48(), Random::drand48())));
                            //     mvecObjects.push_back(temp);
                                 grid->AddObject(temp);
                            //     std::cout << "moving " << std::endl;
                        } else {
                            MaterialObject *temp = new MaterialObject(new SimpleSphere(center, 0.2),
                            new MatteMaterial(Color3f(Random::drand48(),
                                                      Random::drand48(), Random::drand48())));
                            //     mvecObjects.push_back(temp);
                                 grid->AddObject(temp);
                        }
                    } else if (choose_mat < 0.95) {
                        MaterialObject *temp = new MaterialObject(new SimpleSphere(center, 0.2),
                        new MirrorMaterial(Color3f(Random::drand48(),
                                                  Random::drand48(), Random::drand48()), Random::drand48()));
                    //    mvecObjects.push_back(temp);
                        grid->AddObject(temp);
                    } else {
                        MaterialObject *temp = new MaterialObject(new SimpleSphere(center, 0.2),
                        new GlassMaterial(1.5, WHITE, WHITE));
                    //    mvecObjects.push_back(temp);
                        grid->AddObject(temp);
                    }
                }
            }
        }

        MaterialObject *big0 = new MaterialObject(new SimpleSphere(Vec3f(4, 1, 0), 1),
        new GlassMaterial(1.5, WHITE, WHITE));
//        mvecObjects.push_back(big0);
        grid->AddObject(big0);

        MaterialObject *big1 = new MaterialObject(new SimpleSphere(Vec3f(-4, 1, 0), 1),
        new MirrorMaterial(Color3f(0.8, 0.8, 0.8), 0));
//        mvecObjects.push_back(big1);
        grid->AddObject(big1);

        grid->EnableAcceleration(true);
        grid->BuildupAccelerationStructure();

        mvecObjects.push_back(grid);

    }

private:
	float mnPointLightX;
	float mnPointLightY;
	float mnPointLightZ;
};
