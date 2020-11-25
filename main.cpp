#include <iostream>
#include <string>

#include "DemoScene.h"

//
int main() {

    CDemoScene demo;
    std::string saved_file = "./result_output.tga";

    RenderProcessReporter renderProcReporter;
    demo.SetRenderWndSink(&renderProcReporter);
    demo.Setup(400, 400);
    demo.BuildScene();
    demo.RenderScene();
    demo.SaveScene(saved_file.c_str());

    std::string report_str = renderProcReporter.GetRenderReport((void *)&saved_file);
    std::cout << report_str << std::endl;

    return 0;
}
