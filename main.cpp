#include <sys/time.h>
#include <iostream>

#include "DemoScene.h"

//
long long GetCurrentTime() {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//
class RenderProcessCallback : public IRenderWindowSink {
public:
    RenderProcessCallback() {

    }

    virtual ~RenderProcessCallback() {

    }

public:
    virtual void OnNotifyRenderProgress(int row, int total_row) {
        float per = (float)(row + 1) / (float)total_row;
        std::cout << std::fixed << std::setprecision(2) << "[" << 100.0f * per << "%" << "]" << std::endl;
    }

};

//
int main() {

    std::cout << "begin to render" << std::endl;
    long long begin_time = GetCurrentTime();

    CDemoScene demo;

    RenderProcessCallback renderProcCallback;
    demo.SetRenderWndSink(&renderProcCallback);
    demo.Setup(400, 400);
    demo.BuildScene();
    demo.RenderScene();
    demo.SaveScene("./result_output.tga");

    long long end_time = GetCurrentTime();
    long long delta_time = end_time - begin_time;
    std::cout << "render done!" << " elapse: " << delta_time << " ms" << std::endl;

    return 0;
}
