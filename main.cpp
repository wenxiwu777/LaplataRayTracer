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
        this->draw_progress_bar(per, '=');
        std::cout << std::fixed << std::setprecision(2) << "[" << 100.0f * per << "%" << "]" << std::endl;
    }

private:
    inline void draw_progress_bar(float per, char tag) {
        const int MAX_TAG = 40;
        std::cout << "[";
        int draw_count = MAX_TAG * per;
        for (int i = 0; i < draw_count; ++i) {
            std::cout << tag;
        }
        for (int i = draw_count; i < MAX_TAG; ++i) {
            std::cout << " ";
        }
        std::cout << "]";
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
