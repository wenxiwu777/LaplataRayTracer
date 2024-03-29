# LaplataRayTracer
This is a ray tracer renderer written in C/C++ on windows based on &lt;&lt;ray tracing from the ground up>>  , &lt;&lt;ray tracing in one weekend>> &lt;&lt;ray tracing the next week>>,  &lt;&lt;ray tracing the rest of your life>>, &lt;&lt;PBRT>>, it offers a wide varieties of features from the basic drawing geometry objects to smooth and clean (less noise) cornell box with the physically based sampling , and it supports mesh grid object drawing, like high resolution standford bunny 69K, horse 97K, happy buddha and the dragon model, and you can find more features in its source code. 

(about three moths later... ...）

And now it supports more features, i can't list them all here, its too many, like noise texture mapping, OBJ file loader, panoramic cameram, a wide range of new brdf and materials included, for instance, cook-torrance lighting model, oren nayar diffuse model, smith ggx microfacet model, lots of new gemoetric objects and part object, like
part shpere/clyinder/cone/tori, and complex objects like bowl, thick ring, solid cylinder and so on, and some more complex objects, like rotational sweeping, the tellslation of sphere and sweeping objects.

I used my new tracer to render many images, lik the material ball, teapot from PBRT.org with different materials(metal, substrate, glass, plastic, mirror, matte), and fixed sooo many bugs, not its a robust version.


DemoScene.h shows how to use it.

The following images are rendered by this ray tracer:

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/RandomSphereScene.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/RoughGlass_Bunny.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/Rough_Glass_Compare.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/aluminum_like_teapot.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/teapot_full.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/big_helmet.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/material_ball2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/fish_bowl_with_diff_material.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/FlatRimmedBowl.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/instance_cone.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/instance_thick_ring.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/part_cylinder_0_270.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/part_cone.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/full_clipping_cone_2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/part_tori_2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/cor_bunny1.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/cor_bunny2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mac1.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mac2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mac3.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mac4.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mac5.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/v3_cover_cuastics.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/glassball1.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/golden_horse.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/finally_golden_dragon.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/mirror+mircofacet_sliver_dragon.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b1.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b2.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b3.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b4.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b5.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b6.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b7.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b8.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b9.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b10.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b11.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b12.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b13.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b14.png)

![Image text](https://github.com/wenxiwu777/LaplataRayTracer/blob/master/ShowCaseOfScreenShoot/b15.png)

